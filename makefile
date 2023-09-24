C_INCLUDES = -Iinclude "-LC:\Program Files\VideoLAN\VLC" -llibvlc
C_ARGS = -Wall -Wextra -Wconversion -pedantic

ifeq ($(OS),Windows_NT)
RM         = del /q /f $(NOSTDOUT) $(NOSTDERR)
else
RM         = rm -f
endif

all: lunar-media.exe

lunar-media.exe: main.c
	gcc $(C_ARGS) $(C_INCLUDES) $^ -o $@

clean:
	$(RM) lunar-media.exe