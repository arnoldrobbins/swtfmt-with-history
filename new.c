/*
** new.c
**
** provide some new routines for the formatter
*/

#include "fmt.h"
#include "extern.h"

/* putlin --- shove out some text */

putlin(text, fp)
register char *text;
FILE *fp;
{
#ifdef notdef
	register int i;

if (Dvflag == NO)
	for (i = 0; text[i]; i++)
		if (text[i] == PHANTOMBL)
			text[i] = ' ';
#endif

	fputs (text, fp);
	fflush (fp);
}

/* length --- return the length of a string */

int length(str)
char *str;
{
	return strlen(str);
}

/* mapstr --- map a string up or down */

mapstr(str, dir)
char *str;
int dir;
{
	int i;

	for (i = 0; str[i] != '\0'; i++)
		if (dir == UPPER)
			str[i] = islower(str[i]) ? toupper(str[i]) : str[i];
		else
			str[i] = isupper(str[i]) ? tolower(str[i]) : str[i];
}

/* remark --- say something */

remark(str)
char *str;
{
	fprintf(stderr, "%s\n", str);
}

/* getlin --- read in a string */

getlin(str, fp, max)
char *str;
FILE *fp;
int max;
{
	int ret = 0;

	while (--max)	/* at most max-1 chars used in str */
	{
		*str = getc(fp);
		if (*str == '\n')
		{
			ret++;
			str++;
			break;
		}
		else if (*str == EOF)
		{
			ret = EOF;
			break;
		}
		else
		{
			str++;
			ret++;
		}
	}
	*str = '\0';

	return ret;
}

/* cant --- give up */

cant(name)
char *name;
{
	fprintf(stderr, "%s: can't open\n", name);
	exit (1);
}

/* sdrop --- drop characters from a string APL-style */

/*
	this code removed for your protection,
	since the code which calls it has been removed.
	sdrop is not needed under Unix.

int sdrop(from, to, chars)
char *from, *to;
int chars;
{
	int len, ctoc(), length();

	len = length(from);

	if (chars < 0)
		return (ctoc(from, to, len + chars + 1));
	else
	{
		if (chars < len)
			len = chars;
		strcpy(to, &from[len + 1]);
		return length(to);
	}
}
*/

/* date --- format a date into str */

#include <time.h>

date (how, str)
int how;
char *str;
{
	char buf[100], *ctime();
	long clock, time();
	int i;
	struct tm *now, *localtime();
	static char *days[] = {
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"
		};
	
	static char *months[] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
		};

	time(&clock);
	strcpy(buf, ctime(&clock));
	now = localtime(&clock);

	switch (how) {
	case SYS_DAY:		/* day of the week */
		strcpy(str, days[now->tm_wday]);
		break;

	case SYS_DATE:		/* mm/dd/yy */
		sprintf(str, "%2.2d/%02.2d/%2.2d", now->tm_mon + 1,
			now->tm_mday, now->tm_year);
		break;

	case SYS_TIME:		/* hh:mm:ss */
		sprintf(str, "%2.2d:%02.2d:%2.2d", now->tm_hour,
			now->tm_min, now->tm_sec);
		break;

	case SYS_LDATE:		/* Month day, year */
		sprintf(str, "%s %d, %d", months[now->tm_mon],
			now->tm_mday, now->tm_year + 1900);
		break;
	}
}

/* ctoc -- copy strings w/bounds checking */

int ctoc (from, to, max)
register char *from, *to;
register int max;
{
	register int i;

	for (i = 0; i < max - 1; i++)
		if (from[i] == '\0')
		{
			to[i] = '\0';
			return(i);
		}
		else
			to[i] = from[i];
	
	to[i] = '\0';
	return (i);
}

/* ctoi --- convert decimal string to single precision integer */

int ctoi(str, i)
char *str;
int *i;
{
	int gctoi();

	return gctoi (str, i, 10);
}

/* itoc --- convert single precison integer to decimal string */

itoc(val, str, size)
int val, size;
char *str;
{
	int intval;
	int d, i, j, k;
	int ret;
	static char digits[] = "0123456789";

	intval = val;
	str[0] = '\0';
	i = 0;

	do				/* generate digits */
	{
		i++;
		d = abs(intval % 10);
		str[i] = digits[d];
		intval /= 10;
	} while (intval != 0 && i < size);

	if (val < 0 && i < size)	/* then sign */
		str[++i] = '-';
	
	ret = i;

	for (j = 0; j <= i; j++)	/* then reverse */
	{
		k = str[i];
		str[i] = str[j];
		str[j] = k;
		i--;
	}

	return ret;
}

