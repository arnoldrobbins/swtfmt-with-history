/*
** fmt.c
**
** global variables used by the text formatter.
**
** main program and some initialization.
*/

#include "fmt.h"

/* Concerning output: */
int Outp = 0;		/* last char position in outbuf */
int Outw = 0;		/* width of text currently in outbuf */
int Outwds = 0;		/* number of words in outbuf */
char Outbuf[MAXOUT];	/* lines to be filled collect here */

/* Concerning pages: */
int Start_page = 0;	/* page on which printing begins */
int End_page = HUGE;	/* page on which printing ends */
int Curpag = 0;		/* current output page number */
int Newpag = 1;		/* next output page number */
int Lineno = 0;		/* next line to be printed */
int Plval = PAGELEN;	/* page length in lines */
int M1val = MARGIN1;	/* margin before and including header */
int M2val = MARGIN2;	/* margin after header */
int M3val = MARGIN3;	/* margin after last text line */
int M4val = MARGIN4;	/* bottom margin, including footer */
int Bottom;		/* last live line on page, = plval-m3val-m4val */
char Even_header[MAXOUT] = "";		/* heading for even pages */
char Odd_header[MAXOUT] = "";		/* heading for odd pages */
char Even_footer[MAXOUT] = "";		/* footing for even pages */
char Odd_footer[MAXOUT] = "";		/* footing for odd pages */

/* Concerning paramaters set by user: */
int Fill = YES;		/* fill if YES */
int Nospace = YES;	/* inhibit spacing */
char Adjust = BOTH;	/* adjustment mode */
int Dvflag = NO;	/* divert unformatted output */
int Tiwidth = PAGEWIDTH;	/* title width */
int Poval = 0;		/* page offset */
int Moval = 0;		/* marginal character offset */
int Inval = 0;		/* current indent; >= 0; always = lmval - 1 */
int Tival = 0;		/* current temporary indent */
int Lmval = 1;		/* left margin; always equals inval + 1 */
int Rmval = PAGEWIDTH;	/* current right margin */
int Bfval = 0;		/* number of lines to boldface */
int Ceval = 0;		/* number of lines to center */
int Itval = 0;		/* number of lines to italicize */
int Ulval = 0;		/* number of lines to underline */
int Lsval = 1;		/* current line spacing */
int Ooval = 0;		/* odd page offset value */
int Eoval = 0;		/* even page offset value */

/* Concerning miscellanious control variables: */
int Hyphenation = YES;		/* YES if hyphenation is on */
int Extra_blank_mode = YES;	/* YES if extra blanks wanted after periods */
int Nobreak = NO;		/* YES if no-break char specified */
int Word_last = NO;		/* YES if word was just put in outbuf */
int F_ptr;			/* index into f_list, indicates current input file */
INPUT F_list[MAXFILES];		/* stack of currently-open input files */
TEMPFILE O_list[MAXFILES+1];	/* descriptors of output temporaries; init = NULL */
int Using_temps = NO;		/* Is the current divert a temporary? */
FILE *Out_file = stdout;	/* output file descriptor for divert */
int Next_arg = 0;		/* next argument to be checked for file name */
int Tabs[MAXLINE+1];		/* tab stops for tab expansion */
int Numreg[MAXNUMREGS+1];	/* number registers for general use */
int Mcout = YES;		/* YES if current marginal char has been output */
char Mcch = ' ';		/* marginal character */
char Tmcch = ' ';		/* temporary marginal character */
char Cmdch = '.';		/* command character */
char Nbcch = '`';		/* no-break command character */
char Replch = PHANTOMBL;	/* tab replacement character */
char Tabch = '\t';		/* tab character */
char File_name[MAXPATH];	/* buffer used by various file handlers */

/* Concering macros: */
int Maclvl = 1;			/* nesting level for macro calls */
int Argv[MAXMACLVL] = { 1 };	/* beginning of arg list for each macro level */
int Argtop = 0;			/* next free location in argbuf */
int First_macro = ENDOFLIST;	/* location of first macro in macbuf */
int Mactop = 0;			/* next free location in macbuf */
char Macbuf[MACBUFSIZE];	/* macro text buffer area */
char Argbuf[ARGBUFSIZE];	/* buffer for macro arguments */

