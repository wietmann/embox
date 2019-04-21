
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define OPTSTR "gGunh"

/* No UID_MAX set anywhere, so just look in sys/types.h and set it accordingly */
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

int main(int argc, char **argv) {
	int print_user = 0, print_pgroup = 0, print_agroup = 0;
	int print_names = 0;
	char *user_name = NULL;
	char *group_name = NULL;
	uid_t uid;
	gid_t gid;
	struct passwd *pwd_entry;
	struct group *grp_entry;
	
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
	
	pwd_entry = get_pwd_entry(user_name);
	if (print_user) {
		if (print_names)
			printf("%s\n", pwd_entry->pw_name);
		else
			printf("%d\n", pwd_entry->pw_uid);
		return 0;
	}
	
	/* TODO: malloc is shit */
	user_name = (char *) malloc((strlen(pwd_entry->pw_name)+1) * sizeof(char));
	if (NULL == user_name) {
		printf("%s: %s\n", argv[0], strerror(errno));
		return -errno;
	}
	strcpy(user_name, pwd_entry->pw_name);
	uid = pwd_entry->pw_uid;

	gid = pwd_entry->pw_gid;
	if (print_pgroup && !print_names) {
		printf("%d\n", gid);
		
		free(user_name);
		return 0;
	}

	grp_entry = getgrgid(pwd_entry->pw_gid);
	if (print_pgroup && print_names) {
		printf("%s\n", grp_entry->gr_name);
		
		free(user_name);
		return 0;
	}
	
	group_name = (char *) malloc((strlen(grp_entry->gr_name)+1)*sizeof(char));
	if (NULL == group_name) {
		printf("%s: %s\n", argv[0], strerror(errno));
		return -errno;
	}
	strcpy(group_name, grp_entry->gr_name);

	/* Get secondary group list */
	/* TODO: Implement */
	if (print_agroup) {
		printf("%s: Not implemented\n", argv[0]);
		
		free(user_name);
		free(group_name);
		return -ENOSYS;
	}
	
	/* Print summary */
	printf("uid=%d(%s) gid=%d(%s)\n", uid, user_name, gid, group_name);
	
	free(user_name);
	free(group_name);
	return 0;
}
