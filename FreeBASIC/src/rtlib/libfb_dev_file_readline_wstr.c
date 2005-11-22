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
 * dev_file_readling_wstr - wstring LINE INPUT for file devices
 *
 * chng: nov/2005 written [v1ctor]
 *
 */

#include "fb.h"
#include "fb_rterr.h"

/*:::::*/
int fb_DevFileReadLineWstr( struct _FB_FILE *handle, FB_WCHAR *dst, int dst_chars )
{
    int res;
    FILE *fp;
    FBSTRING temp = { 0 };

	FB_LOCK();

    fp = (FILE *)handle->opaque;
    if( fp == stdout || fp == stderr )
        fp = stdin;

	if( fp == NULL )
	{
		FB_UNLOCK();
		return fb_ErrorSetNum( FB_RTERROR_ILLEGALFUNCTIONCALL );
	}

    res = fb_DevFileReadLineDumb( fp, &temp, NULL );

	/* convert to wchar, file should be opened with the ENCODING option
	   to allow UTF characters to be read */
	if( res == FB_RTERROR_OK )
    	fb_WstrAssignFromA( dst, dst_chars, (void *)&temp, -1 );

    fb_StrDelete( &temp );

	FB_UNLOCK();

	return res;
}
