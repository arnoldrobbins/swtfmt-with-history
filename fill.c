/*
** fill.c
**
** do filling of lines, underlining, and other stuff.
*/

#include "fmt.h"
#include "extern.h"

extern char mapdn();

#define ADDCHAR(c,s,i)	(s[i] = c, (i)++)


/* boface --- print a line in boldface */

int boface (buf, tbuf, size)
int size;
char *buf, *tbuf;
{
	int i, j, k, size_minus_1;

	j = 0;
	size_minus_1 = size - 1;
	for (i = 0; buf [i] != '\0'; i++)
	{
		if (buf [i] > ' ')	/* if character is printable */
			for (k = 1; k <= OVERSTRIKES; k++)
				if (j <= size_minus_1)
				{
					ADDCHAR (buf [i], tbuf, j);
					ADDCHAR ('\b', tbuf, j);
				}
				else
					break;
		if (j < size)
			ADDCHAR (buf [i], tbuf, j);
		else
			break;
	}

	tbuf [j] = '\0';

	return (j);
}



/* do_brk --- end current filled line */

do_brk()
{
	if (Nobreak == YES)
		return;

	if (Outp > 0)
	{
		do {
			Outp--;
			Outw--;
		}
		while ((Outbuf[Outp] == ' ' || Outbuf[Outp] == PHANTOMBL)
			&& Outp >= 0);

		Outp++;
		Outw++;

		if (Adjust == 'r')
			Tival = Rmval - Lmval - Outw + 1;
		else if (Adjust == 'c')
			Tival = (Rmval - Lmval - Outw + 1) / 2;
		Outbuf [Outp] = '\n';
		Outbuf [Outp + 1] = '\0';
		put (Outbuf);
	}

	Outp = 0;
	Outw = 0;
	Outwds = 0;
	Word_last = NO;
}



/* center --- center a line by setting tival */

center (buf)
char *buf;
{
	int width();

	tailbl (buf);
	Tival = max ((Rmval - Lmval - width (buf) + 1) / 2, 0);
}

/* max --- return the maximum of two integers */

max(a, b)
int a, b;
{
	return (a >= b) ? a : b;
}


/* divok --- returns OK if word is divisible at i, i - 1 */

int divok (word, i)
char *word;
int i;
{
	int k1, k2, k3, j, count1, count2;
	int suffix();
	char *index();
	char lc, rc, c;
	int ret;
	static short int lck [2][26] = {
		000000,
		073555,
		073155,
		072575,
		000000,
		071555,
		073171,
		073575,
		000000,
		073575,
		073575,
		073575,
		073575,
		052565,
		000000,
		073155,
		073575,
		073524,
		053114,
		053175,
		000000,
		073575,
		073155,
		073575,
		000000,
		073575,
		000000,
		133500,
		133500,
		133500,
		000000,
		123500,
		133500,
		173500,
		000000,
		173500,
		173500,
		171500,
		173500,
		143500,
		000000,
		133500,
		173500,
		123500,
		161500,
		112500,
		000000,
		173500,
		133500,
		173500,
		000000,
		173500
	};


	ret = ERR;
	count1 = 0;
	for (j = 0; j <= i; j++)
	{
		c = word [j];
		if (c == '-' || c == '/' || c == '_' || c == '<' || c == '>'
		    || c == hlf || c == hlr)
			return ret;
		if (isalpha (c))
			count1++;
	}

	count2 = 0;
	for (j = i + 1; word [j] != '\0'; j++)
	{
		c = word [j];
		if (c == '-' || c == '/' || c == '_' || c == '<' || c == '>'
		    || c == hlf || c == hlr)
			return ret;
		if (isalpha (c))
			count2++;
	}

	if (count1 < 3 || count2 < 3)
		return ret;	/* must be >= 3 letters on each side of division */

	for (j = 0; ; j++)
	{
		/* check for vowel in each side */
		if (index ("eaoiuyEAOIUY", word [j]) != NULL)
			break;
		else if (j >= i - 1)
			return ret;
	}

	for (j = i; ; j++)
	{
		if (index ("eaoiuyEAOIUY", word [j]) != NULL)
			break;
		else if (word [j] == '\0')
			return ret;
	}

	if (suffix (& word [i]) == YES && word [i - 2] != word [i - 1])
	{
		ret = OK;
		return ret;
	}

	lc = mapdn (word [i - 1]);
	rc = mapdn (word [i]);

	if (! isalpha (lc) || ! isalpha (rc))
		return ret;

	k1 = lc - 'a';
	k2 = (rc - 'a') / BITS_PER_WORD;
	k3 = (rc - 'a') %  BITS_PER_WORD + 1;
	if (((lck[k2][k1] >> (BITS_PER_WORD - k3)) & 1) != 1)
		return ret;   /* division cannot occur between these letters */

	ret = OK;

	return ret;
}



