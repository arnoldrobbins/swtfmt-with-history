/*
*	lz --- overstrike a print file for a 9700 laser printer
*/

#include <stdio.h>
#include <ctype.h>

#define PAGE_SIZE	87
#define FORMAT		"GTPF6"
#define MAXLINE		128

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

char *yn[] = { "YES", "NO" };	/* duplex operation (two sided pages) */
char **dup = yn;


/*
*	main --- main driver for the 'lz' program
*/

main(argc, argv)
int argc;
char **argv;
{
	int ibp, cnt;
	char c, ibuf[BUFSIZ];

	int read(), write();

/*
*	parse arguments --- lz [-l <len>] [-d]
*/

	while (argv++,--argc)
		if (argv[0][0] == '-')
			switch (argv[0][1]) {
			case 'l':
				if (argv[0][2] != '\0')
					Lctr = atoi (& argv[0][2]);
				else
					Lctr = atoi ((++argv)[0]);
				if (Lctr <= 0 || Lctr > 90)
					Lctr = PAGE_SIZE;
				break;

			case 'd':
				if (dup == yn)
					dup++;
				break;

			default:
				usage ();
				break;
			}
		else
			usage ();

/*
*	output initial DJDE statement
*/

	printf("+  DJDE DUPLEX=%s,\n", *dup);
	printf("+  DJDE FONTINDEX=0,\n");
	printf("+  DJDE FORMAT=%s,\n", FORMAT);
	printf("+  DJDE FORM=NO,\n");
	printf("+  DJDE BOF=%d,\n", Lctr);
	printf("+  DJDE DATA=(1,%d),\n", MAXLINE);
	printf("+  DJDE SIDE=(NUFRONT,NOFFSET),\n");
	printf("+  DJDE END;\n");

/*
*	read and process the entire input file
*/

	while ((cnt = read(0, ibuf, sizeof ibuf)) > 0)
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
			else			/* panic */
			{
				int sp;

				sp = Obp;	/* save the pointer */
				dump();		/* output current line */

				Lctr--;		/* to keep actual line count */
				Os = 1;		/* overstrike next line */
				Obp = sp;	/* restore the pointer */
				Norm[Obp] = c;	/* start again */
			}

			Empty = 0;
			Obp++;
		}

		exit (0);	/* success */
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
			printf("11\n");
			Nls = 0;
			Os = 1;
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
		printf(" 1\n");

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

	printf ("%c1%s\n", Os ? '+' : ' ', Norm);
	for (; i >= 0; i--)
		Norm[i] = '\0';
	Os = 0;

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
		printf("+2%s\n", Bold);

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
		printf("+3%s\n", Ital);

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
		printf("+2%s\n", Unbo);

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
		printf("+1%s\n", Undr);

	for (; i >= 0; i--)
		Undr[i] = '\0';

/*
*	clean up and return for another exciting line...
*/

	Empty = 1;
	Obp = 0;
	return;
}

/*
*	usage --- print an error message and die
*/

usage ()
{
	fprintf (stderr, "usage: lz [-l page length] [-d]\n");
	exit (1);
}
