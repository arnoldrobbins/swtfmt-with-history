
From:     robert@gitpyr
Received: by gitpyr.UUCP (4.12/4.7)
	id AA29686; Fri, 1 Mar 85 22:51:50 est
Date:     Fri, 1 Mar 85 22:51:50 est
From:     gitpyr!robert (Robert Viduya)
Message-Id: <8503020351.AA29686@gitpyr.UUCP>
To:       arnold
Subject:  dprint
Via:  Gitpyr; 1 Mar 85 22:58-EDT

here it is.
#include <stdio.h>
#include <strings.h>
#include <varargs.h>
#include <sgtty.h>

/*
 * sprint --- perform bi-directional printing on a Spinwriter
 */

#define	MAXBUF		1024
#define	ETX		'\3'
#define	ACK		'\6'
#define	BEL		'\7'
#define	HT		'\t'
#define	VT		'\13'
#define	FF		'\14'
#define	ESC		'\33'
#define	QUIT		'\177'
#define	poll()		write(spf,pollseq,sizeof pollseq)
#define	isprint(c)	((c)>=' ' && (c)<'\177')

#define	D_FWD		'5'
#define	D_BWD		'6'
#define	D_MAXNCP	63

#define	S_FWD		'>'
#define	S_BWD		'<'
#define	S_MAXNCP	127

int     currow,		/* current vertical position of print head */
        curcol,		/* current horizontal position of print head */
        bscol,		/* column number of first char in buf */
        becol,		/* column number of last char in buf */
        blen,		/* number of chars in buf */
        ncp,		/* number of chars written since last poll */
        maxncp,		/* max number of chars to write between polls */
        nlc,		/* number of pending newlines */
        plen,		/* number of lines per page */
        quit,		/* a QUIT character has been typed on Spinwriter */
        sflg,		/* stop between pages */
        dflg,		/* Diablo flag */
        inf,		/* input file descriptor */
        spf;		/* Spinwriter file descriptor */

char    dir,		/* current print direction */
        fwd,		/* forward print character */
        bwd,		/* backward print character */
        buf[MAXBUF];	/* line buffer */

#ifdef DEBUG
char    pollseq[] = {
    '<', 'E', 'T', 'X', '>'
};
char    ding[] = {
    '<', 'B', 'E', 'L', '>'
};
#else
char    pollseq[] = {
    ETX
};
char    ding[] = {
    BEL
};
#endif
char   *spindev = "/dev/tty";

struct sgttyb   sgttyb;
int     old_sg_flags;

main (argc, argv)
int     argc;
char   *argv[];
{
    int     ds, de;
    register int    nb;
    register char  *bp;

    setup (argc, argv);

    while (getbuf ()) {
	if (blen == 0) {
	    put ('\n');
	    continue;
	}
	ds = abs (curcol - bscol);
	de = abs (curcol - becol);
	if (ds < de || ds == de && dir == fwd) {
	    pfwd ();
	    hskip (bscol - curcol);
	    for (bp = &buf[1]; *bp; bp++) {
		for (nb = 0; *bp == ' ' || *bp == '\b'; bp++)
		    nb += (*bp == ' ') ? 1 : -1;
		hskip (nb);
		put (*bp);
	    }
	}
	else {
	    pbwd ();
	    hskip (curcol - becol);
	    for (bp = &buf[blen]; *bp; bp--) {
		for (nb = 0; *bp == ' ' || *bp == '\b'; bp--)
		    nb += (*bp == ' ') ? 1 : -1;
		hskip (nb);
		put (*bp);
	    }
	}
	put ('\n');
    }
    cleanup ();
}


getbuf ()
{
    register    len, col, scol;
    char    c;

    if (quit)
	return (0);

    len = col = 0;
    for (;;) {
	if (read (inf, &c, 1) != 1)
	    return (0);
	if (c == '\t')
	    col |= 7;
	else if (c != ' ')
	    break;
	col++;
    }
    bscol = col + 1;
    scol = col;
    while (c != '\n') {
	while (col < scol) {
	    buf[++len] = ' ';
	    col++;
	}
	buf[++len] = c;
	if (isprint (c))
	    col++;
	else if (c == '\b')
	    col--;
	scol = col;
	for (;;) {
	    if (read (inf, &c, 1) != 1)
		return (0);
	    if (c == '\t')
		scol |= 7;
	    else if (c != ' ')
		break;
	    scol++;
	}
    }
    becol = col;
    blen = len;
    buf[++len] = '\0';
    return (1);
}


put (c)
char    c;
{
    if (c == '\n') {
	nlc++;
	if (nlc + currow <= plen)
	    return;
	c = FF;
    }
    if (c == FF) {
	if (++ncp > maxncp) {
	    poll ();		/* mark end of current chunk */
	    wait ();		/* wait for previous chunk to print */
	    ncp = 1;
	}
#ifdef DEBUG
	write (spf, "<FF>", 4);
#else
	write (spf, &c, 1);
#endif
	currow = 1;
	nlc = 0;
	if (sflg) {
	    wait ();		/* wait for last full chunk to print */
	    write (spf, ding, sizeof ding);
	    wait ();		/* wait for user to type an ACK */
	    poll ();		/* mark end of (empty) chunk */
	    ncp = 0;
	}
	return;
    }
    else if (c == '\r') {
	curcol = 1;
	dir = fwd;
    }
    else if (c == '\b')
	curcol -= (dir == fwd) ? 1 : -1;
    else if (c == EOF) {
	poll ();	/* mark end of current chunk */
	wait ();	/* wait for previous chunk to print */
	wait ();	/* wait for current chunk to print */
	return;
    }
    else if (!isprint (c))
	return;
    if (nlc) {
	vskip (nlc);
	nlc = 0;
    }
    if (curcol > 0) {
	if (++ncp > maxncp) {
	    poll ();		/* mark end of current chunk */
	    wait ();		/* wait for previous chunk to print */
	    ncp = 1;
	}
	write (spf, &c, 1);
    }
    if (isprint (c))
	if (dir == fwd)
	    curcol++;
	else
	    curcol--;
}