/* getwrd --- get non-blank word from in (i) into  out, increment i */

int getwrd (in, i, out)
char in [], out [];
int *i;
{
	int j;
	char c;

	while ((in [*i] == ' ' || in [*i] == '\t') && in [*i] != Tabch)
		(*i)++;

	j = 0;
	if (in [*i] == Tabch)
	{
		out [j] = Tabch;
		(*i)++;
		j++;
	}
	else
	{
		for (;;)
		{
			c = in [*i];
			if (c == ' ' || c == '\t' || c == Tabch || c == '\n'
					|| c == '\0')
				break;
			out [j] = c;
			(*i)++;
			j++;
		}
	}

	out [j] = '\0';
	return j;
}



/* hynate --- divide a word for hyphenation */

int hynate (word, room, left, right)
char *word, *left, *right;
int room;
{
	int i, j, l;
	int divok(), length(), width();
	int ret;

	ret = Hyphenation;
	if (ret == NO)
		return ret;

	l = length (word);
	for (i = l - 2; i >= 2; i--)
		if (divok (word, i) == OK)
		{
			for (j = 0; j < i; j++)
				left [j] = word [j];
			left [j] = '-';
			left [j + 1] = '\0';
			if (width (left) <= room)
			{
				strcpy (right, &word[i]);
				return ret;
			}
		} 
		else if (word [i] == '-')   /* attempt to divide on hyphen */
		{
			if (l - i >= 2)
			{
				/* make sure word is long enough */
				for (j = 0; j < i; j++)
					left [j] = word [j];
				left [j] = '\0';
				if (width (left) <= room)
				{
					strcpy (right,  &word[i + 1]);
					return ret;
				}
			}
		}

	ret = NO;

	return ret;
}



/* leadbl --- delete leading blanks, set tival */

leadbl (buf)
char *buf;
{
	int i, j;

	do_brk();
	for (i = 0; buf [i] == ' '; i++)   /* find 1st non-blank */
		;
	if (buf [i] != '\n')
		/* Tival = i - 1 + Inval; */	/* originally this way... */
		Tival = i + Inval - 1 + 1;	/* add 1 since i is 0-based */
						/* compiler will fold constants */
	for (j = 0; buf [i] != '\0'; j++, i++)	/* move line to left */
		buf [j] = buf [i];
	buf [j] = '\0';
}



/* nxblnk --- determine if an extra blank is needed after a word */

int nxblnk (buf, i)
char *buf;
int i;
{
	int ret;

	ret = Extra_blank_mode;
	if (ret == NO)
		return ret;
	
	switch (buf[i]) {
	case '.':		/* does word have sentence punctuation */
	case '!':
	case '?':
	case ':':
		ret = YES;
		break;
	
	case '"':			/* or is it enclosed, */
	case '\'':
	case ')':
	case ']':
	case '}':
	case '>':
		switch (buf[i - 1]) {
		case '.':		/* with punctuation inside? */
		case '!':
		case '?':
		case ':':
			ret = YES;
			break;

		default:
			ret = NO;
			break;
		}
		break;

	default:
		ret = NO;
		break;
	}

	return ret;
}



