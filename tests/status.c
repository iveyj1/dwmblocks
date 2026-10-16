#define NO_X
#define main dwmblocks_main
#include "../dwmblocks.c"
#undef main
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>

static void
testinterruptedcommand(const char *command, const char *expected)
{
	Block block = {"", (char *)command, 0, 0};
	char out[CMDLENGTH] = "previous value";
	pid_t parent = getpid(), sender;
	int status;

	pending = 0;
	sender = fork();
	assert(sender >= 0);
	if (sender == 0) {
		struct timespec delay = {0, 10000000};
		for (int i = 0; i < 25; i++) {
			nanosleep(&delay, NULL);
			kill(parent, signalbase + 10);
		}
		_exit(0);
	}
	getcmd(&block, out);
	while (waitpid(sender, &status, 0) < 0)
		assert(errno == EINTR);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(pending & (1 << 10));
	if (strcmp(out, expected))
		fprintf(stderr, "interrupted command: expected '%s', got '%s'\n", expected, out);
	assert(!strcmp(out, expected));
}

static void reset(void)
{
	memset(statusbar, 0, sizeof statusbar);
	memset(statusstr, 0, sizeof statusstr);
	separator = " | ";
}

int main(void)
{
	char out[CMDLENGTH];
	Block empty = {"Icon ", "printf '\\n'", 0, 0};
	Block null = {NULL, "printf 'hello\\nignored\\n'", 0, 0};
	Block longtext = {"", "awk 'BEGIN {for(i=0;i<10000;i++) printf \"x\"}'", 0, 0};
	Block utf8 = {"", "awk 'BEGIN {for(i=0;i<200;i++) printf \"界\"}'", 0, 0};
	char icon[1024];
	memset(icon, 'i', sizeof icon - 1);
	icon[sizeof icon - 1] = '\0';
	Block longicon = {icon, "printf ok", 0, 0};

	reset();
	assert(getstatus(statusstr[0], statusstr[1]) == 0);
	getcmd(&empty, out);
	assert(!out[0]);
	getcmd(&null, out);
	assert(!strcmp(out, "hello"));
	getcmd(&longtext, out);
	assert(strlen(out) == CMDLENGTH - 1);
	getcmd(&longicon, out);
	assert(strlen(out) == CMDLENGTH - 1);
	getcmd(&utf8, out);
	assert(strlen(out) == 255);
	assert(!strcmp(out + 252, "界"));
	assert(textprefix("a界b", 3) == 1);
	assert(textprefix("a界b", 4) == 4);

	strcpy(statusbar[0], "one");
	strcpy(statusbar[2], "two");
	assert(getstatus(statusstr[0], statusstr[1]));
	assert(!strcmp(statusstr[0], "one | two"));
	assert(!getstatus(statusstr[0], statusstr[1]));
	separator = "";
	getstatus(statusstr[0], statusstr[1]);
	assert(!strcmp(statusstr[0], "onetwo"));
	separator = icon; /* Longer than the entire status buffer. */
	getstatus(statusstr[0], statusstr[1]);
	assert(!strcmp(statusstr[0], "one"));

	reset();
	memset(statusbar[0], 'x', 250);
	strcpy(statusbar[1], "界");
	getstatus(statusstr[0], statusstr[1]);
	assert(strlen(statusstr[0]) == 250); /* No dangling separator. */
	separator = "";
	getstatus(statusstr[0], statusstr[1]);
	assert(strlen(statusstr[0]) == 253);

	reset();
	for (size_t i = 0; i < LENGTH(blocks); i++)
		memset(statusbar[i], 'x', CMDLENGTH - 1);
	getstatus(statusstr[0], statusstr[1]);
	assert(strlen(statusstr[0]) == STATUSLENGTH - 1);
	char *missing[] = {"dwmblocks", "-d", NULL};
	assert(dwmblocks_main(2, missing) == 1);
	char *unknown[] = {"dwmblocks", "--unknown", NULL};
	assert(dwmblocks_main(2, unknown) == 1);
	/* Handlers only coalesce requests; no commands or X calls occur here. */
	signalbase = SIGPLUS;
	pending = 0;
	sighandler(signalbase + 10);
	sighandler(signalbase + 10);
	sighandler(signalbase + 2);
	assert(takepending() == ((1 << 10) | (1 << 2)));
	assert(takepending() == 0);
	sighandler(signalbase);
	assert(takepending() == 0);
	setupsignals();
	/* Test both the normal restartable handler and explicit EINTR recovery. */
	for (int restart = 1; restart >= 0; restart--) {
		struct sigaction sa;
		assert(sigaction(signalbase + 10, NULL, &sa) == 0);
		if (restart)
			assert(sa.sa_flags & SA_RESTART);
		else {
			sa.sa_flags &= ~SA_RESTART;
			assert(sigaction(signalbase + 10, &sa, NULL) == 0);
		}
		testinterruptedcommand("sleep 0.15; printf 'CPU 12%%\\n'", "CPU 12%");
		testinterruptedcommand("printf 'Power '; sleep 0.15; printf '8W\\n'", "Power 8W");
		testinterruptedcommand("printf 'CPU 12%%\\n'; sleep 0.15; printf 'unused\\n'", "CPU 12%");
		testinterruptedcommand("sleep 0.15; printf '\\n'", "");
	}
	termhandler();
	assert(!statusContinue);
	puts("status tests passed");
	return 0;
}
