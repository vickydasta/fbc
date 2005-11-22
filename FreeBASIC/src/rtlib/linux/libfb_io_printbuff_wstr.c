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
 * io_printbuff_wstr.c -- low-level print to console function for wstrings
 *
 * chng: nov/2005 written [v1ctor]
 *
 */

#include "fb.h"
#include "fb_linux.h"

#define ENTER_UTF8	"\e%G"
#define EXIT_UTF8  	"\e%@"

/*:::::*/
void fb_ConsolePrintBufferWstrEx( const FB_WCHAR *buffer, size_t chars, int mask )
{
	size_t avail, avail_len;
	char *temp;

    if( !fb_con.inited )
    {
        /* !!!FIXME!!! is this ok or should it be converted to UTF-8 too? */
        fwrite( buffer, sizeof( FB_WCHAR ), len, stdout );
        fflush( stdout );
		return;
	}

	fb_hResize();

	temp = alloca( chars * 4 + 1 );

	/* ToDo: handle scrolling for internal characters/attributes buffer? */
    avail = (fb_con.w * fb_con.h) - (((fb_con.cur_y - 1) * fb_con.w) + fb_con.cur_x - 1);
    avail_len = chars;
	if (avail < avail_len)
		avail_len = avail;

	/* !!!FIXME!!! to support unicode the char_buffer would have to be a wchar_t,
				   slowing down non-unicode printing.. */
	fb_wstr_ConvToA( temp, buffer, avail_len );

	memcpy( fb_con.char_buffer + ((fb_con.cur_y - 1) * fb_con.w) + fb_con.cur_x - 1,
		    temp,
		    avail_len );

	memset( fb_con.attr_buffer + ((fb_con.cur_y - 1) * fb_con.w) + fb_con.cur_x - 1,
			fb_con.fg_color | (fb_con.bg_color << 4),
	        avail_len );

	/* convert wchar_t to UTF-8 */
	int bytes;

    fb_WCharToUTF( FB_FILE_ENCOD_UTF8, buffer, chars, temp, &bytes );
    /* add null-term */
    temp[bytes] = '\0';

	fputs( ENTER_UTF8, fb_con.f_out );

    fputs( temp, fb_con.f_out );

	fputs( EXIT_UTF8, fb_con.f_out );

	/* update x and y coordinates.. */
	for( ; len; len--, buffer++ )
	{
		++fb_con.cur_x;
		if( (*buffer == _LC('\n')) || (fb_con.cur_x >= fb_con.w) )
		{
			fb_con.cur_x = 1;
			++fb_con.cur_y;
			if( fb_con.cur_y > fb_con.h )
				fb_con.cur_y = fb_con.h;
		}
	}

	fflush( fb_con.f_out );
}

/*:::::*/
void fb_ConsolePrintBufferWstr( const FB_WCHAR *buffer, int mask )
{
    return fb_ConsolePrintBufferWstrEx( buffer, fb_wstr_Len( buffer ), mask );
}