/* pfoot --- put out page footer */

pfoot()
{
	char title [MAXOUT];

	skip (M3val);
	if (M4val > 0)
	{
		if ((Curpag & 1) == 0)  /* this is an even page */
			massin (Even_footer, title, MAXOUT);
		else
			massin (Odd_footer, title, MAXOUT);
		puttl (title, Curpag);
		skip (M4val-1);
	}
	Lineno = 0;
	Nospace = YES;
}


#ifdef USG
#include <termio.h>
#else
#include <sgtty.h>
#endif

/* phead --- put out page header */

phead()
{
	char title [MAXOUT];
#ifdef USG
	struct termio buf;
#else
	struct sgttyb buf;
#endif

	Curpag = Newpag;	/* these lines moved to effect */
	Newpag++;		/* starting/ending page options */
	Lineno = M1val + M2val + 1;
	CHECK_PAGE_RANGE;
	if (Stop_mode == YES)
	{
		static FILE *tty = NULL;

		if (tty == NULL)
			if ((tty = fopen("/dev/tty", "r")) == NULL)
				error ("couldn't open terminal for reading");
			else
			{
#ifdef USG
				ioctl (fileno (tty), TCGETA, & buf);
				buf.c_lflag |= ISIG|NOFLSH;
				buf.c_lflag &= ~(ICANON|XCASE|ECHO|ECHOE|ECHOK
						|ECHONL);
				ioctl (fileno (tty), TCSETAW, & buf);
#else
				gtty (fileno (tty), & buf);
				buf.sg_flags |= CBREAK;
				buf.sg_flags &= ~ ECHO;
				stty (fileno (tty), & buf);
#endif
			}

		putc (BELL, stderr);
		getc (tty);
	}
	if (M1val > 0)
	{
		skip (M1val-1);
		if ((Curpag  & 1) == 0)  /* this is an even-numbered page */
			massin (Even_header, title, MAXOUT);
		else
			massin (Odd_header, title, MAXOUT);
		puttl (title, Curpag);
	}

	skip (M2val);
}



/* put --- put out line with proper spacing and indenting */

put (buf)
char *buf;
{
	int i, j, w, numbl, offset;
	char buffer [MAXOUT];

	Nospace = NO;
	if (Lineno == 0 || Lineno > Bottom)
		phead ();

	if (Curpag >= Start_page && Curpag <= End_page)	/* if printing */
	{
		/*
		 * use buffered i/o to handle page offset,
		 * margin, and indentation:
		 */
		offset = Poval;
		if ((Curpag & 1) != 0)  /* an odd page */
			offset += Ooval;
		else
			offset += Eoval;
		numbl = offset + Lmval + Tival + Moval - 1;
		for (i = 0; i < numbl; i++)
			buffer [i] = ' ';
		buffer [i] = '\0';
		if (Moval > 0)              /* set the marginal character */
			buffer [offset] = Tmcch;
		if (numbl > 0)
			putlin (buffer, stdout);
	}

	Tival = Inval;
	Tmcch = Mcch;
	Mcout = YES;
	w = 0;
	j = 0;

	if (Curpag >= Start_page && Curpag <= End_page)
	{
		for (i = 0; buf [i] != '\0'; i++)
		{
			if (buf [i] == Tabch)
			{
				do {
					buffer [j] = Replch;
					j++;
					w++;
				} while(Tabs [w] != YES && w <= MAXLINE);
			} 
			else
			{
				if (buf [i] == '\b')
					w--;
				else
					w++;
				buffer [j] = buf [i];
				j++;
			}
		}
		buffer [j] = '\0';
		putlin (buffer, stdout);
	}
	skip (min (Lsval-1, Bottom  - Lineno));
	Lineno += Lsval;
	if (Lineno > Bottom)
		pfoot ();
}



/* min --- return min of two integers */

