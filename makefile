CFLAGS := -Wall -pedantic -Wextra -Dmain=SDL_main
LDLIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows -mconsole -l lua -LF:\Programming\lunar-media -l:libvlc.dll
CC := gcc

all: lunar-media.exe

lunar-media.exe: src/*.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

.PHONY: clean
clean:
	@rm -f lunar-media.exe
