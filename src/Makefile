ifneq ("$(WIN32)", "1")
ifneq ("$(shell grep GTK2 config.h)","\#define GTK2_SUPPORT 1")
GTK_FLAGS = `gtk-config --cflags`
GTK_LIBS = `gtk-config --libs`
else
GTK_FLAGS = `pkg-config gtk+-2.0 --cflags` -D_GTK2
GTK_LIBS = `pkg-config gtk+-2.0 --libs`
endif
endif

GLUI_PREFIX	= /home/zig/Dev/gl/glui/src
GLUT_PREFIX	= /home/zig/Dev/glui/glut-3.7/
GLUI_LIBS	= $(GLUI_PREFIX)/lib/libglui.a -lglut
#$(GLUT_PREFIX)/lib/glut/libglut.so.3.7 #-lglut
GLUI_INCS	= -I$(GLUI_PREFIX)/include

ifneq ("$(WIN32)", "1")
FTGL_PREFIX	= /usr/
else
FTGL_PREFIX	= /usr/mingw32/usr/
endif
FTGL_INCS	= `$(FTGL_PREFIX)bin/freetype-config --cflags` -I$(FTGL_PREFIX)include/FTGL
FTGL_LIBS	= -lftgl `$(FTGL_PREFIX)bin/freetype-config --libs`

ifneq ("$(WIN32)", "1")
CC	= gcc
CXX	= g++
LD	= g++
STRIP	= strip
#CFLAGS	= -g -DGCC -DUSE_GTK `sdl-config --cflags` $(GTK_FLAGS) -O2 -mtune=athlon-xp -ffast-math -funroll-loops -m3dnow -mmmx -msse -msse2 #-mfpmath=sse #-fomit-frame-pointer
CFLAGS	= -g -DGCC -DUSE_GTK `sdl-config --cflags` $(GTK_FLAGS) -O1 -ffast-math -m3dnow -mmmx -msse -msse2
LDFLAGS	= -g -rdynamic -lGL -lGLU -L/usr/X11R6/lib `sdl-config --libs` -lz
DLFLAGS = -shared -Wl,-Bsymbolic
O	= o
COUT	= -c -o #
LOUT	= -o #
else
STRIP	= $(TOOL_PREFIX)strip
WINDRES	= $(TOOL_PREFIX)windres
ifeq ("$(MSVC)", "1")
MSVCP=/mnt/big1/msvc7.1/Vc7
MSVCP2=I:\\mnt\\big1\\msvc7.1\\Vc7
#MSVC_TOOLS_PREFIX=	(pkill -9 wine && sleep 0.1 || true) && wine --debugmsg -all -- $(MSVCP)/bin/
MSVC_TOOLS_PREFIX=	wine $(MSVCP)/bin/
#MSVC_TOOLS_PREFIX=wine --debugmsg -all -- /mnt/windaube/msvc7.1/Vc7/bin/
MSVC_CFLAGS = -I$(MSVCP2)\\include -I$(MSVCP2)\\PlatformSDK\\Include
MSVC_LDFLAGS = -LIBPATH:$(MSVCP2)\\lib -LIBPATH:$(MSVCP2)\\PlatformSDK\\Lib

CC        = $(MSVC_TOOLS_PREFIX)cl
CXX       = $(CC)
COUT      = -c -Fo
LD        = $(MSVC_TOOLS_PREFIX)link
LOUT      = -OUT:
LDPATH    = -libpath:

DEFINES += -DWIN32 -D_WINDOWS -DNT_PLUGIN -D_WIN32

CFLAGS	= $(DEFINES) $(MSVC_CFLAGS)
CFLAGS	+= -Ot -Ox

SYSLIBS += Gdi32.lib winspool.lib comdlg32.lib Opengl32.lib
SYSLIBS += advapi32.lib ole32.lib oleaut32.lib uuid.lib 
SYSLIBS += odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib
SYSLIBS += kernel32.lib user32.lib shell32.lib ws2_32.lib winmm.lib -subsystem:windows libcmt.lib

LDFLAGS = $(MSVC_LDFLAGS) $(SYSLIBS)
DLFLAGS	= -dll
# \
#	-export:CloseDLL \
#	-export:DllAbout \
#	-export:GetDllInfo \
#	-export:RomClosed \
#	-export:DllConfig

CPPFLAGS = $(CFLAGS)
O	= owmsvc
RES	= RES
else
CC	= $(TOOL_PREFIX)gcc
CXX	= $(TOOL_PREFIX)g++
LD	= $(TOOL_PREFIX)g++
#CFLAGS	= -g -mno-cygwin -DGCC -O2 -ffast-math -funroll-loops -mmmx -msse -msse2 -fomit-frame-pointer $(FTGL_INCS)
CFLAGS	= -g -mno-cygwin -DGCC -O2 -ffast-math -funroll-loops -fomit-frame-pointer $(FTGL_INCS)
#CFLAGS	= -mno-cygwin -DGCC -O2 $(FTGL_INCS) -g
#CFLAGS	+= -O0 -g
LDFLAGS	= -mno-cygwin $(FTGL_LIBS) -lole32 -luuid -lcomctl32 -lwinspool -lws2_32 -lwsock32 -lopengl32 -lglu32 -lglut32 -mno-cygwin -mwindows -mconsole # -lmingw32  -lSDLmain -lSDL
SDL_LIB	= -lmingw32  -lSDLmain  /usr/mingw32/usr/lib/libSDL.a  -lwinmm # -lSDL
#SDL_LIB	= -lmingw32  -lSDLmain -lwinmm -lSDL
DLFLAGS	= -shared
O	= ow32
RES	= ow32
COUT	= -c -o #
LOUT	= -o #
endif
endif

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
CFLAGS += -fPIC
endif

