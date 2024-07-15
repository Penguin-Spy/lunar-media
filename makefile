#SRCDIR := src
#BUILDDIR := build

#OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

#C_INCLUDES := "-IF:\Programming\Toolchains\w64devkit\include" -Iinclude -I$(SRCDIR) "-LC:\Program Files\VideoLAN\VLC" -llibvlc -LF:\Programming\Toolchains\w64devkit\lib -llua54 -l:raylib.dll
#C_INCLUDES := -Iinclude -I$(SRCDIR) "-LC:\Program Files\VideoLAN\VLC" -l:lua54.dll -l:raylib.dll
CFLAGS := -Wall -pedantic -I include
LDLIBS := -static -l lua -l raylib -l opengl32 -l gdi32 -l winmm -LF:\Programming\lunar-media -l:libvlc.dll
CC := gcc

#ifeq ($(OS),Windows_NT)
#RM 		:= del /q /f $(NOSTDOUT) $(NOSTDERR)
#RMDIR = if exist "$1" rmdir /s /q "$1"
#MKDIR = if not exist "$1" mkdir "$1"
#else
RM 		:= rm -f
#RMDIR = rm -rf $1
#MKDIR = mkdir -p $1
#endif
#ENSUREDIR = $(call MKDIR,$(dir $@))


all: lunar-media.exe

#$(BUILDDIR)/%.o: $(SRCDIR)/%.c
#	@$(ENSUREDIR)
#	$(CC) $(CFLAGS) -c $^ -o $@

lunar-media.exe: src/*.c
#	@$(ENSUREDIR)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@
#	$(CC) $^ -o $@


.PHONY: clean
clean:
#	@$(call RMDIR,$(BUILDDIR))
	@$(RM) lunar-media.exe
