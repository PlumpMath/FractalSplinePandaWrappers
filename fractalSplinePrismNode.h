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
//! \brief FractalSplinePrismNode class

// This file is based on panda/src/parametrics/sheetnode.h, from http://panda3d.org

#ifndef FRACTALSPLINEPRISMNODE_H
#define FRACTALSPLINEPRISMNODE_H

#include <iostream>
using namespace std;

#include "fractalSplineNode.h"

#include "PrimitivePrism.h"

////////////////////////////////////////////////////////////////////
//       Class : FractalSplinePrismNode
////////////////////////////////////////////////////////////////////
class EXPCL_FRACTALSPLINEPANDAWRAP FractalSplinePrismNode : public FractalSplineNode {
PUBLISHED:
  FractalSplinePrismNode(const string &name);
  ~FractalSplinePrismNode()
  {
     delete pfractalsplineprimitive;
     pfractalsplineprimitive = 0;
   }

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PandaNode::init_type();
    register_type(_type_handle, "FractalSplinePrismNode",
                  PandaNode::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

// #include "FractalSplinePrismNode.I"

#endif