/* Concerning options: */
int Stop_mode = NO;	/* YES iff -s option on command */

/* Concerning the command line: */
int xargc;	/* argument count */
char **xargv;	/* argument values */

/* Concerning tables: */

/*
 * table entries MUST be sorted, since the search routines use a
 * binary search.  If you add functionality to the formatter, make sure
 * the new requests, or whatever, are put in the table in sorted order.
 */

TABLE Requests[] = {		/* formatter requests */
	"ad",	ad_REQ,	/* adjust */
	"am",	am_REQ,	/* append to a macro */
	"bf",	bf_REQ,	/* boldface */
	"bp",	bp_REQ,	/* begin page */
	"br",	br_REQ,	/* break */
	"c2",	c2_REQ,	/* no-break control character */
	"cc",	cc_REQ,	/* control character */
	"ce",	ce_REQ,	/* center */
	"de",	de_REQ,	/* define */
	"dv",	dv_REQ,	/* divert unformatted output */
	"ef",	ef_REQ,	/* set footer for even pages */
	"eh",	eh_REQ,	/* set header for even pages */
	"en",	en_REQ,	/* end macro definition */
	"eo",	eo_REQ,	/* set even page offset */
	"er",	er_REQ,	/* error message */
	"ex",	ex_REQ,	/* exit */
	"fi",	fi_REQ,	/* fill */
	"fo",	fo_REQ,	/* three-part footer */
	"he",	he_REQ,	/* three-part header */
	"hy",	hy_REQ,	/* hyphenation on */
	"if",	if_REQ,	/* conditional macro execution */
	"in",	in_REQ,	/* indent */
	"it",	it_REQ,	/* italicize */
	"lm",	lm_REQ,	/* left margin */
	"ls",	ls_REQ,	/* line spacing */
	"lt",	lt_REQ,	/* length of titles */
	"m1",	m1_REQ,	/* top margin,	incl. header */
	"m2",	m2_REQ,	/* margin after header */
	"m3",	m3_REQ,	/* margin before footer */
	"m4",	m4_REQ,	/* bottom margin,	incl. footer */
	"mc",	mc_REQ,	/* insert marginal character */
	"mo",	mo_REQ,	/* set marginal character output margin */
	"na",	na_REQ,	/* no-adjust */
	"ne",	ne_REQ,	/* need */
	"nf",	nf_REQ,	/* no-fill */
	"nh",	nh_REQ,	/* hyphenation off */
	"ns",	ns_REQ,	/* no-space */
	"nx",	nx_REQ,	/* next file */
	"of",	of_REQ,	/* set footer for odd pages */
	"oh",	oh_REQ,	/* set header for odd pages */
	"oo",	oo_REQ,	/* set odd page offset */
	"pl",	pl_REQ,	/* page length */
	"pn",	pn_REQ,	/* page number */
	"po",	po_REQ,	/* page offset */
	"ps",	ps_REQ,	/* skip pages */
	"rc",	rc_REQ,	/* replacement character */
	"rm",	rm_REQ,	/* right margin */
	"rs",	rs_REQ,	/* restore spacing */
	"sb",	sb_REQ,	/* single blank mode */
	"so",	so_REQ,	/* source */
	"sp",	sp_REQ,	/* space */
	"ta",	ta_REQ,	/* tab stops */
	"tc",	tc_REQ,	/* tab character */
	"ti",	ti_REQ,	/* temporary indent */
	"tl",	tl_REQ,	/* three-part title */
	"ul",	ul_REQ,	/* underline */
	"xb",	xb_REQ,	/* extra blank mode */
	NULL,	0
	};