min(a, b)
int a, b;
{
	return (a <= b) ? a : b;
}




/* puttab --- evaluate tabs into outbuf */

puttab()
{
	int i, llval;

	llval = Rmval - Tival - Lmval + 1;
	for (i = 0; i < Outp; i++) /* fix output to this point (no filling) */
		if (Outbuf [i] == ' ')
			Outbuf [i] = PHANTOMBL;

	do {
		if (Outw >= llval)
			break;
		Outbuf [Outp] = Replch;
		Outp++;
		Outw++;
	} while(Tabs [Outw+1] != YES);

	Outwds = 1;
	Word_last = NO;
}



/* puttl --- put out title line with optional page number */

puttl (buf, pageno)
char *buf;
int pageno;
{
	char title [MAXOUT];
	int i;

	CHECK_PAGE_RANGE;
	i = Poval + Moval + Lmval - 1;
	if ((Curpag &  1) != 0)
		i += Ooval;
	else
		i += Eoval;

	while (i--)
		putchar (' ');

	mktl (buf, pageno, title, MAXOUT);
	putlin (title, stdout);
}



/* putw --- insert a word in outbuf, bump counters */

putw (word, ww, wl)
char *word;
int ww, wl;
{
	int nxblnk();

	if (Word_last == YES || Outp == 0)
		Outwds++;
	strcpy(& Outbuf[Outp], word);
	Outp += wl;
	Outw += ww;
	if (nxblnk (Outbuf, Outp - 1) == YES)
	{
		Outbuf [Outp] = PHANTOMBL;
		Outp++;
		Outw++;
	}
}



/* putwrd --- put a word in outbuf, handle tabs */

putwrd (wrdbuf)
char *wrdbuf;
{
	char left [INSIZE], right [INSIZE];
	int ww, llval, room;

	if (wrdbuf [0] == Tabch)
		puttab();
	else
	{
		llval = Rmval - Tival - Lmval + 1;
		ww = width (wrdbuf);
		if (Word_last == YES)
		{
			Outbuf [Outp] = ' ';    /* blank between words */
			Outp++;
			Outw++;
		}
		room = llval - Outw;
		if (ww <= room)
			putw (wrdbuf, ww, length (wrdbuf));
		else if (hynate (wrdbuf, room, left, right) == YES)
		{
			putw (left, width (left), length (left));
			if (Adjust == BOTH)
				spread (Outbuf, & Outp, llval - Outw, Outwds);
			do_brk();
			putw (right, width (right), length (right));
		}
		else
		{
			/* no room for word, and can't hyphenate */
			do {
				Outp--;
				Outw--;
			} while (Outp >= 0 && (Outbuf [Outp] == ' '
				|| Outbuf [Outp] == PHANTOMBL));

			Outp++;
			Outw++;

			if (Adjust == BOTH)
				spread (Outbuf, & Outp, llval - Outw, Outwds);
			do_brk();
			putw (wrdbuf, ww, length (wrdbuf));
		}
		Word_last = YES;
	}
}



/* set --- set parameter and check range */

set (param, val, argtyp, defval, minval, maxval)
int *param, val, argtyp, defval, minval, maxval;
{
	if (argtyp == '\n')     /* defaulted */
		*param = defval;
	else if (argtyp == '+')     /* relative + */
		*param += val;
	else if (argtyp == '-')     /* relative - */
		*param -= val;
	else                       /* absolute */
		*param = val;
	if (*param > maxval)
		*param = maxval;
	else if (*param < minval)
		*param = minval;
}



/* settab --- set tab stops after a .ta command */

settab (buf)
char *buf;
{
	int i, n, l;

	l = 1;
	for (i = 1; i <= MAXLINE; i++)
		Tabs [i] = NO;
	for (i = 0; buf [i] != ' ' && buf [i] != '\t' && buf [i] != '\n'; i++)
		;
	for (;;)
	{
		n = ctoi (buf, & i);
		if (n == 0)
			if (buf [i] == '\n')
				break;
			else
			{
				i++;
				continue;
			}
		if (n >= 1 && n <= MAXLINE)
		{
			Tabs [n] = YES;
			if (l <= n)
				l = n;
		}
	}

	for (; l <= MAXLINE; l++)
		Tabs [l] = YES;
}



