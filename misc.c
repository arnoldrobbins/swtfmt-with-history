/*
** misc.c
**
** miscellanious functions for formatter
*/

#include "fmt.h"
#include "extern.h"

/* chkif --- process an if command */

chkif (buf)
char *buf;
{
	int i, negcond, cond, j;
	char mac[MAXLINE], delim;
	int ctoi();

	/* get the condition, 0 == false */
	i = 0;
	SKIPCM (buf, i);

	SKIPBL (buf, i);

	if (buf[i] == '\n' || buf[i] == '\0')	/* empty if command */
		return;

	if (buf[i] == '~')
	{
		negcond = YES;
		i++;
	}
	else
		negcond = NO;
	
	cond = ctoi (buf, & i);

	SKIPBL (buf, i);
	delim = buf[i];		/* get delim */
	i++;

	/* check condition */
	if ((negcond == YES && cond == 0) || (negcond == NO && cond != 0))
	{
		/* process true command */
		SKIPBL (buf, i);
		for (j = 0; buf[i] != delim && buf[i] != '\0'; i++, j++)
			mac[j] = buf[i];
		mac[j] = '\0';
		processline (mac);
	}
	else	/* process false command */
	{
		for (; buf[i] != delim && buf[i] != '\0'; i++)
			;	/* skip over if part */
		if (buf[i] == '\0')
			return;		/* no else part there */
		/* else
			hit the delimiter */
		i++;		/* skip over delimiter */
		SKIPBL (buf, i);
		if (buf[i] != '\n' && buf[i] != '\0' && buf[i] != delim)
		{
			/* there is an else part */
			for (j = 0; buf[i] != delim && buf[i] != '\0'; i++, j++)
				mac[j] = buf[i];
			mac[j] = '\0';
			processline (mac);
		}
	}
}


/* esc --- map  array [i]  into escaped character if appropriate */

char esc (array, i)
char *array;
int *i;
{
	int ret;

	if (array [*i] != ESCAPE)
		ret = array [*i];
	else if (array [*i+1] == '\0')   /* @ not special at end */
		ret = ESCAPE;
	else
	{
		(*i)++;
		if (array [*i] == 'n')
			ret = '\n';
		else if (array [*i] == 't')
			ret = '\t';
		else
			ret = array [*i];
	}

	return ret;
}



/* getstr --- get string following a command */

int getstr (buf, str, size)
int size;
char *buf, *str;
{
	int i, j;

	i = 0;
	SKIPCM (buf, i);
	j = ctoc (& buf [i], str, size);
	if (j > 0 && str [j-1] == '\n')
	{
		str [j-1] = '\0';
		j--;
	}

	return (j);
}



/* getsc --- get single character parameter after command */

getsc (buf, param, defalt)
char *buf, *param, defalt;
{
	int i;

	i = 0;
	SKIPCM (buf, i);
	if (buf [i] == '\n')
		*param = defalt;
	else
		*param = buf [i];
}



/* getval --- evaluate optional numeric argument */

int getval (buf, argtyp)
char *buf;
int *argtyp;
{
	int i;

	i = 0;
	SKIPCM (buf, i);
	*argtyp = buf [i];
	if (*argtyp == '+' || *argtyp == '-')
		i++;

	return  ctoi (buf, &i);
}



/* mktl --- make 'title' from three-part-title in 'buf' */

mktl (buf, pageno, title, size)
int pageno, size;
char *buf, *title;
{
	char lpart [MAXLINE], cpart [MAXLINE], rpart [MAXLINE], delim;
	int lw, cw, rw, i, numsp, p, size_minus_1;

	if (buf [0] == '\0')
	{
		title [0] = '\n';
		title [1] = '\0';
		return;
	}

	i = 0;
	delim = buf [0];
	size_minus_1 = size - 1;

	lw = mvpart (buf, & i, delim, pageno, lpart, MAXLINE);
	cw = mvpart (buf, & i, delim, pageno, cpart, MAXLINE);
	rw = mvpart (buf, & i, delim, pageno, rpart, MAXLINE);

	p = ctoc(lpart, title, size_minus_1); 

	numsp = (Tiwidth / 2) - lw - cw / 2;
	if (numsp < 1)
		numsp = 1;

	for (i = 1; i <= numsp && p < size_minus_1; p++, i++)
		title [p] = ' ';

	p += ctoc(cpart, &title[p], size - p);

	numsp = Tiwidth - (lw + numsp + cw + rw);
	if (numsp < 1)
		numsp = 1;

	for (i = 1; i <= numsp && p < size_minus_1; p++, i++)
		title [p] = ' ';

	p += ctoc(rpart, &title[p], size - p);

	title [p] = '\n';
	title [p + 1] = '\0';
}



/* mvpart --- move part of a three-part title to a buffer */

int mvpart (buf, i, delim, pageno, part, size)
int *i, pageno, size;
char *buf, delim, *part;
{
	int j;

	if (buf [*i] != delim)
	{
		part [0] = '\0';
		return (0);
	}

	(*i)++;
	j = 0;
	while (j < size && buf [*i] != delim
	    && buf [*i] != '\n' && buf [*i] != '\0')
	{
		if (buf [*i] == ESCAPE)
		{
			if (buf [*i + 1] != PAGENUM)
				part [j] = ESCAPE;
			else
			{
				part [j] = PAGENUM;
				(*i)++;
			}
			j++;
		}
		else if (buf [*i] != PAGENUM)
		{
			part [j] = buf [*i];
			j++;
		}
		else
			j += itoc (pageno, & part [j], size - j + 1);
		(*i)++;
	}

	part [j] = '\0';
	return (width (part));
}



/* skip --- output  n  blank lines */

skip (n)
int n;
{
	int i;

	CHECK_PAGE_RANGE;
	for (i = 1; i <= n; i++)
		putc ('\n', stdout);
}


/* tailbl --- delete trailing blanks from buf */

tailbl (buf)
char *buf;
{
	int i;

	i = strlen (buf) - 1;

	if (i > 0 && buf [i] == '\n')
		i--;
	for ( ; i >= 0 && buf [i] == ' '; i--)
		;

	buf [i + 1] = '\n';
	buf [i + 2] = '\0';
}



/* width --- compute width of character string */

int width (buf)
char *buf;
{
	int i;
	int ret;

	ret = 0;
	for (i = 0; buf [i] != '\0'; i++)
		if (toascii(buf[i]) == '\b')
			ret--;
		else if (toascii(buf[i]) >= ' ' && toascii(buf[i]) < DEL)
			ret++;

	return ret;
}
