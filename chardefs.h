/*
** chardefs.h
**
** definitions of special chars for swtfmt, dprint, & sprint
** use one file, so that all three programs agree.
**
** These must be written as char constants to avoid sign
** extension problems on the vax. sigh.
*/

#define alpha	'\341'	/* a */
#define ALPHA	'\301'	/* A */
#define beta	'\342'	/* b */
#define BETA	'\302'	/* B */
#define gamma	'\347'	/* g */
#define GAMMA	'\307'	/* G */
#define delta	'\344'	/* d */
#define DELTA	'\304'	/* D */
#define epsilon	'\345'	/* e */
#define EPSILON	'\305'	/* E */
#define zeta	'\372'	/* z */
#define ZETA	'\332'	/* Z */
#define eta	'\356'	/* n */
#define ETA	'\316'	/* N */
#define theta	'\350'	/* h */
#define THETA	'\310'	/* H */
#define iota	'\351'	/* i */
#define IOTA	'\311'	/* I */
#define kappa	'\353'	/* k */
#define KAPPA	'\312'	/* J */
#define lambda	'\354'	/* l */
#define LAMBDA	'\314'	/* L */
#define mu	'\365'	/* u */
#define MU	'\325'	/* U */
#define nu	'\366'	/* v */
#define NU	'\326'	/* V */
#define xi	'\370'	/* x */
#define XI	'\330'	/* X */
#define omicron	'\357'	/* o */
#define OMICRON	'\317'	/* O */
#define pi	'\263'	/* 3 */
#define PI	'\264'	/* 4 */
#define rho	'\362'	/* r */
#define RHO	'\322'	/* R */
#define sigma	'\363'	/* s */
#define SIGMA	'\323'	/* S */
#define tau	'\364'	/* t */
#define TAU	'\324'	/* T */
#define upsilon	'\361'	/* q */
#define UPSILON	'\321'	/* Q */
#define phi	'\360'	/* p */
#define PHI	'\320'	/* P */
#define chi	'\343'	/* c */
#define CHI	'\303'	/* C */
#define psi	'\371'	/* y */
#define PSI	'\331'	/* Y */
#define omega	'\367'	/* w */
#define OMEGA	'\327'	/* W */

#define infinity	'\270'	/* 8 */
#define integral	'\273'	/* ; */
#define INTEGRAL	'\271'	/* 9 */
#define nabla		'\336'	/* ^ */
#define not		'\376'	/* ~ */
#define partial		'\255'	/* - */
#define downarrow	'\267'	/* 7 */
#define uparrow		'\266'	/* 6 */
#define backslash	'\265'	/* 5 */
#define tilde		'\262'	/* 2 */
#define largerbrace	'\260'	/* 0 */
#define largelbrace	'\261'	/* 1 */
#define proportional	'\275'	/* = */
#define apeq		'\315'	/* M */
#define ge		'\335'	/* ] */
#define imp		'\300'	/* @ */
#define exist		'\333'	/* [ */
#define AND		'\337'	/* _ */
#define ne		'\334'	/* \ */
#define psset		'\274'	/* < */
#define sset		'\276'	/* > */
#define le		'\277'	/* ? */
#define nexist		'\375'	/* } */
#define univ		'\340'	/* ` */
#define OR		'\373'	/* { */
#define iso		'\374'	/* | */
#define lfloor		'\346'	/* f */
#define rfloor		'\352'	/* j */
#define lceil		'\355'	/* m */
#define rceil		'\306'	/* F */
#define small0		'\241'	/* ! */
#define small1		'\242'	/* " */
#define small2		'\243'	/* # */
#define small3		'\244'	/* $ */
#define small4		'\245'	/* % */
#define small5		'\246'	/* & */
#define small6		'\247'	/* ' */
#define small7		'\250'	/* ( */
#define small8		'\251'	/* ) */
#define small9		'\252'	/* * */
#define scolon		'\253'	/* + */
#define dquote		'\254'	/* , */
#define dollar		'\256'	/* . */
