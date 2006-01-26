''	FreeBASIC - 32-bit BASIC Compiler.
''	Copyright (C) 2004-2005 Andre Victor T. Vicentini (av1ctor@yahoo.com.br)
''
''	This program is free software; you can redistribute it and/or modify
''	it under the terms of the GNU General Public License as published by
''	the Free Software Foundation; either version 2 of the License, or
''	(at your option) any later version.
''
''	This program is distributed in the hope that it will be useful,
''	but WITHOUT ANY WARRANTY; without even the implied warranty of
''	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
''	GNU General Public License for more details.
''
''	You should have received a copy of the GNU General Public License
''	along with this program; if not, write to the Free Software
''	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA.

'' AST conditional IF nodes
'' l = cond expr, r = link(true expr, false expr)
''
'' chng: sep/2004 written [v1ctor]

option explicit
option escape

#include once "inc\fb.bi"
#include once "inc\fbint.bi"
#include once "inc\ir.bi"
#include once "inc\ast.bi"

'':::::
function astNewIIF( byval condexpr as ASTNODE ptr, _
					byval truexpr as ASTNODE ptr, _
					byval falsexpr as ASTNODE ptr ) as ASTNODE ptr static
    dim as ASTNODE ptr n
    dim as integer true_dtype, false_dtype
    dim as FBSYMBOL ptr falselabel

	function = NULL

	if( condexpr = NULL ) then
		exit function
	end if

	true_dtype = truexpr->dtype
	false_dtype = falsexpr->dtype

    '' string? invalid
    select case irGetDataClass( true_dtype )
    case IR_DATACLASS_STRING
    	exit function
    case IR_DATACLASS_INTEGER
    	select case true_dtype
    	case IR_DATATYPE_CHAR, IR_DATATYPE_WCHAR
    		exit function
    	end select
    end select

    select case irGetDataClass( false_dtype )
    case IR_DATACLASS_STRING
    	exit function
    case IR_DATACLASS_INTEGER
    	select case false_dtype
    	case IR_DATATYPE_CHAR, IR_DATATYPE_WCHAR
    		exit function
    	end select
    end select

	'' UDT's? ditto
	if( true_dtype = IR_DATATYPE_USERDEF ) then
		exit function
    end if

    if( false_dtype = IR_DATATYPE_USERDEF ) then
    	exit function
    end if

    '' are the data types different?
    if( true_dtype <> false_dtype ) then
    	if( irMaxDataType( true_dtype, false_dtype ) <> INVALID ) then
    		exit function
    	end if
    end if

	falselabel = symbAddLabel( NULL )

	condexpr = astUpdComp2Branch( condexpr, falselabel, FALSE )
	if( condexpr = NULL ) then
		exit function
	end if

	'' alloc new node
	n = astNewNode( AST_NODECLASS_IIF, true_dtype, truexpr->subtype )
	function = n

	if( n = NULL ) then
		exit function
	end if

	n->iif.sym 		  = symbAddTempVar( true_dtype, truexpr->subtype )
	n->l  			  = condexpr
	n->r  			  = astNewLINK( truexpr, falsexpr )
	n->iif.falselabel = falselabel

end function

'':::::
function astLoadIIF( byval n as ASTNODE ptr ) as IRVREG ptr
    dim as ASTNODE ptr l, r, t
    dim as FBSYMBOL ptr exitlabel

	l = n->l
	r = n->r

	if( (l = NULL) or (r = NULL) ) then
		return NULL
	end if

	if( ast.doemit ) then
		'' IR can't handle inter-blocks and live vregs atm, so any
		'' register used must be spilled now or that could happen in a
		'' function call done in any child trees and also if complex
		'' expressions were used
		'''''if( astIsClassOnTree( AST_NODECLASS_FUNCT, r->l ) <> NULL ) then
		irEmitSPILLREGS( )
		'''''end if
	end if

	'' condition
	astFLush( l )

	''
	exitlabel = symbAddLabel( NULL )

	'' true expr
	t = astNewASSIGN( astNewVAR( n->iif.sym, _
								 0, _
								 symbGetType( n->iif.sym ), _
								 symbGetSubType( n->iif.sym ) ), _
					  r->l )
	astLoad( t )
	astDel( t )

	if( ast.doemit ) then
		irEmitBRANCH( IR_OP_JMP, exitlabel )
	end if

	'' false expr
	if( ast.doemit ) then
		irEmitLABELNF( n->iif.falselabel )
	end if

	if( ast.doemit ) then
		'' see above
		'''''if( astIsClassOnTree( AST_NODECLASS_FUNCT, r->r ) <> NULL ) then
		irEmitSPILLREGS( )
		'''''end if
	end if

	t = astNewASSIGN( astNewVAR( n->iif.sym, _
								 0, _
								 symbGetType( n->iif.sym ), _
								 symbGetSubType( n->iif.sym ) ), _
					  r->r )
	astLoad( t )
	astDel( t )

    if( ast.doemit ) then
		'' exit
		irEmitLABELNF( exitlabel )
	end if

	t = astNewVAR( n->iif.sym, 0, symbGetType( n->iif.sym ), symbGetSubType( n->iif.sym ) )
	function = astLoad( t )
	astDel( t )

	astDel( r )

end function

