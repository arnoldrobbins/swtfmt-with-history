/*
** io.c
**
** do i/o for formatter.
*/

#include "fmt.h"
#include "extern.h"

static trim (char *s);

/* newinp --- open new input file, stack its descriptor */

newinp (name)
char *name;
{
	FILE *fd;
	FILE *xopen();

	trim (name);
	fd = xopen (name, "r");
	if (fd != NULL)
	{
		if (++F_ptr >= MAXFILES)
		{
			xclose (fd);
			reset_files();
			putlin (name, stderr);
			error (": too many macros/input files");
		}
		rewind (fd);
		F_list [F_ptr].F_type = FILETYPE;
		F_list [F_ptr].source.fp = fd;
		/* return; */
	}
	else
	{
		reset_files();
		cant (name);
	}
}


/* newout --- create set up new output file for divert */

newout (name)
char *name;
{
	FILE *fd;
	FILE *xopen();

	fd = xopen (name, "r+");
	if (fd == NULL)
	{
		reset_files();
		putlin (name, stderr);
		error (": can't open divert output");
	}

	wind (fd);
	Out_file = fd;
}

/* wind --- get to end of file */

wind (fd)
FILE *fd;
{
	fseek(fd, 0L, 2);
}

/* readln --- read a line from the current input source */

int readln (buf, size)
char *buf;
int size;
{
	int massin(), rdmac();
	char inbuf [MAXOUT];
	int ret;

	for (;;)
	{
		if (F_list [F_ptr].F_type == MACRO)  /* input from macro */
			ret = rdmac (inbuf, & (F_list [F_ptr].source.md), MAXOUT);
		else	/* input from file */
			ret = getlin (inbuf, F_list [F_ptr].source.fp, MAXOUT);

		if (ret != EOF)      /* return if getlin was successful */
			return (massin (inbuf, buf, size));
		else
		{
			if (F_list[F_ptr].F_type  != MACRO)
				xclose (F_list [F_ptr].source.fp);
				/* close current input file */
			else
				endmac();
			F_ptr--;
			if (F_ptr >= 0)	/* if there is more input available, */
				continue;             /* try another getlin. */
			if (xargv[Next_arg] == NULL)
			{
				/* clear out last page */
				Nobreak = NO;
				do_brk ();
				if (Lineno > 0)
					space (HUGE);
				if (F_ptr >= 0)
					continue;
				else
					return EOF;
				/* return EOF, no more input is available */
			}
			else
			{
				/* open file given as argument */
				strcpy (File_name, xargv[Next_arg]);
				Next_arg++;
				newinp (File_name);
			}
		}
	}
}



/* reset_files --- close all currently opened files (excepting STDIN) */

reset_files()
{
	int i;

	for (; F_ptr >= 0; F_ptr--)
		if (F_list [F_ptr].F_type == FILETYPE)
			xclose (F_list [F_ptr].source.fp);

	for (i = 1; i <= MAXFILES; i++)
		if (O_list[i].fp != NULL)
		{
			char buf[MAXLINE];

			sprintf(buf, "/tmp/fmt.%d.%d",
				getpid(), O_list[i].id);
			fclose (O_list[i].fp);
			/*
			unlink(buf);
			*/
			O_list[i].fp = NULL;
		}
}


/* xclose --- close file, if necessary */

xclose (fd)
FILE *fd;
{
	int i;

	for (i = 1; i <= MAXFILES; i++)	/* don't close temporary files */
		if (fd == O_list [i].fp)
			return;

	fclose (fd);
}



/* xopen --- open named file; if name is "-", return STDIN */

FILE *xopen (name, mode)
char *name, *mode;
{
	int i, fn;
	TEMPFILE temp_file();
	FILE *fopen();
	char *index();
	char *expand_env();

	i = 0;
	fn = ctoi (name, & i);

	if (name [0] == '-' && name [1] == '\0')
		return stdin;
	else if (fn >= 1 && fn <= MAXFILES)
	{
		if (O_list[fn].fp == NULL)
			O_list [fn] = temp_file ();
		return O_list[fn].fp;
	}
	else
	{
		/*
		 * if file doesn't exist at all, stdio will barf to open it r+
		 * so we creat it, and then let stdio do the rest of the work
		 */
		if (strcmp (mode, "r+") == 0 && access (name, 0) != 0)
		{
			int junk;

			if ((junk = creat (expand_env(name), 0600)) < 0)
			{
				reset_files();
				putlin (expand_env(name), stderr);
				error (": can't open divert output");
			}
			else
				close (junk);
		}

		return fopen (expand_env(name), mode);
	}
}

/* trim --- remove trailing blanks from a file name */

static trim (s)
char *s;
{
	int i;

	i = strlen (s) - 1;
	while (isspace (s[i]))
		i--;
	s[++i] = '\0';
}

/* expand_env --- expand environment variables in file names */

char *expand_env (file)
register char *file;
{
	register int i, j, k;	/* indices */
	char *getenv ();
	char var[BUFSIZ];		/* variable name */
	char *val;			/* value of environment variable */
	static char buf[BUFSIZ * 2];	/* expanded file name, static */

	i = j = k = 0;
	while (file[i] != '\0')
	{
		if (file[i] == ESCAPE)
		{
			if (file[i+1] == '$')
			{
				buf[j++] = file[++i];	/* the actual $ */
				i++;	/* for next time around the loop */
			}
			else
				buf[j++] = file[i++];	/* the \ */
		}
		else if (file[i] != '$')	/* normal char */
			buf[j++] = file[i++];
		else			/* environment var */
		{
			i++;	/* skip $ */
			k = 0;
			while (file[i] != '/' && file[i] != '\0')
				var[k++] = file[i++];	/* get var name */
			var[k] = '\0';

			if ((val = getenv (var)) != NULL)
				for (k = 0; val[k] != '\0'; k++)
					buf[j++] = val[k];
					/* copy val into file name */
			/* else
				var not in enviroment; leave file
				name alone, multiple slashes are legal */
		}
	}
	buf[j] = '\0';

	return (buf);
}