/* gctoi --- convert any radix string to single precision integer */

gctoi(str, i, radix)
char *str;
int *i, radix;
{
	int base, v, d, j;
	char *index();
	char mapdn();
	int neg;
	static char digits[] = "0123456789abcdef";

	v = 0;
	base = radix;

	SKIPBL(str, *i);

	neg = (str[*i] == '-');
	if (str[*i] == '+' || str[*i] == '-')
		(*i)++;
	
	if (str[*i+2] == 'r' && str[*i] == '1' && isdigit(str[*i+1])
	    || str[*i+1] == 'r' && isdigit(str[*i]))
	{
		base = str[*i] - '0';
		j = *i;
		if (str[*i + 1] != 'r')
		{
			j++;
			base = base * 10 + str[j] - '0';
		}

		if (base < 2 || base > 16)
			base = radix;
		else
			*i = j + 2;
	}

	for (; str[*i] != '\0'; (*i)++)
	{
		if (isdigit(str[*i]))
			d = str[*i] - '0';
		else
			d = digits - index(digits, mapdn(str[*i]));
		
		if (d < 0 || d >= base)
			break;

		v = v * base + d;
	}

	return (neg ? -v : v);
}

/* mapdn --- return a lower case character if c was upper case */

char mapdn(c)
char c;
{
	return (isupper(c) ? tolower(c) : c);
}

/* mapup --- return an upper case character if c was lower case */

char mapup(c)
char c;
{
	return (islower(c) ? toupper(c) : c);
}

/* lookup --- lookup a value in a table */

int lookup(name, fval, table)
char *name;
int *fval;
register TABLE *table;
{
	register int i, j;
	int strbsr();

	/* compute sizeof table */
	for (j = 0; table[j].val != 0; j++)
		;
	j *= sizeof(table[0]);

	i = strbsr(table, j, sizeof(table[0]), name);

	if (i == EOF)
		return (NO);
	else
	{
		*fval = table[i].val;
		return (YES);
	}
	
}

/* tmpfile --- create a temporary file open for writing */

TEMPFILE tmpfile()
{
	static int i = 1;
	char buf[MAXLINE];
	TEMPFILE t;

	sprintf(buf, "/usr/tmp/fmt.%d.%d", getpid(), i++);
	if (i > 999)
		i = 0;
	
	t.id = i - 1;
	if ((t.fp = fopen(buf, "w+")) == NULL)
	{
		reset_files ();
		putlin (buf, stderr);
		error (": couldn't open temp file for diversion");
		exit (1);
	}
	wind (t.fp);
	return t;
}

/* strbsr --- binary search stab for an entry equal to str */

int strbsr (stab, tsize, esize, str)
char *stab, str[];
int tsize, esize;
{
	/* stab should have been declared like this:

	static struct {
	    char *s;
	    ...
	    } stab[] = {
		"string1",      ...
		"string2",      ...
		...             ...
		};

    The call to strbsr should look like this:

	i = strbsr (stab, sizeof (stab), sizeof (stab[0]), str);
    */

	register int i, j, k, x;
	int strcmp ();

	i = 0;
	j = tsize / esize - 1;
	do {
		k = (i + j) / 2;
		if ((x = strcmp (str, *(char **)(stab + esize * k))) < 0)
			j = k - 1;              /* GREATER */
		else if (x == 0)
			return (k);             /* EQUAL */
		else
			i = k + 1;              /* LESS */
	} while (i <= j);

	return (EOF);
}

/* strlst --- linear search stab for an entry equal to str */

int strlsr (stab, tsize, esize, str)
char *stab, str[];
int tsize, esize;
{
	/* stab should have been declared like this:

	static struct {
		char *s;
		...
		} stab[] = {
			"string1",	...
			"string1",	...
			...		...
			};
	
	The call to strlsr should look like this:

	i = strlsr (stab, sizeof (stab), sizeof (stab[0]), str);
	*/

	register int i, j;
	int strcmp ();

	for (j = 0; & stab[j * esize] < stab + tsize; j++)
		if (strcmp (str, *(char **)(stab + esize * j)) == 0)
			return j;
	
	return EOF;
}


/* swtstrcmp --- compare two strings and return 1 2 or 3 for < = or > */

int swtstrcmp (str1, str2)
char *str1, *str2;
{
	int i;

	for (i = 0; str1[i] == str2[i]; i++)
		if (str1[i] == '\0')
			return (2);

	if (str1[i] == '\0' || str1[i] < str2[i])
		return (1);
	else if (str2[i] == '\0' || str1[i] > str2[i])
		return (3);

	return (2);             /* should never happen */
}
