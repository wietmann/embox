/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <assert.h>
#include <stdint.h>

#include <system_stm32f7xx.h>
#include <stm32f7xx_hal.h>
#include <stm32746g_discovery.h>
#include <stm32f7xx_hal_cortex.h>

#include <framework/mod/options.h>

#include <stm32746g_discovery_sdram.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(sdram_init);

static int sdram_init(void) {
	if (BSP_SDRAM_Init() != SDRAM_OK) {
		return -1;
	}
	return 0;
}

uint32_t sdram_start_address(void) {
	return SDRAM_DEVICE_ADDR;
}

uint32_t sdram_size(void) {
	return SDRAM_DEVICE_SIZE;
}
