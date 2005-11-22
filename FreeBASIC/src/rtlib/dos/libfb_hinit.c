/*
 *  libfb - FreeBASIC's runtime library
 *	Copyright (C) 2004-2005 Andre V. T. Vicentini (av1ctor@yahoo.com.br) and others.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * init.c -- libfb initialization for DOS
 *
 * chng: jan/2005 written [DrV]
 *
 */


#include "fb.h"

#include <float.h>
#include <conio.h>
#include <unistd.h>
#include <sys/farptr.h>

/* globals */
int ScrollWasOff = FALSE;
FB_DOS_TXTMODE fb_dos_txtmode;
int fb_force_input_buffer_changed = FALSE;


/*:::::*/
void fb_hInit ( int argc, char **argv )
{
	/* set FPU precision to 64-bit and round to nearest (as in QB) */
	_control87(PC_64|RC_NEAR, MCW_PC|MCW_RC);

	/* turn off blink */
    intensevideo();
}
