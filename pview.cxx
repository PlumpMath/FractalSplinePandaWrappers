// Filename: pview.cxx
// Created by:  drose (25Feb02)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "pandaFramework.h"
#include "textNode.h"
#include "configVariableBool.h"
#include "texturePool.h"
#include "multitexReducer.h"
#include "sceneGraphReducer.h"

// By including checkPandaVersion.h, we guarantee that runtime
// attempts to run pview will fail if it inadvertently links with the
// wrong version of libdtool.so/.dll.

#include "checkPandaVersion.h"

// #include "testNode.h"
#include "FractalSplineBoxNode.h"
#include "FractalSplinePrismNode.h"
#include "FractalSplineCylinderNode.h"
#include "FractalSplineTorusNode.h"
#include "FractalSplineTubeNode.h"
#include "FractalSplineRingNode.h"

#include "pointLight.h"
#include "directionalLight.h"

#include "Texture.h"

#ifndef HAVE_GETOPT
  #include "gnu_getopt.h"
#else
  #ifdef HAVE_GETOPT_H
    #include <getopt.h>
  #endif
#endif

PandaFramework framework;

ConfigVariableBool pview_test_hack
("pview-test-hack", true,
 "Enable the '0' key in pview to run whatever hacky test happens to be in "
 "there right now.");

bool
output_screenshot(Filename &fn)
{
  // Only one frame crashes.
  framework.do_frame();
  framework.do_frame();

  WindowFramework *wf = framework.get_window(0);
  bool ok = wf->get_graphics_window()->save_screenshot(fn);
  if (!ok) {
    cerr << "Could not generate screenshot " << fn << "\n";
  }
  return ok;
}

void
event_W(CPT_Event, void *) {
  // shift-W: open a new window on the same scene.

  // If we already have a window, use the same GSG.
  GraphicsPipe *pipe = (GraphicsPipe *)NULL;
  GraphicsStateGuardian *gsg = (GraphicsStateGuardian *)NULL;

  if (framework.get_num_windows() > 0) {
    WindowFramework *old_window = framework.get_window(0);
    GraphicsWindow *win = old_window->get_graphics_window();
    pipe = win->get_pipe();
    gsg = win->get_gsg();
  }

  WindowFramework *window = framework.open_window(pipe, gsg);
  if (window != (WindowFramework *)NULL) {
    window->enable_keyboard();
    window->setup_trackball();
    framework.get_models().instance_to(window->get_render());
  }
}

void
event_F(CPT_Event, void *) {
  // shift-F: flatten the model hierarchy.
  framework.get_models().flatten_strong();
}

void
event_Enter(CPT_Event, void *) {
  // alt-enter: toggle between window/fullscreen in the same scene.

  // If we already have a window, use the same GSG.
  GraphicsPipe *pipe = (GraphicsPipe *)NULL;
  GraphicsStateGuardian *gsg = (GraphicsStateGuardian *)NULL;

  WindowProperties props;

  if (framework.get_num_windows() > 0) {
    WindowFramework *old_window = framework.get_window(0);
    GraphicsWindow *win = old_window->get_graphics_window();
    pipe = win->get_pipe();
    gsg = win->get_gsg();
    props = win->get_properties();
    framework.close_window(old_window);
  }

  // set the toggle
  props.set_fullscreen(!props.get_fullscreen());
  
  WindowFramework *window = framework.open_window(props, pipe, gsg);
  if (window != (WindowFramework *)NULL) {
    window->enable_keyboard();
    window->setup_trackball();
    framework.get_models().instance_to(window->get_render());
  }
}

void
event_2(CPT_Event event, void *) {
  // 2: split the window into two display regions.

  EventParameter param = event->get_parameter(0);
  WindowFramework *wf;
  DCAST_INTO_V(wf, param.get_ptr());

  WindowFramework *split = wf->split_window();
  if (split != (WindowFramework *)NULL) {
    split->enable_keyboard();
    split->setup_trackball();
    framework.get_models().instance_to(split->get_render());
  }
}

WindowFramework *window;
  
void
event_0(CPT_Event event, void *) {
  // 0: run hacky test.

     NodePath loading_np;
     
   //   NodePath aspect_2d = window->get_aspect_2d();
   //   PT(TestNode) mynode = new TestNode("blah");
   //    loading_np = aspect_2d.attach_new_node(mynode);
   //    loading_np.set_scale(0.125f);
   
   //exit(123);
}

void 
usage() {
  cerr <<
    "\n"
    "Usage: pview [opts] model [model ...]\n"
    "       pview -h\n\n";
}

