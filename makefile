SRCDIR := src
BUILDDIR := build

OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

C_INCLUDES := -Iinclude -I$(SRCDIR) "-LC:\Program Files\VideoLAN\VLC" -llibvlc -LF:\Programming\Libraries\lua-5.4.6\install\lib -llua54 -LC:\msys64\mingw64\bin -llibncursesw6
C_ARGS := -Wall -Wextra -Wconversion -pedantic

ifeq ($(OS),Windows_NT)
RM 		:= del /q /f $(NOSTDOUT) $(NOSTDERR)
RMDIR = if exist "$1" rmdir /s /q "$1"
MKDIR = if not exist "$1" mkdir "$1"
else
RM 		:= rm -f
RMDIR = rm -rf $1
MKDIR = mkdir -p $1
endif
ENSUREDIR = $(call MKDIR,$(dir $@))


all: lunar-media.exe

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@$(ENSUREDIR)
	gcc $(C_ARGS) $(C_INCLUDES) -c $^ -o $@

lunar-media.exe: $(OBJECTS)
	@$(ENSUREDIR)
	gcc $(C_ARGS) $(C_INCLUDES) $^ -o $@


.PHONY: clean
clean:
	@$(call RMDIR,$(BUILDDIR))
	@$(RM) lunar-media.exe
