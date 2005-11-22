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
 *	file_putback - some kind of ungetc function
 *
 * chng: jul/2005 written [mjs]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fb.h"
#include "fb_rterr.h"

/*:::::*/
int fb_FilePutBackEx( FB_FILE *handle, const void *src, size_t chars )
{
    int res, bytes;

    if( !FB_HANDLE_USED(handle) )
		return fb_ErrorSetNum( FB_RTERROR_ILLEGALFUNCTIONCALL );

    FB_LOCK();

    res = fb_ErrorSetNum( FB_RTERROR_OK );

    /* UTF? */
    if( handle->encod != FB_FILE_ENCOD_ASCII )
    	bytes = chars * sizeof( FB_WCHAR );
    else
    	bytes = chars;

    if( handle->putback_size + bytes > sizeof(handle->putback_buffer) )
    {
        res = fb_ErrorSetNum( FB_RTERROR_FILEIO );
    }
    else
    {
        /* note: if encoding != ASCII, putback buffer will be in
           wchar format, not in UTF */
        if( handle->putback_size )
        {
            memmove( handle->putback_buffer + bytes,
                     handle->putback_buffer,
                     handle->putback_size );
        }

        if( handle->encod == FB_FILE_ENCOD_ASCII )
        	memcpy( handle->putback_buffer, src, bytes );
        else
        {
    		/* char to wchar */
    		FB_WCHAR *dst = (FB_WCHAR *)handle->putback_buffer;
    		const char *patch = (const char *)src;
        	while( chars-- > 0 )
        		*dst++ = *patch++;
        }

        handle->putback_size += bytes;
    }

	FB_UNLOCK();

	return res;
}

/*:::::*/
FBCALL int fb_FilePutBack( int fnum, const void *data, size_t length)
{
    return fb_FilePutBackEx( FB_FILE_TO_HANDLE(fnum), data, length );
}

