include config.mk

default: disassembler

disassembler: disassembler.o
	$(CC) $(CFLAGS) -o disassembler disassembler.o

disassembler.o:
	$(CC) $(CFLAGS) -c disassembler.c

install: disassembler
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f disassembler $(DESTDIR)$(PREFIX)/bin/sc64disassembler
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sc64disassembler
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp sc64disassembler.1 $(DESTDIR)$(MANPREFIX)/man1/sc64disassembler.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/sc64disassembler.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/sc64disassembler \
		$(DESTDIR)$(MANPREFIX)/man1/sc64disassembler.1

clean:
	rm -f disassembler *.o *~
