/* 
   Copyright (c) 1991-1999 Thomas T. Wetmore IV

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
/* modified 05 Jan 2000 by Paul B. McBride (pmcbride@tiac.net) */
/*=============================================================
 * main.c -- Main program of LifeLines
 * Copyright(c) 1992-95 by T.T. Wetmore IV; all rights reserved
 * pre-SourceForge version information:
 *   2.3.4 - 24 Jun 93    2.3.5 - 07 Aug 93
 *   2.3.6 - 02 Oct 93    3.0.0 - 11 Oct 94
 *   3.0.1 - 11 Oct 93    3.0.2 - 01 Jan 95
 *   3.0.3 - 02 Jul 96
 *===========================================================*/

#include "llstdlib.h"
/* llstdlib.h pulls in standard.h, config.h, sys_inc.h */
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include "btree.h"
#include "table.h"
#include "translat.h"
#include "gedcom.h"
#include "liflines.h"
#include "arch.h"
#include "lloptions.h"
#include "interp.h"

#include "llinesi.h"
#include "screen.h" /* calling initscr, noecho, ... */


#ifdef HAVE_GETOPT
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* HAVE_GETOPT_H */
#endif /* HAVE_GETOPT */

/*********************************************
 * external variables (no header)
 *********************************************/

extern STRING qSidldir,qSidldrp,qSnodbse,qScrdbse,qSiddbse;
extern STRING qSmtitle,qSnorwandro,qSnofandl,qSbdlkar;
extern STRING qSusgFinnOpt,qSusgFinnAlw,qSusgNorm;
extern STRING qSbaddb;

extern INT csz_indi, icsz_indi;
extern INT csz_fam, icsz_fam;
extern INT csz_sour, icsz_sour;
extern INT csz_even, icsz_even;
extern INT csz_othr, icsz_othr;
extern INT winx, winy;

extern int opterr;
extern BTREE BTR;

/*********************************************
 * required global variables
 *********************************************/


static STRING usage = "";      /* usage string */
int opt_finnish  = FALSE;      /* Finnish Language sorting order if TRUE */
BOOLEAN debugmode = FALSE;     /* no signal handling, so we can get coredump */
BOOLEAN opt_nocb  = FALSE;     /* no cb. data is displayed if TRUE */
BOOLEAN keyflag   = TRUE;      /* show key values */
BOOLEAN readonly  = FALSE;     /* database is read only */
BOOLEAN writeable = FALSE;     /* database must be writeable */
BOOLEAN immutable = FALSE;     /* make no changes at all to database, for access to truly read-only medium */
BOOLEAN cursesio  = TRUE;      /* use curses i/o */
INT alldone       = 0;         /* completion flag */
BOOLEAN progrunning = FALSE;   /* program is running */
BOOLEAN progparsing = FALSE;   /* program is being parsed */
INT     progerror = 0;         /* error count during report program */
BOOLEAN traceprogram = FALSE;  /* trace program */
BOOLEAN traditional = TRUE;    /* use traditional family rules */
BOOLEAN selftest = FALSE;      /* selftest rules (ignore paths) */
BOOLEAN showusage = FALSE;     /* show usage */
STRING  btreepath = NULL;      /* database path given by user */
STRING  readpath = NULL;       /* database path used to open */
STRING  ext_codeset = 0;       /* default codeset from locale */

/*********************************************
 * local function prototypes
 *********************************************/

/* alphabetical */
static BOOLEAN is_unadorned_directory(STRING path);
static void load_usage(void);
static void main_db_notify(STRING db, BOOLEAN opening);
static BOOLEAN open_or_create_database(INT alteration, STRING dbrequested
	, STRING *dbused);
static void platform_init(void);
static void show_open_error(INT dberr);

/*********************************************
 * local function definitions
 * body of module
 *********************************************/

/*==================================
 * main -- Main routine of LifeLines
 *================================*/
