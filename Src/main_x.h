/*
 *  main_x.h - Main program, Unix specific stuff
 *
 *  Frodo (C) 1994-1997,2002-2005 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Version.h"
#include <debug.h>
#include <fat.h>
#include <sys/time.h>
#ifdef HAVE_GLADE
#include <gnome.h>
#endif

// Qtopia Windowing System
#ifdef QTOPIA
extern "C" int main(int argc, char *argv[]);
#include <SDL.h>
#endif
#include <SDL.h>
extern int init_graphics(void);


// Global variables
char Frodo::prefs_path[256] = "";


/*
 *  Create application object and start it
 */
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv)
{
        
	//DEBUG_Init(GDBSTUB_DEVICE_WIFI, 2300);
	//_break();
	
	
	fatInitDefault();

#ifdef HAVE_GLADE
	gnome_program_init(PACKAGE_NAME, PACKAGE_VERSION, LIBGNOMEUI_MODULE, argc, argv,
	                   GNOME_PARAM_APP_DATADIR, DATADIR, NULL);
#endif

#ifndef R14
	timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
#endif

#ifndef HAVE_GLADE
	printf(
		"%s Copyright (C) 1994-1997,2002-2005 Christian Bauer\n"
		"This is free software with ABSOLUTELY NO WARRANTY.\n"
		, VERSION_STRING
	);
#endif
	if (!init_graphics())
		return 1;

	fflush(stdout);
	
	
	Frodo *the_app = new Frodo();
	the_app->ArgvReceived(argc, argv);
	the_app->ReadyToRun();
	delete the_app;

	return 0;
}


/*
 *  Constructor: Initialize member variables
 */

Frodo::Frodo()
{
	TheC64 = NULL;
}


/*
 *  Process command line arguments
 */

void Frodo::ArgvReceived(int argc, char **argv)
{
	if (argc == 2)
		strncpy(prefs_path, argv[1], 255);
}


/*
 *  Arguments processed, run emulation
 */

void Frodo::ReadyToRun(void)
{
	getcwd(AppDirPath, 256);

	// Load preferences
	if (!prefs_path[0]) {
		char *home = getenv("HOME");
		if (home != NULL && strlen(home) < 240) {
			strncpy(prefs_path, home, 200);
			strcat(prefs_path, "/");
		}
		strcat(prefs_path, "Frodo.fpr");
	}
	
	//strncpy(prefs_path, "sd:/APPS/FRODO/Frodo.fpr",200);
	ThePrefs.Load(prefs_path);

	// Show preferences editor
#ifdef HAVE_GLADE
	if (!ThePrefs.ShowEditor(true, prefs_path))
		return;
#endif

	// Create and start C64
	TheC64 = new C64;
	load_rom_files();
	TheC64->Run();
	delete TheC64;
}


Prefs *Frodo::reload_prefs(void)
{
	static Prefs newprefs;
	newprefs.Load(prefs_path);
	return &newprefs;
}


/*
 *  Determine whether path name refers to a directory
 */

bool IsDirectory(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}