/*VARARGS1*/
putctl (ac, va_alist)
va_dcl
{
    va_list	pvar;
    char	buf, *bufptr;

    if (ncp + ac > maxncp) {	/* not enough room in current chunk */
	poll ();		/* mark end of current chunk */
	wait ();		/* wait for previous chunk to print */
	ncp = 0;
    }
    ncp += ac;
    bufptr = &buf;
    va_start (pvar);
    while (ac-- > 0) {
	buf = va_arg (pvar, char);
	write (spf, bufptr, 1);
    }
    va_end (pvar);
}


hskip (d)
{
    register    newcol;
    register char   mc;

    if (d == 0)
	return;

    newcol = (dir == fwd) ? curcol + d : curcol - d;
    mc = ' ';
    if (d < 0) {
	mc = '\b';
	d = -d;
    }
#ifndef DEBUG
    if (d < 3)
#endif
	do
	    put (mc);
	while (--d);
#ifndef DEBUG
    else {
	if (dflg)
	    putctl (3, ESC, HT, newcol);
	else
	    putctl (3, ESC, ((newcol - 1) >> 5) + 'P',
		    ((newcol - 1) & 037) + '@');
	curcol = newcol;
    }
#endif
}


vskip (d)
{
    if (d == 0)
	return;
    currow += d;
#ifdef DEBUG
    while (d-- > 0)
	putctl (1, '\n');
#else
    if (d < 3)
	putctl (d, '\n', '\n');
    else if (dflg)
	putctl (3, ESC, VT, currow);
    else {
	while (d > 63) {
	    putctl (3, ESC, ']', '_');
	    d -= 63;
	}
	putctl (3, ESC, ((d >> 5) & 1) + 'Z', (d & 037) + '@');
    }
#endif
}


pfwd ()
{
    if (dir != fwd) {
	dir = fwd;
#ifdef DEBUG
	putctl (5, '<', 'F', 'W', 'D', '>');
#else
	putctl (2, ESC, dir);
#endif
    }
}


pbwd ()
{
    if (dir != bwd) {
	dir = bwd;
#ifdef DEBUG
	putctl (5, '<', 'B', 'W', 'D', '>');
#else
	putctl (2, ESC, dir);
#endif
    }
}


wait ()
{
    char    c;
    static char ctl[2] = {
	'^', '\0'
    };

    for (;;) {
	if (read (spf, &c, 1) != 1) {
	    write (2, "\r\n\nError reading ACK\r\n\n", 23);
	    quit++;
	    c = ACK;
	}
	switch (c &= 0x7F) {
	    case ACK: 
		return;
	    case QUIT: 
		quit++;
		break;
#ifdef DEBUG
	    default: 
		if (c >= ' ')
		    write (2, &c, 1);
		else {
		    ctl[1] = c + '@';
		    write (2, ctl, 2);
		}
#endif
	}
    }
}


setup (argc, argv)
int     argc;
char   *argv[];
{
    char   *usage = "usage: sprint [-d] [-s] [-o dev] [file]\n";
    char   *ar;

    inf = 0;			/* read the standard input */
    ar = argv[0];
    if (index (ar, '/'))
	ar = rindex (ar, '/') + 1;
    if (*ar == 'd') {	/* invoked as dprint */
	dflg++;
	usage[7] = 'd';
    }
    while (--argc > 0) {
	++argv;
	if (**argv == '-')
	    switch (argv[0][1]) {
		case 'd': 
		    dflg++;
		    break;
		case 's': 
		    sflg++;
		    break;
		case 'o': 
		    if (argv[0][2])
			spindev = &argv[0][2];
		    else
			spindev = *++argv;
		    break;
		default: 
		    fputs (usage, stderr);
		    exit (1);
	    }
	else {
	    if ((inf = open (*argv, 0)) < 0) {
		perror (*argv);
		exit (1);
	    }
	    break;
	}
    }

    if ((spf = open (spindev, 2)) == -1) {
	perror (spindev);
	exit (1);
    }

    if (dflg) {
	maxncp = D_MAXNCP;
	fwd = D_FWD;
	bwd = D_BWD;
    }
    else {
	maxncp = S_MAXNCP;
	fwd = S_FWD;
	bwd = S_BWD;
    }

    gtty (spf, &sgttyb);
    old_sg_flags = sgttyb.sg_flags;
/*    sgttyb.sg_flags = ANYP | RAW; */
    sgttyb.sg_flags = CBREAK | TANDEM | EVENP;
    stty (spf, &sgttyb);
    currow = curcol = 1;
    plen = 66;			/* make this optional some day */
    poll ();			/* mark end of first (empty) chunk */
    put ('\r');			/* place print head in known state */
    put (FF);
}


cleanup ()
{
    put ('\r');			/* flush out pending newlines */
    if (currow > 1)
	put (FF);
    put (EOF);			/* won't return till all chars have been printed */

    sgttyb.sg_flags = old_sg_flags;
    stty (spf, &sgttyb);
}

