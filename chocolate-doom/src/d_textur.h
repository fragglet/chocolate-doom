// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// DESCRIPTION:
//	Typedefs related to to textures etc.,
//	 isolated here to make it easier separating modules.
//    
//-----------------------------------------------------------------------------


#ifndef __D_TEXTUR__
#define __D_TEXTUR__

#include "doomtype.h"




//
// Flats?
//
// a pic is an unmasked block of pixels
typedef struct
{
    byte		width;
    byte		height;
    byte		data;
} pic_t;




#endif
//-----------------------------------------------------------------------------
//
// $Log$
// Revision 1.2  2005/07/23 16:44:55  fraggle
// Update copyright to GNU GPL
//
// Revision 1.1.1.1  2005/07/23 16:20:00  fraggle
// Initial import
//
//
//-----------------------------------------------------------------------------
