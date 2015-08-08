ifeq ($(LINUX),1)
C++FLAGS = -I. \
           -I/usr/include \
           -I/home/hughperkins/dev/Mesa-6.2.1/include \
           -I/home/hughperkins/dev/mathgl-pp-0.5.0/src/libmathglpp \
           -g -fPIC 
CCFLAGS = $(C++FLAGS)
C++ = g++ $(C++FLAGS) -c
OBJPOSTFIX = o

all:	libfractalsplinepanda.so 

endif

ifeq ($(MSVC),1)
C++FLAGS = /I. /MD /DWIN32_VC /DWIN32 /DFORCE_INLINING
CCFLAGS = $(C++FLAGS)
C++ = cl $(C++FLAGS) /c
OBJPOSTFIX = obj

all:	libfractalsplinepanda.dll libfractalsplinepandawrap.dll

endif


ALLOBJECTS = fractalSplineBoxNode.$(OBJPOSTFIX) fractalSplineCylinderNode.$(OBJPOSTFIX) \
   fractalSplineNode.$(OBJPOSTFIX) fractalSplinePrismNode.$(OBJPOSTFIX) \
   fractalSplineRingNode.$(OBJPOSTFIX) fractalSplineTorusNode.$(OBJPOSTFIX) \
   fractalSplineTubeNode.$(OBJPOSTFIX) pandaNodeHack.$(OBJPOSTFIX) bitMask.$(OBJPOSTFIX)

libfractalsplinepanda.so:	$(ALLOBJECTS)
	ld -shared -out:libfractalsplinepanda.so $(ALLOBJECTS)

libfractalsplinepanda.dll:  $(ALLOBJECTS)
	link /DLL /OUT:libfractalsplinepanda.dll $(ALLOBJECTS)

libfractalsplinepandawrap.dll:  fractalSplineNode_pandawrap.$(OBJPOSTFIX) \
     fractalSplineNode_pandawrap_module.$(OBJPOSTFIX) bitMask.$(OBJPOSTFIX) \
     libfractalsplinepanda.dll
	link /DLL /OUT:libfractalsplinepandawrap.dll fractalSplineNode_pandawrap.$(OBJPOSTFIX) \
	   fractalSplineNode_pandawrap_module.$(OBJPOSTFIX) \
	   bitMask.$(OBJPOSTFIX) libfractalsplinepanda.lib


ifeq ($(MSVC),1)

pview:   pview.exe
	

pview.exe:	libfractalsplinepanda.lib pview.obj
	link /OUT:pview.exe pview.obj libfractalsplinepanda.lib libframework.lib
	
endif


ifeq ($(LINUX),1)

pview:	libfractalsplinepanda.so pview.obj
	g++ -out:pview pview.obj libfractalsplinepanda.so libframework.so
	
endif


fractalSplineNode.$(OBJPOSTFIX): fractalSplineNode.cxx fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP FractalSplineNode.cxx
   
fractalSplineBoxNode.$(OBJPOSTFIX): fractalSplineBoxNode.cxx fractalSplineBoxNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP FractalSplineBoxNode.cxx
   
fractalSplineCylinderNode.$(OBJPOSTFIX): fractalSplineCylinderNode.cxx fractalSplineCylinderNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP fractalSplineCylinderNode.cxx
   
fractalSplinePrismNode.$(OBJPOSTFIX): fractalSplinePrismNode.cxx fractalSplinePrismNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP fractalSplinePrismNode.cxx
   
fractalSplineRingNode.$(OBJPOSTFIX): fractalSplineRingNode.cxx fractalSplineRingNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP fractalSplineRingNode.cxx
   
fractalSplineTorusNode.$(OBJPOSTFIX): fractalSplineTorusNode.cxx fractalSplineTorusNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP fractalSplineTorusNode.cxx
   
fractalSplineTubeNode.$(OBJPOSTFIX): fractalSplineTubeNode.cxx fractalSplineTubeNode.h fractalSplineNode.h
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP fractalSplineTubeNode.cxx
   
bitMask.$(OBJPOSTFIX): bitMask.cxx bitMask.h bitMask.I
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP bitMask.cxx
   
pandaNodeHack.$(OBJPOSTFIX): pandaNodeHack.cxx 
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAP pandaNodeHack.cxx
   
pview.$(OBJPOSTFIX): pview.cxx fractalSplineNode.h
	$(C++) /DBUILDING_PVIEW pview.cxx
   
fractalSplineNode_pandawrap.$(OBJPOSTFIX): fractalSplineNode_pandawrap.cxx
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAPPYTHON fractalSplineNode_pandawrap.cxx
   
fractalSplineNode_pandawrap_module.$(OBJPOSTFIX): fractalSplineNode_pandawrap_module.cxx
	$(C++) /DBUILDING_FRACTALSPLINEPANDAWRAPPYTHON /DFORCE_INLINING /DWIN32_VC /D_WIN32 fractalSplineNode_pandawrap_module.cxx
   
fractalSplineNode_pandawrap_module.cxx: fractalSplineNode_pandawrap.cxx libfractalsplinepandawrap.in
	interrogate_module -oc fractalSplineNode_pandawrap_module.cxx  -module libfractalsplinepandawrap -library libfractalsplinepandawrap -python libfractalsplinepandawrap.in

fractalSplineNode_pandawrap.cxx: libfractalsplinepanda.dll
	interrogate -oc fractalSplineNode_pandawrap.cxx -od libfractalsplinepandawrap.in -DCPPPARSER \
	   -D__STDC__=1 -D__cplusplus -longlong __int64 -D_X86_ -DWIN32_VC -D_WIN32 \
	   -D"_declspec(param)=" -D_near -D_far -D__near -D__far -D__stdcall -DFORCE_INLINING \
	   -DCPPPARSER -D__STDC__=1 -D__cplusplus -D__i386__ -D__const=const  -module libfractalsplinepandawrap \
	   -library libfractalsplinepandawrap -fnames -string -refcount -assert -python \
	   -S\panda3d-1.0.3\include\parser-inc -DBUILDING_PYTHONINTERFACES \
	   -I"f:\program files\microsoft sdk\include" \
	   -I"f:\program files\microsoft visual c++ toolkit 2003\include" \
	   -I\Panda3D-1.0.3\include -If:\dev\pandacvspymake\thirdparty\win-python\include \
	   -If:\dev\FSDev fractalSplineNode.h fractalSplineBoxNode.h fractalSplineCylinderNode.h \
	   fractalSplinePrismNode.h fractalSplineRingNode.h fractalSplineTorusNode.h \
	   fractalSplineTubeNode.h fractalSplineNode.cxx fractalSplineBoxNode.cxx \
	   fractalSplineCylinderNode.cxx fractalSplinePrismNode.cxx fractalSplineRingNode.cxx \
	   fractalSplineTorusNode.cxx fractalSplineTubeNode.cxx

