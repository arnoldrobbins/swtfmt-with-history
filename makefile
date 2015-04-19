# makefile for swtfmt formatter

DESTDIR = /usr/local/bin
OWNER = bin
GROUP = bin
MANSEC=1
NROFF=s5nroff

HDR = chardefs.h fmt.h extern.h
SRC = main.c cmd.c fill.c io.c mac.c misc.c new.c
OBJ = main.o cmd.o fill.o io.o mac.o misc.o new.o
TARGET1 = swtfmt
TARGET2 = lz
TARGET3 = fos
TARGET4 = os
PRINTS = $(HDR) $(SRC) lz.c os.c fos.sh makefile

CFLAGS= -g -DUSG
P= pr

all: $(TARGET1) $(TARGET2) $(TARGET4)
	@echo all done

$(TARGET1): $(OBJ)
	$(CC) $(OBJ) -o $@

$(TARGET2): lz.c
	$(CC) $(CFLAGS) lz.c -o $@

$(TARGET4): os.c
	$(CC) $(CFLAGS) os.c -o $@

$(OBJ): $(HDR)

lint:
	lint $(SRC) | sort | $(P) -h fmt.lint | lpr

cxref:
	cxref -C $(HDR) $(SRC) | pr -h fmt.cxref | lpr
	cxref -C lz.c | pr -h lz.cxref | lpr
	cxref -C os.c | pr -h os.cxref | lpr

install: $(TARGET1) $(TARGET2) fos.sh $(TARGET4)
	cp $(TARGET1) $(DESTDIR)
	chown $(OWNER) $(DESTDIR)/$(TARGET1)
	chgrp $(GROUP) $(DESTDIR)/$(TARGET1)
	chmod 711 $(DESTDIR)/$(TARGET1)
	cp $(TARGET1).1 /usr/man/man$(MANSEC)/$(TARGET1).$(MANSEC)
	chmod 644 /usr/man/man$(MANSEC)/$(TARGET1).$(MANSEC)
	cp $(TARGET2) $(DESTDIR)
	chown $(OWNER) $(DESTDIR)/$(TARGET2)
	chgrp $(GROUP) $(DESTDIR)/$(TARGET2)
	chmod 711 $(DESTDIR)/$(TARGET2)
	cp $(TARGET2).1 /usr/man/man$(MANSEC)/$(TARGET2).$(MANSEC)
	chmod 644 /usr/man/man$(MANSEC)/$(TARGET2).$(MANSEC)
	cp fos.sh $(DESTDIR)/$(TARGET3)
	chown $(OWNER) $(DESTDIR)/$(TARGET3)
	chgrp $(GROUP) $(DESTDIR)/$(TARGET3)
	chmod 755 $(DESTDIR)/$(TARGET3)
	cp $(TARGET3).1 /usr/man/man$(MANSEC)/$(TARGET3).$(MANSEC)
	chmod 644 /usr/man/man$(MANSEC)/$(TARGET3).$(MANSEC)
	cp $(TARGET4) $(DESTDIR)
	chown $(OWNER) $(DESTDIR)/$(TARGET4)
	chgrp $(GROUP) $(DESTDIR)/$(TARGET4)
	chmod 711 $(DESTDIR)/$(TARGET4)
	cp $(TARGET4).1 /usr/man/man$(MANSEC)/$(TARGET4).$(MANSEC)
	chmod 644 /usr/man/man$(MANSEC)/$(TARGET4).$(MANSEC)
	cd macros.u; make install
	cd guide.u; make install

print:
	$(P) $(PRINTS) | lpr
	touch print2

print2: $(PRINTS)
	$(P) $? | lpr -b
	touch print2

man: swtfmt.1 lz.1 fos.1 os.1
	$(NROFF) -man swtfmt.1 lz.1 fos.1 os.1 | lpr

printall: print man
	@echo printing done

clean:
	rm -fr $(OBJ)

clobber: clean
	rm -fr $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) print2
