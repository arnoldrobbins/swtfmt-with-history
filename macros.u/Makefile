# makefile for swtfmt macros

FILES= ds_report envd envp evd evl evl2 evp evt gt_thesis ics4120 info \
       let numpar report ugh ugm ugnh
DESTDIR = /usr/local/lib/swtfmt

install: $(FILES)
	if test -d $(DESTDIR); then \
	rm -f $(DESTDIR)/*; \
	cp $(FILES) $(DESTDIR); \
	chmod 644 $(DESTDIR)/*; \
	else echo you must first do a mkdir $(DESTDIR); \
	fi
