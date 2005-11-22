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
 * file_input_tok - input function core
 *
 * chng: nov/2004 written [v1ctor]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "fb.h"
#include "fb_rterr.h"

/*:::::*/
static int hReadChar( FB_INPUTCTX *ctx )
{
    /* device? */
    if( FB_HANDLE_USED(ctx->handle) )
    {
        int res;
        int c;

        size_t len = 1;
        res = fb_FileGetDataEx( ctx->handle, 0, &c, &len, FALSE, FALSE );
        if( (res != FB_RTERROR_OK) || (len == 0) )
            return EOF;

        return c & 0x000000FF;
    }
    /* console.. */
    else
    {
		if( ctx->index >= FB_STRSIZE( &ctx->str.len ) )
			return EOF;
		else
			return ctx->str.data[ctx->index++];
	}

}

/*:::::*/
static int hUnreadChar( FB_INPUTCTX *ctx, int c )
{
    /* device? */
    if( FB_HANDLE_USED(ctx->handle) )
    {
        return fb_FilePutBackEx( ctx->handle, &c, 1 );
    }
    /* console .. */
    else
    {
		if( ctx->index <= 0 )
			return FALSE;
		else
		{
			--ctx->index;
			return TRUE;
		}
	}

}

/*:::::*/
static int hSkipWhiteSpc( FB_INPUTCTX *ctx )
{
	int c;

	/* skip white space */
	do
	{
		c = hReadChar( ctx );
		if( c == EOF )
			break;
	} while( (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') );

	return c;
}

/*:::::*/
static void hSkipComma( FB_INPUTCTX *ctx, int c )
{
	/* skip white space */
	while( (c == ' ') || (c == '\t') )
		c = hReadChar( ctx );

	switch( c )
	{
	case ',':
	case EOF:
		break;

    case '\n':
        break;

	case '\r':
		if( (c = hReadChar( ctx )) != '\n' )
			hUnreadChar( ctx, c );
		break;

	default:
    	hUnreadChar( ctx, c );
        break;
	}
}

/*:::::*/
void fb_hGetNextToken( char *buffer, int max_chars, int is_string )
{
    int c, len, isquote, skipcomma;
	FB_INPUTCTX *ctx = FB_TLSGETCTX( INPUT );

	c = hSkipWhiteSpc( ctx );

	/* */
	isquote = 0;
	len = 0;
	skipcomma = 0;

	while( c != EOF )
	{
		switch( c )
		{
		case '\n':
			len = max_chars;						/* exit */
			break;

		case '\r':
			if( (c = hReadChar( ctx )) != '\n' )
				hUnreadChar( ctx, c );

			len = max_chars;						/* exit */
			break;

		case '"':
			if( !isquote )
			{
				if( len == 0 )
					isquote = 1;
				else
					goto savechar;
			}
			else
			{
				isquote = 0;
				if( is_string )
				{
					c = hReadChar( ctx );
					skipcomma = 1;
					len = max_chars;				/* exit */
				}
			}

			break;

		case ',':
			if( !isquote )
			{
				len = max_chars;					/* exit */
				break;
			}

			goto savechar;

		case '\t':
		case ' ':
			if( len == 0 )
			{
				if( !is_string || !isquote )
					break;						/* skip white-space */
			}
			else if( !is_string && !isquote )
			{
				len = max_chars;					/* exit */
				break;
			}

		default:
savechar:
			*buffer++ = c;
            ++len;
            break;
		}

		if( len >= max_chars )
			break;

		c = hReadChar( ctx );
	}

	/* add the null-term */
	*buffer = '\0';

	/* skip comma or newline */
	if( skipcomma )
		hSkipComma( ctx, c );
}

