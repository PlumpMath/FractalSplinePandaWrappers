// Copyright Hugh Perkins 2005
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
//  more details.
//
// You should have received a copy of the GNU General Public License along
// with this program in the file licence.txt; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-
// 1307 USA
// You can find the licence also on the web at:
// http://www.opensource.org/licenses/gpl-license.php
//

//! \file
//! \brief FractalSplineBoxNode class

// This file is based on panda/src/parametrics/sheetnode.cxx, from http://panda3d.org

#include "FractalSplineBoxNode.h"

#include "PrimitiveBox.h"

TypeHandle FractalSplineBoxNode::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineBoxNode::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
FractalSplineBoxNode::
FractalSplineBoxNode(const string &name) :
  FractalSplineNode(name)
{
   pfractalsplineprimitive = new FractalSpline::PrimitiveBox( this );
   
   iNumberTextures = 9;
   
	int i;
	for( i = 0; i < iNumberTextures; i++ )
	{
	  FaceTextures[ i ] = 0;
	  pfractalsplineprimitive->SetTexture( i, i );
	}
}