int
main (INT argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	char * msg;
	int c;
	BOOLEAN ok=FALSE;
	STRING dbrequested=NULL; /* database (path) requested */
	STRING dbused=NULL; /* database (path) found */
	BOOLEAN forceopen=FALSE, lockchange=FALSE;
	char lockarg = 0; /* option passed for database lock */
	INT alteration=0;
	STRING dbdir = 0;
	BOOLEAN exprog=FALSE;
	STRING exprog_name="";
	STRING progout=NULL;
	BOOLEAN graphical=TRUE;
	STRING configfile=0;

#if HAVE_SETLOCALE
	/* initialize locales */
	setlocale(LC_CTYPE, "");
#endif /* HAVE_SETLOCALE */
	
	/* capture user's default codeset */
	ext_codeset = strsave(ll_langinfo());
	/* TODO: We can use this info for default conversions */

#if ENABLE_NLS
	/* setup gettext translation */
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	save_original_locales();
	load_usage();


	/* Parse Command-Line Arguments */
	opterr = 0;	/* turn off getopt's error message */
	while ((c = getopt(argc, argv, "adkrwil:fmntc:Fu:yx:o:zC:")) != -1) {
		switch (c) {
		case 'c':	/* adjust cache sizes */
			while(optarg && *optarg) {
				if(isalpha((uchar)*optarg) && isupper((uchar)*optarg))
					*optarg = tolower((uchar)*optarg);
				if(*optarg == 'i') {
					sscanf(optarg+1, "%d,%d", &csz_indi, &icsz_indi);
				}
				else if(*optarg == 'f') {
					sscanf(optarg+1, "%d,%d", &csz_fam, &icsz_fam);
				}
				else if(*optarg == 's') {
					sscanf(optarg+1, "%d,%d", &csz_sour, &icsz_sour);
				}
				else if(*optarg == 'e') {
					sscanf(optarg+1, "%d,%d", &csz_even, &icsz_even);
				}
				else if((*optarg == 'o') || (*optarg == 'x')) {
					sscanf(optarg+1, "%d,%d", &csz_othr, &icsz_othr);
				}
				optarg++;
				while(*optarg && isdigit((uchar)*optarg)) optarg++;
				if(*optarg == ',') optarg++;
				while(*optarg && isdigit((uchar)*optarg)) optarg++;
			}
			break;
#ifdef FINNISH
# ifdef FINNISHOPTION
		case 'F':	/* Finnish sorting order [toggle] */
			opt_finnish = !opt_finnish;
			/*
			TO DO - need to mark Finnish databases, as 
			name records are not interoperable, because of
			different soundex encoding
			2001/02/17, Perry Rapp
			*/
			break;
# endif
#endif
		case 'a':	/* debug allocation */
			alloclog = TRUE;
			break;
		case 'd':	/* debug = no signal catchers */
			debugmode = TRUE;
			break;
		case 'k':	/* don't show key values */
			keyflag = FALSE;
			break;
		case 'r':	/* request for read only access */
			readonly = TRUE;
			break;
		case 'w':	/* request for write access */
			writeable = TRUE;
			break;
		case 'i': /* immutable access */
			immutable = TRUE;
			readonly = TRUE;
			break;
		case 'l': /* locking switch */
			lockchange = TRUE;
			lockarg = *optarg;
			break;
		case 'm':
			cursesio = FALSE;
			break;
		case 'f':	/* force database open in all cases */
			forceopen = TRUE;
			break;
		case 'n':	/* use non-traditional family rules */
			traditional = FALSE;
			break;
		case 't': /* show lots of trace statements for debugging */
			traceprogram = TRUE;
			break;
		case 'u': /* specify screen dimensions */
			sscanf(optarg, "%d,%d", &winx, &winy);
			break;
		case 'y': /* only look in current path for databases */
			selftest = TRUE;
			break;
		case 'x': /* execute program */
			exprog = TRUE;
			exprog_name = optarg;
			break;
		case 'o': /* output directory */
			progout = optarg;
			break;
		case 'z': /* nongraphical box */
			graphical = FALSE;
			break;
		case 'C': /* specify config file */
			configfile = optarg;
			break;
		case '?':
			showusage = TRUE;
			goto usage;
			break;
		}
	}
prompt_for_db:

	/* catch any fault, so we can close database */
	if (debugmode)
		stdstring_hardfail();
	else
		set_signals();

	/* Initialize Curses UI */
	if (!initscr())
		goto finish;
	platform_init();
	noecho();
	keypad(0, 1);
	set_displaykeys(keyflag);
	/* initialize options & misc. stuff */
	if (!init_lifelines_global(configfile, &msg, &main_db_notify)) {
		llwprintf("%s", msg);
		goto finish;
	}
	/* setup crashlog in case init_screen fails (eg, bad menu shortcuts) */
	crash_setcrashlog(getoptstr("CrashLog", NULL));
	/* initialize curses interface */
	if (!init_screen(graphical))
		goto finish;
	if (selftest) {
		/* need to always find test stuff locally */
		changeoptstr("LLPROGRAMS", strsave("."));
		changeoptstr("LLREPORTS", strsave("."));
		changeoptstr("LLDATABASES", strsave("."));
		changeoptstr("LLNEWDBDIR", strsave("."));
	}
	init_interpreter(); /* give interpreter its turn at initialization */

	/* Validate Command-Line Arguments */
	if ((readonly || immutable) && writeable) {
		llwprintf(_(qSnorwandro));
		goto finish;
	}
	if (forceopen && lockchange) {
		llwprintf(_(qSnofandl));
		goto finish;
	}
	if (lockchange && lockarg != 'y' && lockarg != 'n') {
		llwprintf(_(qSbdlkar));
		goto finish;
	}
	if (forceopen)
		alteration = 3;
	else if (lockchange) {
		if (lockarg == 'y')
			alteration = 2;
		else
			alteration = 1;
	}
	c = argc - optind;
	if (c > 1) {
		showusage = TRUE;
		goto usage;
	}

	dbdir = getoptstr("LLDATABASES", ".");
	/* Get Database Name (Prompt or Command-Line) */
	if (alldone || c <= 0) {
		/* ask_for_db_filename returns static buffer, we save it below */
		dbrequested = ask_for_db_filename(_(qSidldir), _(qSidldrp), dbdir);
		if (!dbrequested || !dbrequested[0]) {
			dbrequested = NULL;
			llwprintf(_(qSiddbse));
			goto finish;
		}
		dbrequested = strsave(dbrequested);
		if (eqstr(dbrequested, "?")) {
			INT n=0;
			LIST dblist=0, dbdesclist=0;
			strfree(&dbrequested);
			if ((n=get_dblist(dbdir, &dblist, &dbdesclist)) > 0) {
				INT i;
				i = choose_from_list(
					_("Choose database to open")
					, dbdesclist);
				if (i >= 0) {
					dbrequested = strsave(get_list_element(dblist, i+1));
				}
				release_dblist(dblist);
				release_dblist(dbdesclist);
			} else {
				llwprintf(_("No databases found in database path"));
				goto finish;
			}
			if (!dbrequested) {
				llwprintf(_(qSiddbse));
				goto finish;
			}
		}
	} else {
		dbrequested = strsave(argv[optind]);
	}

	/* search for database */
	/* search for file in lifelines path */
	dbused = filepath(dbrequested, "r", dbdir, NULL);
	if (!dbused) dbused = dbrequested;
	dbused = strsave(dbused);

	if (!open_or_create_database(alteration, dbrequested, &dbused))
		goto finish;

	/* Start Program */
	if (!init_lifelines_db()) {
		llwprintf(_(qSbaddb));
		goto finish;
	}
	init_show_module();
	init_browse_module();
	if (exprog) {
		if (progout) {
			BOOLEAN append=FALSE;
			set_output_file(progout, append);
		}
		interp_program("main", 0, NULL, 1, &exprog_name, NULL, FALSE);
	} else {
		alldone = 0;
		while (!alldone)
			main_menu();
	}
	term_show_module();
	term_browse_module();
	ok=TRUE;

finish:
	/* we free this not because we care so much about these tiny amounts
	of memory, but to ensure we have the memory management right */
	/* strfree frees memory & nulls pointer */
	if (dbused) strfree(&dbused);
	if (dbrequested) strfree(&dbrequested);
	if (btreepath) strfree(&btreepath);
	shutdown_interpreter();
	close_lifelines();
	shutdown_ui(!ok);
	if (alldone == 2)
		goto prompt_for_db; /* changing databases */
	termlocale();

usage:
	/* Display Command-Line Usage Help */
	if (showusage) puts(usage);

	/* Exit */
	return !ok;
}
/*===================================================
 * shutdown_ui -- Do whatever is necessary to close GUI
 * Created: 2001/11/08, Perry Rapp
 *=================================================*/
