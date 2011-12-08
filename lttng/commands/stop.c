/*
 * Copyright (C) 2011 - David Goulet <david.goulet@polymtl.ca>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; only version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define _GNU_SOURCE
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../cmd.h"
#include "../conf.h"
#include "../utils.h"

static char *opt_session_name;

enum {
	OPT_HELP = 1,
};

static struct poptOption long_options[] = {
	/* longName, shortName, argInfo, argPtr, value, descrip, argDesc */
	{"help",      'h', POPT_ARG_NONE, 0, OPT_HELP, 0, 0},
	{0, 0, 0, 0, 0, 0, 0}
};

/*
 * usage
 */
static void usage(FILE *ofp)
{
	fprintf(ofp, "usage: lttng stop [options] [NAME]\n");
	fprintf(ofp, "\n");
	fprintf(ofp, "Where NAME is an optional session name. If not specified, lttng will\n");
	fprintf(ofp, "get it from the configuration directory (.lttng).\n");
	fprintf(ofp, "\n");
	fprintf(ofp, "  -h, --help               Show this help\n");
	fprintf(ofp, "\n");
}

/*
 * Start tracing for all trace of the session.
 */
static int stop_tracing(void)
{
	int ret = CMD_SUCCESS;
	char *session_name;

	if (opt_session_name == NULL) {
		session_name = get_session_name();
		if (session_name == NULL) {
			ret = CMD_ERROR;
			goto error;
		}
	} else {
		session_name = opt_session_name;
	}

	ret = lttng_stop_tracing(session_name);
	if (ret < 0) {
		goto free_name;
	}

	MSG("Tracing stopped for session %s", session_name);

free_name:
	if (opt_session_name == NULL) {
		free(session_name);
	}

error:
	return ret;
}

/*
 *  cmd_stop
 *
 *  The 'stop <options>' first level command
 */
int cmd_stop(int argc, const char **argv)
{
	int opt, ret;
	static poptContext pc;

	pc = poptGetContext(NULL, argc, argv, long_options, 0);
	poptReadDefaultConfig(pc, 0);

	while ((opt = poptGetNextOpt(pc)) != -1) {
		switch (opt) {
		case OPT_HELP:
			usage(stderr);
			ret = CMD_SUCCESS;
			goto end;
		default:
			usage(stderr);
			ret = CMD_UNDEFINED;
			goto end;
		}
	}

	opt_session_name = (char*) poptGetArg(pc);

	ret = stop_tracing();

end:
	return ret;
}
