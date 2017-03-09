# GNUmakefile for objectViewer

# use VERBOSE=1   to see the build commands
# use CFG=release to build release
# use CFG=debug   to build debug

VPATH = src src/imageload

CFG ?= debug

TARGET = objectViewer

UNAME := $(shell uname)

OBJECTVIEWER_C = \
  stb_image.c \
  $(NULL)

OBJECTVIEWER_CPP = \
  controller.cpp \
  geometry.cpp \
  glslprogram.cpp \
  glslshader.cpp \
  light.cpp \
  main.cpp \
  material.cpp \
  menus.cpp \
  object.cpp \
  picker.cpp \
  ray.cpp \
  scene.cpp \
  texture.cpp \
  viewer.cpp \
  $(NULL)

OBJECTVIEWER_OBJS = \
     $(patsubst %.cpp, obj/$(CFG)/%.o, $(OBJECTVIEWER_CPP)) \
     $(patsubst %.c,   obj/$(CFG)/%.o, $(OBJECTVIEWER_C)) \
     $(NULL)

OBJS = $(OBJECTVIEWER_OBJS)

ifeq ($(CFG),debug)
DEBUG_OPT = -g -DSK_DEBUG
else
OPT_OPT = -O2 -DNDEBUG -fno-strict-aliasing
endif

CC         = gcc
CXX        = g++

CFLAGS     += -Wall -Wstrict-aliasing=1 -Wno-sign-compare $(DEBUG_OPT) $(OPT_OPT)
CXXFLAGS   += -Wall -Wno-strict-aliasing -Wno-sign-compare $(DEBUG_OPT) $(OPT_OPT)
CFLAGS     += -DGLEW_STATIC
CXXFLAGS   += -DGLEW_STATIC
CFLAGS     += 
CXXFLAGS   += -std=c++11

ifeq ($(UNAME), Darwin)
  CLINKFLAGS += -framework GLUT
  CLINKFLAGS += -framework OpenGL
  CLINKFLAGS += -Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
  CFLAGS     += $(DEPEND_OPTS)
  CXXFLAGS   += $(DEPEND_OPTS)
else
  ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
    CFLAGS     += -D_WIN32
    CXXFLAGS   += -D_WIN32
    CFLAGS     += -DGLUT_DISABLE_ATEXIT_HACK
    CXXFLAGS   += -DGLUT_DISABLE_ATEXIT_HACK

    # XXX assume Visual Studio built Release/cg4cpp.lib
    CLINKFLAGS += -LRelease

    CLINKFLAGS += -lglut32
    CLINKFLAGS += -lglu32 -lopengl32 -lm
    EXE = .exe
  else
      APP_PLATFORM ?= 64
      ifeq ($(APP_PLATFORM),32)
        APP_PLATFORM_OPT = -m32
      else
        APP_PLATFORM_OPT = -m64
      endif
      CFLAGS     += -DGLUT_DISABLE_ATEXIT_HACK
      CXXFLAGS   += -DGLUT_DISABLE_ATEXIT_HACK
      CFLAGS     += $(APP_PLATFORM_OPT)
      CXXFLAGS   += $(APP_PLATFORM_OPT)
      CLINKFLAGS += $(APP_PLATFORM_OPT)
      # freeglut
      CLINKFLAGS += -lglut
      CLINKFLAGS += -lXi -lXmu -lX11 -lm
      CLINKFLAGS += -L/usr/lib/nvidia-367
      CLINKFLAGS += -lGLU -lGL -lGLEW
      CLINKFLAGS += -lpthread
      CFLAGS     += $(DEPEND_OPTS)
      CXXFLAGS   += $(DEPEND_OPTS)
  endif
endif

BINARY := $(TARGET:=$(EXE))

.PHONY: all run clean clobber inform both release debug rrun drun

all: bin/$(CFG)/$(BINARY)

both: release debug

release:
	$(MAKE) CFG=release

debug:
	$(MAKE) CFG=debug

run: bin/$(CFG)/$(BINARY)
	./bin/$(CFG)/$(BINARY) $(ARGS)

rrun:
	$(MAKE) CFG=release run

drun:
	$(MAKE) CFG=debug run

gdb: bin/$(CFG)/$(BINARY)
	gdb ./bin/$(CFG)/$(BINARY)

clean:
	$(RM) $(BINARY) $(OBJS) $(DEPEND_FILES)

clobber: clean
	$(RM) *.bak *.o *~ $(DEPEND_FILES)
	$(RM) -rf bin*.debug obj.debug
	$(RM) -rf bin*.release obj.release

inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	@echo "Invalid configuration "$(CFG)" specified"
	@echo "Possible choices for configuration are 'release' and 'debug'"
	@exit 1
endif
endif
	@echo "Configuration "$(CFG)
ifdef APP_PLATFORM
	@echo "Platform      "$(APP_PLATFORM)
endif
	@echo "--------------------------"

### implicit rules

bin/$(CFG)/$(BINARY): $(OBJS) | GNUmakefile inform $(LIBRARIES_TO_BUILD)
	@mkdir -p $(dir $@)
ifndef VERBOSE
	@echo Linking $@...
endif
	$(CXX) -g -o $@ $^ $(CLINKFLAGS)

obj/$(CFG)/%.o : %.cpp | inform
	@mkdir -p $(dir $@)
ifndef VERBOSE
	@echo Compiling $(notdir $@)
endif
	$(CXX) -c $(CXXFLAGS) -o $@ $<

obj/$(CFG)/%.o : %.c | inform
	@mkdir -p $(dir $@)
ifndef VERBOSE
	@echo Compiling $(notdir $@)
endif
	$(CC) -c $(CXXFLAGS) -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
sinclude $(DEPEND_FILES)
endif
endif

ifndef VERBOSE
.SILENT:
endif
