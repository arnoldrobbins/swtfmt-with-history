/*
** cmd.c
**
** do formatting commands
*/

#include "fmt.h"
#include "extern.h"

/* comand --- perform formatting command */

comand (buf)
char *buf;
{
	int i, ct, spval, val, val2, need;
	char title [MAXOUT], tbuf [MAXOUT];
	int argtyp;
	char esc();

	ct = comtyp (buf);
	val = getval (buf, & argtyp);

	if (domac (buf) == OK)	/* check for macro invocation */
		;		/* if so, do nothing here */
	else
	{
		switch (ct) {	/* branch on command type */

		case fi_REQ:   /* enable "fill" mode */
			do_brk();
			Fill = YES;
			break;

		case nf_REQ:   /* disable "fill" mode */
			do_brk();
			Fill = NO;
			break;

		case br_REQ:   /* force a break */
			Nobreak = NO;	/* override "no-break" control char */
			do_brk();
			break;

		case ls_REQ: /* set line spacing */
			set (& Lsval, val, argtyp, 1, 1, HUGE);
			break;

		case bp_REQ:   /* skip to the bottom of the page */
			do_brk();
			if (Lineno > 0)
				space (HUGE);
			set (& Newpag, val, argtyp, Newpag, -HUGE, HUGE);
			break;

		case ps_REQ:   /* skip pages */
			do_brk();
			if (Lineno > 0)		/* empty out current page */
				space (HUGE);
			i = 0;
			SKIPCM (buf, i);
			val = ctoi (buf, & i);
			SKIPBL (buf, i);
			val2 = ctoi (buf, & i);
			if (val2 <= 0)
				val2 = Newpag;
			for (i = Newpag % val2; i < val; i++)
				space (HUGE);
			break;

		case sp_REQ:   /* generate blank lines */
			do_brk();
			if (Nospace == NO)
			{
				set (& spval, val, argtyp, 1, 0, HUGE);
				space (spval);
			}
			break;

		case in_REQ:   /* set indent value */
			do_brk();
			set (& Inval, val, argtyp, 0, 0, Rmval - 1);
			Tival = Inval;
			break;

		case rm_REQ:   /* set right margin */
			do_brk();
			set (& Rmval, val, argtyp, PAGEWIDTH, Tival + 1, MAXLINE - 2);
			set (& Tiwidth, Rmval - Lmval + 1, '0', Tiwidth, 0, HUGE);
			break;

		case ti_REQ:   /* set temporary indent value */
			do_brk();
			set (& Tival, val, argtyp, 0, 0, Rmval);
			break;

		case ce_REQ:   /* center subsequent lines */
			do_brk();
			set (& Ceval, val, argtyp, 1, 0, HUGE);
			break;
		
		case it_REQ:  /* italicize subsequent lines */
			set (& Itval, val, argtyp, 1, 0, HUGE);
			break;

		case ul_REQ:  /* underline subsequent lines */
			set (& Ulval, val, argtyp, 1, 0, HUGE);
			break;

		case he_REQ:   /* set page headings */
			getstr (buf, Even_header, MAXOUT);
			strcpy (Odd_header, Even_header);
			break;

		case fo_REQ:   /* set page footings */
			getstr (buf, Even_footer, MAXOUT);
			strcpy (Odd_footer, Even_footer);
			break;

		case pl_REQ:   /* set page length */
			set (& Plval, val, argtyp, PAGELEN,
			    M1val + M2val + M3val + M4val + 1, HUGE);
			Bottom = Plval - M3val - M4val;
			break;

		case bf_REQ: /* boldface subsequent lines */
			set (& Bfval, val, argtyp, 1, 0, HUGE);
			break;

		case so_REQ: /* divert input stream */
			if (getstr (buf, File_name, MAXPATH) > 0)
				newinp (File_name);
			break;

		case de_REQ: /* define a macro */
			defmac (buf);
			break;

		case en_REQ: /* end a macro definition (processed by defmac) */
			break;

		case ta_REQ: /* set tab stops */
			settab (buf);
			break;

		case tc_REQ: /* set the tab character */
			getsc (buf, & Tabch, '\t');
			break;

		case cc_REQ: /* set the control character */
			getsc (buf, & Cmdch, '.');
			break;

		case c2_REQ: /* set the "no-break" control character */
			getsc (buf, & Nbcch, '`');
			break;

		case hy_REQ: /* enable automatic hyphenation */
			Hyphenation = YES;
			break;

		case nh_REQ: /* disable automatic hyphenation */
			Hyphenation = NO;
			break;

		case ne_REQ: /* check availability of contiguous lines */
			if (Nospace == NO)
			{
				do_brk();
				need = 0;
				set (& need, val, argtyp, 1, 0, HUGE);
				if (Lineno + need - 1 > Bottom  &&  Lineno > 0)
					space(HUGE);
			}
			break;

		case po_REQ:   /* set page offset */
			do_brk();
			set (& Poval, val, argtyp, 0, 0, HUGE);
			break;

		case oo_REQ:   /* set odd page offset */
			do_brk();
			set (& Ooval, val, argtyp, 0, -HUGE, HUGE);
			break;

		case eo_REQ:   /* set even page offset */
			do_brk();
			set (& Eoval, val, argtyp, 0, -HUGE, HUGE);
			break;

		case lm_REQ:   /* set left margin */
			do_brk();
			set (& Lmval, val, argtyp, 1, 1, Rmval);
			set (& Tiwidth, Rmval - Lmval + 1, '0', Tiwidth, 0, HUGE);
			break;

		case er_REQ:   /* write a message to the terminal */
			i = 0;
			SKIPCM (buf, i);
			if (buf[i] == '\'' || buf [i] == '"')
				i++;
			for (; buf [i] != '\n' && buf [i] != '\0'; i++)
				putc (esc (buf, & i) , stderr);
			break;

		case lt_REQ: /* set title length */
			set (& Tiwidth, val, argtyp, PAGEWIDTH, 1, HUGE);
			break;

		case tl_REQ:   /* generate a three-part title */
			do_brk();
			getstr (buf, title, MAXOUT);
			mktl (title, Curpag, tbuf, MAXOUT);
			/* Tival = 0;	/* titles are never indented */
			put (tbuf);
			break;

		case rc_REQ: /* set the tab replacement */
			getsc (buf, & Replch, PHANTOMBL);
			break;

		case ad_REQ:   /* set adjustment mode */
			getsc (buf, & Adjust, BOTH);
			if (Adjust != 'l' && Adjust != 'r' && Adjust != 'c')
				Adjust = BOTH;
			break;

		case na_REQ:   /* disable margin adjustment */
			Adjust = 'l';
			break;

		case ns_REQ:  /* enable no-space mode */
			Nospace = YES;
			break;

		case rs_REQ:  /* disable no-space mode */
			Nospace = NO;
			break;

		case pn_REQ: /* set next page number */
			set (& Newpag, val, argtyp, Newpag, -HUGE, HUGE);
			break;

		case ex_REQ:   /* exit */
			do_brk();
			reset_files();
			exit (0);
			break;

		case nx_REQ:   /* proceed to next input file */
			reset_files();
			if (getstr (buf, File_name, MAXPATH) > 0)
				newinp (File_name);
			else if (xargv [Next_arg] != NULL)
			{
				strcpy (File_name, xargv[Next_arg]);
				Next_arg++;
				newinp (File_name);
			}
			else
				exit (0);
			break;

		case xb_REQ: /* enable extra-blank mode */
			Extra_blank_mode = YES;
			break;

		case sb_REQ: /* disable extra-blank mode */
			Extra_blank_mode = NO;
			break;

		case m1_REQ: /* set margin-1 value */
			set (& M1val, val, argtyp, MARGIN1, 0,
			    Plval - M2val - M3val - M4val - 1);
			break;

		case m2_REQ: /* set margin-2 value */
			set (& M2val, val, argtyp, MARGIN2, 0,
			    Plval - M1val - M3val - M4val - 1);
			break;

		case m3_REQ:   /* set margin-3 value */
			set (& M3val, val, argtyp, MARGIN3, 0,
			    Plval - M1val - M2val - M4val - 1);
			Bottom = Plval - M3val - M4val;
			break;

		case m4_REQ:   /* set margin-4 value */
			set (& M4val, val, argtyp, MARGIN4, 0,
			    Plval - M1val - M2val - M3val - 1);
			Bottom = Plval - M3val - M4val;
			break;

		case dv_REQ:   /* divert output stream */
			do_brk();
			if (getstr (buf, File_name, MAXPATH) > 0)
			{
				Dvflag = YES;
				newout (File_name);
			}
			else
			{
				Dvflag = NO;
				fflush (Out_file);
				if (! Using_temps)
					fclose (Out_file);
				else
					Using_temps = NO;
			}
			break;

		case mc_REQ:   /* set margin character */
			getsc (buf, & Mcch, ' ');
			if (Mcch != ' ' || Outp <= 0 && Mcout == YES)
				Tmcch = Mcch;
			Mcout = NO;
			break;

		case mo_REQ: /* set margin-offset */
			set (& Moval, val, argtyp, 0, 0, HUGE);
			break;

		case oh_REQ: /* set odd-numbered page heading */
			getstr (buf, Odd_header, MAXOUT);
			break;

		case of_REQ: /* set odd-numbered page footing */
			getstr (buf, Odd_footer, MAXOUT);
			break;

		case eh_REQ: /* set even-numbered page heading */
			getstr (buf, Even_header, MAXOUT);
			break;

		case ef_REQ:	/* set even-numbered page footing */
			getstr (buf, Even_footer, MAXOUT);
			break;

		case if_REQ:	/* conditional macro execution */
			chkif (buf);
			break;
		
		case am_REQ:	/* append to a macro */
			apndmac (buf);
			break;
		
		case COMMENT:
			break;

		default:
			text(buf);	/* not a recognizable command */

		}
	}
}



