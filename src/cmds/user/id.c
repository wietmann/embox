
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define OPTSTR "gGunh"

/* No UID_MAX set anywhere, so just look sys/types.h and set it accordingly */
#ifndef UID_MAX
#define UID_MAX USHRT_MAX
#endif

static char *prog_name;

int isnumber(char *str) {
	for (; *str; ++str)
		if (!(isdigit(*str)))
			return 0;
	return 1;
}

struct passwd *get_pwd_entry(char *user_name)
{
	uid_t uid;
	long uid_buf;
	struct passwd *pwd_entry = NULL;
	
	if (user_name) {
		if (isnumber(user_name)) {
			uid_buf = strtol(user_name, NULL, 10);
			if (ERANGE == errno || uid_buf > UID_MAX) {
				printf("%s: UID value is out of range.\n", prog_name);
				exit(EXIT_FAILURE);
			}
			uid = (uid_t) uid_buf;
			if (NULL == (pwd_entry = getpwuid(uid))) {
				printf("%s: No user with such UID.\n", prog_name);
				exit(EXIT_FAILURE);
			}
		}
		else {
			if (NULL == (pwd_entry = getpwnam(user_name))) {
				printf("%s: No user with such name.\n", prog_name);
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		uid = getuid();
		if (NULL == (pwd_entry = getpwuid(uid))) {
			printf("%s: Error retrieving current user's passwd entry.\n", prog_name);
			exit(EXIT_FAILURE);
		}
	}
	
	return pwd_entry;
}

//struct group *get_grp_entry(

int main(int argc, char **argv) {
	int print_user = 0, print_pgroup = 0, print_agroup = 0;
	int print_names = 0;
	char *user_name = NULL;
	uid_t user_uid;
	long uid_buf;
	struct passwd *pwd_entry = NULL;
	struct group *grp_entry = NULL;
	
	prog_name = argv[0];
	
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
		printf("%s: Extra operand '%s'\n", argv[0], argv[optind]);
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
	
	/*if (user_name) {
		if (isnumber(user_name)) {
			uid_buf = strtol(user_name, NULL, 10);
			if (ERANGE == errno || uid_buf >= UID_MAX) {
				printf("%s: UID value is out of range.\n", argv[0]);
				return -1;
			}
			user_uid = (uid_t) uid_buf;
			if (NULL == (pwd_entry = getpwuid(user_uid))) {
				printf("%s: No user with such UID.\n", argv[0]);
				return -1;
			}
			user_name = pwd_entry->pw_name;
		}
		else {
			if (NULL == (pwd_entry = getpwnam(user_name))) {
				printf("%s: No user with such name.\n", argv[0]);
				return -1;
			}
			user_uid = pwd_entry->pw_uid;
		}
	}
	else {
		user_uid = getuid();
		pwd_entry = getpwuid(user_uid);
		user_name = pw_uid->pw_name;
	}*/
	
	pwd_entry = get_pwd_entry(user_name);
	
	if (print_user) {
		if (print_names)
			printf("%s\n", pwd_entry->pw_name);
		else
			printf("%d\n", pwd_entry->pw_uid);
		return 0;
	}

	

	/*if (print_user) {
		if(user_name)
		{
			
		}
		else
		{
			user_uid = getuid();
			if(print_names)
			{
				struct passwd *pwd = getpwuid(user_uid);
				printf("%s\n", pwd->pw_name);
			}
			else
			{
				printf("%d\n", user_uid);
			}
		}
	}
	else if (print_pgroup) {
		
	}
	else if (print_agroup) {
		
	}
	else {
		
	}*/

	return 0;
}
