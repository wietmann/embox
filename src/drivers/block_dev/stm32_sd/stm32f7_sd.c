/**
 * @file
 * @brief SD-Card driver for STM32F7-Discovery
 * Derived from stm32f4_sd.c
 *
 * @author  Alex Kalmuk
 * @date    11.08.2017
 */

#include <string.h>
#include <util/log.h>

#include <drivers/block_dev.h>
#include <framework/mod/options.h>
#include <kernel/time/ktime.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"

static int stm32f7_sd_init(void *arg);
static int stm32f7_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f7_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f7_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F7_SD_DEVNAME "stm32f7_sd_card"
#define SD_BUF_SIZE OPTION_GET(NUMBER, sd_buf_size)

block_dev_driver_t stm32f7_sd_driver = {
	.name  = STM32F7_SD_DEVNAME,
	.ioctl = stm32f7_sd_ioctl,
	.read  = stm32f7_sd_read,
	.write = stm32f7_sd_write,
	.probe = stm32f7_sd_init,
};

BLOCK_DEV_DEF(STM32F7_SD_DEVNAME, &stm32f7_sd_driver);

static int stm32f7_sd_init(void *arg) {
	struct block_dev *bdev;

	//if (block_dev_lookup(STM32F7_SD_DEVNAME)) {
	//	log_error("block_dev_lookup failed");
	//	return -1;
	//}
	if (BSP_SD_Init() != SD_OK) {
		log_error("BSP_SD_Init() failed");
		return -1;
	}
	bdev = block_dev_create(STM32F7_SD_DEVNAME, &stm32f7_sd_driver, NULL);
	bdev->size = stm32f7_sd_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
	return 0;
}

static int stm32f7_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	HAL_SD_CardInfoTypedef info;

	BSP_SD_GetCardInfo(&info);

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return info.CardCapacity;
	case IOCTL_GETBLKSIZE:
		return info.CardBlockSize;
	default:
		log_error("Unknown cmd %d", cmd);
		return -1;
	}
}

static uint8_t sd_buf[SD_BUF_SIZE];

static int stm32f7_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;
	
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);

	res = BSP_SD_ReadBlocks((uint32_t*) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_ReadBlocks failed count=%d,blkno=%d", count, blkno);
		return -1;
	}
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
	memcpy(buf, sd_buf, bsize);
	return res;
}

static int stm32f7_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;

	memcpy(sd_buf, buf, bsize);
	res = BSP_SD_WriteBlocks((uint32_t*) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_WriteBlock failed");
		return -1;
	}
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
	return res;
}