void
shutdown_ui (BOOLEAN pause)
{
	term_screen();
	if (pause) /* if error, give user a second to read it */
		sleep(1);
	/* TO DO - signals also calls into here -- how do we figure out
	whether or not we should call endwin ? In case something happened
	before curses was invoked, or after it already closed ? */
	/* Terminate Curses UI */
	endwin();
}
/*===================================================
 * show_open_error -- Display database opening error
 *=================================================*/
static void
show_open_error (INT dberr)
{
	char buffer[256];
	describe_dberror(dberr, buffer, ARRSIZE(buffer));
	llwprintf(buffer);
	sleep(5);
}
/*==================================================
 * platform_init -- platform specific initialization
 *================================================*/
static void
platform_init (void)
{
#ifdef WIN32
	char buffer[80];
	STRING title = _(qSmtitle);
	sprintf(buffer, title, get_lifelines_version(sizeof(buffer)-1-strlen(title)));
	wtitle(buffer);
#endif
}
/*==================================================
 * is_unadorned_directory -- is it a bare directory name,
 *  with no subdirectories ?
 * Created: 2001/01/24, Perry Rapp
 *================================================*/
static BOOLEAN
is_unadorned_directory (STRING path)
{
	for ( ; *path; path++) {
		if (is_dir_sep(*path))
			return FALSE;
	}
	return TRUE;
}
/*==================================================
 * open_or_create_database -- open database, prompt for
 *  creating new one if it doesn't exist
 * if fails, displays error (show_open_error) and returns 
 *  FALSE
 *  alteration:   [IN]  flags for locking, forcing open...
 *  dbrequested:  [IN]  database specified by user (usually relative)
 *  dbused:       [I/O] actual database path (may be relative)
 * If this routine creates new database, it will alter dbused
 * Created: 2001/04/29, Perry Rapp
 *================================================*/
