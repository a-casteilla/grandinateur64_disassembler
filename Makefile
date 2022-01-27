include config.mk

default: disassembler

disassembler: disassembler.o
	$(CC) $(CFLAGS) -o disassembler disassembler.o

disassembler.o:
	$(CC) $(CFLAGS) -c disassembler.c

install: disassembler
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f disassembler $(DESTDIR)$(PREFIX)/bin/m64dasm
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sc64disassembler
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp m64dasm.1 $(DESTDIR)$(MANPREFIX)/man1/m64dasm.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/m64dasm.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/m64dasm \
		$(DESTDIR)$(MANPREFIX)/man1/m64dasm.1

clean:
	rm -f disassembler *.o *~
