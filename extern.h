/*
** extern.h
**
** global variables used by the text formatter.
**
** fmt.h MUST be included in front of this file.
*/

/* Concerning output: */
extern int Outp;		/* last char position in outbuf */
extern int Outw;		/* width of text currently in outbuf */
extern int Outwds;		/* number of words in outbuf */
extern char Outbuf[MAXOUT];	/* lines to be filled collect here */

/* Concerning pages: */
extern int Start_page;	/* page on which printing begins */
extern int End_page;	/* page on which printing ends */
extern int Curpag;	/* current output page number */
extern int Newpag;	/* next output page number */
extern int Lineno;	/* next line to be printed */
extern int Plval;	/* page length in lines */
extern int M1val;	/* margin before and including header */
extern int M2val;	/* margin after header */
extern int M3val;	/* margin after last text line */
extern int M4val;	/* bottom margin, including footer */
extern int Bottom;	/* last live line on page, = plval-m3val-m4val */
extern char Even_header[MAXOUT];	/* heading for even pages */
extern char Odd_header[MAXOUT];		/* heading for odd pages */
extern char Even_footer[MAXOUT];	/* footing for even pages */
extern char Odd_footer[MAXOUT];		/* footing for odd pages */

/* Concerning paramaters set by user: */
extern int Fill;	/* fill if YES */
extern int Nospace;	/* inhibit spacing */
extern char Adjust;	/* adjustment mode */
extern int Dvflag;	/* divert unformatted output */
extern int Tiwidth;	/* title width */
extern int Poval;	/* page offset */
extern int Moval;	/* marginal character offset */
extern int Inval;	/* current indent; >= 0; always = lmval - 1 */
extern int Tival;	/* current temporary indent */
extern int Lmval;	/* left margin; always equals inval + 1 */
extern int Rmval;	/* current right margin */
extern int Bfval;	/* number of lines to boldface */
extern int Ceval;	/* number of lines to center */
extern int Itval;	/* number of lines to italicize */
extern int Ulval;	/* number of lines to underline */
extern int Lsval;	/* current line spacing */
extern int Ooval;	/* odd page offset value */
extern int Eoval;	/* even page offset value */

/* Concerning miscellanious control variables: */
extern int Hyphenation;		/* YES if hyphenation is on */
extern int Extra_blank_mode;	/* YES if extra blanks wanted after periods */
extern int Nobreak;		/* YES if no-break char specified */
extern int Word_last;		/* YES if word was just put in outbuf */
extern int F_ptr;		/* index into f_list, indicates current input file */
extern INPUT F_list[MAXFILES];		/* stack of currently-open input files */
extern TEMPFILE O_list[MAXFILES+1];	/* descriptors of output temporaries; init = NULL */
extern int Using_temps;	/* Is the current divert a temporary? */
extern FILE *Out_file;	/* output file descriptor for divert */
extern int Next_arg;		/* next argument to be checked for file name */
extern int Tabs[MAXLINE+1];		/* tab stops for tab expansion */
extern int Numreg[MAXNUMREGS+1];	/* number registers for general use */
extern int Mcout;		/* YES if current marginal char has been output */
extern char Mcch;		/* marginal character */
extern char Tmcch;		/* temporary marginal character */
extern char Cmdch;		/* command character */
extern char Nbcch;		/* no-break command character */
extern char Replch;		/* tab replacement character */
extern char Tabch;		/* tab character */
extern char File_name[MAXPATH];	/* buffer used by various file handlers */

/* Concering macros: */
extern int Maclvl;			/* nesting level for macro calls */
extern int Argv[MAXMACLVL];	/* beginning of arg list for each macro level */
extern int Argtop;			/* next free location in argbuf */
extern int First_macro;	/* location of first macro in macbuf */
extern int Mactop;			/* next free location in macbuf */
extern char Macbuf[MACBUFSIZE];	/* macro text buffer area */
extern char Argbuf[ARGBUFSIZE];	/* buffer for macro arguments */

/* Concerning options: */
extern int Stop_mode;		/* YES iff -s option on command */

/* Concerning the command line: */
extern int xargc;	/* argument count */
extern char **xargv;	/* argument values */

/* Concerning tables: */
extern TABLE Requests[];	/* formatter requests */
extern TABLE Functions[];	/* inline special functions */
extern TABLE Spchars[];		/* special characters */
