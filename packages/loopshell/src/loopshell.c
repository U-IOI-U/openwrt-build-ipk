#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#define _GNU_SOURCE
#include <errno.h>

static int loop_success = 0;
static int loop_error = 0;
static char* shell = 0;

static void loopshell_help()
{
	printf("loopshell [OPTIONS...]\n"
		   "  -h            Show this help\n"
		   "  -t            success & error time\n"
		   "  --st          success time\n"
		   "  --et          error time\n"
		   "  -l            shell\n");
}

static int parse_argv(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",      no_argument,       NULL,  'h'    },
		{ "st",        required_argument, NULL,  's'    },
		{ "et",        required_argument, NULL,  'e'    },
		{}
	};

	int c, loop;
	while ((c = getopt_long(argc, argv, "t:l:h", options, NULL)) >= 0) {
		loop = 0;
		switch (c) {
		case 'h':
			loopshell_help();
			return 0;
		case 't':
			if ((loop = atoi(optarg)) > 0) {
				if (loop_success == 0) {
					loop_success = loop;
				}
				if (loop_error == 0) {
					loop_error = loop;
				}
			}
			break;
		case 's':
			if ((loop = atoi(optarg)) > 0) {
				loop_success = loop;
			}
			break;
		case 'e':
			if ((loop = atoi(optarg)) > 0) {
				loop_error = loop;
			}
			break;
		case 'l':
			shell = optarg;
			break;
		case '?':
			return 0;
		default:
			;
		}
	}

	if ((loop_success + loop_error) == 0) {
		printf("-t or --st or --et ?\n");
		return 0;
	}

	if (shell == NULL) {
		printf("-l ?\n");
		return 0;
	}

	// if (optind == argc) {
	// 	printf("no string ?\n");
	// 	return 0;
	// }

	return 1;
}

int loopshell_main(int argc, char* argv[])
{
	if (parse_argv(argc, argv) == 0) {
		return 0;
	}

	while(1) {
		pid_t status = system(shell);

		if (status == -1) {
			printf("call system(%s) failed.\n", shell);
			return -1;
		} else {
			if (WIFEXITED(status)) {
				if (WEXITSTATUS(status) == 0) {
					if (loop_success > 0) {
						sleep(loop_success);
					} else {
						return (WEXITSTATUS(status));
					}
				} else {
					if (loop_error > 0) {
						sleep(loop_error);
					} else {
						return (WEXITSTATUS(status));
					}
				}
			} else {
				printf("call system(%s) exit status = [%d]\n", shell, WEXITSTATUS(status));
				return -1;
			}
		}
	}
	return 0;
}

extern int base64_main(int argc, char **argv);
extern int parse_ssr_v2ray_main(int argc, char **argv);

int main(int argc, char* argv[])
{
	if (program_invocation_short_name) {
		if (strstr(program_invocation_short_name, "base64")) {
			return base64_main(argc, argv);
		}

		if (strstr(program_invocation_short_name, "parse-ssr-v2ray")) {
			return parse_ssr_v2ray_main(argc, argv);
		}
	}

	return loopshell_main(argc, argv);
}