TABLE Functions[] = {		/* inline special functions */
	"LETTER",	LETTER_FN,	/* convert to upper-case letter equiv. */
	"RN",		RN_FN,		/* convert to upper-case Roman numerals */
	"add",		add_FN,		/* add quantity to number register */
	"bf",		bf_FN,		/* boldface text */
	"bottom",	bottom_FN,	/* return the number of the last printed line */
	"c2",		c2_FN,		/* no-break control character */
	"cap",		cap_FN,		/* capitalize text */
	"cc",		cc_FN,		/* control character */
	"cmp",		cmp_FN,		/* perform string comparison and return 1 (true) or 0 (false) */
	"cu",		cu_FN,		/* continuously underline text */
	"date",		date_FN,	/* mm/dd/yy */
	"day",		day_FN,		/* day of the week */
	"even",		even_FN,	/* test if number is even */
	"evenfooter",	evenfooter_FN,	/* return even page footer */
	"evenheader",	evenheader_FN,	/* return even page header */
	"footer",	footer_FN,	/* return footer */
	"header",	header_FN,	/* return header */
	"icmp",		icmp_FN,	/* perform integer comparison and return 1 (true) or 0 (false) */
	"in",		in_FN,		/* indentation */
	"it",		it_FN,		/* italicize */
	"ldate",	ldate_FN,	/* long date (e.g., January 31, 1966) */
	"letter",	letter_FN,	/* convert to lower-case letter equiv. */
	"lm",		lm_FN,		/* left margin */
	"ln",		ln_FN,		/* line number */
	"ls",		ls_FN,		/* line spacing */
	"lt",		lt_FN,		/* title width */
	"m1",		m1_FN,		/* margin 1 */
	"m2",		m2_FN,		/* margin 2 */
	"m3",		m3_FN,		/* margin 3 */
	"m4",		m4_FN,		/* margin 4 */
	"minus",	minus_FN,	/* subtract arg2 from arg1 */
	"ml",		ml_FN,		/* macro level */
	"ns",		ns_FN,		/* no space flag */
	"num",		num_FN,		/* fetch value of number register */
	"odd",		odd_FN,		/* test if a number is odd */
	"oddfooter",	oddfooter_FN,	/* return odd page footer */
	"oddheader",	oddheader_FN,	/* return odd page header */
	"pl",		pl_FN,		/* page length */
	"plus",		plus_FN,	/* add 2 numbers */
	"pn",		pn_FN,		/* page number */
	"po",		po_FN,		/* page offset */
	"rm",		rm_FN,		/* right margin */
	"rn",		rn_FN,		/* convert to lower-case Roman numerals */
	"set",		set_FN,		/* set value of number register */
	"small",	small_FN,	/* map all characters of text to lower case */
	"sub",		sub_FN,		/* generate a subscript */
	"sup",		sup_FN,		/* generate a superscript */
	"tc",		tc_FN,		/* tab character */
	"tcpn",		tcpn_FN,	/* page number, rt. justified in 4-char field */
	"ti",		ti_FN,		/* temporary indentation */
	"time",		time_FN,	/* hh:mm:ss */
	"top",		top_FN,		/* return the number of the first printed line */
	"ul",		ul_FN,		/* selectively underline text */
	"vertspace",	vertspace_FN,	/* change vert space on spinwriter */
	NULL,		0
};

