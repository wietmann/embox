
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>

#define OPTSTR "gGunh"

int main(int argc, char **argv) {
	int print_user = 0, print_pgroup = 0, print_agroup = 0;
	int print_names = 0;
	char *user_name = NULL;
	
	uid_t user_uid;
	
	int opt;
	while (-1 != (opt = getopt(argc, argv, OPTSTR))) {
		switch (opt) {
			case 'u':
				print_user = 1;
				break;
			case 'g':
				print_pgroup = 1;
				break;
			case 'G':
				print_agroup = 1;
				break;
			case 'n':
				print_names = 1;
				break;
			case '?':
				printf("EINVAL MESSAGE\n");
				return -EINVAL;
			case 'h':
				printf("HELP MESSAGE\n");
				return 0;
			default:
				break;
		}
	}
	if (optind < argc) {
		user_name = argv[optind];
		++optind;
	}
	
	/* Error checks */
	if (optind < argc) {
		printf("%s: Extra operand '%s'\n", argv[optind]);
		return -EINVAL;
	}
	if (print_user + print_pgroup + print_agroup > 1) {
		printf("%s: Only one of 'u g G' options is allowed.\n", argv[0]);
		return -EINVAL;
	}
	if (print_names && !(print_user + print_pgroup + print_agroup)) {
		printf("%s: 'n' should be used with one of 'u g G' options.\n", argv[0]);
		return -EINVAL;
	}

	if (user_name) {
		
	}

	/*if (print_user) {
		
	}
	else if (print_pgroup) {
		
	}
	else if (print_agroup) {
		
	}
	else {
		
	}*/

	return 0;
}