/* comtyp --- decode command type */

int comtyp (buf)
char *buf;
{
	int lookup();
	char comstr [3];
	int ret;

	comstr [0] = buf [1];
	comstr [1] = buf [2];
	comstr [2] = '\0';

	if (lookup (comstr, & ret, Requests) == NO)
		if (comstr [0] == '#')
			ret = COMMENT;
		else
			ret = UNKNOWN;
	/* else
		ret now has command type in it */
	
	return (ret);
}



/* text --- process text lines */

text (inbuf)
char *inbuf;
{
	int i;
	int getwrd();
	char wrdbuf [MAXOUT];

	if (Dvflag == YES)
	{
		putlin (inbuf, Out_file);
		return;
	}

	if (inbuf [0] == ' ' || inbuf [0] == '\n')
		leadbl (inbuf);  /* move left, set tival */

	if (Itval > 0)		/* italicizing */
	{
		italic (inbuf, wrdbuf, MAXOUT);
		ctoc (wrdbuf, inbuf, MAXOUT);
		Itval--;
	}

	if (Ulval > 0)		/* underlining */
	{
		underl (inbuf, wrdbuf, MAXOUT, NO);
		ctoc (wrdbuf, inbuf, MAXOUT);
		Ulval--;
	}

	if (Bfval > 0)		/* boldface type */
	{
		boface (inbuf, wrdbuf, MAXOUT);
		ctoc (wrdbuf, inbuf, MAXOUT);
		Bfval--;
	}

	if (Ceval > 0)		/* centering */
	{
		center (inbuf);
		put (inbuf);
		Ceval--;
	}
	else if (inbuf [0] == '\n')	/* all blank line */
		put (inbuf);
	else if (Fill == NO)		/* unfilled text */
		put (inbuf);
	else				/* filled text */
		for (i = 0; getwrd (inbuf, & i, wrdbuf) > 0;)
			putwrd (wrdbuf);
}

/* processline --- process line of input (text or commands) */

processline (inbuf)
char *inbuf;
{
	Nobreak = NO;
	if (inbuf [0] == Cmdch || inbuf [0] == Nbcch)
	{
		/* it's a command */
		if (inbuf [0] == Nbcch)
			Nobreak = YES;
		comand (inbuf);
	}
	else	/* it's text */
		text (inbuf);
}