void 
help() {
  usage();
  cerr <<
    "pview opens a quick Panda window for viewing one or more models and/or\n"
    "animations.\n\n"

    "Options:\n\n"
    
    "  -c\n"
    "      Automatically center models within the viewing window on startup.\n"
    "      This can also be achieved with the 'c' hotkey at runtime.\n\n"

    "  -l\n"
    "      Open the window before loading any models with the text \"Loading\"\n"
    "      displayed in the window.  The default is not to open the window\n"
    "      until all models are loaded.\n\n"

    "  -s filename\n"
    "      After displaying the models, immediately take a screenshot and\n"
    "      exit.\n\n"
    
    "  -h\n"
    "      Display this help text.\n\n";
}

int
main(int argc, char *argv[]) {
  framework.open_framework(argc, argv);
  framework.set_window_title("Panda Viewer");

  bool auto_center = false;
  bool show_loading = false;
  bool auto_screenshot = false;
  Filename screenshotfn;

  extern char *optarg;
  extern int optind;
  static const char *optflags = "clhs:";
  int flag = getopt(argc, argv, optflags);

  while (flag != EOF) {
    switch (flag) {
    case 'c':
      auto_center = true;
      break;

    case 'l':
      show_loading = true;
      break;

    case 'h':
      help();
      return 1;
      
    case 's':
      auto_screenshot = true;
      screenshotfn = optarg;
      break;

    case '?':
      usage();
      return 1;

    default:
      cerr << "Unhandled switch: " << flag << endl;
      break;
    }
    flag = getopt(argc, argv, optflags);
  }
  argc -= (optind - 1);
  argv += (optind - 1);

  window = framework.open_window();
  if (window != (WindowFramework *)NULL) {
    // We've successfully opened a window.

    NodePath loading_np;

    if (show_loading) {
      // Put up a "loading" message for the user's benefit.
      NodePath aspect_2d = window->get_aspect_2d();
      PT(TextNode) loading = new TextNode("loading");
      loading_np = aspect_2d.attach_new_node(loading);
      loading_np.set_scale(0.125f);
      loading->set_text_color(1.0f, 1.0f, 1.0f, 1.0f);
      loading->set_shadow_color(0.0f, 0.0f, 0.0f, 1.0f);
      loading->set_shadow(0.04f, 0.04f);
      loading->set_align(TextNode::A_center);
      loading->set_text("Loading...");

      // Allow a couple of frames to go by so the window will be fully
      // created and the text will be visible.
     // framework.do_frame();
     // framework.do_frame();
    }
     
      NodePath render = window->get_render();
      
     //     NodePath &panda = window->load_default_model(framework.get_models());
    // panda.set_pos(0,0, 1);
     
    // panda = window->load_default_model(framework.get_models());
     //panda.set_pos(0,0, 2);
     
    // panda = window->load_default_model(framework.get_models());
    // panda.set_pos(1,0, 3);
     
     //panda = window->load_default_model(framework.get_models());
     //panda.set_pos(0,1, 4);
     
     // TestNode::init_type();
      FractalSplineBoxNode::init_type();
      FractalSplinePrismNode::init_type();

   int i;
   
   const int iNumberPrims = 20;
   
     cout << *( ConfigPageManager::get_global_ptr() ) << endl;
     	
   PT(FractalSplineBoxNode) boxnodes[iNumberPrims];
   
   for( i = 0; i < iNumberPrims; i++ )
   {
    cout << "about to create FractalSplineBoxNode" << endl;
     boxnodes[i] = new FractalSplineBoxNode("blah2");
         //cout << mynode->GetName() << endl;
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(boxnodes[i]);
      loading_np.set_scale(0.5f);
      boxnodes[i]->set_twist( 30 );
      boxnodes[i]->set_cut_start( 30 );
      boxnodes[i]->set_face_color( -1, Colorf( 1,0,0, 0 ) );
      boxnodes[i]->set_hollow( 30 );
      boxnodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,0,-0.5);
    cout << "done" << endl;
   // loading_np.show_bounds();
    }
    
   PT(FractalSplinePrismNode) prismnodes[iNumberPrims];
   
   for( i = 0; i < iNumberPrims; i++ )
   {
     prismnodes[i] = new FractalSplinePrismNode("blah2");
         //cout << prismnode1->GetName() << endl;
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(prismnodes[i]);
      loading_np.set_scale(0.5f);
      prismnodes[i]->set_twist( 30 );
      prismnodes[i]->set_cut_start( 30 );
      prismnodes[i]->set_hollow( 30 );
      prismnodes[i]->set_face_color( -1, Colorf( 0,0,1, 0 ) );
      prismnodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,1.25,-0.5);
    cout << "done" << endl;
    //loading_np.show_bounds();
   }
    
   PT(FractalSplineCylinderNode) cylnodes[iNumberPrims];
   
   for( i = 0; i < iNumberPrims; i++ )
   {
     cylnodes[i] = new FractalSplineCylinderNode("blah2");
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(cylnodes[i]);
      loading_np.set_scale(0.5f);
      cylnodes[i]->set_twist( 30 );
      cylnodes[i]->set_cut_start( 30 );
      cylnodes[i]->set_hollow( 30 );
      cylnodes[i]->set_face_color( -1, Colorf( 0,1,1, 0 ) );
       cylnodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,2.5,-0.5);
    cout << "done" << endl;
    //loading_np.show_bounds();
   }
    
   PT(FractalSplineTorusNode) torusnodes[iNumberPrims];
   for( i = 0; i < iNumberPrims; i++ )
   {
     torusnodes[i] = new FractalSplineTorusNode("blah2");
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(torusnodes[i]);
      loading_np.set_scale(0.5f);
      torusnodes[i]->set_twist( 30 );
      torusnodes[i]->set_cut_start( 30 );
      torusnodes[i]->set_hollow( 30 );
      torusnodes[i]->set_face_color( -1, Colorf( 0,1,0, 0 ) );
      torusnodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,3.75,-0.5);
    cout << "done" << endl;
    //loading_np.show_bounds();
   }
   PT(FractalSplineRingNode) ringnodes[iNumberPrims];
   for( i = 0; i < iNumberPrims; i++ )
   {
     ringnodes[i] = new FractalSplineRingNode("blah2");
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(ringnodes[i]);
      loading_np.set_scale(0.5f);
      ringnodes[i]->set_twist( 30 );
      ringnodes[i]->set_cut_start( 30 );
      ringnodes[i]->set_hollow( 30 );
      ringnodes[i]->set_face_color( -1, Colorf( 1,0,1, 0 ) );
      ringnodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,5,-0.5);
    cout << "done" << endl;
    //loading_np.show_bounds();
   }
   PT(FractalSplineTubeNode) tubenodes[iNumberPrims];
   for( i = 0; i < iNumberPrims; i++ )
   {
     tubenodes[i] = new FractalSplineTubeNode("blah2");
    cout << "created node, attaching..." << endl;
      loading_np = render.attach_new_node(tubenodes[i]);
      loading_np.set_scale(0.5f);
      tubenodes[i]->set_twist( 30 );
      tubenodes[i]->set_cut_start( 30 );
      tubenodes[i]->set_face_color( -1, Colorf( 1,1,0, 0 ) );
      tubenodes[i]->set_hollow( 30 );
      tubenodes[i]->update_transforms();
      loading_np.set_pos(1.25 * i,6.25,-0.5);
    cout << "done" << endl;
    //loading_np.show_bounds();
   }
         
