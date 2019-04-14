/**
 * @file
 * @brief Print all or part of enviroment
 *
 * @date 2019-04-14
 * @author Dmitry Danilov
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	char *env_val;
	int opt;
	int print_null = 0;
	int env_not_found = 0;
	
	while (-1 != (opt = getopt(argc, argv, "0h"))) {
		switch (opt) {
			case '0':
				print_null = 1;
				break;
			case '?':
				printf("Try '%s -h` for more inforamtion.\n", argv[0]);
				return -EINVAL;
			case 'h':
				printf("Usage: %s [-0 | -h] [NAME]...\n", argv[0]);
				return 0;
			default:
				break;
		}
	}

	if (optind < argc) {
		do {
			if (NULL != (env_val = getenv(argv[optind])))
				printf("%s%c", env_val, print_null ? '\0' : '\n');
			else
				env_not_found = 1;
			++optind;
		}
		while (optind < argc);
		if (env_not_found)
			return -ENOENT;
	}
	else
		for (char **env = environ; *env; env++)
			printf("%s%c", *env, print_null ? '\0' : '\n');
	return 0;
}
