/*
** fmt.h
**
** defines and other stuff for fmt
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* some Software Tools defines */
#define YES		1
#define NO		0
#define OK		1
#define ERR		-2
#define BITS_PER_WORD	16
#define ESCAPE		'@'
#define UPPER		'A'
#define LOWER		'a'
#define BELL		'\007'
#define DEL		'\177'
#define MAXPATH		180

#define SYS_DAY		1
#define SYS_DATE	2
#define SYS_TIME	3
#define SYS_LDATE	4

#define BOTH		'b'	/* how to adjust: 'b' =-> both */

#define INSIZE		500
#define MAXLINE		134	/* max number of output columns + 2 */
#define OVERSTRIKES	2	/* number of times to overstrike for boldfacing */
#define MAXOUT		((4 * OVERSTRIKES + 3) * (MAXLINE - 2) + 2)
#define PAGENUM		'#'
#define PAGEWIDTH	60
#define PAGELEN		66
#define MARGIN1		3
#define MARGIN2		2
#define MARGIN3		2
#define MARGIN4		3
#define MAXFILES	200	/* 100 */
#define MACBUFSIZE	50000	/* 4000 */
#define MAXMACLVL	100
#define ENDOFLIST	-3
#define MACSTART	5
#define INBETWEEN	-1
#define ARGBUFSIZE	1000
#define MAXNUMREGS	200	/* 64 */
#define PHANTOMBL	'\240'		/* ' '|0200 */
#define MAXTRAP		66
#define NOMK		-1

#define COMMENT	-1      /* must be < 1, differ from UNKNOWN */
#define UNKNOWN	-2      /* must be < 1, differ from COMMENT */

#define HUGE		10000
#define BITS_PER_WORD	16

#define CHECK_PAGE_RANGE	if (Curpag < Start_page || Curpag > End_page)\
					return

#define SKIPCM(b,i)	{while (b[i] != ' ' && b[i] != '\t' &&  b[i] != '\n' \
				&& b[i] != '\0')(i)++; SKIPBL(b,i);}

#define SKIPBL(s,i)	while (s[i] == ' ' || s[i] == '\t') (i)++

/* special chars are done by turning on parity bit of normal char */
#include "chardefs.h"

/* up and down for sub- and super-scripts: */
#define hlf		'\212'	/* linefeed */
#define hlr		'\213'	/* vertical tab */

/* formatter requests */
#define ad_REQ	1
#define bf_REQ	2
#define bp_REQ	3
#define br_REQ	4
#define c2_REQ	5
#define cc_REQ	6
#define ce_REQ	7
#define de_REQ	8
#define dv_REQ	9
#define ef_REQ	10
#define eh_REQ	11
#define en_REQ	12
#define er_REQ	13
#define ex_REQ	14
#define fi_REQ	15
#define fo_REQ	16
#define he_REQ	17
#define hy_REQ	18
#define in_REQ	19
#define lm_REQ	20
#define ls_REQ	21
#define lt_REQ	22
#define m1_REQ	23
#define m2_REQ	24
#define m3_REQ	25
#define m4_REQ	26
#define mc_REQ	27
#define mo_REQ	28
#define na_REQ	29
#define ne_REQ	30
#define nf_REQ	31
#define nh_REQ	32
#define ns_REQ	33
#define nx_REQ	34
#define of_REQ	35
#define oh_REQ	36
#define pl_REQ	37
#define pn_REQ	38
#define po_REQ	39
#define rc_REQ	40
#define rm_REQ	41
#define rs_REQ	42
#define sb_REQ	43
#define so_REQ	44
#define sp_REQ	45
#define ta_REQ	46
#define tc_REQ	47
#define ti_REQ	48
#define tl_REQ	49
#define ul_REQ	50
#define xb_REQ	51
#define ps_REQ	52
#define oo_REQ	53
#define eo_REQ	54
#define if_REQ	55
#define am_REQ	56
#define it_REQ	57

/* function calls */
#define add_FN		1
#define c2_FN		2
#define cc_FN		3
#define date_FN		4
#define day_FN		5
#define in_FN		6
#define ldate_FN	7
#define lm_FN		8
#define ln_FN		9
#define ls_FN		10
#define ml_FN		11
#define num_FN		12
#define pl_FN		13
#define pn_FN		14
#define po_FN		15
#define rm_FN		16
#define set_FN		17
#define tc_FN		18
#define tcpn_FN		19
#define ti_FN		20
#define time_FN		21
#define rn_FN		22
#define RN_FN		23
#define bf_FN		24
#define ul_FN		25
#define cu_FN		26
#define sub_FN		27
#define sup_FN		28
#define letter_FN	29
#define LETTER_FN	30
#define vertspace_FN	31
#define m1_FN		33
#define m2_FN		34
#define m3_FN		35
#define m4_FN		36
#define even_FN		37
#define odd_FN		38
#define cap_FN		39
#define small_FN	40
#define plus_FN		41
#define minus_FN	42
#define header_FN	43
#define evenheader_FN	44
#define oddheader_FN	45
#define footer_FN	46
#define evenfooter_FN	47
#define oddfooter_FN	48
#define cmp_FN		49
#define bottom_FN	50
#define top_FN		51
#define lt_FN		52
#define icmp_FN		53
#define ns_FN		54
#define it_FN		55

/* define what a tabel entry looks like */
typedef struct _table {
	char *text;
	int val;
	} TABLE;

/* where input comes from */
typedef struct _input {
	enum intypes { FILETYPE, MACRO } F_type;
	union _sources {
		FILE *fp;	/* file pointers for real files */
		char *md;	/* macro descriptor, for macros */
		} source;
	} INPUT;

/* temporary files */

typedef struct _temp {
	int id;
	FILE *fp;
	} TEMPFILE;
