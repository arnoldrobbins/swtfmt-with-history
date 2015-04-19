/*
** mac.c
**
** do macros for formatter
*/

#include "fmt.h"
#include "extern.h"

#define printable(c)	(isprint(c) || c == ' ')

/* apndmac --- append to a macro */

apndmac (buf)
char *buf;
{
	char macnam[3], *mactext, *getmac();
	char newtext[MACBUFSIZE];
	int i, j;

	getnam (buf, macnam);

	mactext = getmac(macnam);
	if (mactext == NULL)
	{
		char msg[BUFSIZ];

		sprintf (msg, "macro (%s) cannot be appended, %s\n",
			macnam, "it does not exist.");
		error (msg);
	}

	macinp (macnam);	/* read in additional text */

	for (i = 0; mactext[i] != EOF; i++)
		;	/* find end of original macro */
	mactext[i] = '\0';	/* remove EOF which marked the end */

	/* put together */
	for (j = 0; j <= i; j++)
		newtext[j] = mactext[j];

	for (j = 0; Macbuf[j] != EOF; j++, i++)
		newtext[i] = Macbuf[j];
	
	newtext[i] = EOF;
	newtext[++i] = '\0';


	addmac (macnam, newtext, i);	/* put it into macro table */
}


/* defmac --- define a macro */

defmac (buf)
char *buf;
{
	char macnam[3];

	getnam (buf, macnam);	/* get name */

	macinp (macnam);	/* get body */

	addmac (macnam, Macbuf, Mactop - 1);	/* install it */
}


/* macinp --- read in the body of a macro */

macinp (macnam)
char *macnam;
{
	char ennam [3], line [MAXOUT];
	int len;
	int readln();

	Mactop = 0;		/* save beginning of macro text */

	for (;;)
	{
		len = readln (line, MAXOUT);
		if (len == EOF)
		{
			reset_files();
			putlin (macnam, stderr);
			error (": missing end of definition");
		}

		if (line [0] == Cmdch && line [1] == 'e' && line [2] == 'n')
		{
			getnam (line, ennam);
			if (macnam [0] == ennam [0] && macnam [1] == ennam [1])
				break;
		}

		if (len >= MACBUFSIZE)
		{
			reset_files();
			putlin (macnam, stderr);
			error (": macro body too large");
		}

		/* put macro text in the buffer */
		strcpy (& Macbuf[Mactop], line);
		Mactop += strlen(line) + 1;
	}

	Macbuf [Mactop] = EOF;		/* marks end of macro */
	Macbuf [++Mactop] = '\0';
	Mactop++;
}



/* eval_fcn --- evaluate a function call */

