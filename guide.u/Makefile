# makefile for swtfmt guide

FILES = alpha_commands applications basics conditionals filling \
	foreword func_commands guide headings input macros margins \
	misc packages spacing tabs title_page

DESTDIR= /usr/doc/swtfmt
EV=/usr/local/lib/swtfmt/evp # line printer on vax
FGUIDE = fguide

install: $(FILES)
	if test -d /usr/doc ; \
	then if test -d $(DESTDIR) ; \
	then cp $(FILES) $(DESTDIR); \
	swtfmt $(EV) guide > $(DESTDIR)/$(FGUIDE) ; \
	chmod 644 $(DESTDIR)/* ; \
	else echo you must first mkdir $(DESTDIR) ; \
	fi ; \
	else echo you must first mkdir /usr/doc ; \
	fi
