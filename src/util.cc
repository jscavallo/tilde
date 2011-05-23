/* Copyright (C) 2011 G.P. Halkes
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3, as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <sys/time.h>
#include <sys/resource.h>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <string>

#include <widget.h>
#include "option.h"

using namespace std;
using namespace t3_widget;

static char debug_buffer[1024];
static const char *executable;

static void start_debugger_on_segfault(int sig) {
	struct rlimit vm_limit;
	(void) sig;

	fprintf(stderr, "Handling signal\n");

	signal(SIGSEGV, SIG_DFL);
	signal(SIGABRT, SIG_DFL);

	getrlimit(RLIMIT_AS, &vm_limit);
	vm_limit.rlim_cur = vm_limit.rlim_max;
	setrlimit(RLIMIT_AS, &vm_limit);
	sprintf(debug_buffer, "DISPLAY=:0.0 ddd %s %d", executable, getpid());
	system(debug_buffer);
}

void enable_debugger_on_segfault(const char *_executable) {
	executable = strdup(_executable);
	signal(SIGSEGV, start_debugger_on_segfault);
	signal(SIGABRT, start_debugger_on_segfault);
}

#ifdef DEBUG
void set_limits() {
	int mb = option.vm_limit == 0 ? 250 : option.vm_limit;
	struct rlimit vm_limit;
	printf("Debug mode: Setting VM limit\n");
	getrlimit(RLIMIT_AS, &vm_limit);
	vm_limit.rlim_cur = mb * 1024 * 1024;
	setrlimit(RLIMIT_AS, &vm_limit);
}
#endif

/** Alert the user of a fatal error and quit.
    @param fmt The format string for the message. See fprintf(3) for details.
    @param ... The arguments for printing.
*/
void fatal(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

char *resolve_links(const char *start_name) {
	long buffer_max = pathconf("/", _PC_PATH_MAX);

	if (buffer_max < PATH_MAX)
		buffer_max = PATH_MAX;

	char buffer[buffer_max + 1];
	ssize_t retval;

	while ((retval = readlink(start_name, buffer, buffer_max)) > 0) {
		if (retval == buffer_max)
			return NULL;
		buffer[retval] = 0;
		start_name = buffer;
	}
	return strdup(start_name);
}

char *canonicalize_path(const char *path) {
	string result;

	if (path[0] != '/') {
		result = get_working_directory();
		result += "/";
	}

	result += path;

	/* TODO:
		- remove all occurences of // and /./
	    - remove all occurences of ^/..
		- remove all occurences of XXX/../
	*/
	return strdup(result.c_str());
}

#define BUFFER_START_SIZE 256
#define BUFFER_MAX 4096
void printf_into(string *message, const char *format, ...) {
	static char *message_buffer = NULL;
	static int message_buffer_size;

	char *new_message_buffer;
	int result;
	va_list args;

	message->clear();
	va_start(args, format);

	if (message_buffer == NULL) {
		if ((message_buffer = (char *) malloc(BUFFER_START_SIZE)) == NULL) {
			va_end(args);
			return;
		}
		message_buffer_size = BUFFER_START_SIZE;
	}

	result = vsnprintf(message_buffer, message_buffer_size, format, args);
	if (result < 0) {
		va_end(args);
		return;
	}

	result = min(BUFFER_MAX - 1, result);
	if (result < message_buffer_size || (new_message_buffer = (char *) realloc(message_buffer, result + 1)) == NULL) {
		*message = message_buffer;
		va_end(args);
		return;
	}

	message_buffer = new_message_buffer;
	message_buffer_size = result + 1;
	result = vsnprintf(message_buffer, message_buffer_size, format, args);
	va_end(args);
	if (result < 0)
		return;

	*message = message_buffer;
	return;
}
