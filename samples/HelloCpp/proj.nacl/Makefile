PROJECT:=HelloWorld

# Project Build flags
WARNINGS:=-Wno-long-long -Wall -Wswitch-enum
CXXFLAGS:=-pthread -std=gnu++98 -s -O2 $(WARNINGS) 

MKDIR:= python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir

#
# Compute tool paths
#
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_newlib)
CXX:=$(TC_PATH)/bin/i686-nacl-g++

#
# Disable DOS PATH warning when using Cygwin based tools Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN

all: $(PROJECT)_x86_32.nexe $(PROJECT)_x86_64.nexe

INCLUDE_DIRS := ../../../cocos2dx ../../../cocos2dx/include ../../../cocos2dx/kazmath/include ../../../cocos2dx/platform ../../../cocos2dx/platform/nacl \
				$(TC_PATH)/i686-nacl/usr/include/libxml2 $(TC_PATH)/i686-nacl/usr/include/libpng12
INCLUDE_DIRS := $(addprefix -I, $(INCLUDE_DIRS))

SRC_DIRS := cocos2dx \
			cocos2dx/actions \
			cocos2dx/base_nodes \
			cocos2dx/cocoa \
			cocos2dx/effects \
			cocos2dx/extensions \
			cocos2dx/extensions/CCNotificationCenter \
			cocos2dx/extensions/CCControlExtension \
			cocos2dx/extensions/CCListView \
			cocos2dx/extensions/CCTextureWatcher \
			cocos2dx/extensions/CCBReader \
			cocos2dx/extensions/CCScrollView \
			cocos2dx/kazmath/src \
			cocos2dx/kazmath/src/GL \
			cocos2dx/keypad_dispatcher \
			cocos2dx/label_nodes \
			cocos2dx/layers_scenes_transitions_nodes \
			cocos2dx/menu_nodes \
			cocos2dx/misc_nodes \
			cocos2dx/particle_nodes \
			cocos2dx/platform \
			cocos2dx/platform/nacl \
			cocos2dx/script_support \
			cocos2dx/shaders \
			cocos2dx/sprite_nodes \
			cocos2dx/support \
			cocos2dx/support/data_support \
			cocos2dx/support/image_support \
			cocos2dx/support/zip_support \
			cocos2dx/text_input_node \
			cocos2dx/textures \
			cocos2dx/tileMap_parallax_nodes \
			cocos2dx/touch_dispatcher
			
SRC_DIRS := $(addprefix ../../../, $(SRC_DIRS))

COCOS2DX_CPP_OBJS := $(patsubst ../../../%.cpp, %.o, $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
COCOS2DX_C_OBJS := $(patsubst ../../../%.c, %.o, $(wildcard $(addsuffix /*.c, $(SRC_DIRS))))

$(COCOS2DX_CPP_OBJS) : %.o : ../../../%.cpp
	$(MKDIR) -p $(dir obj32/$@) $(dir obj64/$@)
	$(CXX) -o obj32/$@ -c $< -m32 $(CXXFLAGS) $(INCLUDE_DIRS)
	$(CXX) -o obj64/$@ -c $< -m64 $(CXXFLAGS) $(INCLUDE_DIRS)
	
$(COCOS2DX_C_OBJS) : %.o : ../../../%.c
	$(MKDIR) -p $(dir obj32/$@) $(dir obj64/$@)
	$(CXX) -o obj32/$@ -c $< -m32 $(CXXFLAGS) $(INCLUDE_DIRS)		
	$(CXX) -o obj64/$@ -c $< -m64 $(CXXFLAGS) $(INCLUDE_DIRS)				

SRC_DIRS := ../Classes

CLASSES_OBJS := $(patsubst ../%.cpp, %.o, $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))

$(CLASSES_OBJS) : %.o : ../%.cpp
	$(MKDIR) -p $(dir obj32/$@) $(dir obj64/$@)
	$(CXX) -o obj32/$@ -c $< -m32 $(CXXFLAGS) $(INCLUDE_DIRS)
	$(CXX) -o obj64/$@ -c $< -m64 $(CXXFLAGS) $(INCLUDE_DIRS)
		
INCLUDE_DIRS += -I../Classes
	
HELLOWORLD_OBJS := $(patsubst %.cpp, %.o, $(wildcard ./*.cpp))

$(HELLOWORLD_OBJS) : %.o : %.cpp
	$(MKDIR) -p $(dir obj32/$@) $(dir obj64/$@)
	$(CXX) -o obj32/$@ -c $< -m32 $(CXXFLAGS) $(INCLUDE_DIRS)
	$(CXX) -o obj64/$@ -c $< -m64 $(CXXFLAGS) $(INCLUDE_DIRS)
		
OBJECTS := $(COCOS2DX_CPP_OBJS) $(COCOS2DX_C_OBJS) $(CLASSES_OBJS) $(HELLOWORLD_OBJS)

$(PROJECT)_x86_32.nexe : $(OBJECTS)
	$(CXX) -o $@ $(addprefix obj32/, $^) -m32 $(CXXFLAGS) $(LDFLAGS)

$(PROJECT)_x86_64.nexe : $(OBJECTS)
	$(CXX) -o $@ $(addprefix obj64/, $^) -m64 $(CXXFLAGS) $(LDFLAGS)

	
LDFLAGS :=	-lppapi_gles2 -lppapi_cpp -lppapi \
			-lxml2 -lpng12 -ljpeg -ltiff -lz -lnosys
			
clean :
	rm -rf bin obj32
	rm -rf bin obj64
			
# Define a phony rule so it always runs, to build nexe and start up server.
.PHONY: RUN 
RUN: all
	python httpd.py