static BOOLEAN
open_or_create_database (INT alteration, STRING dbrequested, STRING *dbused)
{
	/* Open Database */
	if (open_database(alteration, dbrequested, *dbused)) {
		return TRUE;
	}
	/* filter out real errors */
	if (bterrno != BTERR_NODB && bterrno != BTERR_NOKEY)
	{
		show_open_error(bterrno);
		return FALSE;
	}
	if (readonly || immutable || alteration)
	{
		llwprintf("Cannot create new database with -r, -i, -l, or -f flags.");
		return FALSE;
	}
	/*
	error was only that db doesn't exist, so lets try
	making a new one 
	If no database directory specified, add prefix llnewdbdir
	*/
	if (!selftest && is_unadorned_directory(*dbused)) {
		STRING newdbdir = getoptstr("LLNEWDBDIR", ".");
		STRING temp = *dbused;
		*dbused = strsave(concat_path(newdbdir, *dbused));
		stdfree(temp);
	}

	/* Is user willing to make a new db ? */
	if (!ask_yes_or_no_msg(_(qSnodbse), _(qScrdbse))) 
		return FALSE;

	/* try to make a new db */
	if (create_database(dbrequested, *dbused))
		return TRUE;

	show_open_error(bterrno);
	return FALSE;
}

/* Finnish language support modifies the soundex codes for names, so
 * a database created with this support is not compatible with other
 * databases. 
 *
 * define FINNISH for Finnish Language support
 *
 * define FINNISHOPTION to have a runtime option -F which will enable
 * 	  	Finnish language support, but the name indices will all be
 *      wrong if you make modifications whilst in the wrong mode.
 */

static void
load_usage (void)
{
#ifdef FINNISH
# ifdef FINNISHOPTION
	opt_finnish  = FALSE;/* Finnish Language sorting order if TRUE */
	usage = _(qSusgFinnOpt);
# else
	opt_finnish  = TRUE;/* Finnish Language sorting order if TRUE */
	usage = _(qSusgFinnAlw);
# endif
#else
	opt_finnish  = FALSE;/* Finnish Language sorting order id disabled*/
	usage = _(qSusgNorm);
#endif
}
/*==================================================
 * main_db_notify -- callback called whenever a db is
 *  opened or closed
 * Created: 2002/06/16, Perry Rapp
 *================================================*/
static void
main_db_notify (STRING db, BOOLEAN opening)
{
	if (opening)
		crash_setdb(db);
	else
		crash_setdb("");
}

