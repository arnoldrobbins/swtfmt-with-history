/*
*	os --- overstrike a print file for the 750 console
*/

#include <stdio.h>
#include <ctype.h>

#define PAGE_SIZE	66
#define MAXLINE		85

#define min(a,b)	(((a) < (b)) ? (a) : (b))

char Norm[MAXLINE];		/* Normal output */
char Undr[MAXLINE];		/* underlined output */
char Ital[MAXLINE];		/* italicized output */
char Bold[MAXLINE];		/* boldfaced output */
char Unbo[MAXLINE];		/* underlined bold */

int Empty = 1;			/* current line is still empty */
int Lctr = PAGE_SIZE;		/* current line counter */
int Obp = 0;			/* output buffer pointer */
int Nls = 1;			/* current number of newlines pending */
int Os = 0;			/* current line should be overstruck */


/*
*	main --- main driver for the 'os' program
*/

main(argc,argv)
char *argv[];
int argc;
{
	int ibp, cnt;
	char c, ibuf[BUFSIZ];

	int read(), write();

/*
*	read and process the entire input file
*/

	while ((cnt = read(0, ibuf, 1024)) > 0)
		for (ibp = 0; ibp < cnt; ibp++)
		{

/*
*	test for special characters that affect formatting
*/

			c = toascii(ibuf[ibp]);
			switch (c)
			{
				case '\n':
					dump();
					continue;

				case '\f':
					dump();
					Lctr = PAGE_SIZE;
					continue;

				case '\t':
					Obp += 8 - (Obp % 8);
					continue;

				case '\b':
					Obp--;
					continue;

				case ' ':
					Obp++;
					continue;
			}

/*
*	process the rest of the characters, appropriately
*/

			if (c <= ' ' || Obp < 0 || Obp >= MAXLINE - 1)
				continue;

			if (c == '_')
				if (Norm[Obp] != '\0')
					Ital[Obp] = Norm[Obp];
				else if (Undr[Obp] == '_')
					Unbo[Obp] = '_';
				else
					Undr[Obp] = '_';
			else if (Norm[Obp] == c)
				Bold[Obp] = c;
			else if (Norm[Obp] == '\0')
				Norm[Obp] = c;
			else			/* panic, we must overstrike */
			{
				int sp;

				sp = Obp;	/* save the pointer */
				Os = 1;		/* overstrike next line */
				dump();		/* output current line */

				Lctr--;		/* to keep actual line count */
				Obp = sp;	/* restore the pointer */
				Norm[Obp] = c;	/* start again */
			}

			Empty = 0;
			Obp++;
		}

/*
*	print a trailing pageup to finish things off (if needed)
*/

	printf("\f");
	exit (0);
}


/*
*	dump --- process each of the output buffers and empty them
*/

dump()
{
	int i, j;

/*
*	check if we are at the top of the next page
*/

	if (++Lctr > PAGE_SIZE)
	{
		if (Nls > 0)
		{
			printf("\f");
			Nls = 0;
		}

		Lctr = 1;
	}

/*
*	see if this is a non-empty line and output it if possible
*/

	if (Empty)
	{
		Nls++;			/* another pending newline */
		Obp = 0;		/* set to beginning of the line */
		return;
	}

	for (; Nls > 0; Nls--)		/* output pending newlines */
		printf("\n");

	for (i = 0; i < Obp; i++)	/* fix character precedence */
	{				/* and remove redundant underscores */
		if (Bold[i])		/* Bold over Ital and Norm */
		{
			Ital[i] = '\0';
			Norm[i] = '\0';
		}
		if (Ital[i])		/* Ital over Norm */
			Norm[i] = '\0';
		if (Unbo[i])		/* Bold underscores over underscores */
			Undr[i] = '\0';
		if (Undr[i] == '_' && !Norm[i])
		{
			Norm[i] = '_';
			Undr[i] = '\0';
		}
	}

/*
*	trim, output, and clean Norm
*/

	for (i = min(Obp, MAXLINE - 1); i >= 0; i--)
		if (Norm[i] && Norm[i] != ' ')
			break;
	Norm[i + 1] = '\0';
	for (j = i; j >= 0; j--)
		if (!Norm[j])
			Norm[j] = ' ';

	if (i >= 0)
		printf("%s\r", Norm);

	for (; i >= 0; i--)
		Norm[i] = '\0';

/*
*	trim, output (if necessary), and clean Bold
*/

	for (i = min(Obp, MAXLINE - 1); i >= 0; i--)
		if (Bold[i] && Bold[i] != ' ')
			break;
	Bold[i + 1] = '\0';
	for (j = i; j >= 0; j--)
		if (!Bold[j])
			Bold[j] = ' ';

	if (i >= 0)
	{
		printf("%s\r", Bold);
		printf("%s\r", Bold);
	}

	for (; i >= 0; i--)
		Bold[i] = '\0';

/*
*	trim, output (if necessary), and clean Ital
*/

	for (i = min(Obp, MAXLINE - 1); i >= 0; i--)
		if (Ital[i] && Ital[i] != ' ')
			break;
	Ital[i + 1] = '\0';
	for (j = i; j >= 0; j--)
		if (!Ital[j])
			Ital[j] = ' ';

	if (i >= 0)
	{
		printf("%s\r", Ital);
		for (j = 0; j < i; i++)
			if (Ital[j] != ' ')
				printf("_");
		printf("\r");
	}

	for (; i >= 0; i--)
		Ital[i] = '\0';

/*
*	trim, output (if necessary), and clean Unbo
*/

	for (i = min(Obp, MAXLINE - 1); i >= 0; i--)
		if (Unbo[i] && Unbo[i] != ' ')
			break;
	Unbo[i + 1] = '\0';
	for (j = i; j >= 0; j--)
		if (!Unbo[j])
			Unbo[j] = ' ';

	if (i >= 0)
	{
		printf("%s\r", Unbo);
		printf("%s\r", Unbo);
	}

	for (; i >= 0; i--)
		Unbo[i] = '\0';

/*
*	trim, output (if necessary), and clean Undr
*/

	for (i = min(Obp, MAXLINE - 1); i >= 0; i--)
		if (Undr[i] && Undr[i] != ' ')
			break;
	Undr[i + 1] = '\0';
	for (j = i; j >= 0; j--)
		if (!Undr[j])
			Undr[j] = ' ';

	if (i >= 0)
		printf("%s", Undr);

	for (; i >= 0; i--)
		Undr[i] = '\0';

/*
*	clean up and return for another exciting line...
*/

	if (Os)
	{
		printf ("\r");
		Os = 0;
	}
	else
		printf ("\n");
	Empty = 1;
	Obp = 0;
	return;
}