int eval_fcn (fbuf, buf, maxbuf)
char *fbuf, *buf;
int maxbuf;
{
	char str [MAXLINE], fcn_buf[MAXOUT], tempstr[MAXOUT], name [MAXLINE];
	char str1[MAXLINE], str2[MAXLINE], rel[4];
	int i, j, k, nr, amount, fnum, val;
	int opnd1, opnd2, r, v, s1, s2;
	int itoc(), ctoi(), gctoi(), lookup(), ctoc(), itorm(), trnarg();
	int underl(), boface(), itolet();
	char *index(), mapup();
	int ret;

	static char *relops[] = {
			"<=", "=<", "<", "==", "=", "~=",
			"<>", "><", ">=", "=>", ">", NULL
			};
	
	static int rel_vals[][2] = { 
		1,	2,	/* <= */
		1,	2,	/* =< */
		1,	0,	/* < */
		2,	0,	/* == */
		2,	0,	/* = */
		1,	3,	/* ~= */	/* < and > are also != */
		1,	3,	/* <> */
		1,	3,	/* >< */
		2,	3,	/* >= */
		2,	3,	/* => */
		3,	0	/* > */
		};
	
	static int vert[] = {
		020, 021, 022, 023, 024, 025, 026, 027,
		030, 031, 032, 033, 034, 035, 036, 037
		};

	massin (fbuf, fcn_buf, MAXOUT);    /* evaluate nested calls */
	i = 0;
	SKIPBL (fcn_buf, i);
	for (j = 0; fcn_buf [i] != '\0' && fcn_buf [i] != ' '; j++, i++)
		name [j] = fcn_buf [i];

	name [j] = '\0';

	j = 0;               /* check for macro argument call: [<integer>] */
	nr = ctoi (name, & j);
	if (name [j] == '\0' && Maclvl > 1)
		return trnarg (nr, buf, maxbuf);

	SKIPBL (fcn_buf, i);
	if (lookup (name, & fnum, Functions) == NO)   /* not a function name */
	{
		if (lookup (name, & fnum, Spchars) == YES)
		{
			/* special character */
			j = ctoi (fcn_buf, & i);  /* get repeat count, if any */
			j = min (max (1, j), maxbuf - 1); /* apply limits */
			for (k = 0; k < j; k++)
				buf [k] = fnum;
			buf [j] = '\0';
			return (j);
		}
		else	/* neither a function name nor a special character */
		{
			buf [0] = '[';
			j = ctoc(fcn_buf, &buf[1], maxbuf - 2) + 1;
			buf [j] = ']';
			buf [j + 1] = '\0';
			return (++j);
		}
	}
	else	/* it's a function name */
	{
		switch (fnum) {   /* these functions yield a numeric result */

		case ns_FN:
			val = Nospace;
			goto ifany2;
			break;

		case pl_FN:
			val = Plval;
			goto ifany2;
			break;

		case pn_FN:
		case tcpn_FN:
			val = Curpag;
			goto ifany2;
			break;

		case ln_FN:
			val = Lineno;
			goto ifany2;
			break;

		case po_FN:
			val = Poval;
			goto ifany2;
			break;

		case ls_FN:
			val = Lsval;
			goto ifany2;
			break;

		case in_FN:
			val = Inval;
			goto ifany2;
			break;

		case rm_FN:
			val = Rmval;
			goto ifany2;
			break;

		case ti_FN:
			val = Tival;
			goto ifany2;
			break;

		case lm_FN:
			val = Lmval;
			goto ifany2;
			break;

		case ml_FN:
			val = Maclvl;
			goto ifany2;
			break;

		case m1_FN:
			val = M1val;
			goto ifany2;
			break;

		case m2_FN:
			val = M2val;
			goto ifany2;
			break;

		case m3_FN:
			val = M3val;
			goto ifany2;
			break;

		case m4_FN:
			val = M4val;
			goto ifany2;
			break;

		case lt_FN:
			val = Tiwidth;
			goto ifany2;
			break;

		case plus_FN:
			opnd1 = gctoi (fcn_buf, &i, 10);
			opnd2 = gctoi (fcn_buf, &i, 10);
			val = opnd1 + opnd2;
			goto ifany2;
			break;

		case minus_FN:
			opnd1 = gctoi (fcn_buf, &i, 10);
			opnd2 = gctoi (fcn_buf, &i, 10);
			val = opnd1 - opnd2;
			goto ifany2;
			break;

		case bottom_FN:
			val = Bottom;
			goto ifany2;
			break;

		case top_FN:
			val = M1val + M2val + 1;
			goto ifany2;
			break;

		case num_FN:
			switch (fcn_buf[i]) {
			case '+':
				amount = 1;
				goto ifany1;
				break;

			case '-':
				amount = -1;
				/* fall thru into the ifany */
			ifany1:
				i++;
				break;
			default:
				amount = 0;
			}
			nr = ctoi (fcn_buf, & i);
			if (nr >= 1 && nr <= MAXNUMREGS)
			{
				Numreg [nr] += amount;  /* pre-incr or -decr */
				val = Numreg [nr];
				switch (fcn_buf[i]) {
				case '+':
					Numreg [nr]++;
					break;

				case '-':
					Numreg [nr]--;
					break;
				}
			}
			else	/* register number out of range, use 0 */
				val = 0;
			goto ifany2;
			break;

		default:
			goto switch2;
		}

	ifany2:
		k = itoc (val, str, 32);
		if (k < 4 && fnum == tcpn_FN)    /* right justify tcpn */
			for (j = 4; j >= 0; j--, k--)
				if (k >= 0)
					str [j] = str [k];
				else
					str [j] = PHANTOMBL;
		ret = ctoc (str, buf, maxbuf);
		goto end_of_proc;


switch2: 
		switch (fnum) {  /* these functions yield a character result */
		case cc_FN:
			str [0] = Cmdch;
			goto ifany3;
			break;

		case c2_FN:
			str [0] = Nbcch;
			goto ifany3;
			break;

		case tc_FN:
			str [0] = Tabch;
			goto ifany3;
			break;

		case even_FN:
			val = ctoi (fcn_buf, &i);
			if ((val & 1) == 0)	/* even number */
				str[0] = '1';
			else	/* odd number */
				str[0] = '0';
			goto ifany3;
			break;

		case odd_FN:
			val = ctoi (fcn_buf, &i);
			if ((val & 1) == 0)	/* even number */
				str[0] = '0';
			else
				str[0] = '1';
			goto ifany3;
			break;

		case cmp_FN:
			for (j = 0; fcn_buf[i] != ' '; i++, j++)
				str1[j] = fcn_buf[i];
			str1[j] = '\0';
			SKIPBL(fcn_buf, i);
			for (j = 0; fcn_buf[i] != ' '; i++, j++)
				rel[j] = fcn_buf[i];
			rel[j] = '\0';
			SKIPBL(fcn_buf, i);
			for (j = 0; fcn_buf[i] != ' ' && fcn_buf[i] != '\0'; i++, j++)
				str2[j] = fcn_buf[i];
			str2[j] = '\0';
			r = strlsr (relops, sizeof relops, sizeof(relops[0]), rel);
			if (r == EOF)
				error ("cmp: bad relational operator");

			v = swtstrcmp (str1, str2);
			if (rel_vals[r][0] == v || rel_vals[r][1] == v)
				str[0] = '1';	/* true */
			else
				str[0] = '0';	/* false */
			goto ifany3;
			break;

		case icmp_FN:
			s1 = gctoi (fcn_buf, &i, 10);
			SKIPBL (fcn_buf, i);
			for (j = 0; fcn_buf[i] != ' '; i++, j++)
				rel[j] = fcn_buf[i];
			rel[j] = '\0';
			s2 = gctoi (fcn_buf, &i, 10);
			r = strlsr (relops, sizeof relops, sizeof(relops[0]), rel);
			if (r == EOF)
				error ("icmp: bad relational operator");
			switch (r) {
			case 0:
			case 1:
				str[0] = (s1 <= s2) ? '1' : '0';
				break;
			
			case 2:
				str[0] = (s1 < s2) ? '1' : '0';
				break;
			
			case 3:
			case 4:
				str[0] = (s1 == s2) ? '1' : '0';
				break;

			case 5:
			case 6:
			case 7:
				str[0] = (s1 != s2) ? '1' : '0';
				break;
			
			case 8:
			case 9:
				str[0] = (s1 >= s2) ? '1' : '0';
				break;
			
			case 10:
				str[0] = (s1 > s2) ? '1' : '0';
				break;
			}
			goto ifany3;
			break;

		default:
			goto switch3;
			break;

		}
	ifany3:
		str [1] = '\0';
		ret = ctoc (str, buf, maxbuf);
		goto end_of_proc;

switch3: 
		switch (fnum) {   /* these functions yield a string result */
		case day_FN:
			date (SYS_DAY, str);
			str [0] = mapup (str [0]);
			goto ifany4;
			break;

		case date_FN:
			date (SYS_DATE, str);
			goto ifany4;
			break;

		case time_FN:
			date (SYS_TIME, str);
			goto ifany4;

		case ldate_FN:
			date (SYS_LDATE, str);
		/*
			this code removed for your protection.
			it removes everything from the front of the
			string up to and including the first blank.
			but since date is something specially written,
			not the original PRIMOS routine, there isn't
			a problem, and str should be LEFT ALONE.

			sdrop (str, str, index (str, ' ') - str);
		 */
			goto ifany4;
			break;

		case rn_FN:
		case RN_FN:
			val = ctoi (fcn_buf, & i);
			itorom (val, str, 32);
			if (fnum == RN_FN)
				mapstr (str, UPPER);
			goto ifany4;
			break;

		case letter_FN:
		case LETTER_FN:
			val = ctoi (fcn_buf, & i);
			itolet (val, str);
			if (fnum == LETTER_FN)
				mapstr (str, UPPER);
			goto ifany4;
			break;

		case vertspace_FN:
			val = ctoi (fcn_buf, &i);
			if (val < 1 || val > 16)
				error ("fmt error: illegal vertical spacing");
			str[0] = vert[val - 1];
			str[1] = '\0';
			goto ifany4;
			break;

		case even_FN:
			val = ctoi (fcn_buf, &i);
			if ((val & 1) == 0)	/* even number */
				str[0] = '1';
			else	/* odd number */
				str[0] = '0';
			str[1] = '\0';
			goto ifany4;
			break;

		case odd_FN:
			val = ctoi (fcn_buf, &i);
			if ((val & 1) == 0)	/* even number */
				str[0] = '0';
			else	/* odd number */
				str[0] = '1';
			str[1] = '\0';
			goto ifany4;
			break;

		case header_FN:
			if ((Curpag & 1) == 0)	/* this is an even numbered page */
				massin (Even_header, tempstr, MAXOUT);
			else
				massin (Odd_header, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		case evenheader_FN:
			massin (Even_header, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		case oddheader_FN:
			massin (Odd_header, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		case footer_FN:
			if ((Curpag & 1) == 0)	/* this is an even numbered page */
				massin (Even_footer, tempstr, MAXOUT);
			else
				massin (Odd_footer, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		case evenfooter_FN:
			massin (Even_footer, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		case oddfooter_FN:
			massin (Odd_footer, tempstr, MAXOUT);
			mktl (tempstr, Curpag, str, MAXOUT);
			goto ifany4;
			break;

		default:
			goto switch5;
		}
ifany4:
		ret = ctoc (str, buf, maxbuf);
		goto end_of_proc;

switch5: 
		switch (fnum) {
		case bf_FN:
			ret = boface (& fcn_buf [i], buf, maxbuf);
			goto end_of_proc;

		case it_FN:
			ret = italic (& fcn_buf [i], buf, maxbuf);
			goto end_of_proc;

		case ul_FN:
			ret = underl (& fcn_buf [i], buf, maxbuf, NO);
			goto end_of_proc;

		case cu_FN:
			ret = underl (& fcn_buf [i], buf, maxbuf, YES);
			goto end_of_proc;

		case sub_FN:
			buf [0] = hlf;
			ret = ctoc (&fcn_buf[i], &buf[1], maxbuf - 2) + 2;/*?*/
			buf [ret] = hlr;
			buf [ret + 1] = '\0';
			goto end_of_proc;

		case sup_FN:
			buf [0] = hlr;
			ret = ctoc (&fcn_buf[i], &buf[1], maxbuf - 2) + 2;/*?*/
			buf [ret] = hlf;
			buf [ret + 1] = '\0';
			goto end_of_proc;

		case cap_FN:
			mapstr (& fcn_buf[i], UPPER);
			ret = ctoc (& fcn_buf[i], buf, maxbuf);
			goto end_of_proc;
		
		case small_FN:
			mapstr (& fcn_buf[i], LOWER);
			ret = ctoc (& fcn_buf[i], buf, maxbuf);
			goto end_of_proc;

		default:
			break;
		}

		switch (fnum) {   /* these functions yield no result */
		case set_FN:
		case add_FN:
			nr = ctoi (fcn_buf, & i);
			val = gctoi (fcn_buf, & i, 10);
			if (1 <= nr && nr <= MAXNUMREGS)
				if (fnum == set_FN)
					Numreg [nr] = val;
				else
					Numreg [nr] += val;
			goto ifany5;

		default:
			buf [0] = '\0';
			ret = 0;
			remark ("in eval_fcn: can't happen");
			goto end_of_proc;
		}


ifany5:
		buf [0] = '\0';
		ret = 0;
	}

end_of_proc:
	return (ret);
}


#define PRINTABLE	'~' - ' '	/* range of printable chars */

/*
 * table is indexed by characters in macro name.
 * macro name is always blank padded to two chars.
 */

static char *mac_table[PRINTABLE][PRINTABLE];

/* addmac --- store the text into the table as indexed by name */

addmac(name, text, len)
char *name, *text;
int len;
{
	register int i, j, k;

	if (! printable(name[0]) || ! printable(name[1]))
	{
		putlin("in addmac: ", stderr);
		putlin(name, stderr);
		error(": macro name contains non-printable characters.");
	}

	i = name[0] - ' ';	/* make 0-based */
	j = name[1] - ' ';

	if (mac_table[i][j] != NULL)
		free (mac_table[i][j]);		/* release the storage */
		/*
		 * don't print a warning message, since
		 * it could be a macro addition.
		 */

	mac_table[i][j] = malloc(len);
	for (k = 0; k < len; k++)
		mac_table[i][j][k] = text[k];
	
}

/* getmac --- return the macro text for the macro named str */

char *getmac(str)
char *str;
{
	int i, j;
	char nam[2];

	nam[0] = str[0];
	nam[1] = (str[1] == '\n' || str[1] == '\0') ? ' ' : str[1];

	if (! printable(nam[0]) || ! printable(nam[1]))
		return NULL;	/* could be something like .#<NEWLINE> */

	i = nam[0] - ' ';
	j = nam[1] - ' ';


	return mac_table[i][j];		/* caller checks for NULL */
}

/* domac --- invoke a macro specified by buf */

int domac (buf)
char *buf;
{
	char *cp, *getmac();
	int ret;

	ret = OK;

	if ((cp = getmac(&buf[1])) != NULL)	/* returns macro text */
	{
		F_ptr++;
		if (F_ptr >= MAXFILES)
		{
			reset_files();
			putlin (buf, stderr);
			error ("Too many macros/input files.");
		}
		F_list [F_ptr].F_type = MACRO;
		F_list [F_ptr].source.md = cp;
		emargs (buf);    /* enter macro arguments into argbuf */
		return ret;
	}

	ret = ERR;

	return ret;
}



/* emargs --- enter macro arguments in argbuf */

emargs (buf)
char *buf;
{
	int i, j, k;
	char args [MAXLINE];

	j = 0;
	i = 1;		/* skip Cmdch, name of macro is arg 0 */

	for(;;)
	{
		SKIPBL (buf, i);
		if (buf [i] == '\n' || buf [i] == '\0')
			break;
		do {
			while (buf [i] == '"')
			{
				for (i++; buf [i] != '"'; i++)
				{
					if (buf [i] == '\n' || buf [i] == '\0')
					{
						reset_files();
						putlin (buf, stderr);
						error ("missing quote");
					}
					args [j++] = buf [i];
				}
				i++;
				if (buf [i] == '"')
					args [j++] = '"';
			}
			for ( ; buf [i] != ' ' && buf [i] != '\t' && buf [i] != '"'
			    && buf [i] != '\n' && buf [i] != '\0'; i++)
				args [j++] = buf [i];
		}  while (buf [i] == '"');
		args [j++] = '\0';
	}

	if (Argtop + j >= ARGBUFSIZE)
	{
		reset_files();
		putlin (buf, stderr);
		error ("Too many macro arguments");
	}

	Maclvl++;
	if (Maclvl >= MAXMACLVL)
	{
		reset_files();
		putlin (buf, stderr);
		error ("Too many nested macro calls");
	}
	Argv [Maclvl] = Argtop;
	for (k = 0; k < j; k++)
	{
		Argbuf [Argtop] = args [k];
		Argtop++;
	}
}



/* endmac --- close out an invocation of a macro */

endmac()
{
	Argtop = Argv [Maclvl];
	Maclvl--;
}



/* getnam --- get macro name after command (.de or .en) */

getnam (buf, name)
char *buf, *name;
{
	int i, j;

	i = 0;
	SKIPCM (buf, i);
	for (j = 0; j <= 1 && buf [i] != ' ' && buf [i] != '\t'
	    && buf [i] != '\n'; j++)
	{
		name [j] = buf [i];
		i++;
	}
	for (; j <= 1; j++)
		name [j] = ' ';
	name [2] = '\0';	/* insure name is blank padded to two chars */
}



/* massin --- massage a line of input; evaluate functions, etc. */

int massin (buf, obuf, size)
int size;
char *buf, *obuf;
{
	int i, j;
	char fcn_buf [MAXLINE];

	j = 0;
	for (i = 0; buf [i] != '\0'; i++)
		if (buf [i] == '['
		    && extract_fcn (buf, & i, fcn_buf, MAXLINE) == OK)
			j += eval_fcn (fcn_buf, & obuf [j], size - j + 1);
		else
		{
			if (buf [i] == ESCAPE
			    && (buf [i + 1] == ESCAPE || buf [i + 1] == '['))
				i++;
			if (j < size)
				obuf [j++] = buf [i];
		}

	obuf [j] = '\0';

	return (j);
}



/* extract_fcn --- isolate the text between balanced square brackets */

int extract_fcn (buf, index, fcn_buf, size)
int *index, size;
char *buf, *fcn_buf;
{
	int i, j, level;
	int ret;

	j = 0;
	level = 1;
	for (i = *index + 1; buf [i] != '\0' && buf [i] != '\n'; i++)
	{
		switch (buf [i]) {
		case '@':
			if (buf [i + 1] != '\0' && buf [i + 1] != '\n')
				i++;
			break;

		case '[':
			level++;
			break;

		case ']':
			if (--level <= 0)
				goto exit_for;
		}
		if (j < size)
			fcn_buf [j++] = buf [i];
		else
			break;
	}

exit_for:
	fcn_buf [j] = '\0';
	if (level == 0)
	{
		*index = i;
		ret = OK;
	}
	else
		ret = ERR;

	return ret;
}



/* rdmac --- transfer line from a macro definition to a buffer */

int rdmac (buf, cp, size)
char **cp, *buf;
int size;
{
	int ret;

	ret = ctoc(*cp, buf, size);
	*cp += ret + 1;
	if (buf [0] == EOF)
		ret = EOF;

	return ret;
}



/* trnarg --- transfer macro argument to buf */

int trnarg (argno, buf, maxbuf)
int argno, maxbuf;
char *buf;
{
	int j, k;
	int ret;

	ret = 0;
	if (argno >= 0)
	{
		for (j = argno, k = Argv [Maclvl]; k < Argtop && j > 0; j--)
		{
			while (Argbuf [k] != '\0')
				k++;
			k++;
		}

		if (k < Argtop)
		{
			ret = ctoc(& Argbuf[k], buf, maxbuf);
		}
	}
	return ret;
}



/* itorom --- convert integer to roman numerals */

int itorom (val, str, size)
int val, size;
char *str;
{
	int aval, out;

#define outchar(ch)	{if(out<size){str[out]=ch;out++;}}

	out = 0;
	aval = abs (val);
	if (val < 0)
		outchar ('-');

	while (aval >= 1000)
	{
		outchar ('m');
		aval -= 1000;
	}
	if (aval >= 900)
	{
		outchar ('c');
		outchar ('m');
		aval -= 900;
	}
	else if (aval >= 500)
	{
		outchar ('d');
		aval -= 500;
	}

	if (aval >= 400)
	{
		outchar ('c');
		outchar ('d');
		aval -= 400;
	}
	else
		while (aval >= 100)
		{
			outchar ('c');
			aval -= 100;
		}
	if (aval >= 90)
	{
		outchar ('x');
		outchar ('c');
		aval -= 90;
	}
	else if (aval >= 50)
	{
		outchar ('l');
		aval -= 50;
	}

	if (aval >= 40)
	{
		outchar ('x');
		outchar ('l');
		aval -= 40;
	}
	else
		while (aval >= 10)
		{
			outchar ('x');
			aval -= 10;
		}

	if (aval >= 9)
	{
		outchar ('i');
		outchar ('x');
		aval -= 9;
	}
	else if (aval >= 5)
	{
		outchar ('v');
		aval -= 5;
	}

	if (aval >= 4)
	{
		outchar ('i');
		outchar ('v');
		aval -= 4;
	}
	else
		while (aval >= 1)
		{
			outchar ('i');
			aval--;
		}

	str [out] = '\0';

	return (out - 1);
}

/* itolet --- convert a number between 1 and 26 to its letter equivalent */

itolet (num, let)
int num;
char *let;
{
	if (num < 1 || num > 26)
		let[0] = '?';
	else
		let[0] = num + 'a' - 1;
	
	let[1] = '\0';
}
