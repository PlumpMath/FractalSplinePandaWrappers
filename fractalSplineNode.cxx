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
//! \brief FractalSplineNode class

// This file is based on panda/src/parametrics/sheetnode.cxx, from http://panda3d.org

#include "bitMask.h"
#ifndef BUILDING_PYTHONINTERFACES
//template class BitMask<unsigned long, 32>;
#endif

#include "cullTraverser.h"
#include "cullTraverserData.h"
#include "cullableObject.h"
#include "cullHandler.h"
//#include "geomTristrip.h"
//#include "bamWriter.h"
//#include "bamReader.h"
//#include "datagram.h"
//#include "datagramIterator.h"
#include "pStatTimer.h"

#ifdef USE_QPGEOM
#include "qpgeom.h"
#include "qpgeomTristrips.h"
#include "qpgeomTriangles.h"
#include "qpgeomVertexWriter.h"
#else
#include "GeomTri.h"
#endif // USE_QPGEOM

#include "textureAttrib.h"
#include "colorAttrib.h"

#include "FractalSplineNode.h"

TypeHandle FractalSplineNode::_type_handle;

PStatCollector FractalSplineNode::_sheet_node_pcollector("*:FractalSplineNode");

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::CData::make_copy
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
//CycleData *FractalSplineNode::CData::
//make_copy() const {
//  return new CData(*this);
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
FractalSplineNode::
FractalSplineNode(const string &name) :
  PandaNode(name)
{
   int i;
   for( i = 0; i < FractalSplineNodeNamespace::iMaxTextures; i++ )
   {
      FaceColors[i] = Colorf( 1,1,1, 0 );
   }
   
   olddetail = 16;
   bDirty = true;
}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::Copy Constructor
//       Access: Protected
//  Description:
////////////////////////////////////////////////////////////////////
//FractalSplineNode::
//FractalSplineNode(const FractalSplineNode &copy) :
//  PandaNode(copy),
//  _cycler(copy._cycler)
//{
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::make_copy
//       Access: Public, Virtual
//  Description: Returns a newly-allocated Node that is a shallow copy
//               of this one.  It will be a different Node pointer,
//               but its internal data may or may not be shared with
//               that of the original Node.
////////////////////////////////////////////////////////////////////
//PandaNode *FractalSplineNode::
//make_copy() const {
//  return new FractalSplineNode(*this);
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::safe_to_transform
//       Access: Public, Virtual
//  Description: Returns true if it is generally safe to transform
//               this particular kind of Node by calling the xform()
//               method, false otherwise.  For instance, it's usually
//               a bad idea to attempt to xform a FractalSplineNode.
////////////////////////////////////////////////////////////////////
//bool FractalSplineNode::
//safe_to_transform() const {
//  return false;
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::has_cull_callback
//       Access: Public, Virtual
//  Description: Should be overridden by derived classes to return
//               true if cull_callback() has been defined.  Otherwise,
//               returns false to indicate cull_callback() does not
//               need to be called for this node during the cull
//               traversal.
////////////////////////////////////////////////////////////////////
bool FractalSplineNode::
has_cull_callback() const {
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::cull_callback
//       Access: Public, Virtual
//  Description: If has_cull_callback() returns true, this function
//               will be called during the cull traversal to perform
//               any additional operations that should be performed at
//               cull time.  This may include additional manipulation
//               of render state or additional visible/invisible
//               decisions, or any other arbitrary operation.
//
//               By the time this function is called, the node has
//               already passed the bounding-volume test for the
//               viewing frustum, and the node's transform and state
//               have already been applied to the indicated
//               CullTraverserData object.
//
//               The return value is true if this node should be
//               visible, or false if it should be culled.
////////////////////////////////////////////////////////////////////
bool FractalSplineNode::
cull_callback(CullTraverser *trav, CullTraverserData &data) {
	render_sheet(trav, data );
  
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::output
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
//void FractalSplineNode::
//output(ostream &out) const {
 // }
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::write
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
//void FractalSplineNode::
//write(ostream &out, int indent_level) const {
  //}
//}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::reset_bound
//       Access: Published
//  Description: Recomputes the bounding volume.  This is normally
//               called automatically, but it must occasionally be
//               called explicitly when the surface has changed
//               properties outside of this node's knowledge.
////////////////////////////////////////////////////////////////////
void FractalSplineNode::
reset_bound(const NodePath &rel_to) {
  do_recompute_bound(rel_to);
  changed_internal_bound();
}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::recompute_internal_bound
//       Access: Protected, Virtual
//  Description: Called when needed to recompute the node's
//               _internal_bound object.  Nodes that contain anything
//               of substance should redefine this to do the right
//               thing.
////////////////////////////////////////////////////////////////////
BoundingVolume *FractalSplineNode::
recompute_internal_bound() {
  return do_recompute_bound(NodePath(this));
}

////////////////////////////////////////////////////////////////////
//     Function: FractalSplineNode::do_recompute_bound
//       Access: Private
//  Description: Does the actual internal recompute.
////////////////////////////////////////////////////////////////////
BoundingVolume *FractalSplineNode::
do_recompute_bound(const NodePath &rel_to) {
  // First, get ourselves a fresh, empty bounding volume.
  BoundingVolume *bound = PandaNode::recompute_internal_bound();
//  nassertr(bound != (BoundingVolume *)NULL, bound);
  
  //  pvector<LPoint3f> verts;
  
  LPoint3f BoundingPoints[2];
  
  BoundingPoints[0].set_x( -0.5 );
  BoundingPoints[0].set_y( -0.5 );
  BoundingPoints[0].set_z( -0.5 );

  BoundingPoints[1].set_x( 0.5 );
  BoundingPoints[1].set_y( 0.5 );
  BoundingPoints[1].set_z( 0.5 );  
  
 // cout << "boundingvolume: " << *bound << endl;
  
  GeometricBoundingVolume *gbv;
  DCAST_INTO_R(gbv, bound, bound);
  
 // cout << "gbv: " << *gbv << endl;
  
  gbv->around(BoundingPoints, BoundingPoints + 2 );
  
 // cout << "gbv after around: " << *gbv << endl;
  
 // exit(1);
  
  return bound;
}

void FractalSplineNode::AddVertex( float x, float y, float z )
{
  // cout << "add vertex" << endl;
  //cout << CurrentNormal << endl;
#ifdef USE_QPGEOM
   unprotectedpVertexWriter[iCurrentTextureID]->add_data3f(x,y,z);
   unprotectedpNormalWriter[iCurrentTextureID]->add_data3f( CurrentNormal );
   unprotectedpTextureCoordWriter[iCurrentTextureID]->add_data2f( CurrentTextureCoord );
#else
   unprotectedpVertexArray[iCurrentTextureID]->push_back( LPoint3f( x,y,z ) );
   unprotectedpNormalArray[iCurrentTextureID]->push_back( CurrentNormal );
   unprotectedpTextureCoordArray[iCurrentTextureID]->push_back( CurrentTextureCoord );
#endif
}

void FractalSplineNode::StartTriangle()
{
	// dont need to do anything really
	
  // cout << "starting triangle" << endl;
  // exit(1);
}

//static iNumTriangles = 0;

void FractalSplineNode::EndTriangle()
{
//	iNumTriangles++;
#ifdef USE_QPGEOM
  pGeomTrianglesObject[iCurrentTextureID]->add_next_vertices(3);
  pGeomTrianglesObject[iCurrentTextureID]->close_primitive();
#else
  iNumberTriangles[iCurrentTextureID]++;
#endif
}

void FractalSplineNode::SetNormal( float x, float y, float z )
{
 // cout << "FractalSplineNode::SetNormal " << x << y << z << endl;
  CurrentNormal.set_x(x);
  CurrentNormal.set_y(y);
  CurrentNormal.set_z(z);
 // cout << CurrentNormal << endl;
}

void FractalSplineNode::SetTextureCoord( float u, float v )
{
   CurrentTextureCoord.set_x( u );
   CurrentTextureCoord.set_y( v );
}

void FractalSplineNode::SetTextureID( int iTexture )
{
//	cout << "texture switched to: " << iTexture << " rendered " << iNumTriangles << " triangles since last texture change" << endl;
	//if( iTexture != 0 )
	//{
//		exit(1);
//	}
	//iNumTriangles = 0;
	iCurrentTextureID = iTexture;
}

void FractalSplineNode::
render_sheet(CullTraverser *trav, CullTraverserData &data ) {
#ifdef USE_QPGEOM
   render_sheetqpGeom( trav, data );
#else
   render_sheetGeom( trav, data );
#endif
}

int GetLevelOfDetail( CullTraverser *trav, CullTraverserData &data )
{
  CPT(TransformState) rel_transform = 
    trav->get_scene()->get_cull_center().get_net_transform()->
    invert_compose(data._net_transform);

 // LPoint3f center = cdata->_center * rel_transform->get_mat();
   LPoint3f objectposobjectaxes;
   objectposobjectaxes.set_x(0);
   objectposobjectaxes.set_y(0);
   objectposobjectaxes.set_z(0);
   LPoint3f objectposworldaxes = objectposobjectaxes * data._net_transform->get_mat();
   
   LPoint3f objectposcameraaxes = objectposworldaxes * rel_transform->get_mat();

  // Determine which child to traverse
  float dist = fabs(dot(objectposcameraaxes, LVector3f::forward()));

   int detail = 128 / dist;
     
     if( detail > 32 )
     {
      detail = 32;
   }
   else if( detail < 2 )
   {
      detail = 2;
   }
   
   if( detail >= 32 )
   {
      detail = 32;
   }
   else if( detail >= 16 )
   {
      detail = 16;
   }   
   else if( detail >= 8 )
   {
      detail = 8;
   }  
   else if( detail >= 4 )
   {
      detail = 4;
   }  
   else if( detail >= 2 )
   {
      detail = 2;
   }     
   return detail;
}

#ifdef USE_QPGEOM
void FractalSplineNode::
render_sheetqpGeom(CullTraverser *trav, CullTraverserData &data ) {
 // cout << "render_sheet()" << endl;
  
//  PT(Texture) myTex = iFaceTextures[iFaceID]; 
// CPT(RenderAttrib) texattrib = TextureAttrib::make(myTex); 
// CPT(RenderState) state = data._state->add_attrib(texattrib); 
// CullableObject *object = new CullableObject(geom, state, data._render_transform);
  
  int i;
  
  iCurrentTextureID = 0;
  
  CullableObject *objects[FractalSplineNodeNamespace::iMaxTextures];
  CPT(RenderAttrib) texattrib;
  CPT(RenderState) state;
        
   int detail = GetLevelOfDetail( trav, data );
        
   if( detail != olddetail )
   {
       pfractalsplineprimitive->SetLevelOfDetail( detail );
       pfractalsplineprimitive->UpdateTransforms();
       olddetail = detail;
       bDirty = true;
   }
         
  static bFirst = true;

  if( bDirty )
  {
     pGeomVertexFormatObject = qpGeomVertexFormat::get_v3n3t2();
           
      for( i = 0; i < iNumberTextures; i++ )
      {
         pGeomVertexDataObject[i] = new qpGeomVertexData
         ("FractalSplineNode", pGeomVertexFormatObject, qpGeom::UH_stream);
      
         unprotectedpVertexWriter[i] = new qpGeomVertexWriter(pGeomVertexDataObject[i], InternalName::get_vertex());
         unprotectedpNormalWriter[i] = new qpGeomVertexWriter(pGeomVertexDataObject[i], InternalName::get_normal());
         //unprotectedpColorWriter[i] = new qpGeomVertexWriter (pGeomVertexDataObject[i], InternalName::get_color());
         unprotectedpTextureCoordWriter[i] = new qpGeomVertexWriter (pGeomVertexDataObject[i], InternalName::get_texcoord());

         pGeomTrianglesObject[i] = new qpGeomTriangles(qpGeom::UH_stream);
         pGeomTrianglesObject[i]->set_shade_model( qpGeom::SM_smooth );

         geom[i] = new qpGeom;
         geom[i]->set_vertex_data( pGeomVertexDataObject[i] );
         geom[i]->add_primitive( pGeomTrianglesObject[i] );
      }

     pfractalsplineprimitive->Render();

     for( i = 0; i < iNumberTextures; i++ )
     {
       delete( unprotectedpVertexWriter[i] );
       delete( unprotectedpNormalWriter[i] );
       //delete( unprotectedpColorWriter[i] );
       delete( unprotectedpTextureCoordWriter[i] );
     }
     bDirty = false;
     bFirst = false;
  }
  
  for( i = 0; i < iNumberTextures; i++ )
  {
      CPT( RenderAttrib ) colorattrib = ColorAttrib::make_flat( FaceColors[i] );
      state = data._state->add_attrib( colorattrib, 1 );

    if( FaceTextures[i] != 0 )
    { 
        texattrib = TextureAttrib::make( FaceTextures[i] );
        state = state->add_attrib(texattrib, 1 ); 
    }
      objects[i] = new CullableObject(geom[i], state,
                                              data._render_transform);
    trav->get_cull_handler()->record_object(objects[i], trav);
  }  
}
#else
void FractalSplineNode::
render_sheetGeom(CullTraverser *trav, CullTraverserData &data ) {
   CullableObject *objects[FractalSplineNodeNamespace::iMaxTextures];
   CPT(RenderAttrib) texattrib;
   CPT(RenderState) state;
        
   int i;
   iCurrentTextureID = 0;  
   int detail = GetLevelOfDetail( trav, data );
        
   if( detail != olddetail )
   {
       pfractalsplineprimitive->SetLevelOfDetail( detail );
       pfractalsplineprimitive->UpdateTransforms();
       olddetail = detail;
       bDirty = true;
   }
         
  static bFirst = true;

   if( bDirty )
   {
      for( i = 0; i < iNumberTextures; i++ )
      {
         unprotectedpVertexArray[i] = new PTA_Vertexf;
         unprotectedpNormalArray[i] = new PTA_Normalf;
         unprotectedpTextureCoordArray[i] = new PTA_TexCoordf;
         unprotectedpColorArray[i] = new PTA_Colorf;
         
         iNumberTriangles[i] = 0;
      }
      
      pfractalsplineprimitive->Render();
      
      for( i = 0; i < iNumberTextures; i++ )
      {   
         pGeomTri[i] = new GeomTri;
         pGeomTri[i]->set_num_prims(iNumberTriangles[i]);
         pGeomTri[i]->set_coords(*(unprotectedpVertexArray[i]));
         pGeomTri[i]->set_normals(*(unprotectedpNormalArray[i]), G_PER_VERTEX);
         pGeomTri[i]->set_texcoords(*(unprotectedpTextureCoordArray[i]), G_PER_VERTEX);
         unprotectedpColorArray[i]->push_back(Colorf(1.0f, 1.0f, 1.0f, 1.0f));
         pGeomTri[i]->set_colors(*(unprotectedpColorArray[i]), G_OVERALL);
      }
      
      for( i = 0; i < iNumberTextures; i++ )
      {
       delete( unprotectedpVertexArray[i] );
       delete( unprotectedpNormalArray[i] );
       delete( unprotectedpTextureCoordArray[i] );
       delete( unprotectedpColorArray[i] );
      }
      
      bDirty = false;
      bFirst = false;
   }
     
   for( i = 0; i < iNumberTextures; i++ )
   {
      CPT( RenderAttrib ) colorattrib = ColorAttrib::make_flat( FaceColors[i] );
      state = data._state->add_attrib( colorattrib, 1 );

      if( ! FaceTextures[i].is_null() )
      {
         texattrib = TextureAttrib::make( FaceTextures[i] );
         state = state->add_attrib(texattrib, 1 );
      }
      objects[i] = new CullableObject(pGeomTri[i], state,
         data._render_transform);
      trav->get_cull_handler()->record_object( objects[i] );
   }
}
#endif
