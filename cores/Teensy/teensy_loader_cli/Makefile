OS ?= LINUX
#OS ?= WINDOWS
#OS ?= MACOSX
#OS ?= BSD

CC ?= gcc
CFLAGS ?= -O2 -Wall 
SDK ?= /Developer/SDKs/MacOSX10.5.sdk

linux64: teensy_loader_cli.c
	$(CC) $(CFLAGS) -s -DUSE_LIBUSB -o teensy_loader_cli teensy_loader_cli.c -lusb

linux32: teensy_loader_cli.c
	$(CC) $(CFLAGS) -m32 -s -DUSE_LIBUSB -o teensy_loader_cli teensy_loader_cli.c -lusb

windows: teensy_loader_cli.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -s -DUSE_WIN32 -o teensy_loader_cli.exe teensy_loader_cli.c -lhid -lsetupapi 

mac: teensy_loader_cli.c
	$(CC) $(CFLAGS) -DUSE_APPLE_IOKIT -isysroot $(SDK) -o teensy_loader_cli teensy_loader_cli.c -Wl,-syslibroot,$(SDK) -framework IOKit -framework CoreFoundation

teensy_loader_cli_bsd: teensy_loader_cli.c
	$(CC) $(CFLAGS) -s -DUSE_UHID -o teensy_loader_cli_bsd teensy_loader_cli.c 

clean:
	rm -f teensy_loader_cli teensy_loader_cli.exe