/* space --- space  n  lines or to bottom of page */

space (n)
int n;
{
	if (Lineno > Bottom)
		return;
	if (Lineno == 0)
		phead ();
	skip (min (n, Bottom + 1 - Lineno));
	Lineno += n;
	if (Lineno > Bottom)
		pfoot ();
}



/* spread --- spread words to justify right margin */

spread (buf, outp, nextra, outwds)
char *buf;
int *outp, nextra, outwds;
{
	static int dir = 0;
	int i, j, nb, num_e, nholes;

	if (nextra <= 0 || outwds <= 1)
		return;
	dir = 1 - dir;   /* reverse previous direction */
	num_e = nextra;
	nholes = outwds - 1;
	i = *outp - 1;
	j = min (MAXOUT-3, i+num_e);   /* leave room for NEWLINE, EOS */
	while (i < j)
	{
		buf [j] = buf [i];
		if (buf [i] == ' ')
		{
			if (dir == 0)
				nb = (num_e-1) / nholes + 1;
			else
				nb = num_e / nholes;
			num_e -= nb;
			nholes--;
			for ( ; nb > 0; nb--)
			{
				j--;
				buf [j] = ' ';
			}
		}
		i--;
		j--;
	}
	*outp += nextra;
}



/* suffix --- return YES if str is a suffix, NO otherwise */

int suffix (str)
char *str;
{
	int i;
	char lcstr [MAXLINE];
	static char *suffixes[] = { "ing", "tion", NULL	};

	for (i = 0; str [i] != '\0'; i++)
		lcstr [i] = mapdn (str[i]);
	lcstr [i] = '\0';

	/*
	 * should use binary search, but since only comparing
	 * against two suffixes (currently), can get away with
	 * a simple linear search.
	 */

	for(i = 0; suffixes[i] != NULL; i++)
	{
		if (strcmp(lcstr, suffixes[i]) == 0)
			return(YES);
	}

	return(NO);
}



/* italic --- italicize a line */

int italic (buf, tbuf, size)
int size;
char *buf, *tbuf;
{
	int i, j, size_minus_1;

	j = 0;
	size_minus_1 = size - 1;
	for (i = 0; buf [i] != '\0'; i++)
	{
		if (buf [i] != Tabch && buf [i] >= ' ' && ! isspace (buf [i]))
		{
			if (j <= size_minus_1)
			{
				ADDCHAR (buf [i], tbuf, j);
				ADDCHAR ('\b', tbuf, j);
			}
			else
				break;
		}
		if (j < size)
			if (isspace (buf[i]))
				ADDCHAR (buf [i], tbuf, j);
			else
				ADDCHAR ('_', tbuf, j);
		else
			break;
	}

	tbuf [j] = '\0';

	return j;
}



/* underl --- underline a line */

int underl (buf, tbuf, size, cflag)
int size, cflag;
char *buf, *tbuf;
{
	int i, j, size_minus_1;
	char *index();
	char *schars = "#$%&'=-~^@+*";

	j = 0;
	size_minus_1 = size - 1;
	for (i = 0; buf [i] != '\0'; i++)
	{
		if (buf [i] != Tabch && buf [i] >= ' '
		    && (cflag != NO || isalnum (buf [i])
			|| index (schars, buf [i]) != NULL))
		{
			if (j <= size_minus_1)
			{
				ADDCHAR ('_', tbuf, j);
				ADDCHAR ('\b', tbuf, j);
			}
			else
				break;
		}
		if (j < size)
			ADDCHAR (buf [i], tbuf, j);
		else
			break;
	}

	tbuf [j] = '\0';

	return j;
}