//PT(Texture) myTexture = TexturePool::load_texture("/f/Panda3D-1.0.2/models/maps/panda-model.jpg");
PT(Texture) myTexture;// = TexturePool::load_texture("f/dev/FSDev/2.tga");

int j;

char filename[50];
for( i = 0; i < 10; i++ )
{
   sprintf( filename, "/f/dev/FSDev/%i.tga", i );    
   myTexture = TexturePool::load_texture(filename);

   for( j= 0; j < iNumberPrims; j++ )
   {      
      boxnodes[j]->set_face_texture(i, myTexture );
      prismnodes[j]->set_face_texture(i, myTexture );
      cylnodes[j]->set_face_texture(i, myTexture );
      torusnodes[j]->set_face_texture(i, myTexture );
      ringnodes[j]->set_face_texture(i, myTexture );
      tubenodes[j]->set_face_texture(i, myTexture );
   }
}
  
PT( PointLight ) plight = new PointLight("plight");
plight->set_color(LVector4f(1, 1, 1, 1));
NodePath plnp = render.attach_new_node(plight);
plnp.set_pos(10, 20, 0);
render.set_light(plnp);

PT( DirectionalLight ) dlight = new DirectionalLight("dlight");
dlight->set_color(LVector4f(1, 1, 1, 1));
NodePath dlnp = render.attach_new_node(dlight);
dlnp.set_hpr(0, -60, 0);
render.set_light(dlnp);
 
    window->enable_keyboard();
    window->setup_trackball();
    framework.get_models().instance_to(window->get_render());
    if (argc < 2) {
      // If we have no arguments, get that trusty old triangle out.
     // window->load_default_model(framework.get_models());
    } else {
      window->load_models(framework.get_models(), argc, argv);
    }
    window->loop_animations();
    //loading_np.remove_node();

    if (auto_center) {
      window->center_trackball(framework.get_models());
    }

    if (auto_screenshot) {
      return(output_screenshot(screenshotfn) ? 0:1);
    }

    framework.enable_default_keys();
    framework.define_key("shift-w", "open a new window", event_W, NULL);
    framework.define_key("shift-f", "flatten hierarchy", event_F, NULL);
    framework.define_key("alt-enter", "toggle between window/fullscreen", event_Enter, NULL);
    framework.define_key("2", "split the window", event_2, NULL);
   // if (pview_test_hack) {
      framework.define_key("0", "run quick hacky test", event_0, NULL);
  //  }
    framework.main_loop();
    framework.report_frame_rate(nout);
  }

  return (0);
}