TABLE Spchars[] = {		/* special characters */
	"ALPHA",	ALPHA,
	"AND",		AND,
	"BETA",		BETA,
	"BS",		'\b',
	"CHI",		CHI,
	"DELTA",	DELTA,
	"EPSILON",	EPSILON,
	"ETA",		ETA,
	"GAMMA",	GAMMA,
	"INTEGRAL",	INTEGRAL,
	"IOTA",		IOTA,
	"KAPPA",	KAPPA,
	"LAMBDA",	LAMBDA,
	"MU",		MU,
	"NU",		NU,
	"OMEGA",	OMEGA,
	"OMICRON",	OMICRON,
	"OR",		OR,
	"PHI",		PHI,
	"PI",		PI,
	"PSI",		PSI,
	"RHO",		RHO,
	"SIGMA",	SIGMA,
	"TAU",		TAU,
	"THETA",	THETA,
	"UPSILON",	UPSILON,
	"XI",		XI,
	"ZETA",		ZETA,
	"alpha",	alpha,
	"apeq",		apeq,
	"backslash",	backslash,
	"beta",		beta,
	"bl",		PHANTOMBL,
	"bs",		'\b',
	"chi",		chi,
	"delta",	delta,
	"dollar",	dollar,
	"downarrow",	downarrow,
	"dquote",	dquote,
	"epsilon",	epsilon,
	"eta",		eta,
	"exist",	exist,
	"gamma",	gamma,
	"ge",		ge,
	"hlf",		hlf,
	"hlr",		hlr,
	"imp",		imp,
	"infinity",	infinity,
	"integral",	integral,
	"iota",		iota,
	"iso",		iso,
	"kappa",	kappa,
	"lambda",	lambda,
	"largelbrace",	largelbrace,
	"largerbrace",	largerbrace,
	"lceil",	lceil,
	"le",		le,
	"lfloor",	lfloor,
	"mu",		mu,
	"nabla",	nabla,
	"ne",		ne,
	"nexist",	nexist,
	"not",		not,
	"nu",		nu,
	"omega",	omega,
	"omicron",	omicron,
	"partial",	partial,
	"phi",		phi,
	"pi",		pi,
	"proportional",	proportional,
	"psi",		psi,
	"psset",	psset,
	"rceil",	rceil,
	"rfloor",	rfloor,
	"rho",		rho,
	"scolon",	scolon,
	"sigma",	sigma,
	"small0",	small0,
	"small1",	small1,
	"small2",	small2,
	"small3",	small3,
	"small4",	small4,
	"small5",	small5,
	"small6",	small6,
	"small7",	small7,
	"small8",	small8,
	"small9",	small9,
	"sset",		sset,
	"tau",		tau,
	"theta",	theta,
	"tilde",	tilde,
	"univ",		univ,
	"uparrow",	uparrow,
	"upsilon",	upsilon,
	"xi",		xi,
	"zeta",		zeta,
	NULL,		0
	};


/* fmt --- text formatter main program */

main(argc, argv)
int argc;
char **argv;
{
	int readln();
	char inbuf [MAXOUT];

	static char errbuf[BUFSIZ];

	setbuf (stderr, errbuf);

	xargc = argc;
	xargv = argv;

	initialize();

	while (readln (inbuf, MAXOUT) != EOF)
		processline (inbuf);

	reset_files();
	exit(0);
}



/* options --- process options, if any */

options()
{
	int j;
	char arg [30];
	static char usage[]  = "Usage: %s [-s] [-p first[-last]] {pathname}";
	char *name;

	name = xargv[0];
	for (xargc--, xargv++; xargv[0][0] == '-' && xargv[0][1] != '\0';
	    xargc--, xargv++)
	{
		switch (xargv[0][1]) {
		case 's':
			Stop_mode = YES;
			break;

		case 'p':
			if (xargv[0][2] == '\0')
			{
				strcpy (arg, xargv[1]);
				xargc--;
				xargv++;
			}
			else
				strcpy (arg, &xargv[0][2]);

			for (j = 0; arg[j] != '-' && arg[j] != '\0'; j++)
				Start_page = Start_page * 10 + arg[j] - '0';

			if (arg[j] == '-')
			{
				End_page = 0;
				for (j++; arg[j] != '\0'; j++)
					End_page = End_page * 10 + arg[j] - '0';
				if (End_page <= Start_page)
					End_page = HUGE;
			}
			break;

		default:
			fprintf (stderr, usage, name);
			exit (1);
		}
	}
}



/* initialize --- set parameters to default values */

initialize()
{
	int i;

	Bottom = Plval - M3val - M4val;

	for (i = 0; i <= MAXNUMREGS; i++)
		Numreg[i] = 0;

	for (i = 1; i <= MAXLINE; i++)
		Tabs[i] = ((i % 8) == 1);

	/* output file list */
	for (i = 0; i <= MAXFILES; i++)
	{
		O_list[i].id = 0;
		O_list[i].fp = NULL;
	}

	/* command line options and file names: */
	F_ptr = -1;
	options();		/* check for any options */
	if (xargc == 0)
	{
		/* no args left */
		F_ptr++;
		F_list [F_ptr].F_type = FILETYPE;
		F_list [F_ptr].source.fp = stdin;
	}
	else
	{
		strcpy(File_name, xargv[Next_arg]);
		Next_arg++;
		newinp (File_name);
	}
}

/* error --- print a string on the error output and die */

error(message)
char *message;
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}
