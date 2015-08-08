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

// This file is based on panda/src/parametrics/sheetnode.h, from http://panda3d.org

#ifndef FRACTALSPLINENODE_H
#define FRACTALSPLINENODE_H

#include <iostream>
using namespace std;

// define this to use qpgeom (doesnt exist in panda 1.0.3)
#undef USE_QPGEOM

#include "fspandawrapdllsymbols.h"

#ifdef USE_QPGEOM
#include "GeomVertexWriter.h"
#include "qpGeomTriangles.h"
#else
#include "GeomTri.h"
#endif

#include "pandabase.h"
#include "pandaNode.h"
#include "pStatCollector.h"

#include "cullableObject.h"

#include "Primitive.h"
#include "RendererCallbacks.h"


namespace FractalSplineNodeNamespace
{
   static const int iMaxTextures = 10;
}

////////////////////////////////////////////////////////////////////
//       Class : FractalSplineBoxNode
////////////////////////////////////////////////////////////////////
class EXPCL_FRACTALSPLINEPANDAWRAP FractalSplineNode : public PandaNode, public FractalSpline::RendererCallbacksClass {
PUBLISHED:

   FractalSplineNode( const string &name );
   
	virtual void set_twist( int newtwist )
	{
	  pfractalsplineprimitive->SetTwist( newtwist );
	}
	
	virtual void set_cut_start( int newcutstart )
	{
	  pfractalsplineprimitive->SetCutStart( newcutstart );
	}
	
	virtual void set_cut_end( int newcutend )
	{
	  pfractalsplineprimitive->SetCutEnd( newcutend );
	}
	
	virtual void set_advanced_cut_start( int newcutstart )
	{
	  pfractalsplineprimitive->SetAdvancedCutStart( newcutstart );
	}
	virtual void set_advanced_cut_end( int newcutend )
	{
	  pfractalsplineprimitive->SetAdvancedCutEnd( newcutend );
	}
	virtual void set_hollow( int newhollow )
	{
	  pfractalsplineprimitive->SetHollow( newhollow );
	}
	virtual void set_face_color( int iFaceNumber, Colorf newcolor )
	{
	   if( iFaceNumber == -1 )
	   {
	      int i;
	      for( i = 0; i < FractalSplineNodeNamespace::iMaxTextures; i++ )
	      {
	         FaceColors[ i ] = newcolor;
	      }
	   }
	   else
	   {
	      FaceColors[iFaceNumber ] = newcolor;
	   }
	}
	virtual void set_face_texture( int iFaceNumber, PT( Texture ) texture )
	{
	   if( iFaceNumber == -1 )
	   {
	      int i;
	      for( i = 0; i < FractalSplineNodeNamespace::iMaxTextures; i++ )
	      {
	         FaceTextures[ i ] = texture;
	      }
	   }
	   else
	   {
	      FaceTextures[iFaceNumber ] = texture;
	   }	
	}
	virtual void set_texture_offset_start( const TexCoordf &fNewTextureOffsetStart )
	{
	  pfractalsplineprimitive->SetTextureStartOffset( fNewTextureOffsetStart.get_x(), fNewTextureOffsetStart.get_y() );
	}
	virtual void set_texture_offset_end( const TexCoordf &fNewTextureOffsetEnd )
	{
	  pfractalsplineprimitive->SetTextureEndOffset( fNewTextureOffsetEnd.get_x(), fNewTextureOffsetEnd.get_y() );
	}
	
	virtual void set_shear( float fNew )
	{
	  pfractalsplineprimitive->SetShear( fNew );
	}
	virtual void set_top_size_x( float fNew )
	{
	  pfractalsplineprimitive->SetTopSizeX( fNew );
	}	
	virtual void set_top_size_y( float fNew )
	{
	  pfractalsplineprimitive->SetTopSizeY( fNew );
	}
	
	virtual void update_transforms()
	{
	  pfractalsplineprimitive->UpdateTransforms();
	  bDirty = true;
	}
	
	
	
	
 // virtual void output(ostream &out) const;
 // virtual void write(ostream &out, int indent_level = 0) const;

 // virtual PandaNode *make_copy() const;

