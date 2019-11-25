// SPDX-License-Identifier: GPL-2.0+
/* NetworkManager Applet -- allow user control over networking
 *
 * Dan Williams <dcbw@redhat.com>
 *
 * This applet used the GNOME Wireless Applet as a skeleton to build from.
 *
 * (C) Copyright 2005 Red Hat, Inc.
 */

// 11/24/2019 CLI: modified to allow external hook to popup menu

#include "nm-default.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "applet.h"

gboolean shell_debug = FALSE;
gboolean with_agent = TRUE;
gboolean with_appindicator = FALSE;
static const char *PID_FILE = "/tmp/nm-applet.pid";
static void
usage (const char *progname)
{
	gs_free char *basename = g_path_get_basename (progname);

	fprintf (stdout, "%s %s\n\n%s\n%s\n\n",
	                 _("Usage:"),
	                 basename,
	                 _("This program is a component of NetworkManager (https://wiki.gnome.org/Projects/NetworkManager/)."),
	                 _("It has been modified on 11/24/2019 for command line interaction. --menu to trigger popup menu. --status to trigger popup status. --set-pid [pid] to manually update the pidfile"));
}

// CLI: add external hook for controlling this applet
struct stat st = {0};
static pid_t *pid;

static void
init_pid_file()
{
  if (stat(PID_FILE, &st) == -1) {
    FILE *pfile;
    pfile = fopen(PID_FILE, "w");
    fprintf(pfile, "%d\n", getpid());
    fclose(pfile);
  } else {

    FILE *pfile;
    pfile = fopen(PID_FILE, "r");
    intmax_t tmp;
    fscanf(pfile, "%d\n", &tmp);
    pid_t ptmp = (pid_t)tmp;
    pid = &ptmp;
    fclose(pfile);
  }
}

int main (int argc, char *argv[])
{
	GApplication *applet;
	char *fake_args[1] = { argv[0] };
	guint32 i;
	int status;

  init_pid_file();

	for (i = 1; i < argc; i++) {
		if (!strcmp (argv[i], "--help")) {
			usage (argv[0]);
			exit (0);
		}
		if (!strcmp (argv[i], "--shell-debug"))
			shell_debug = TRUE;
		else if (!strcmp (argv[i], "--no-agent"))
			with_agent = FALSE;
		else if (!strcmp (argv[i], "--indicator")) {
#ifdef WITH_APPINDICATOR
			with_appindicator = TRUE;
#else
			g_error ("Error: --indicator requested but indicator support not available");
#endif
		}
    // CLI: insert cli hook
		if (!strcmp (argv[i], "--menu")) {
      kill(*pid, SIGUSR1);
    }
		else if (!strcmp (argv[i], "--status")) {
      kill(*pid, SIGUSR2);
    }
		else if (!strcmp (argv[i], "--set-pid")) {
      int pid = atoi(argv[++i]);
      FILE *pfile;
      pfile = fopen(PID_FILE, "w");
      fprintf(pfile, "%d\n", pid);
      fclose(pfile);
    }
	}

	bindtextdomain (GETTEXT_PACKAGE, NMALOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	if (!with_appindicator)
		gdk_set_allowed_backends ("x11,*");
	gtk_init (&argc, &argv);
	textdomain (GETTEXT_PACKAGE);

	applet = g_object_new (NM_TYPE_APPLET,
	                       "application-id", "org.freedesktop.network-manager-applet",
	                       NULL);
	status = g_application_run (applet, 1, fake_args);
	g_object_unref (applet);

	return status;
}

