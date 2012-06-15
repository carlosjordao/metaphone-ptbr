

main:
	rm -f ../postgresql/*.{o,so}
	gcc -DPT_METAPHONE_MAIN -o metaphone_ptbr metaphone_ptbr.c

clean:
	rm -f metaphone_ptbr *.o

distclean:
	rm -f metaphone_ptbr *.o


install: main
	install -D metaphone_ptbr $(DESTDIR)/usr/bin/metaphone_ptbr

uninstall:
	rm -f $(DESTDIR)/usr/bin/metaphone_ptbr
