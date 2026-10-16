#define _POSIX_C_SOURCE 200809L
#include<stdlib.h>
#include<limits.h>
#include<time.h>
#include<sys/select.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#ifndef NO_X
#include<X11/Xlib.h>
#endif
#ifdef __OpenBSD__
#define SIGPLUS			SIGUSR1+1
#define SIGMINUS		SIGUSR1-1
#else
#define SIGPLUS			SIGRTMIN
#define SIGMINUS		SIGRTMIN
#endif
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		256
/* Match dwm's stext buffer, including the terminating NUL. */
#define STATUSLENGTH 256

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;
#ifndef __OpenBSD__
void dummysighandler(int num);
#endif
void sighandler(int num);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();
#ifndef NO_X
void setroot();
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif


#include "blocks.h"

static char statusbar[LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[2][STATUSLENGTH];
static volatile sig_atomic_t statusContinue = 1;
static volatile sig_atomic_t pending;
static int signalbase;
static sigset_t handledsignals;
static const char *separator = delim;
// static int returnStatus = 0;

/* Length of a bounded prefix, without splitting valid UTF-8 input. */
static size_t textprefix(const char *text, size_t available)
{
	size_t n = strlen(text);
	if (n <= available)
		return n;
	n = available;
	while (n && ((unsigned char)text[n] & 0xc0) == 0x80)
		n--;
	return n;
}

static void appendtext(char *dest, size_t size, const char *text)
{
	size_t used = strlen(dest);
	size_t n = textprefix(text, size - used - 1);
	memcpy(dest + used, text, n);
	dest[used + n] = '\0';
}

/* Store text only; separators are inserted between nonempty blocks later. */
void getcmd(const Block *block, char *output)
{
	char text[CMDLENGTH + 1];
	size_t n = 0;
	int c;
	FILE *cmdf = popen(block->command, "r");
	if (!cmdf) {
		output[0] = '\0';
		return;
	}
	while ((c = fgetc(cmdf)) != EOF && c != '\n')
		if (n < sizeof(text) - 1)
			text[n++] = c;
	text[n] = '\0';
	/* Drain unused output so pclose cannot deadlock on a full pipe. */
	while (fgetc(cmdf) != EOF)
		;
	pclose(cmdf);
	output[0] = '\0';
	if (!text[0])
		return;
	if (block->icon)
		appendtext(output, CMDLENGTH, block->icon);
	appendtext(output, CMDLENGTH, text);
}

void getcmds(int time)
{
	const Block* current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current,statusbar[i]);
	}
}

void getsigcmds(unsigned int signal)
{
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current,statusbar[i]);
	}
}

void
setupsignals(void)
{
	struct sigaction sa;
	unsigned int i;

	signalbase = SIGPLUS;
	sigemptyset(&handledsignals);
	sigaddset(&handledsignals, SIGTERM);
	sigaddset(&handledsignals, SIGINT);
	sigfillset(&sa.sa_mask);
	sa.sa_flags = 0;
#ifndef __OpenBSD__
	sa.sa_handler = dummysighandler;
	for (int sig = SIGRTMIN; sig <= SIGRTMAX; sig++)
		sigaction(sig, &sa, NULL);
#endif
	sa.sa_handler = sighandler;
	for (i = 0; i < LENGTH(blocks); i++) {
		unsigned int n = blocks[i].signal;
		if (n > 0 && n < sizeof(sig_atomic_t) * CHAR_BIT - 1) {
			sigaddset(&handledsignals, signalbase + n);
			sigaction(signalbase + n, &sa, NULL);
		}
	}
}

int getstatus(char *str, char *last)
{
	snprintf(last, STATUSLENGTH, "%s", str);
	str[0] = '\0';
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		size_t used = strlen(str);
		size_t sep = used ? strlen(separator) : 0;
		size_t available = STATUSLENGTH - used - 1;
		if (!statusbar[i][0])
			continue;
		/* Never leave a dangling or partial separator. */
		if (sep >= available || !textprefix(statusbar[i], available - sep))
			break;
		if (used)
			appendtext(str, STATUSLENGTH, separator);
		appendtext(str, STATUSLENGTH, statusbar[i]);
	}
	return strcmp(str, last);//0 if they are the same
}

#ifndef NO_X
void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1]))//Only set root if text has changed.
		return;
	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX()
{
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1]))//Only write out if text has changed.
		return;
	printf("%s\n",statusstr[0]);
	fflush(stdout);
}


/* Caller blocks handledsignals while consuming the coalesced requests. */
static sig_atomic_t
takepending(void)
{
	sig_atomic_t result = pending;
	pending = 0;
	return result;
}

void
statusloop(void)
{
	struct timespec now, next, delay;
	sigset_t oldmask;
	unsigned int tick = 0;
	sig_atomic_t requests;

	setupsignals();
	clock_gettime(CLOCK_MONOTONIC, &next);
	next.tv_sec++;
	getcmds(-1);
	sigprocmask(SIG_BLOCK, &handledsignals, &oldmask);
	while (statusContinue) {
		requests = takepending();
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		for (unsigned int n = 1; n < sizeof(sig_atomic_t) * CHAR_BIT - 1; n++)
			if (requests & ((sig_atomic_t)1 << n))
				getsigcmds(n);
		clock_gettime(CLOCK_MONOTONIC, &now);
		while (now.tv_sec > next.tv_sec ||
		    (now.tv_sec == next.tv_sec && now.tv_nsec >= next.tv_nsec)) {
			getcmds(++tick);
			next.tv_sec++;
		}
		writestatus();
		sigprocmask(SIG_BLOCK, &handledsignals, NULL);
		if (!statusContinue)
			break;
		if (pending)
			continue;
		clock_gettime(CLOCK_MONOTONIC, &now);
		delay.tv_sec = next.tv_sec - now.tv_sec;
		delay.tv_nsec = next.tv_nsec - now.tv_nsec;
		if (delay.tv_nsec < 0) {
			delay.tv_sec--;
			delay.tv_nsec += 1000000000L;
		}
		if (delay.tv_sec < 0)
			delay = (struct timespec){0, 0};
		/* Atomic unblock-and-wait avoids a lost signal before sleeping. */
		pselect(0, NULL, NULL, NULL, &delay, &oldmask);
	}
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void
dummysighandler(int signum)
{
	(void)signum;
}
#endif

void
sighandler(int signum)
{
	int n = signum - signalbase;

	if (n > 0 && (unsigned int)n < sizeof(sig_atomic_t) * CHAR_BIT - 1)
		pending |= (sig_atomic_t)1 << n;
}

void termhandler()
{
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp("-d", argv[i]) && i + 1 < argc)
			separator = argv[++i];
		else if (!strcmp("-p", argv[i]))
			writestatus = pstdout;
		else {
			fprintf(stderr, "usage: dwmblocks [-p] [-d delimiter]\n");
			return 1;
		}
	}
#ifndef NO_X
	if (writestatus == setroot && !setupX())
		return 1;
#endif
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
#ifndef NO_X
	if (dpy)
		XCloseDisplay(dpy);
#endif
	return 0;
}