 // virtual bool safe_to_transform() const;  // returns no
  virtual bool has_cull_callback() const;  // returns yes
  virtual bool cull_callback(CullTraverser *trav, CullTraverserData &data);  // renders object?
  
  inline virtual void AddVertex( float x, float y, float z ); // over-riding RendererCallbacksClass
  inline virtual void StartTriangle(); // over-riding RendererCallbacksClass
  inline virtual void EndTriangle(); // over-riding RendererCallbacksClass
  inline virtual void SetTextureCoord( float u, float v );
  inline virtual void SetNormal( float x, float y, float z );
  inline virtual void SetTextureID( int iTexture );
  //inline virtual void SetColor( float r, float g, float b );
 
PUBLISHED:
  void reset_bound(const NodePath &rel_to);

protected:
   int iNumberTextures;
   
  virtual BoundingVolume *recompute_internal_bound();
   
  FractalSpline::Primitive *pfractalsplineprimitive;
  
  bool bDirty;
  int olddetail;
  
#ifdef USE_QPGEOM
  CPT( qpGeomVertexFormat ) pGeomVertexFormatObject;
  
  PT(qpGeom) geom[FractalSplineNodeNamespace::iMaxTextures];
  
  PT( qpGeomVertexData ) pGeomVertexDataObject[FractalSplineNodeNamespace::iMaxTextures];
  
  qpGeomVertexWriter *unprotectedpVertexWriter[FractalSplineNodeNamespace::iMaxTextures];
  qpGeomVertexWriter *unprotectedpNormalWriter[FractalSplineNodeNamespace::iMaxTextures];
  qpGeomVertexWriter *unprotectedpTextureCoordWriter[FractalSplineNodeNamespace::iMaxTextures];
  qpGeomVertexWriter *unprotectedpColorWriter[FractalSplineNodeNamespace::iMaxTextures];
  
  PT( qpGeomTriangles ) pGeomTrianglesObject[FractalSplineNodeNamespace::iMaxTextures];
#else // not defined USE_QPGEOM
  PT(GeomTri) pGeomTri[FractalSplineNodeNamespace::iMaxTextures];
  
  PTA_Vertexf *unprotectedpVertexArray[FractalSplineNodeNamespace::iMaxTextures];
  PTA_Normalf *unprotectedpNormalArray[FractalSplineNodeNamespace::iMaxTextures];
  PTA_TexCoordf *unprotectedpTextureCoordArray[FractalSplineNodeNamespace::iMaxTextures];  
  PTA_Colorf *unprotectedpColorArray[FractalSplineNodeNamespace::iMaxTextures]; 
   
  int iNumberTriangles[FractalSplineNodeNamespace::iMaxTextures];
#endif // USE_QPGEOM
  
  PT( Texture ) FaceTextures[ FractalSplineNodeNamespace::iMaxTextures];
  Colorf FaceColors[ FractalSplineNodeNamespace::iMaxTextures];
  
  int iCurrentTextureID;
  LVector3f CurrentNormal;
  LVector2f CurrentTextureCoord;

private:
  BoundingVolume *do_recompute_bound(const NodePath &rel_to);
  void render_sheet(CullTraverser *trav, CullTraverserData &data );
  #ifdef USE_QPGEOM
  void render_sheetqpGeom(CullTraverser *trav, CullTraverserData &data );
  #else
  void render_sheetGeom(CullTraverser *trav, CullTraverserData &data );
  #endif

private:
  // This is the data that must be cycled between pipeline stages.
//  class CData : public CycleData {
 // public:
  //  virtual CycleData *make_copy() const;
//};

  //PipelineCycler<CData> _cycler;
  //typedef CycleDataReader<CData> CDReader;
  //typedef CycleDataWriter<CData> CDWriter;

  static PStatCollector _sheet_node_pcollector;

public:
//  static void register_with_read_factory();
//  virtual void write_datagram(BamWriter *manager, Datagram &dg);

protected:
//  static TypedWritable *make_from_bam(const FactoryParams &params);
 // void fillin(DatagramIterator &scan, BamReader *manager);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PandaNode::init_type();
    register_type(_type_handle, "FractalSplineNode",
                  PandaNode::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

// #include "FractalSplineNode.I"

#endif
