/**
 * @file Politely refuse a login
 * @brief 
 *
 * @date 2019-04-20
 * @author Dmitry Danilov
 */

#include <stdio.h>
#include <errno.h>
#include <framework/mod/options.h>

#define MESSAGE OPTION_STRING_GET(message)

int main(void) {
	puts(MESSAGE);
	return -EACCES;
}
