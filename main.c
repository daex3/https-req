#include <https-req.c>
#include <fcntl.h>

void options(
	int *   argc,
	char ***argv,
	size_t *max,
	char ** save_to
) {
	char c;
	while ((c = getopt(*argc, *argv, "m:o:")) != -1) {
		switch (c) {
			case 'm':
				*max = atoi(optarg);
				
				break;
			case 'o':
				*save_to = optarg;
				
				break;
		}
	}
	*argc -= optind;
	*argv += optind;
}

void usage() {
	printf(
		"\x1b[36mUsage:\x1b[m "
		"https-req "
		"[-m allocate_bytes_for_download] "
		"[-o path] "
		"[domains] "
		"\n"
	);
	exit(-1);
}

// Downloads an https website
// TODO: Separate domain+path from url
int main(int argc, char **argv) {
	size_t max;
	char *save_to = NULL;

	options(&argc, &argv, &max, &save_to);

	if (argc < 1) usage();

	while (*argv) {
		printf("%s\n", *argv);

		char *source = simply_download(*argv, max);
		
		if (save_to) {
			FILE *f = fopen(save_to, "a");
			fprintf(f, "%s", source);
			fclose(f);
		} else printf("%s\n", source);

		free(source);
		++argv;
	}
}