ifeq ("$(RGL_ASSERT)", "1")
CFLAGS	+= -DRGL_ASSERT
endif

#CFLAGS	= -DUSE_GTK `sdl-config --cflags` $(GTK_FLAGS) -O2 -g
CPPFLAGS= $(CFLAGS)

OBJECTS = \
	rdp-mess.$(O) \
	disasm.$(O) \
	main.$(O)

OBJECTS4 = \
	rgl.$(O) \
	rgl_settings.$(O) \
	rgl_tiles.$(O) \
	rgl_rendermode.$(O) \
	rgl_geometry.$(O) \
	rgl_debugger.$(O) \
	rgl_glut.$(O) \
	rgl_osdep.$(O) \
	rdp.$(O) \
	glshader.$(O) \
	disasm.$(O) \
	capture.$(O) \
	maingl.$(O)

OBJECTS5 = \
	benchmark.$(O) \
	glshader.$(O)

OBJECTS2 = \
	rsp.$(O) \
	rsp_opinfo.$(O) \
	rsp_recomp.$(O) \
	rsp_dasm.$(O) \
	main_rsp.$(O)

OBJECTS3 = \
	rsp_dasm.$(O) \
	tester.$(O)

ifneq ("$(WIN32)", "1")
TARGET	= z64.so
TARGET2	= z64-rsp.so
TARGET3 = tester
TARGET3LD = -ldl
TARGET4 = z64gl.so
TARGET5 = zbench64
TARGET4LD = -lGL
else
TARGET	= z64.dll
TARGET2	= z64-rsp.dll
TARGET3 = tester.exe
TARGET4 = z64gl.dll
TARGET5 = zbench64.exe
TARGET4LD = $(SDL_LIB) # /usr/mingw32/usr/lib/libSDL.a #-lSDL
#TARGET5LD = /usr/mingw32/usr/lib/libSDL.a  -Dmain=SDL_main -lSDLmain -lmingw32 #-lSDL
TARGET5LD = $(SDL_LIB)
#CFLAGS	+=  -Dmain=SDL_main
endif

ifeq ("$(RDP_DEBUG)", "1")
CFLAGS += $(GLUI_INCS) $(FTGL_INCS) -DRDP_DEBUG
LDFLAGS += $(GLUI_LIBS) $(FTGL_LIBS)
TARGET4LD += -lIL
endif

all: $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) instruction

$(TARGET): $(OBJECTS)
	$(LD) $(GTK_LIBS) $(LOUT)$@ $(OBJECTS) $(LDFLAGS) $(DLFLAGS) $(SDL_LIB)
ifeq ("$(WIN32)", "1")
ifneq ("$(MSVC)", "1")
# don't know why a .comment section is being added, but it prevents the dll
# from being loaded
	$(STRIP) -R .comment $@
endif
endif

$(TARGET4): $(OBJECTS4)
	$(LD) $(GTK_LIBS) $(LOUT)$@ $(OBJECTS4) $(LDFLAGS) $(DLFLAGS) $(TARGET4LD)
ifeq ("$(WIN32)", "1")
ifneq ("$(MSVC)", "1")
# don't know why a .comment section is being added, but it prevents the dll
# from being loaded
	$(STRIP) -R .comment $@
endif
endif

$(TARGET5): $(OBJECTS5)
	$(LD) $(GTK_LIBS) $(LOUT)$@ $(OBJECTS5) $(LDFLAGS) $(TARGET5LD)

$(TARGET2): $(OBJECTS2)
	$(LD) $(GTK_LIBS) $(LOUT)$@ $(OBJECTS2) $(LDFLAGS) $(DLFLAGS)
ifeq ("$(WIN32)", "1")
ifneq ("$(MSVC)", "1")
# don't know why a .comment section is being added, but it prevents the dll
# from being loaded
	$(STRIP) -R .comment $@
endif
endif

$(TARGET3): $(OBJECTS3)
	$(LD) $(GTK_LIBS) $(LOUT)$@ $(OBJECTS3) $(LDFLAGS) $(TARGET3LD)

instruction:
	$(warning please copy $(TARGET2) and $(TARGET4) in the plugins/ folder of the emulator)

clean:
	rm -rf $(OBJECTS) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5) $(ALL)

rebuild: clean $(ALL)

.SUFFIXES: .rc
.rc.$(O):
	$(WINDRES) $< -o $@

.SUFFIXES: .$(O) .cpp .c
.cpp.$(O):
	$(CXX) $(CPPFLAGS) $< $(COUT)$@

.c.$(O):
	$(CC) $(CFLAGS) $< $(COUT)$@


.SUFFIXES: .$(RES)
.rc.RES:
	$(MSVC_TOOLS_PREFIX)rc -I$(MSVCP2)/atlmfc/include -I$(MSVCP2)/PlatformSDK/Include/ $<

rsp_gen.$(O): rsp.h
rsp.$(O): rsp.h
tester.$(O): rsp.h
rgl.$(O): rgl.h
rgl_tiles.$(O) : rgl.h
