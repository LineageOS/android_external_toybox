/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

/*** Begin copyright for ytab.c ***/

/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/*** End copyright for ytab.c ***/


/* lasciate ogne speranza, voi ch'intrate. */

/* awk.c - Stream editor

USE_AWK(NEWTOY(awk, "(help)(version)F:v*f*", TOYFLAG_USR|TOYFLAG_BIN|TOYFLAG_LOCALE|TOYFLAG_NOHELP))

config AWK
  bool "awk"
  default n
  help
    usage: awk [-F fs] [-v var=value] [-f progfile | 'prog'] [file ...]
*/

#define FOR_awk
#include "toys.h"

/*** Common source preamble ***/
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>


/*** awk.h ***/

typedef double	Awkfloat;

/* unsigned char is more trouble than it's worth */

typedef	unsigned char uschar;

#define	xfree(a)	{ if ((a) != NULL) { free((void *) (a)); (a) = NULL; } }

#define	NN(p)	((p) ? (p) : "(null)")	/* guaranteed non-null for dprintf 
*/
#define	DEBUG
#ifdef	DEBUG
			/* uses have to be doubly parenthesized */
#	define	dprintf(x)	if (dbg) printf x
#else
#	define	dprintf(x)
#endif

extern int	compile_time;	/* 1 if compiling, 0 if running */
extern int	safe;		/* 0 => unsafe, 1 => safe */

#define	RECSIZE	(8 * 1024)	/* sets limit on records, fields, etc., etc. */
extern int	recsize;	/* size of current record, orig RECSIZE */

extern char	**FS;
extern char	**RS;
extern char	**ORS;
extern char	**OFS;
extern char	**OFMT;
extern Awkfloat *NR;
extern Awkfloat *FNR;
extern Awkfloat *NF;
extern char	**FILENAME;
extern char	**SUBSEP;
extern Awkfloat *RSTART;
extern Awkfloat *RLENGTH;

extern char	*record;	/* points to $0 */
extern int	lineno;		/* line number in awk program */
extern int	errorflag;	/* 1 if error has occurred */
extern int	donefld;	/* 1 if record broken into fields */
extern int	donerec;	/* 1 if record is valid (no fld has changed */
extern char	inputFS[];	/* FS at time of input, for field splitting */

extern int	dbg;

extern	char	*patbeg;	/* beginning of pattern matched */
extern	int	patlen;		/* length of pattern matched.  set in b.c */

/* Cell:  all information about a variable or constant */

typedef struct Cell {
	uschar	ctype;		/* OCELL, OBOOL, OJUMP, etc. */
	uschar	csub;		/* CCON, CTEMP, CFLD, etc. */
	char	*nval;		/* name, for variables only */
	char	*sval;		/* string value */
	Awkfloat fval;		/* value as number */
	int	 tval;		/* type info: STR|NUM|ARR|FCN|FLD|CON|DONTFREE */
	struct Cell *cnext;	/* ptr to next if chained */
} Cell;

typedef struct Array {		/* symbol table array */
	int	nelem;		/* elements in table right now */
	int	size;		/* size of tab */
	Cell	**tab;		/* hash table pointers */
} Array;

#define	NSYMTAB	50	/* initial size of a symbol table */
extern Array	*symtab;

extern Cell	*nrloc;		/* NR */
extern Cell	*fnrloc;	/* FNR */
extern Cell	*nfloc;		/* NF */
extern Cell	*rstartloc;	/* RSTART */
extern Cell	*rlengthloc;	/* RLENGTH */

/* Cell.tval values: */
#define	NUM	01	/* number value is valid */
#define	STR	02	/* string value is valid */
#define DONTFREE 04	/* string space is not freeable */
#define	CON	010	/* this is a constant */
#define	ARR	020	/* this is an array */
#define	FCN	040	/* this is a function name */
#define FLD	0100	/* this is a field $1, $2, ... */
#define	REC	0200	/* this is $0 */


/* function types */
#define	FLENGTH	1
#define	FSQRT	2
#define	FEXP	3
#define	FLOG	4
#define	FINT	5
#define	FSYSTEM	6
#define	FRAND	7
#define	FSRAND	8
#define	FSIN	9
#define	FCOS	10
#define	FATAN	11
#define	FTOUPPER 12
#define	FTOLOWER 13
#define	FFLUSH	14

/* Node:  parse tree is made of nodes, with Cell's at bottom */

typedef struct Node {
	int	ntype;
	struct	Node *nnext;
	int	lineno;
	int	nobj;
	struct	Node *narg[1];	/* variable: actual size set by calling malloc */
} Node;

#define	NIL	((Node *) 0)

extern Node	*winner;
extern Node	*nullstat;
extern Node	*nullnode;

/* ctypes */
#define OCELL	1
#define OBOOL	2
#define OJUMP	3

/* Cell subtypes: csub */
#define	CFREE	7
#define CCOPY	6
#define CCON	5
#define CTEMP	4
#define CNAME	3 
#define CVAR	2
#define CFLD	1
#define	CUNK	0

/* bool subtypes */
#define BTRUE	11
#define BFALSE	12

/* jump subtypes */
#define JEXIT	21
#define JNEXT	22
#define	JBREAK	23
#define	JCONT	24
#define	JRET	25
#define	JNEXTFILE	26

/* node types */
#define NVALUE	1
#define NSTAT	2
#define NEXPR	3


extern	int	pairstack[], paircnt;

#define notlegal(n)	(n <= FIRSTTOKEN || n >= LASTTOKEN || proctab[n-FIRSTTOKEN] == nullproc)
#define isvalue(n)	((n)->ntype == NVALUE)
#define isexpr(n)	((n)->ntype == NEXPR)
#define isjump(n)	((n)->ctype == OJUMP)
#define isexit(n)	((n)->csub == JEXIT)
#define	isbreak(n)	((n)->csub == JBREAK)
#define	iscont(n)	((n)->csub == JCONT)
#define	isnext(n)	((n)->csub == JNEXT || (n)->csub == JNEXTFILE)
#define	isret(n)	((n)->csub == JRET)
#define isrec(n)	((n)->tval & REC)
#define isfld(n)	((n)->tval & FLD)
#define isstr(n)	((n)->tval & STR)
#define isnum(n)	((n)->tval & NUM)
#define isarr(n)	((n)->tval & ARR)
#define isfcn(n)	((n)->tval & FCN)
#define istrue(n)	((n)->csub == BTRUE)
#define istemp(n)	((n)->csub == CTEMP)
#define	isargument(n)	((n)->nobj == ARG)
/* #define freeable(p)	(!((p)->tval & DONTFREE)) */
#define freeable(p)	( ((p)->tval & (STR|DONTFREE)) == STR )

/* structures used by regular expression matching machinery, mostly b.c: */

#define NCHARS	(256+3)		/* 256 handles 8-bit chars; 128 does 7-bit */
				/* watch out in match(), etc. */
#define NSTATES	32

typedef struct rrow {
	long	ltype;	/* long avoids pointer warnings on 64-bit */
	union {
		int i;
		Node *np;
		uschar *up;
	} lval;		/* because Al stores a pointer in it! */
	int	*lfollow;
} rrow;

typedef struct fa {
	uschar	gototab[NSTATES][NCHARS];
	uschar	out[NSTATES];
	uschar	*restr;
	int	*posns[NSTATES];
	int	anchor;
	int	use;
	int	initstat;
	int	curstat;
	int	accept;
	int	reset;
	struct	rrow re[1];	/* variable: actual size set by calling malloc */
} fa;


/*** proto.h ***/
extern	int	yywrap(void);
extern	void	setfname(Cell *);
extern	int	constnode(Node *);
extern	char	*strnode(Node *);
extern	Node	*notnull(Node *);
extern	int	yyparse(void);

extern	int	yylex(void);
extern	void	startreg(void);
extern	int	input(void);
extern	void	unput(int);
extern	void	unputstr(const char *);
extern	int	yylook(void);
extern	int	yyback(int *, int);
extern	int	yyinput(void);

extern	fa	*makedfa(const char *, int);
extern	fa	*mkdfa(const char *, int);
extern	int	makeinit(fa *, int);
extern	void	penter(Node *);
extern	void	freetr(Node *);
extern	int	hexstr(uschar **);
extern	int	quoted(uschar **);
extern	char	*cclenter(const char *);
extern	void	overflo(const char *) __attribute__((__noreturn__));
extern	void	cfoll(fa *, Node *);
extern	int	first(Node *);
extern	void	follow(Node *);
extern	int	member(int, const char *);
extern	int	match(fa *, const char *);
extern	int	pmatch(fa *, const char *);
extern	int	nematch(fa *, const char *);
extern	Node	*reparse(const char *);
extern	Node	*regexp(void);
extern	Node	*primary(void);
extern	Node	*concat(Node *);
extern	Node	*alt(Node *);
extern	Node	*unary(Node *);
extern	int	relex(void);
extern	int	cgoto(fa *, int, int);
extern	void	freefa(fa *);

extern	int	pgetc(void);
extern	char	*cursource(void);

extern	Node	*nodealloc(int);
extern	Node	*exptostat(Node *);
extern	Node	*node1(int, Node *);
extern	Node	*node2(int, Node *, Node *);
extern	Node	*node3(int, Node *, Node *, Node *);
extern	Node	*node4(int, Node *, Node *, Node *, Node *);
extern	Node	*stat3(int, Node *, Node *, Node *);
extern	Node	*op2(int, Node *, Node *);
extern	Node	*op1(int, Node *);
extern	Node	*stat1(int, Node *);
extern	Node	*op3(int, Node *, Node *, Node *);
extern	Node	*op4(int, Node *, Node *, Node *, Node *);
extern	Node	*stat2(int, Node *, Node *);
extern	Node	*stat4(int, Node *, Node *, Node *, Node *);
extern	Node	*celltonode(Cell *, int);
extern	Node	*rectonode(void);
extern	Node	*makearr(Node *);
extern	Node	*pa2stat(Node *, Node *, Node *);
extern	Node	*linkum(Node *, Node *);
extern	void	defn(Cell *, Node *, Node *);
extern	int	isarg(const char *);
extern	char	*tokname(int);
extern	Cell	*(*proctab[])(Node **, int);
extern	int	ptoi(void *);
extern	Node	*itonp(int);

extern	void	syminit(void);
extern	void	arginit(char **);
extern	void	envinit(char **);
extern	Array	*makesymtab(int);
extern	void	freesymtab(Cell *);
extern	void	freeelem(Cell *, const char *);
extern	Cell	*setsymtab(const char *, const char *, double, unsigned int, Array *);
extern	int	hash(const char *, int);
extern	void	rehash(Array *);
extern	Cell	*lookup(const char *, Array *);
extern	double	setfval(Cell *, double);
extern	void	funnyvar(Cell *, const char *);
extern	char	*setsval(Cell *, const char *);
extern	double	getfval(Cell *);
extern	char	*getsval(Cell *);
extern	char	*getpssval(Cell *);     /* for print */
extern	char	*tostring(const char *);
extern	char	*qstring(const char *, int);

extern	void	recinit(unsigned int);
extern	void	initgetrec(void);
extern	void	makefields(int, int);
extern	void	growfldtab(int n);
extern	int	getrec(char **, int *, int);
extern	void	nextfile(void);
extern	int	readrec(char **buf, int *bufsize, FILE *inf);
extern	char	*getargv(int);
extern	void	setclvar(char *);
extern	void	fldbld(void);
extern	void	cleanfld(int, int);
extern	void	newfld(int);
extern	int	refldbld(const char *, const char *);
extern	void	recbld(void);
extern	Cell	*fieldadr(int);
extern	void	yyerror(const char *);
extern	void	fpecatch(int);
extern	void	bracecheck(void);
extern	void	bcheck2(int, int, int);
extern	void	SYNTAX(const char *, ...);
extern	void	FATAL(const char *, ...) __attribute__((__noreturn__));
extern	void	WARNING(const char *, ...);
extern	void	error(void);
extern	void	eprint(void);
extern	void	bclass(int);
extern	double	errcheck(double, const char *);
extern	int	isclvar(const char *);
extern	int	is_number(const char *);

extern	int	adjbuf(char **pb, int *sz, int min, int q, char **pbp, const char *what);
extern	void	run(Node *);
extern	Cell	*execute(Node *);
extern	Cell	*program(Node **, int);
extern	Cell	*call(Node **, int);
extern	Cell	*copycell(Cell *);
extern	Cell	*arg(Node **, int);
extern	Cell	*jump(Node **, int);
extern	Cell	*awkgetline(Node **, int);
extern	Cell	*getnf(Node **, int);
extern	Cell	*array(Node **, int);
extern	Cell	*awkdelete(Node **, int);
extern	Cell	*intest(Node **, int);
extern	Cell	*matchop(Node **, int);
extern	Cell	*boolop(Node **, int);
extern	Cell	*relop(Node **, int);
extern	void	tfree(Cell *);
extern	Cell	*gettemp(void);
extern	Cell	*field(Node **, int);
extern	Cell	*indirect(Node **, int);
extern	Cell	*substr(Node **, int);
extern	Cell	*sindex(Node **, int);
extern	int	format(char **, int *, const char *, Node *);
extern	Cell	*awksprintf(Node **, int);
extern	Cell	*awkprintf(Node **, int);
extern	Cell	*arith(Node **, int);
extern	double	ipow(double, int);
extern	Cell	*incrdecr(Node **, int);
extern	Cell	*assign(Node **, int);
extern	Cell	*cat(Node **, int);
extern	Cell	*pastat(Node **, int);
extern	Cell	*dopa2(Node **, int);
extern	Cell	*split(Node **, int);
extern	Cell	*condexpr(Node **, int);
extern	Cell	*ifstat(Node **, int);
extern	Cell	*whilestat(Node **, int);
extern	Cell	*dostat(Node **, int);
extern	Cell	*forstat(Node **, int);
extern	Cell	*instat(Node **, int);
extern	Cell	*bltin(Node **, int);
extern	Cell	*printstat(Node **, int);
extern	Cell	*nullproc(Node **, int);
extern	FILE	*redirect(int, Node *);
extern	FILE	*openfile(int, const char *);
extern	const char	*filename(FILE *);
extern	Cell	*closefile(Node **, int);
extern	void	closeall(void);
extern	Cell	*sub(Node **, int);
extern	Cell	*gsub(Node **, int);

extern	FILE	*popen(const char *, const char *);
extern	int	pclose(FILE *);

/*** ytab.h ***/
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    FIRSTTOKEN = 258,
    PROGRAM = 259,
    PASTAT = 260,
    PASTAT2 = 261,
    XBEGIN = 262,
    XEND = 263,
    NL = 264,
    ARRAY = 265,
    MATCH = 266,
    NOTMATCH = 267,
    MATCHOP = 268,
    FINAL = 269,
    DOT = 270,
    ALL = 271,
    CCL = 272,
    NCCL = 273,
    CHAR = 274,
    OR = 275,
    STAR = 276,
    QUEST = 277,
    PLUS = 278,
    EMPTYRE = 279,
    AND = 280,
    BOR = 281,
    APPEND = 282,
    EQ = 283,
    GE = 284,
    GT = 285,
    LE = 286,
    LT = 287,
    NE = 288,
    IN = 289,
    ARG = 290,
    BLTIN = 291,
    BREAK = 292,
    CLOSE = 293,
    CONTINUE = 294,
    DELETE = 295,
    DO = 296,
    EXIT = 297,
    FOR = 298,
    FUNC = 299,
    SUB = 300,
    GSUB = 301,
    IF = 302,
    INDEX = 303,
    LSUBSTR = 304,
    MATCHFCN = 305,
    NEXT = 306,
    NEXTFILE = 307,
    ADD = 308,
    MINUS = 309,
    MULT = 310,
    DIVIDE = 311,
    MOD = 312,
    ASSIGN = 313,
    ASGNOP = 314,
    ADDEQ = 315,
    SUBEQ = 316,
    MULTEQ = 317,
    DIVEQ = 318,
    MODEQ = 319,
    POWEQ = 320,
    PRINT = 321,
    PRINTF = 322,
    SPRINTF = 323,
    ELSE = 324,
    INTEST = 325,
    CONDEXPR = 326,
    POSTINCR = 327,
    PREINCR = 328,
    POSTDECR = 329,
    PREDECR = 330,
    VAR = 331,
    IVAR = 332,
    VARNF = 333,
    CALL = 334,
    NUMBER = 335,
    STRING = 336,
    REGEXPR = 337,
    GETLINE = 338,
    RETURN = 339,
    SPLIT = 340,
    SUBSTR = 341,
    WHILE = 342,
    CAT = 343,
    NOT = 344,
    UMINUS = 345,
    POWER = 346,
    DECR = 347,
    INCR = 348,
    INDIRECT = 349,
    LASTTOKEN = 350
  };
#endif
/* Tokens.  */
#define FIRSTTOKEN 258
#define PROGRAM 259
#define PASTAT 260
#define PASTAT2 261
#define XBEGIN 262
#define XEND 263
#define NL 264
#define ARRAY 265
#define MATCH 266
#define NOTMATCH 267
#define MATCHOP 268
#define FINAL 269
#define DOT 270
#define ALL 271
#define CCL 272
#define NCCL 273
#define CHAR 274
#define OR 275
#define STAR 276
#define QUEST 277
#define PLUS 278
#define EMPTYRE 279
#define AND 280
#define BOR 281
#define APPEND 282
#define EQ 283
#define GE 284
#define GT 285
#define LE 286
#define LT 287
#define NE 288
#define IN 289
#define ARG 290
#define BLTIN 291
#define BREAK 292
#define CLOSE 293
#define CONTINUE 294
#define DELETE 295
#define DO 296
#define EXIT 297
#define FOR 298
#define FUNC 299
#define SUB 300
#define GSUB 301
#define IF 302
#define INDEX 303
#define LSUBSTR 304
#define MATCHFCN 305
#define NEXT 306
#define NEXTFILE 307
#define ADD 308
#define MINUS 309
#define MULT 310
#define DIVIDE 311
#define MOD 312
#define ASSIGN 313
#define ASGNOP 314
#define ADDEQ 315
#define SUBEQ 316
#define MULTEQ 317
#define DIVEQ 318
#define MODEQ 319
#define POWEQ 320
#define PRINT 321
#define PRINTF 322
#define SPRINTF 323
#define ELSE 324
#define INTEST 325
#define CONDEXPR 326
#define POSTINCR 327
#define PREINCR 328
#define POSTDECR 329
#define PREDECR 330
#define VAR 331
#define IVAR 332
#define VARNF 333
#define CALL 334
#define NUMBER 335
#define STRING 336
#define REGEXPR 337
#define GETLINE 338
#define RETURN 339
#define SPLIT 340
#define SUBSTR 341
#define WHILE 342
#define CAT 343
#define NOT 344
#define UMINUS 345
#define POWER 346
#define DECR 347
#define INCR 348
#define INDIRECT 349
#define LASTTOKEN 350

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{

	Node	*p;
	Cell	*cp;
	int	i;
	char	*s;

};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

/*** ytab.c ***/

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */

void checkdup(Node *list, Cell *item);
int yywrap(void) { return(1); }

Node	*beginloc = 0;
Node	*endloc = 0;
int	infunc	= 0;	/* = 1 if in arglist or body of func */
int	inloop	= 0;	/* = 1 if in while, for, do */
char	*curfname = 0;	/* current function name */
Node	*arglist = 0;	/* list of args for current function */


# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    FIRSTTOKEN = 258,
    PROGRAM = 259,
    PASTAT = 260,
    PASTAT2 = 261,
    XBEGIN = 262,
    XEND = 263,
    NL = 264,
    ARRAY = 265,
    MATCH = 266,
    NOTMATCH = 267,
    MATCHOP = 268,
    FINAL = 269,
    DOT = 270,
    ALL = 271,
    CCL = 272,
    NCCL = 273,
    CHAR = 274,
    OR = 275,
    STAR = 276,
    QUEST = 277,
    PLUS = 278,
    EMPTYRE = 279,
    AND = 280,
    BOR = 281,
    APPEND = 282,
    EQ = 283,
    GE = 284,
    GT = 285,
    LE = 286,
    LT = 287,
    NE = 288,
    IN = 289,
    ARG = 290,
    BLTIN = 291,
    BREAK = 292,
    CLOSE = 293,
    CONTINUE = 294,
    DELETE = 295,
    DO = 296,
    EXIT = 297,
    FOR = 298,
    FUNC = 299,
    SUB = 300,
    GSUB = 301,
    IF = 302,
    INDEX = 303,
    LSUBSTR = 304,
    MATCHFCN = 305,
    NEXT = 306,
    NEXTFILE = 307,
    ADD = 308,
    MINUS = 309,
    MULT = 310,
    DIVIDE = 311,
    MOD = 312,
    ASSIGN = 313,
    ASGNOP = 314,
    ADDEQ = 315,
    SUBEQ = 316,
    MULTEQ = 317,
    DIVEQ = 318,
    MODEQ = 319,
    POWEQ = 320,
    PRINT = 321,
    PRINTF = 322,
    SPRINTF = 323,
    ELSE = 324,
    INTEST = 325,
    CONDEXPR = 326,
    POSTINCR = 327,
    PREINCR = 328,
    POSTDECR = 329,
    PREDECR = 330,
    VAR = 331,
    IVAR = 332,
    VARNF = 333,
    CALL = 334,
    NUMBER = 335,
    STRING = 336,
    REGEXPR = 337,
    GETLINE = 338,
    RETURN = 339,
    SPLIT = 340,
    SUBSTR = 341,
    WHILE = 342,
    CAT = 343,
    NOT = 344,
    UMINUS = 345,
    POWER = 346,
    DECR = 347,
    INCR = 348,
    INDIRECT = 349,
    LASTTOKEN = 350
  };
#endif
/* Tokens.  */
#define FIRSTTOKEN 258
#define PROGRAM 259
#define PASTAT 260
#define PASTAT2 261
#define XBEGIN 262
#define XEND 263
#define NL 264
#define ARRAY 265
#define MATCH 266
#define NOTMATCH 267
#define MATCHOP 268
#define FINAL 269
#define DOT 270
#define ALL 271
#define CCL 272
#define NCCL 273
#define CHAR 274
#define OR 275
#define STAR 276
#define QUEST 277
#define PLUS 278
#define EMPTYRE 279
#define AND 280
#define BOR 281
#define APPEND 282
#define EQ 283
#define GE 284
#define GT 285
#define LE 286
#define LT 287
#define NE 288
#define IN 289
#define ARG 290
#define BLTIN 291
#define BREAK 292
#define CLOSE 293
#define CONTINUE 294
#define DELETE 295
#define DO 296
#define EXIT 297
#define FOR 298
#define FUNC 299
#define SUB 300
#define GSUB 301
#define IF 302
#define INDEX 303
#define LSUBSTR 304
#define MATCHFCN 305
#define NEXT 306
#define NEXTFILE 307
#define ADD 308
#define MINUS 309
#define MULT 310
#define DIVIDE 311
#define MOD 312
#define ASSIGN 313
#define ASGNOP 314
#define ADDEQ 315
#define SUBEQ 316
#define MULTEQ 317
#define DIVEQ 318
#define MODEQ 319
#define POWEQ 320
#define PRINT 321
#define PRINTF 322
#define SPRINTF 323
#define ELSE 324
#define INTEST 325
#define CONDEXPR 326
#define POSTINCR 327
#define PREINCR 328
#define POSTDECR 329
#define PREDECR 330
#define VAR 331
#define IVAR 332
#define VARNF 333
#define CALL 334
#define NUMBER 335
#define STRING 336
#define REGEXPR 337
#define GETLINE 338
#define RETURN 339
#define SPLIT 340
#define SUBSTR 341
#define WHILE 342
#define CAT 343
#define NOT 344
#define UMINUS 345
#define POWER 346
#define DECR 347
#define INCR 348
#define INDIRECT 349
#define LASTTOKEN 350

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{

	Node	*p;
	Cell	*cp;
	int	i;
	char	*s;

};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */


#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4659

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  112
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  185
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  368

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   350

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   104,     2,     2,
      12,    16,   103,   101,    10,   102,     2,    15,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    94,    14,
       2,     2,     2,    93,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    18,     2,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    11,    13,    17,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    95,    96,
      97,    98,    99,   100,   105,   106,   107,   108,   109,   110,
     111
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    98,    98,   100,   104,   104,   108,   108,   112,   112,
     116,   116,   120,   120,   124,   124,   126,   126,   128,   128,
     133,   134,   138,   142,   142,   146,   146,   150,   151,   155,
     156,   161,   162,   166,   167,   171,   175,   176,   177,   178,
     179,   180,   182,   184,   184,   189,   190,   194,   195,   199,
     200,   202,   204,   206,   207,   212,   213,   214,   215,   216,
     220,   221,   223,   225,   227,   228,   229,   230,   231,   232,
     233,   234,   239,   240,   241,   244,   247,   248,   249,   253,
     254,   258,   259,   263,   264,   265,   269,   269,   273,   273,
     273,   273,   277,   277,   281,   283,   287,   287,   291,   291,
     295,   298,   301,   304,   305,   306,   307,   308,   312,   313,
     317,   319,   321,   321,   321,   323,   324,   325,   326,   327,
     328,   329,   332,   335,   336,   337,   338,   338,   339,   343,
     344,   348,   348,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,   376,   378,   381,
     382,   384,   389,   390,   392,   394,   396,   397,   398,   400,
     405,   407,   412,   414,   416,   420,   421,   422,   423,   427,
     428,   429,   435,   436,   437,   442
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FIRSTTOKEN", "PROGRAM", "PASTAT",
  "PASTAT2", "XBEGIN", "XEND", "NL", "','", "'{'", "'('", "'|'", "';'",
  "'/'", "')'", "'}'", "'['", "']'", "ARRAY", "MATCH", "NOTMATCH",
  "MATCHOP", "FINAL", "DOT", "ALL", "CCL", "NCCL", "CHAR", "OR", "STAR",
  "QUEST", "PLUS", "EMPTYRE", "AND", "BOR", "APPEND", "EQ", "GE", "GT",
  "LE", "LT", "NE", "IN", "ARG", "BLTIN", "BREAK", "CLOSE", "CONTINUE",
  "DELETE", "DO", "EXIT", "FOR", "FUNC", "SUB", "GSUB", "IF", "INDEX",
  "LSUBSTR", "MATCHFCN", "NEXT", "NEXTFILE", "ADD", "MINUS", "MULT",
  "DIVIDE", "MOD", "ASSIGN", "ASGNOP", "ADDEQ", "SUBEQ", "MULTEQ", "DIVEQ",
  "MODEQ", "POWEQ", "PRINT", "PRINTF", "SPRINTF", "ELSE", "INTEST",
  "CONDEXPR", "POSTINCR", "PREINCR", "POSTDECR", "PREDECR", "VAR", "IVAR",
  "VARNF", "CALL", "NUMBER", "STRING", "REGEXPR", "'?'", "':'", "GETLINE",
  "RETURN", "SPLIT", "SUBSTR", "WHILE", "CAT", "'+'", "'-'", "'*'", "'%'",
  "NOT", "UMINUS", "POWER", "DECR", "INCR", "INDIRECT", "LASTTOKEN",
  "$accept", "program", "and", "bor", "comma", "do", "else", "for", "$@1",
  "$@2", "$@3", "funcname", "if", "lbrace", "nl", "opt_nl", "opt_pst",
  "opt_simple_stmt", "pas", "pa_pat", "pa_stat", "$@4", "pa_stats",
  "patlist", "ppattern", "pattern", "plist", "pplist", "prarg", "print",
  "pst", "rbrace", "re", "reg_expr", "$@5", "rparen", "simple_stmt", "st",
  "stmt", "$@6", "$@7", "$@8", "stmtlist", "subop", "term", "var",
  "varlist", "varname", "while", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      44,   123,    40,   124,    59,    47,    41,   125,    91,    93,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,    63,    58,   338,   339,   340,   341,   342,
     343,    43,    45,    42,    37,   344,   345,   346,   347,   348,
     349,   350
};
# endif

#define YYPACT_NINF -308

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-308)))

#define YYTABLE_NINF -32

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-32)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     681,  -308,  -308,  -308,    77,  1585,  -308,    86,  -308,    50,
      50,  -308,  4223,  -308,  -308,    66,  4549,   -56,  -308,  -308,
      76,    90,    92,  -308,  -308,  -308,    93,  -308,  -308,   212,
     104,   113,  4549,  4549,  4281,   248,   248,  4549,   812,    34,
    -308,   132,  3509,  -308,  -308,   121,   -11,   -44,   117,  -308,
    -308,   812,   812,  2194,    38,    57,  4038,  4223,  4549,   -11,
      44,  -308,  -308,   130,  4223,  4223,  4223,  4096,  4549,   115,
    4223,  4223,    51,    51,  -308,    51,  -308,  -308,  -308,  -308,
    -308,   151,   141,   141,   -31,  -308,  1745,   149,   153,   141,
     141,  -308,  -308,  1745,   160,   154,  -308,  1426,   812,  3509,
    4339,   141,  -308,   880,  -308,   151,   812,  1585,    82,  4223,
    -308,  -308,  4223,  4223,  4223,  4223,  4223,  4223,   -31,  4223,
    1803,  1861,   -11,  4223,  4397,  4549,  4549,  4549,  4549,  4549,
    4223,  -308,  -308,  4223,   948,  1016,  -308,  -308,  1919,   140,
    1919,   171,  -308,    65,  3509,  2682,   102,  2591,  2591,    69,
    -308,   107,   -11,  4549,  2591,  2591,  -308,   182,  -308,   151,
     182,  -308,  -308,   176,  1687,  -308,  1493,  4223,  -308,  -308,
    1687,  -308,  4223,  -308,  1426,   133,  1084,  4223,  3911,   201,
      10,  -308,   -11,    -2,  -308,  -308,  -308,  1426,  4223,  1152,
    -308,   248,  3760,  -308,  3760,  3760,  3760,  3760,  3760,  3760,
    -308,  2773,  -308,  3680,  -308,  3600,  2591,   201,  4549,    51,
      -6,    -6,    51,    51,    51,  3509,    21,  -308,  -308,  -308,
    3509,   -31,  3509,  -308,  -308,  1919,  -308,   108,  1919,  1919,
    -308,  -308,   -11,     1,  1919,  -308,  -308,  4223,  -308,   200,
    -308,    18,  2864,  -308,  2864,  -308,  -308,  1222,  -308,   208,
     110,  4455,   -31,  4455,  1977,  2035,   -11,  2093,  4549,  4549,
    4549,  4455,  -308,    50,  -308,  -308,  4223,  1919,  1919,   -11,
    -308,  -308,  3509,  -308,    13,   209,  2955,   204,  3046,   211,
     111,  2295,    24,   142,   -31,   209,   209,   125,  -308,  -308,
    -308,   189,  4223,  4524,  -308,  -308,  3831,  4165,  3980,  3911,
     -11,   -11,   -11,  3911,   812,  3509,  2396,  2497,  -308,  -308,
      50,  -308,  -308,  -308,  -308,  -308,  1919,  -308,  1919,  -308,
     151,  4223,   218,   224,   -31,   120,  4455,  1290,  -308,   187,
    -308,   187,   812,  3137,   232,  3228,  1560,  3327,   209,  4223,
    -308,   189,  3911,  -308,   233,   234,  1358,  -308,  -308,  -308,
     218,   151,  1426,  3418,  -308,  -308,  -308,   209,  1560,  -308,
     141,  1426,   218,  -308,  -308,   209,  1426,  -308
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     3,    88,    89,     0,    33,     2,    30,     1,     0,
       0,    23,     0,    96,   183,   145,     0,     0,   131,   132,
       0,     0,     0,   182,   177,   184,     0,   162,   167,   156,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
      45,    29,    35,    77,    94,     0,    78,   174,   175,    90,
      91,     0,     0,     0,     0,     0,     0,     0,     0,   148,
     174,    20,    21,     0,     0,     0,     0,     0,     0,   155,
       0,     0,   141,   140,    95,   142,   149,   150,   178,   107,
      24,    27,     0,     0,     0,    10,     0,     0,     0,     0,
       0,    86,    87,     0,     0,   112,   117,     0,     0,   106,
      83,     0,   129,     0,   126,    27,     0,    34,     0,     0,
       4,     6,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    76,     0,     0,     0,     0,     0,     0,     0,
       0,   151,   152,     0,     0,     0,     8,   159,     0,     0,
       0,     0,   143,     0,    47,     0,   179,     0,     0,     0,
     146,     0,   154,     0,     0,     0,    25,    28,   128,    27,
     108,   110,   111,   105,     0,   116,     0,     0,   121,   122,
       0,   124,     0,    11,     0,   119,     0,     0,    81,    84,
     103,    58,    59,   174,   125,    40,   130,     0,     0,     0,
      46,    75,    71,    70,    64,    65,    66,    67,    68,    69,
      72,     0,     5,    63,     7,    62,     0,    94,     0,   137,
     134,   135,   136,   138,   139,    60,     0,    41,    42,     9,
      79,     0,    80,    97,   144,     0,   180,     0,     0,     0,
     166,   147,   153,     0,     0,    26,   109,     0,   115,     0,
      32,   175,     0,   123,     0,   113,    12,     0,    92,   120,
       0,     0,     0,     0,     0,     0,    57,     0,     0,     0,
       0,     0,   127,    38,    37,    74,     0,     0,     0,   133,
     176,    73,    48,    98,     0,    43,     0,    94,     0,    94,
       0,     0,     0,    27,     0,    22,   185,     0,    13,   118,
      93,    85,     0,    54,    53,    55,     0,    52,    51,    82,
     100,   101,   102,    49,     0,    61,     0,     0,   181,    99,
       0,   157,   158,   161,   160,   165,     0,   173,     0,   104,
      27,     0,     0,     0,     0,     0,     0,     0,   169,     0,
     168,     0,     0,     0,    94,     0,     0,     0,    18,     0,
      56,     0,    50,    39,     0,     0,     0,   163,   164,   172,
       0,    27,     0,     0,   171,   170,    44,    16,     0,    19,
       0,     0,     0,   114,    17,    14,     0,    15
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -308,  -308,  -129,   -96,    61,  -308,  -308,  -308,  -308,  -308,
    -308,  -308,  -308,    -4,   -73,   -90,   210,  -307,  -308,    64,
     148,  -308,  -308,   -39,  -181,   527,  -170,  -308,  -308,  -308,
    -308,  -308,   -32,   -85,  -308,  -203,  -163,   -30,   310,  -308,
    -308,  -308,   -40,  -308,   270,   -16,  -308,    87,  -308
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,   120,   121,   225,    95,   247,    96,   366,   361,
     352,    63,    97,    98,   160,   158,     5,   239,     6,    39,
      40,   310,    41,   143,   178,    99,    54,   179,   180,   100,
       7,   249,    43,    44,    55,   275,   101,   161,   102,   174,
     287,   187,   103,    45,    46,    47,   227,    48,   104
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      60,    38,    74,   240,   124,    51,    52,   250,   157,   124,
     219,   134,   135,    69,    14,   188,    60,    60,    60,    76,
      77,    60,   219,   258,   193,   130,    60,   149,   151,   350,
      61,   136,   157,    62,   136,   106,   133,    60,   207,   285,
     270,   286,    60,   319,   105,    11,    14,   259,   136,   254,
     260,   362,    60,   162,   139,    23,   165,    25,   176,   168,
     169,    11,   284,   171,   131,   132,   189,   261,   181,   236,
     293,   184,   296,   297,   298,   136,   299,     8,    56,   136,
     303,   224,   255,    60,   183,   230,   157,    23,    64,    25,
     125,   126,   127,   128,   216,    49,   129,   127,   128,   308,
      50,   129,    65,    38,    66,    67,   131,   132,    60,    60,
      60,    60,    60,    60,   138,   140,    70,   136,   136,   338,
     136,   136,   325,   231,   273,    71,   291,   315,    60,    60,
     136,    60,    60,   123,   238,   133,   341,    60,    60,    60,
     243,     2,   146,   277,   279,   342,     3,   357,    60,   141,
     156,   156,   131,   132,    60,   159,   320,   153,   129,   365,
     156,   166,    60,   173,   254,   167,   294,   254,   254,   254,
     254,   163,   172,   240,   254,   265,    60,   191,    60,    60,
      60,    60,    60,    60,   221,    60,   223,    60,   226,    60,
      60,   235,    60,   321,   237,   240,   219,   255,   282,    60,
     255,   255,   255,   255,    60,   200,    60,   255,   228,   229,
     157,   136,   246,   254,   283,   233,   234,   290,   309,   181,
     312,   181,   181,   181,   323,   181,    60,   314,    60,   181,
     336,   334,    14,   324,   273,   183,   339,   183,   183,   183,
     257,   183,    60,    60,    60,   183,   255,   157,   348,   354,
     355,   107,   263,   241,    68,   190,    60,    14,     0,   304,
      60,   358,    60,     0,   327,    60,     0,   267,   268,     0,
       0,     0,     0,    23,    24,    25,     0,    60,   157,     0,
      60,    60,    60,    60,     0,     0,    59,    60,   274,    60,
      60,    60,   346,    14,   181,     0,     0,    37,    23,    24,
      25,     0,    72,    73,    75,     0,   332,    78,   271,     0,
     183,   140,   122,   344,     0,   345,     0,    60,     0,    60,
     280,    60,    37,   122,     0,     0,    60,     0,    75,     0,
     363,     0,     0,     0,    23,    24,    25,    60,   152,   295,
       0,   316,   318,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,     0,
       0,     0,     0,     0,     0,     0,     0,   329,   331,   122,
     182,   322,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   140,     0,     0,     0,
       0,     0,     0,     0,   209,   210,   211,   212,   213,   214,
       0,     0,     0,     0,     0,     0,     0,   175,     0,     0,
       0,   340,     0,   186,   122,   122,     0,   122,   122,     0,
       0,     0,     0,   232,   122,   122,     0,     0,     0,     0,
       0,     0,     0,     0,   122,     0,     0,     0,     0,     0,
     122,     0,     0,     0,   186,   186,     0,     0,   256,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   122,     0,   122,   122,   122,   122,   122,   122,
       0,   122,     0,   122,     0,   122,   122,     0,   269,     0,
       0,     0,     0,     0,   245,   122,   186,     0,     0,     0,
     122,     0,   122,     0,     0,     0,     0,   262,     0,   186,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   122,     0,   122,     0,     0,     0,     0,     0,
       0,   182,     0,   182,   182,   182,     0,   182,   300,   301,
     302,   182,    42,     0,     0,     0,     0,     0,     0,    53,
       0,     0,   122,     0,     0,     0,   122,     0,   122,     0,
       0,   122,     0,     0,     0,     0,     0,   289,     0,     0,
       0,     0,     0,   256,     0,     0,   256,   256,   256,   256,
       0,     0,     0,   256,     0,   122,   122,   122,     0,     0,
       0,     0,     0,   144,   145,     0,     0,     0,     0,     0,
       0,   147,   148,   144,   144,     0,   182,   154,   155,     0,
       0,     0,     0,   122,     0,   122,     0,   122,     0,     0,
       0,     0,   256,   164,     0,     0,     0,     0,     0,     0,
     170,     0,     0,   122,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    42,     0,   192,   186,     0,   194,
     195,   196,   197,   198,   199,     0,   201,   203,   205,     0,
     206,     0,     0,     0,     0,     0,   186,   215,     0,     0,
     144,     0,   359,     0,     0,   220,     0,   222,     0,     0,
       0,   364,     0,     0,     0,     0,   367,     0,     0,     0,
       0,   -29,     1,     0,     0,     0,     0,     0,   -29,   -29,
       2,     0,   -29,   -29,   242,     3,   -29,     0,     0,   244,
       0,     0,     0,     0,    53,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   -29,   -29,     0,   -29,
       0,     0,     0,     0,     0,   -29,   -29,   -29,     0,   -29,
       0,   -29,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   272,     0,     0,   276,   278,     0,     0,   -29,
       0,   281,     0,     0,   144,     0,     0,   -29,   -29,   -29,
     -29,   -29,   -29,     0,     0,     0,   -29,     0,   -29,   -29,
       0,     0,   -29,   -29,     0,     0,   -29,     0,     0,   -29,
     -29,   -29,     0,   305,   306,   307,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    79,     0,     0,     0,     0,     0,    53,
       0,    80,     0,    11,    12,     0,    81,    13,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   333,     0,   335,     0,     0,   337,     0,
       0,     0,     0,     0,     0,     0,     0,    14,    15,    82,
      16,    83,    84,    85,    86,    87,   353,    18,    19,    88,
      20,     0,    21,    89,    90,     0,     0,     0,     0,     0,
       0,    79,     0,     0,     0,     0,     0,     0,    91,    92,
      22,    11,    12,     0,    81,    13,     0,   185,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,    93,    30,
      31,    94,     0,    32,    33,     0,     0,    34,     0,     0,
      35,    36,    37,     0,     0,    14,    15,    82,    16,    83,
      84,    85,    86,    87,     0,    18,    19,    88,    20,     0,
      21,    89,    90,     0,     0,     0,     0,     0,     0,    79,
       0,     0,     0,     0,     0,     0,    91,    92,    22,    11,
      12,     0,    81,    13,     0,   217,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,    93,    30,    31,    94,
       0,    32,    33,     0,     0,    34,     0,     0,    35,    36,
      37,     0,     0,    14,    15,    82,    16,    83,    84,    85,
      86,    87,     0,    18,    19,    88,    20,     0,    21,    89,
      90,     0,     0,     0,     0,     0,     0,    79,     0,     0,
       0,     0,     0,     0,    91,    92,    22,    11,    12,     0,
      81,    13,     0,   218,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,    93,    30,    31,    94,     0,    32,
      33,     0,     0,    34,     0,     0,    35,    36,    37,     0,
       0,    14,    15,    82,    16,    83,    84,    85,    86,    87,
       0,    18,    19,    88,    20,     0,    21,    89,    90,     0,
       0,     0,     0,     0,     0,    79,     0,     0,     0,     0,
       0,     0,    91,    92,    22,    11,    12,     0,    81,    13,
       0,   248,    23,    24,    25,    26,    27,    28,     0,     0,
       0,    29,    93,    30,    31,    94,     0,    32,    33,     0,
       0,    34,     0,     0,    35,    36,    37,     0,     0,    14,
      15,    82,    16,    83,    84,    85,    86,    87,     0,    18,
      19,    88,    20,     0,    21,    89,    90,     0,     0,     0,
       0,     0,     0,    79,     0,     0,     0,     0,     0,     0,
      91,    92,    22,    11,    12,     0,    81,    13,     0,   264,
      23,    24,    25,    26,    27,    28,     0,     0,     0,    29,
      93,    30,    31,    94,     0,    32,    33,     0,     0,    34,
       0,     0,    35,    36,    37,     0,     0,    14,    15,    82,
      16,    83,    84,    85,    86,    87,     0,    18,    19,    88,
      20,     0,    21,    89,    90,     0,     0,     0,     0,     0,
       0,     0,     0,    79,     0,     0,     0,     0,    91,    92,
      22,   288,     0,    11,    12,     0,    81,    13,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,    93,    30,
      31,    94,     0,    32,    33,     0,     0,    34,     0,     0,
      35,    36,    37,     0,     0,     0,     0,    14,    15,    82,
      16,    83,    84,    85,    86,    87,     0,    18,    19,    88,
      20,     0,    21,    89,    90,     0,     0,     0,     0,     0,
       0,    79,     0,     0,     0,     0,     0,     0,    91,    92,
      22,    11,    12,     0,    81,    13,     0,   343,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,    93,    30,
      31,    94,     0,    32,    33,     0,     0,    34,     0,     0,
      35,    36,    37,     0,     0,    14,    15,    82,    16,    83,
      84,    85,    86,    87,     0,    18,    19,    88,    20,     0,
      21,    89,    90,     0,     0,     0,     0,     0,     0,    79,
       0,     0,     0,     0,     0,     0,    91,    92,    22,    11,
      12,     0,    81,    13,     0,   356,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,    93,    30,    31,    94,
       0,    32,    33,     0,     0,    34,     0,     0,    35,    36,
      37,     0,     0,    14,    15,    82,    16,    83,    84,    85,
      86,    87,     0,    18,    19,    88,    20,     0,    21,    89,
      90,     0,     0,     0,     0,     0,     0,    79,     0,     0,
       0,     0,     0,     0,    91,    92,    22,    11,    12,     0,
      81,    13,     0,     0,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,    93,    30,    31,    94,     0,    32,
      33,     0,     0,    34,     0,     0,    35,    36,    37,     0,
       0,    14,    15,    82,    16,    83,    84,    85,    86,    87,
       0,    18,    19,    88,    20,     0,    21,    89,    90,     0,
       0,     0,     0,     0,    79,     0,     0,     0,     0,     0,
       0,     0,    91,    92,    22,    12,     0,   -31,    13,     0,
       0,     0,    23,    24,    25,    26,    27,    28,     0,     0,
       0,    29,    93,    30,    31,    94,     0,    32,    33,     0,
       0,    34,     0,     0,    35,    36,    37,     0,    14,    15,
       0,    16,     0,    84,     0,     0,     0,     0,    18,    19,
       0,    20,     0,    21,     0,     0,     0,     0,     0,     0,
       0,    79,     0,     0,     0,     0,     0,     0,     0,    91,
      92,    22,    12,     0,     0,    13,   -31,     0,     0,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     9,    10,    32,    33,    11,    12,    34,     0,
      13,    35,    36,    37,     0,    14,    15,     0,    16,     0,
      84,     0,     0,     0,     0,    18,    19,     0,    20,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,    15,     0,    16,     0,     0,    91,    92,    22,    17,
      18,    19,     0,    20,     0,    21,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,     0,    30,    31,     0,
       0,    32,    33,    22,     0,    34,     0,     0,    35,    36,
      37,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,     0,     0,
      34,     0,     0,    35,    36,    37,   156,     0,     0,    57,
     108,   159,     0,     0,     0,     0,     0,     0,     0,     0,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   110,   111,     0,   112,   113,   114,   115,   116,
     117,   118,    14,    15,     0,    16,     0,     0,     0,     0,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,   156,     0,     0,    12,     0,   159,
      13,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
     119,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      14,    15,    58,    16,     0,    35,    36,    37,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,   202,     0,     0,    12,     0,     0,    13,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    14,    15,
      34,    16,     0,    35,    36,    37,     0,     0,    18,    19,
       0,    20,     0,    21,     0,     0,     0,     0,     0,     0,
     204,     0,     0,    12,     0,     0,    13,     0,     0,     0,
       0,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,     0,    32,    33,    14,    15,    34,    16,
       0,    35,    36,    37,     0,     0,    18,    19,     0,    20,
       0,    21,     0,     0,     0,     0,     0,     0,   219,     0,
       0,    12,     0,     0,    13,     0,     0,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,     0,    30,    31,
       0,     0,    32,    33,    14,    15,    34,    16,     0,    35,
      36,    37,     0,     0,    18,    19,     0,    20,     0,    21,
       0,     0,     0,     0,     0,     0,   202,     0,     0,   292,
       0,     0,    13,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,     0,     0,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,     0,    30,    31,     0,     0,
      32,    33,    14,    15,    34,    16,     0,    35,    36,    37,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,   204,     0,     0,   292,     0,     0,
      13,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
       0,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      14,    15,    34,    16,     0,    35,    36,    37,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,   219,     0,     0,   292,     0,     0,    13,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    14,    15,
      34,    16,     0,    35,    36,    37,     0,     0,    18,    19,
       0,    20,     0,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,     0,    32,    33,     0,     0,    34,     0,
       0,    35,    36,    37,   136,     0,    57,   108,     0,     0,
     137,     0,     0,     0,     0,     0,     0,   109,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   110,
     111,     0,   112,   113,   114,   115,   116,   117,   118,    14,
      15,     0,    16,     0,     0,     0,     0,     0,     0,    18,
      19,     0,    20,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    22,     0,     0,     0,     0,     0,     0,     0,
      23,    24,    25,    26,    27,    28,     0,   119,     0,    29,
       0,    30,    31,     0,     0,    32,    33,     0,     0,    58,
       0,     0,    35,    36,    37,   136,     0,    57,   108,     0,
       0,   317,     0,     0,     0,     0,     0,     0,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     110,   111,     0,   112,   113,   114,   115,   116,   117,   118,
      14,    15,     0,    16,     0,     0,     0,     0,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,     0,   119,     0,
      29,     0,    30,    31,     0,     0,    32,    33,     0,     0,
      58,     0,     0,    35,    36,    37,   136,     0,    57,   108,
       0,     0,   328,     0,     0,     0,     0,     0,     0,   109,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   110,   111,     0,   112,   113,   114,   115,   116,   117,
     118,    14,    15,     0,    16,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,     0,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
       0,     0,    23,    24,    25,    26,    27,    28,     0,   119,
       0,    29,     0,    30,    31,     0,     0,    32,    33,     0,
       0,    58,     0,     0,    35,    36,    37,   136,     0,    57,
     108,     0,     0,   330,     0,     0,     0,     0,     0,     0,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   110,   111,     0,   112,   113,   114,   115,   116,
     117,   118,    14,    15,     0,    16,     0,     0,     0,     0,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
     119,     0,    29,     0,    30,    31,     0,     0,    32,    33,
       0,   136,    58,    57,   108,    35,    36,    37,     0,     0,
       0,     0,     0,     0,   109,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   110,   111,     0,   112,
     113,   114,   115,   116,   117,   118,    14,    15,     0,    16,
       0,     0,     0,     0,     0,     0,    18,    19,     0,    20,
       0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,     0,   119,     0,    29,     0,    30,    31,
       0,     0,    32,    33,    57,   108,    58,     0,   137,    35,
      36,    37,     0,     0,     0,   109,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   110,   111,     0,
     112,   113,   114,   115,   116,   117,   118,    14,    15,     0,
      16,     0,     0,     0,     0,     0,     0,    18,    19,     0,
      20,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      22,     0,     0,     0,     0,     0,     0,     0,    23,    24,
      25,    26,    27,    28,     0,   119,     0,    29,     0,    30,
      31,     0,     0,    32,    33,    57,   108,    58,     0,     0,
      35,    36,    37,     0,     0,     0,   109,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   110,   111,
       0,   112,   113,   114,   115,   116,   117,   118,    14,    15,
       0,    16,     0,     0,     0,     0,     0,     0,    18,    19,
       0,    20,     0,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24,    25,    26,    27,    28,     0,   119,   266,    29,     0,
      30,    31,     0,     0,    32,    33,    57,   108,    58,     0,
     273,    35,    36,    37,     0,     0,     0,   109,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   110,
     111,     0,   112,   113,   114,   115,   116,   117,   118,    14,
      15,     0,    16,     0,     0,     0,     0,     0,     0,    18,
      19,     0,    20,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    22,     0,     0,     0,     0,     0,     0,     0,
      23,    24,    25,    26,    27,    28,     0,   119,     0,    29,
       0,    30,    31,     0,     0,    32,    33,    57,   108,    58,
       0,   311,    35,    36,    37,     0,     0,     0,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     110,   111,     0,   112,   113,   114,   115,   116,   117,   118,
      14,    15,     0,    16,     0,     0,     0,     0,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,     0,   119,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    57,   108,
      58,     0,   313,    35,    36,    37,     0,     0,     0,   109,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   110,   111,     0,   112,   113,   114,   115,   116,   117,
     118,    14,    15,     0,    16,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,     0,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
       0,     0,    23,    24,    25,    26,    27,    28,     0,   119,
       0,    29,     0,    30,    31,     0,     0,    32,    33,    57,
     108,    58,     0,   347,    35,    36,    37,     0,     0,     0,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   110,   111,     0,   112,   113,   114,   115,   116,
     117,   118,    14,    15,     0,    16,     0,     0,     0,     0,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
     119,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      57,   108,    58,     0,   349,    35,    36,    37,     0,     0,
       0,   109,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   110,   111,     0,   112,   113,   114,   115,
     116,   117,   118,    14,    15,     0,    16,     0,     0,     0,
       0,     0,     0,    18,    19,     0,    20,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,    23,    24,    25,    26,    27,    28,
       0,   119,     0,    29,     0,    30,    31,     0,     0,    32,
      33,     0,     0,    58,     0,     0,    35,    36,    37,    57,
     108,   351,     0,     0,     0,     0,     0,     0,     0,     0,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   110,   111,     0,   112,   113,   114,   115,   116,
     117,   118,    14,    15,     0,    16,     0,     0,     0,     0,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
     119,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      57,   108,    58,     0,   360,    35,    36,    37,     0,     0,
       0,   109,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   110,   111,     0,   112,   113,   114,   115,
     116,   117,   118,    14,    15,     0,    16,     0,     0,     0,
       0,     0,     0,    18,    19,     0,    20,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,    23,    24,    25,    26,    27,    28,
       0,   119,     0,    29,     0,    30,    31,     0,     0,    32,
      33,    57,   108,    58,     0,     0,    35,    36,    37,     0,
       0,     0,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   110,   111,     0,   112,   113,   114,
     115,   116,   117,   118,    14,    15,     0,    16,     0,     0,
       0,     0,     0,     0,    18,    19,     0,    20,     0,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,     0,     0,    23,    24,    25,    26,    27,
      28,     0,   119,     0,    29,     0,    30,    31,     0,     0,
      32,    33,    57,   108,    58,     0,     0,    35,    36,    37,
       0,     0,     0,   109,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   110,     0,     0,   112,   113,
     114,   115,   116,   117,   118,    14,    15,     0,    16,     0,
       0,     0,     0,     0,     0,    18,    19,     0,    20,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    57,   108,     0,    29,     0,    30,    31,     0,
       0,    32,    33,   109,     0,    58,     0,     0,    35,    36,
      37,     0,     0,     0,     0,     0,     0,     0,   112,   113,
     114,   115,   116,   117,   118,    14,    15,     0,    16,     0,
       0,     0,     0,     0,     0,    18,    19,     0,    20,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,    57,   -32,     0,    29,     0,    30,    31,     0,
       0,    32,    33,   -32,     0,    58,     0,     0,    35,    36,
      37,     0,     0,     0,     0,     0,     0,     0,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,    14,    15,     0,    16,     0,
       0,     0,     0,     0,     0,    18,    19,     0,    20,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,    57,     0,     0,    23,    24,    25,    26,
      27,    28,     0,     0,   251,     0,     0,    30,    31,     0,
       0,    32,    33,     0,     0,    58,   110,   111,    35,    36,
      37,     0,     0,     0,     0,   252,    14,    15,     0,    16,
       0,     0,     0,     0,     0,     0,    18,    19,     0,    20,
       0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,    57,   253,   326,    29,     0,    30,    31,
       0,     0,    32,    33,   251,     0,    58,     0,     0,    35,
      36,    37,     0,     0,     0,     0,   110,   111,     0,     0,
       0,     0,     0,     0,     0,   252,    14,    15,     0,    16,
       0,     0,     0,     0,     0,     0,    18,    19,     0,    20,
       0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,    57,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,   251,   253,     0,    29,     0,    30,    31,
       0,     0,    32,    33,     0,   110,    58,     0,     0,    35,
      36,    37,     0,     0,   252,    14,    15,     0,    16,     0,
       0,     0,     0,     0,     0,    18,    19,     0,    20,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,     0,     0,    13,   142,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,     0,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,     0,    30,    31,     0,
       0,    32,    33,    14,    15,    58,    16,     0,    35,    36,
      37,     0,     0,    18,    19,     0,    20,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,    13,   150,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,     0,    30,    31,     0,     0,    32,
      33,    14,    15,    34,    16,     0,    35,    36,    37,     0,
       0,    18,    19,     0,    20,     0,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,    57,     0,     0,
       0,     0,    23,    24,    25,    26,    27,    28,   251,     0,
       0,    29,     0,    30,    31,     0,     0,    32,    33,     0,
       0,    34,     0,     0,    35,    36,    37,     0,     0,   252,
      14,    15,     0,    16,     0,     0,     0,     0,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,     0,     0,    13,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    14,    15,
      58,    16,     0,    35,    36,    37,     0,     0,    18,    19,
       0,    20,     0,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    57,     0,     0,    13,     0,     0,     0,
       0,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,     0,    32,    33,    14,    15,    34,    16,
       0,    35,    36,    37,     0,     0,    18,    19,     0,    20,
       0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,     0,     0,    13,     0,     0,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,     0,    30,    31,
       0,     0,    32,    33,    14,    15,    34,    16,     0,    35,
      36,    37,     0,     0,    18,    19,     0,    20,     0,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    57,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,     0,     0,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,     0,    30,    31,     0,     0,
      32,    33,    14,    15,    34,    16,     0,    35,    36,    37,
       0,     0,    18,    19,     0,    20,     0,    21,     0,     0,
       0,     0,     0,     0,     0,     0,   208,   292,     0,     0,
      13,     0,     0,     0,     0,    22,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,     0,
       0,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      14,    15,    58,    16,     0,    35,    36,    37,     0,     0,
      18,    19,     0,    20,     0,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,    57,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,   -32,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,     0,     0,
      34,    57,     0,    35,    36,    37,     0,     0,   -32,    14,
      15,     0,    16,     0,     0,     0,     0,     0,     0,    18,
      19,     0,    20,     0,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,    15,     0,    16,     0,     0,
       0,     0,    22,     0,    18,    19,     0,    20,     0,    21,
      23,    24,    25,    26,    27,    28,     0,     0,     0,     0,
       0,    30,    31,     0,     0,    32,    33,    22,     0,    58,
       0,     0,    35,    36,    37,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,     0,    30,    31,     0,     0,
      32,    33,     0,     0,    58,     0,     0,    35,    36,    37
};

static const yytype_int16 yycheck[] =
{
      16,     5,    34,   166,    15,     9,    10,   177,    81,    15,
       9,    51,    52,    29,    45,   105,    32,    33,    34,    35,
      36,    37,     9,    13,   109,    69,    42,    66,    67,   336,
      86,    10,   105,    89,    10,    39,    18,    53,   123,   242,
      19,   244,    58,    19,    10,    11,    45,    37,    10,   178,
      40,   358,    68,    83,    16,    86,    86,    88,    98,    89,
      90,    11,    44,    93,   108,   109,   106,    69,   100,   159,
     251,   101,   253,   254,   255,    10,   257,     0,    12,    10,
     261,    16,   178,    99,   100,    16,   159,    86,    12,    88,
     101,   102,   103,   104,   133,     9,   107,   103,   104,    86,
      14,   107,    12,   107,    12,    12,   108,   109,   124,   125,
     126,   127,   128,   129,    53,    54,    12,    10,    10,   322,
      10,    10,   292,    16,    16,    12,    16,    16,   144,   145,
      10,   147,   148,    12,   164,    18,    16,   153,   154,   155,
     170,     9,    12,   228,   229,   326,    14,   350,   164,    92,
       9,     9,   108,   109,   170,    14,    14,    42,   107,   362,
       9,    12,   178,     9,   293,    12,   251,   296,   297,   298,
     299,    84,    12,   336,   303,   191,   192,    95,   194,   195,
     196,   197,   198,   199,    44,   201,    15,   203,    86,   205,
     206,     9,   208,   283,    18,   358,     9,   293,   237,   215,
     296,   297,   298,   299,   220,   118,   222,   303,   147,   148,
     283,    10,    79,   342,    14,   154,   155,     9,     9,   251,
      16,   253,   254,   255,    99,   257,   242,    16,   244,   261,
     320,   316,    45,    44,    16,   251,    12,   253,   254,   255,
     179,   257,   258,   259,   260,   261,   342,   320,    16,    16,
      16,    41,   188,   166,    42,   107,   272,    45,    -1,   263,
     276,   351,   278,    -1,   304,   281,    -1,   206,   207,    -1,
      -1,    -1,    -1,    86,    87,    88,    -1,   293,   351,    -1,
     296,   297,   298,   299,    -1,    -1,    16,   303,   227,   305,
     306,   307,   332,    45,   326,    -1,    -1,   110,    86,    87,
      88,    -1,    32,    33,    34,    -1,   310,    37,   221,    -1,
     326,   250,    42,   329,    -1,   331,    -1,   333,    -1,   335,
     233,   337,   110,    53,    -1,    -1,   342,    -1,    58,    -1,
     360,    -1,    -1,    -1,    86,    87,    88,   353,    68,   252,
      -1,   280,   281,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   306,   307,    99,
     100,   284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   325,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,   324,    -1,   103,   144,   145,    -1,   147,   148,    -1,
      -1,    -1,    -1,   153,   154,   155,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   164,    -1,    -1,    -1,    -1,    -1,
     170,    -1,    -1,    -1,   134,   135,    -1,    -1,   178,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   192,    -1,   194,   195,   196,   197,   198,   199,
      -1,   201,    -1,   203,    -1,   205,   206,    -1,   208,    -1,
      -1,    -1,    -1,    -1,   174,   215,   176,    -1,    -1,    -1,
     220,    -1,   222,    -1,    -1,    -1,    -1,   187,    -1,   189,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   242,    -1,   244,    -1,    -1,    -1,    -1,    -1,
      -1,   251,    -1,   253,   254,   255,    -1,   257,   258,   259,
     260,   261,     5,    -1,    -1,    -1,    -1,    -1,    -1,    12,
      -1,    -1,   272,    -1,    -1,    -1,   276,    -1,   278,    -1,
      -1,   281,    -1,    -1,    -1,    -1,    -1,   247,    -1,    -1,
      -1,    -1,    -1,   293,    -1,    -1,   296,   297,   298,   299,
      -1,    -1,    -1,   303,    -1,   305,   306,   307,    -1,    -1,
      -1,    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    67,    -1,   326,    70,    71,    -1,
      -1,    -1,    -1,   333,    -1,   335,    -1,   337,    -1,    -1,
      -1,    -1,   342,    86,    -1,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,   353,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,    -1,   109,   327,    -1,   112,
     113,   114,   115,   116,   117,    -1,   119,   120,   121,    -1,
     123,    -1,    -1,    -1,    -1,    -1,   346,   130,    -1,    -1,
     133,    -1,   352,    -1,    -1,   138,    -1,   140,    -1,    -1,
      -1,   361,    -1,    -1,    -1,    -1,   366,    -1,    -1,    -1,
      -1,     0,     1,    -1,    -1,    -1,    -1,    -1,     7,     8,
       9,    -1,    11,    12,   167,    14,    15,    -1,    -1,   172,
      -1,    -1,    -1,    -1,   177,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    54,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   225,    -1,    -1,   228,   229,    -1,    -1,    78,
      -1,   234,    -1,    -1,   237,    -1,    -1,    86,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    -1,    -1,   105,    -1,    -1,   108,
     109,   110,    -1,   266,   267,   268,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,   292,
      -1,     9,    -1,    11,    12,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   316,    -1,   318,    -1,    -1,   321,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,   339,    55,    56,    57,
      58,    -1,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,
      78,    11,    12,    -1,    14,    15,    -1,    17,    86,    87,
      88,    89,    90,    91,    -1,    -1,    -1,    95,    96,    97,
      98,    99,    -1,   101,   102,    -1,    -1,   105,    -1,    -1,
     108,   109,   110,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    55,    56,    57,    58,    -1,
      60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    78,    11,
      12,    -1,    14,    15,    -1,    17,    86,    87,    88,    89,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,   101,   102,    -1,    -1,   105,    -1,    -1,   108,   109,
     110,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    55,    56,    57,    58,    -1,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    77,    78,    11,    12,    -1,
      14,    15,    -1,    17,    86,    87,    88,    89,    90,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,   101,
     102,    -1,    -1,   105,    -1,    -1,   108,   109,   110,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    58,    -1,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    77,    78,    11,    12,    -1,    14,    15,
      -1,    17,    86,    87,    88,    89,    90,    91,    -1,    -1,
      -1,    95,    96,    97,    98,    99,    -1,   101,   102,    -1,
      -1,   105,    -1,    -1,   108,   109,   110,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    55,
      56,    57,    58,    -1,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    11,    12,    -1,    14,    15,    -1,    17,
      86,    87,    88,    89,    90,    91,    -1,    -1,    -1,    95,
      96,    97,    98,    99,    -1,   101,   102,    -1,    -1,   105,
      -1,    -1,   108,   109,   110,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    55,    56,    57,
      58,    -1,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    76,    77,
      78,     9,    -1,    11,    12,    -1,    14,    15,    86,    87,
      88,    89,    90,    91,    -1,    -1,    -1,    95,    96,    97,
      98,    99,    -1,   101,   102,    -1,    -1,   105,    -1,    -1,
     108,   109,   110,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    55,    56,    57,
      58,    -1,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,
      78,    11,    12,    -1,    14,    15,    -1,    17,    86,    87,
      88,    89,    90,    91,    -1,    -1,    -1,    95,    96,    97,
      98,    99,    -1,   101,   102,    -1,    -1,   105,    -1,    -1,
     108,   109,   110,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    55,    56,    57,    58,    -1,
      60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    78,    11,
      12,    -1,    14,    15,    -1,    17,    86,    87,    88,    89,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,   101,   102,    -1,    -1,   105,    -1,    -1,   108,   109,
     110,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    55,    56,    57,    58,    -1,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    77,    78,    11,    12,    -1,
      14,    15,    -1,    -1,    86,    87,    88,    89,    90,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,   101,
     102,    -1,    -1,   105,    -1,    -1,   108,   109,   110,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    58,    -1,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    77,    78,    12,    -1,    14,    15,    -1,
      -1,    -1,    86,    87,    88,    89,    90,    91,    -1,    -1,
      -1,    95,    96,    97,    98,    99,    -1,   101,   102,    -1,
      -1,   105,    -1,    -1,   108,   109,   110,    -1,    45,    46,
      -1,    48,    -1,    50,    -1,    -1,    -1,    -1,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    78,    12,    -1,    -1,    15,    16,    -1,    -1,    86,
      87,    88,    89,    90,    91,    -1,    -1,    -1,    95,    -1,
      97,    98,     7,     8,   101,   102,    11,    12,   105,    -1,
      15,   108,   109,   110,    -1,    45,    46,    -1,    48,    -1,
      50,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    -1,    48,    -1,    -1,    76,    77,    78,    54,
      55,    56,    -1,    58,    -1,    60,    86,    87,    88,    89,
      90,    91,    -1,    -1,    -1,    95,    -1,    97,    98,    -1,
      -1,   101,   102,    78,    -1,   105,    -1,    -1,   108,   109,
     110,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    -1,    -1,
     105,    -1,    -1,   108,   109,   110,     9,    -1,    -1,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,     9,    -1,    -1,    12,    -1,    14,
      15,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      45,    46,   105,    48,    -1,   108,   109,   110,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,     9,    -1,    -1,    12,    -1,    -1,    15,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    45,    46,
     105,    48,    -1,   108,   109,   110,    -1,    -1,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
       9,    -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    89,    90,    91,    -1,    -1,    -1,    95,    -1,
      97,    98,    -1,    -1,   101,   102,    45,    46,   105,    48,
      -1,   108,   109,   110,    -1,    -1,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,     9,    -1,
      -1,    12,    -1,    -1,    15,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    45,    46,   105,    48,    -1,   108,
     109,   110,    -1,    -1,    55,    56,    -1,    58,    -1,    60,
      -1,    -1,    -1,    -1,    -1,    -1,     9,    -1,    -1,    12,
      -1,    -1,    15,    -1,    -1,    -1,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    95,    -1,    97,    98,    -1,    -1,
     101,   102,    45,    46,   105,    48,    -1,   108,   109,   110,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,     9,    -1,    -1,    12,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      -1,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      45,    46,   105,    48,    -1,   108,   109,   110,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,     9,    -1,    -1,    12,    -1,    -1,    15,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    45,    46,
     105,    48,    -1,   108,   109,   110,    -1,    -1,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    89,    90,    91,    -1,    -1,    -1,    95,    -1,
      97,    98,    -1,    -1,   101,   102,    -1,    -1,   105,    -1,
      -1,   108,   109,   110,    10,    -1,    12,    13,    -1,    -1,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    -1,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    91,    -1,    93,    -1,    95,
      -1,    97,    98,    -1,    -1,   101,   102,    -1,    -1,   105,
      -1,    -1,   108,   109,   110,    10,    -1,    12,    13,    -1,
      -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    36,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    -1,    93,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    -1,    -1,
     105,    -1,    -1,   108,   109,   110,    10,    -1,    12,    13,
      -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    -1,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    88,    89,    90,    91,    -1,    93,
      -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,    -1,
      -1,   105,    -1,    -1,   108,   109,   110,    10,    -1,    12,
      13,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      -1,    10,   105,    12,    13,   108,   109,   110,    -1,    -1,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    91,    -1,    93,    -1,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    12,    13,   105,    -1,    16,   108,
     109,   110,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    -1,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    90,    91,    -1,    93,    -1,    95,    -1,    97,
      98,    -1,    -1,   101,   102,    12,    13,   105,    -1,    -1,
     108,   109,   110,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
      -1,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    89,    90,    91,    -1,    93,    94,    95,    -1,
      97,    98,    -1,    -1,   101,   102,    12,    13,   105,    -1,
      16,   108,   109,   110,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    -1,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    91,    -1,    93,    -1,    95,
      -1,    97,    98,    -1,    -1,   101,   102,    12,    13,   105,
      -1,    16,   108,   109,   110,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    36,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    -1,    93,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    12,    13,
     105,    -1,    16,   108,   109,   110,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    -1,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    88,    89,    90,    91,    -1,    93,
      -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,    12,
      13,   105,    -1,    16,   108,   109,   110,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      12,    13,   105,    -1,    16,   108,   109,   110,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    -1,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,    91,
      -1,    93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,
     102,    -1,    -1,   105,    -1,    -1,   108,   109,   110,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      12,    13,   105,    -1,    16,   108,   109,   110,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    -1,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,    91,
      -1,    93,    -1,    95,    -1,    97,    98,    -1,    -1,   101,
     102,    12,    13,   105,    -1,    -1,   108,   109,   110,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    35,    36,    -1,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      91,    -1,    93,    -1,    95,    -1,    97,    98,    -1,    -1,
     101,   102,    12,    13,   105,    -1,    -1,   108,   109,   110,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,
      90,    91,    12,    13,    -1,    95,    -1,    97,    98,    -1,
      -1,   101,   102,    23,    -1,   105,    -1,    -1,   108,   109,
     110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,
      90,    91,    12,    13,    -1,    95,    -1,    97,    98,    -1,
      -1,   101,   102,    23,    -1,   105,    -1,    -1,   108,   109,
     110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,    12,    -1,    -1,    86,    87,    88,    89,
      90,    91,    -1,    -1,    23,    -1,    -1,    97,    98,    -1,
      -1,   101,   102,    -1,    -1,   105,    35,    36,   108,   109,
     110,    -1,    -1,    -1,    -1,    44,    45,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    91,    12,    93,    94,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    23,    -1,   105,    -1,    -1,   108,
     109,   110,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    12,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    91,    23,    93,    -1,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    -1,    35,   105,    -1,    -1,   108,
     109,   110,    -1,    -1,    44,    45,    46,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      12,    -1,    -1,    15,    16,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,
      90,    91,    -1,    -1,    -1,    95,    -1,    97,    98,    -1,
      -1,   101,   102,    45,    46,   105,    48,    -1,   108,   109,
     110,    -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    12,    -1,
      -1,    15,    16,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,    91,
      -1,    -1,    -1,    95,    -1,    97,    98,    -1,    -1,   101,
     102,    45,    46,   105,    48,    -1,   108,   109,   110,    -1,
      -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    12,    -1,    -1,
      -1,    -1,    86,    87,    88,    89,    90,    91,    23,    -1,
      -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,    -1,
      -1,   105,    -1,    -1,   108,   109,   110,    -1,    -1,    44,
      45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    12,    -1,    -1,    15,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    45,    46,
     105,    48,    -1,   108,   109,   110,    -1,    -1,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    89,    90,    91,    -1,    -1,    -1,    95,    -1,
      97,    98,    -1,    -1,   101,   102,    45,    46,   105,    48,
      -1,   108,   109,   110,    -1,    -1,    55,    56,    -1,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    12,    -1,    -1,    15,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    95,    -1,    97,    98,
      -1,    -1,   101,   102,    45,    46,   105,    48,    -1,   108,
     109,   110,    -1,    -1,    55,    56,    -1,    58,    -1,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    95,    -1,    97,    98,    -1,    -1,
     101,   102,    45,    46,   105,    48,    -1,   108,   109,   110,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    12,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    -1,
      -1,    -1,    95,    -1,    97,    98,    -1,    -1,   101,   102,
      45,    46,   105,    48,    -1,   108,   109,   110,    -1,    -1,
      55,    56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    86,    87,    88,    89,    90,    91,    23,    -1,    -1,
      95,    -1,    97,    98,    -1,    -1,   101,   102,    -1,    -1,
     105,    12,    -1,   108,   109,   110,    -1,    -1,    44,    45,
      46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    46,    -1,    48,    -1,    -1,
      -1,    -1,    78,    -1,    55,    56,    -1,    58,    -1,    60,
      86,    87,    88,    89,    90,    91,    -1,    -1,    -1,    -1,
      -1,    97,    98,    -1,    -1,   101,   102,    78,    -1,   105,
      -1,    -1,   108,   109,   110,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    95,    -1,    97,    98,    -1,    -1,
     101,   102,    -1,    -1,   105,    -1,    -1,   108,   109,   110
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     9,    14,   113,   128,   130,   142,     0,     7,
       8,    11,    12,    15,    45,    46,    48,    54,    55,    56,
      58,    60,    78,    86,    87,    88,    89,    90,    91,    95,
      97,    98,   101,   102,   105,   108,   109,   110,   125,   131,
     132,   134,   137,   144,   145,   155,   156,   157,   159,     9,
      14,   125,   125,   137,   138,   146,    12,    12,   105,   156,
     157,    86,    89,   123,    12,    12,    12,    12,    42,   157,
      12,    12,   156,   156,   144,   156,   157,   157,   156,     1,
       9,    14,    47,    49,    50,    51,    52,    53,    57,    61,
      62,    76,    77,    96,    99,   117,   119,   124,   125,   137,
     141,   148,   150,   154,   160,    10,   125,   128,    13,    23,
      35,    36,    38,    39,    40,    41,    42,    43,    44,    93,
     114,   115,   156,    12,    15,   101,   102,   103,   104,   107,
      69,   108,   109,    18,   154,   154,    10,    16,   116,    16,
     116,    92,    16,   135,   137,   137,    12,   137,   137,   135,
      16,   135,   156,    42,   137,   137,     9,   126,   127,    14,
     126,   149,   149,   159,   137,   149,    12,    12,   149,   149,
     137,   149,    12,     9,   151,   150,   154,    12,   136,   139,
     140,   144,   156,   157,   149,    17,   150,   153,   127,   154,
     132,    95,   137,   145,   137,   137,   137,   137,   137,   137,
     159,   137,     9,   137,     9,   137,   137,   145,    69,   156,
     156,   156,   156,   156,   156,   137,   135,    17,    17,     9,
     137,    44,   137,    15,    16,   116,    86,   158,   116,   116,
      16,    16,   156,   116,   116,     9,   127,    18,   149,   129,
     148,   159,   137,   149,   137,   150,    79,   118,    17,   143,
     138,    23,    44,    93,   114,   115,   156,   116,    13,    37,
      40,    69,   150,   131,    17,   157,    94,   116,   116,   156,
      19,   159,   137,    16,   116,   147,   137,   145,   137,   145,
     159,   137,   135,    14,    44,   147,   147,   152,     9,   150,
       9,    16,    12,   136,   145,   159,   136,   136,   136,   136,
     156,   156,   156,   136,   125,   137,   137,   137,    86,     9,
     133,    16,    16,    16,    16,    16,   116,    16,   116,    19,
      14,   127,   159,    99,    44,   138,    94,   154,    16,   116,
      16,   116,   125,   137,   145,   137,   127,   137,   147,    12,
     159,    16,   136,    17,   157,   157,   154,    16,    16,    16,
     129,    14,   122,   137,    16,    16,    17,   147,   127,   150,
      16,   121,   129,   149,   150,   147,   120,   150
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   112,   113,   113,   114,   114,   115,   115,   116,   116,
     117,   117,   118,   118,   120,   119,   121,   119,   122,   119,
     123,   123,   124,   125,   125,   126,   126,   127,   127,   128,
     128,   129,   129,   130,   130,   131,   132,   132,   132,   132,
     132,   132,   132,   133,   132,   134,   134,   135,   135,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   138,
     138,   139,   139,   140,   140,   140,   141,   141,   142,   142,
     142,   142,   143,   143,   144,   144,   146,   145,   147,   147,
     148,   148,   148,   148,   148,   148,   148,   148,   149,   149,
     150,   150,   151,   152,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   153,   150,   150,   154,
     154,   155,   155,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   157,   157,   157,   157,   158,
     158,   158,   159,   159,   159,   160
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     2,     1,     2,
       1,     2,     1,     2,     0,    12,     0,    10,     0,     8,
       1,     1,     4,     1,     2,     1,     2,     0,     1,     0,
       1,     0,     1,     1,     3,     1,     1,     4,     4,     7,
       3,     4,     4,     0,     9,     1,     3,     1,     3,     3,
       5,     3,     3,     3,     3,     3,     5,     2,     1,     1,
       3,     5,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     5,     4,     3,     2,     1,     1,     3,
       3,     1,     3,     0,     1,     3,     1,     1,     1,     1,
       2,     2,     1,     2,     1,     2,     0,     4,     1,     2,
       4,     4,     4,     2,     5,     2,     1,     1,     1,     2,
       2,     2,     0,     0,     9,     3,     2,     1,     4,     2,
       3,     2,     2,     3,     2,     2,     0,     3,     2,     1,
       2,     1,     1,     4,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     3,     4,     1,     3,     4,     2,     2,
       2,     2,     2,     4,     3,     2,     1,     6,     6,     3,
       6,     6,     1,     8,     8,     6,     4,     1,     6,     6,
       8,     8,     8,     6,     1,     1,     4,     1,     2,     0,
       1,     3,     1,     1,     1,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
    { if (errorflag==0)
			winner = (Node *)stat3(PROGRAM, beginloc, (yyvsp[0].p), endloc); }
    break;

  case 3:
    { yyclearin; bracecheck(); SYNTAX("bailing out"); }
    break;

  case 14:
    {inloop++;}
    break;

  case 15:
    { --inloop; (yyval.p) = stat4(FOR, (yyvsp[-9].p), notnull((yyvsp[-6].p)), (yyvsp[-3].p), (yyvsp[0].p)); }
    break;

  case 16:
    {inloop++;}
    break;

  case 17:
    { --inloop; (yyval.p) = stat4(FOR, (yyvsp[-7].p), NIL, (yyvsp[-3].p), (yyvsp[0].p)); }
    break;

  case 18:
    {inloop++;}
    break;

  case 19:
    { --inloop; (yyval.p) = stat3(IN, (yyvsp[-5].p), makearr((yyvsp[-3].p)), (yyvsp[0].p)); }
    break;

  case 20:
    { setfname((yyvsp[0].cp)); }
    break;

  case 21:
    { setfname((yyvsp[0].cp)); }
    break;

  case 22:
    { (yyval.p) = notnull((yyvsp[-1].p)); }
    break;

  case 27:
    { (yyval.i) = 0; }
    break;

  case 29:
    { (yyval.i) = 0; }
    break;

  case 31:
    { (yyval.p) = 0; }
    break;

  case 33:
    { (yyval.p) = 0; }
    break;

  case 34:
    { (yyval.p) = (yyvsp[-1].p); }
    break;

  case 35:
    { (yyval.p) = notnull((yyvsp[0].p)); }
    break;

  case 36:
    { (yyval.p) = stat2(PASTAT, (yyvsp[0].p), stat2(PRINT, rectonode(), NIL)); }
    break;

  case 37:
    { (yyval.p) = stat2(PASTAT, (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 38:
    { (yyval.p) = pa2stat((yyvsp[-3].p), (yyvsp[0].p), stat2(PRINT, rectonode(), NIL)); }
    break;

  case 39:
    { (yyval.p) = pa2stat((yyvsp[-6].p), (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 40:
    { (yyval.p) = stat2(PASTAT, NIL, (yyvsp[-1].p)); }
    break;

  case 41:
    { beginloc = linkum(beginloc, (yyvsp[-1].p)); (yyval.p) = 0; }
    break;

  case 42:
    { endloc = linkum(endloc, (yyvsp[-1].p)); (yyval.p) = 0; }
    break;

  case 43:
    {infunc++;}
    break;

  case 44:
    { infunc--; curfname=0; defn((Cell *)(yyvsp[-7].p), (yyvsp[-5].p), (yyvsp[-1].p)); (yyval.p) = 0; }
    break;

  case 46:
    { (yyval.p) = linkum((yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 48:
    { (yyval.p) = linkum((yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 49:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 50:
    { (yyval.p) = op3(CONDEXPR, notnull((yyvsp[-4].p)), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 51:
    { (yyval.p) = op2(BOR, notnull((yyvsp[-2].p)), notnull((yyvsp[0].p))); }
    break;

  case 52:
    { (yyval.p) = op2(AND, notnull((yyvsp[-2].p)), notnull((yyvsp[0].p))); }
    break;

  case 53:
    { (yyval.p) = op3((yyvsp[-1].i), NIL, (yyvsp[-2].p), (Node*)makedfa((yyvsp[0].s), 0)); }
    break;

  case 54:
    { if (constnode((yyvsp[0].p)))
			(yyval.p) = op3((yyvsp[-1].i), NIL, (yyvsp[-2].p), (Node*)makedfa(strnode((yyvsp[0].p)), 0));
		  else
			(yyval.p) = op3((yyvsp[-1].i), (Node *)1, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 55:
    { (yyval.p) = op2(INTEST, (yyvsp[-2].p), makearr((yyvsp[0].p))); }
    break;

  case 56:
    { (yyval.p) = op2(INTEST, (yyvsp[-3].p), makearr((yyvsp[0].p))); }
    break;

  case 57:
    { (yyval.p) = op2(CAT, (yyvsp[-1].p), (yyvsp[0].p)); }
    break;

  case 60:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 61:
    { (yyval.p) = op3(CONDEXPR, notnull((yyvsp[-4].p)), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 62:
    { (yyval.p) = op2(BOR, notnull((yyvsp[-2].p)), notnull((yyvsp[0].p))); }
    break;

  case 63:
    { (yyval.p) = op2(AND, notnull((yyvsp[-2].p)), notnull((yyvsp[0].p))); }
    break;

  case 64:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 65:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 66:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 67:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 68:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 69:
    { (yyval.p) = op2((yyvsp[-1].i), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 70:
    { (yyval.p) = op3((yyvsp[-1].i), NIL, (yyvsp[-2].p), (Node*)makedfa((yyvsp[0].s), 0)); }
    break;

  case 71:
    { if (constnode((yyvsp[0].p)))
			(yyval.p) = op3((yyvsp[-1].i), NIL, (yyvsp[-2].p), (Node*)makedfa(strnode((yyvsp[0].p)), 0));
		  else
			(yyval.p) = op3((yyvsp[-1].i), (Node *)1, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 72:
    { (yyval.p) = op2(INTEST, (yyvsp[-2].p), makearr((yyvsp[0].p))); }
    break;

  case 73:
    { (yyval.p) = op2(INTEST, (yyvsp[-3].p), makearr((yyvsp[0].p))); }
    break;

  case 74:
    { 
			if (safe) SYNTAX("cmd | getline is unsafe");
			else (yyval.p) = op3(GETLINE, (yyvsp[0].p), itonp((yyvsp[-2].i)), (yyvsp[-3].p)); }
    break;

  case 75:
    { 
			if (safe) SYNTAX("cmd | getline is unsafe");
			else (yyval.p) = op3(GETLINE, (Node*)0, itonp((yyvsp[-1].i)), (yyvsp[-2].p)); }
    break;

  case 76:
    { (yyval.p) = op2(CAT, (yyvsp[-1].p), (yyvsp[0].p)); }
    break;

  case 79:
    { (yyval.p) = linkum((yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 80:
    { (yyval.p) = linkum((yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 82:
    { (yyval.p) = linkum((yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 83:
    { (yyval.p) = rectonode(); }
    break;

  case 85:
    { (yyval.p) = (yyvsp[-1].p); }
    break;

  case 94:
    { (yyval.p) = op3(MATCH, NIL, rectonode(), (Node*)makedfa((yyvsp[0].s), 0)); }
    break;

  case 95:
    { (yyval.p) = op1(NOT, notnull((yyvsp[0].p))); }
    break;

  case 96:
    {startreg();}
    break;

  case 97:
    { (yyval.s) = (yyvsp[-1].s); }
    break;

  case 100:
    { 
			if (safe) SYNTAX("print | is unsafe");
			else (yyval.p) = stat3((yyvsp[-3].i), (yyvsp[-2].p), itonp((yyvsp[-1].i)), (yyvsp[0].p)); }
    break;

  case 101:
    {
			if (safe) SYNTAX("print >> is unsafe");
			else (yyval.p) = stat3((yyvsp[-3].i), (yyvsp[-2].p), itonp((yyvsp[-1].i)), (yyvsp[0].p)); }
    break;

  case 102:
    {
			if (safe) SYNTAX("print > is unsafe");
			else (yyval.p) = stat3((yyvsp[-3].i), (yyvsp[-2].p), itonp((yyvsp[-1].i)), (yyvsp[0].p)); }
    break;

  case 103:
    { (yyval.p) = stat3((yyvsp[-1].i), (yyvsp[0].p), NIL, NIL); }
    break;

  case 104:
    { (yyval.p) = stat2(DELETE, makearr((yyvsp[-3].p)), (yyvsp[-1].p)); }
    break;

  case 105:
    { (yyval.p) = stat2(DELETE, makearr((yyvsp[0].p)), 0); }
    break;

  case 106:
    { (yyval.p) = exptostat((yyvsp[0].p)); }
    break;

  case 107:
    { yyclearin; SYNTAX("illegal statement"); }
    break;

  case 110:
    { if (!inloop) SYNTAX("break illegal outside of loops");
				  (yyval.p) = stat1(BREAK, NIL); }
    break;

  case 111:
    {  if (!inloop) SYNTAX("continue illegal outside of loops");
				  (yyval.p) = stat1(CONTINUE, NIL); }
    break;

  case 112:
    {inloop++;}
    break;

  case 113:
    {--inloop;}
    break;

  case 114:
    { (yyval.p) = stat2(DO, (yyvsp[-6].p), notnull((yyvsp[-2].p))); }
    break;

  case 115:
    { (yyval.p) = stat1(EXIT, (yyvsp[-1].p)); }
    break;

  case 116:
    { (yyval.p) = stat1(EXIT, NIL); }
    break;

  case 118:
    { (yyval.p) = stat3(IF, (yyvsp[-3].p), (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 119:
    { (yyval.p) = stat3(IF, (yyvsp[-1].p), (yyvsp[0].p), NIL); }
    break;

  case 120:
    { (yyval.p) = (yyvsp[-1].p); }
    break;

  case 121:
    { if (infunc)
				SYNTAX("next is illegal inside a function");
			  (yyval.p) = stat1(NEXT, NIL); }
    break;

  case 122:
    { if (infunc)
				SYNTAX("nextfile is illegal inside a function");
			  (yyval.p) = stat1(NEXTFILE, NIL); }
    break;

  case 123:
    { (yyval.p) = stat1(RETURN, (yyvsp[-1].p)); }
    break;

  case 124:
    { (yyval.p) = stat1(RETURN, NIL); }
    break;

  case 126:
    {inloop++;}
    break;

  case 127:
    { --inloop; (yyval.p) = stat2(WHILE, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 128:
    { (yyval.p) = 0; }
    break;

  case 130:
    { (yyval.p) = linkum((yyvsp[-1].p), (yyvsp[0].p)); }
    break;

  case 133:
    { (yyval.p) = op2(DIVEQ, (yyvsp[-3].p), (yyvsp[0].p)); }
    break;

  case 134:
    { (yyval.p) = op2(ADD, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 135:
    { (yyval.p) = op2(MINUS, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 136:
    { (yyval.p) = op2(MULT, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 137:
    { (yyval.p) = op2(DIVIDE, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 138:
    { (yyval.p) = op2(MOD, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 139:
    { (yyval.p) = op2(POWER, (yyvsp[-2].p), (yyvsp[0].p)); }
    break;

  case 140:
    { (yyval.p) = op1(UMINUS, (yyvsp[0].p)); }
    break;

  case 141:
    { (yyval.p) = (yyvsp[0].p); }
    break;

  case 142:
    { (yyval.p) = op1(NOT, notnull((yyvsp[0].p))); }
    break;

  case 143:
    { (yyval.p) = op2(BLTIN, itonp((yyvsp[-2].i)), rectonode()); }
    break;

  case 144:
    { (yyval.p) = op2(BLTIN, itonp((yyvsp[-3].i)), (yyvsp[-1].p)); }
    break;

  case 145:
    { (yyval.p) = op2(BLTIN, itonp((yyvsp[0].i)), rectonode()); }
    break;

  case 146:
    { (yyval.p) = op2(CALL, celltonode((yyvsp[-2].cp),CVAR), NIL); }
    break;

  case 147:
    { (yyval.p) = op2(CALL, celltonode((yyvsp[-3].cp),CVAR), (yyvsp[-1].p)); }
    break;

  case 148:
    { (yyval.p) = op1(CLOSE, (yyvsp[0].p)); }
    break;

  case 149:
    { (yyval.p) = op1(PREDECR, (yyvsp[0].p)); }
    break;

  case 150:
    { (yyval.p) = op1(PREINCR, (yyvsp[0].p)); }
    break;

  case 151:
    { (yyval.p) = op1(POSTDECR, (yyvsp[-1].p)); }
    break;

  case 152:
    { (yyval.p) = op1(POSTINCR, (yyvsp[-1].p)); }
    break;

  case 153:
    { (yyval.p) = op3(GETLINE, (yyvsp[-2].p), itonp((yyvsp[-1].i)), (yyvsp[0].p)); }
    break;

  case 154:
    { (yyval.p) = op3(GETLINE, NIL, itonp((yyvsp[-1].i)), (yyvsp[0].p)); }
    break;

  case 155:
    { (yyval.p) = op3(GETLINE, (yyvsp[0].p), NIL, NIL); }
    break;

  case 156:
    { (yyval.p) = op3(GETLINE, NIL, NIL, NIL); }
    break;

  case 157:
    { (yyval.p) = op2(INDEX, (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 158:
    { SYNTAX("index() doesn't permit regular expressions");
		  (yyval.p) = op2(INDEX, (yyvsp[-3].p), (Node*)(yyvsp[-1].s)); }
    break;

  case 159:
    { (yyval.p) = (yyvsp[-1].p); }
    break;

  case 160:
    { (yyval.p) = op3(MATCHFCN, NIL, (yyvsp[-3].p), (Node*)makedfa((yyvsp[-1].s), 1)); }
    break;

  case 161:
    { if (constnode((yyvsp[-1].p)))
			(yyval.p) = op3(MATCHFCN, NIL, (yyvsp[-3].p), (Node*)makedfa(strnode((yyvsp[-1].p)), 1));
		  else
			(yyval.p) = op3(MATCHFCN, (Node *)1, (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 162:
    { (yyval.p) = celltonode((yyvsp[0].cp), CCON); }
    break;

  case 163:
    { (yyval.p) = op4(SPLIT, (yyvsp[-5].p), makearr((yyvsp[-3].p)), (yyvsp[-1].p), (Node*)STRING); }
    break;

  case 164:
    { (yyval.p) = op4(SPLIT, (yyvsp[-5].p), makearr((yyvsp[-3].p)), (Node*)makedfa((yyvsp[-1].s), 1), (Node *)REGEXPR); }
    break;

  case 165:
    { (yyval.p) = op4(SPLIT, (yyvsp[-3].p), makearr((yyvsp[-1].p)), NIL, (Node*)STRING); }
    break;

  case 166:
    { (yyval.p) = op1((yyvsp[-3].i), (yyvsp[-1].p)); }
    break;

  case 167:
    { (yyval.p) = celltonode((yyvsp[0].cp), CCON); }
    break;

  case 168:
    { (yyval.p) = op4((yyvsp[-5].i), NIL, (Node*)makedfa((yyvsp[-3].s), 1), (yyvsp[-1].p), rectonode()); }
    break;

  case 169:
    { if (constnode((yyvsp[-3].p)))
			(yyval.p) = op4((yyvsp[-5].i), NIL, (Node*)makedfa(strnode((yyvsp[-3].p)), 1), (yyvsp[-1].p), rectonode());
		  else
			(yyval.p) = op4((yyvsp[-5].i), (Node *)1, (yyvsp[-3].p), (yyvsp[-1].p), rectonode()); }
    break;

  case 170:
    { (yyval.p) = op4((yyvsp[-7].i), NIL, (Node*)makedfa((yyvsp[-5].s), 1), (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 171:
    { if (constnode((yyvsp[-5].p)))
			(yyval.p) = op4((yyvsp[-7].i), NIL, (Node*)makedfa(strnode((yyvsp[-5].p)), 1), (yyvsp[-3].p), (yyvsp[-1].p));
		  else
			(yyval.p) = op4((yyvsp[-7].i), (Node *)1, (yyvsp[-5].p), (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 172:
    { (yyval.p) = op3(SUBSTR, (yyvsp[-5].p), (yyvsp[-3].p), (yyvsp[-1].p)); }
    break;

  case 173:
    { (yyval.p) = op3(SUBSTR, (yyvsp[-3].p), (yyvsp[-1].p), NIL); }
    break;

  case 176:
    { (yyval.p) = op2(ARRAY, makearr((yyvsp[-3].p)), (yyvsp[-1].p)); }
    break;

  case 177:
    { (yyval.p) = op1(INDIRECT, celltonode((yyvsp[0].cp), CVAR)); }
    break;

  case 178:
    { (yyval.p) = op1(INDIRECT, (yyvsp[0].p)); }
    break;

  case 179:
    { arglist = (yyval.p) = 0; }
    break;

  case 180:
    { arglist = (yyval.p) = celltonode((yyvsp[0].cp),CVAR); }
    break;

  case 181:
    {
			checkdup((yyvsp[-2].p), (yyvsp[0].cp));
			arglist = (yyval.p) = linkum((yyvsp[-2].p),celltonode((yyvsp[0].cp),CVAR)); }
    break;

  case 182:
    { (yyval.p) = celltonode((yyvsp[0].cp), CVAR); }
    break;

  case 183:
    { (yyval.p) = op1(ARG, itonp((yyvsp[0].i))); }
    break;

  case 184:
    { (yyval.p) = op1(VARNF, (Node *) (yyvsp[0].cp)); }
    break;

  case 185:
    { (yyval.p) = notnull((yyvsp[-1].p)); }
    break;


      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}


void setfname(Cell *p)
{
	if (isarr(p))
		SYNTAX("%s is an array, not a function", p->nval);
	else if (isfcn(p))
		SYNTAX("you can't define function %s more than once", p->nval);
	curfname = p->nval;
}

int constnode(Node *p)
{
	return isvalue(p) && ((Cell *) (p->narg[0]))->csub == CCON;
}

char *strnode(Node *p)
{
	return ((Cell *)(p->narg[0]))->sval;
}

Node *notnull(Node *n)
{
	switch (n->nobj) {
	case LE: case LT: case EQ: case NE: case GT: case GE:
	case BOR: case AND: case NOT:
		return n;
	default:
		return op2(NE, n, nullnode);
	}
}

void checkdup(Node *vl, Cell *cp)	/* check if name already in list */
{
	char *s = cp->nval;
	for ( ; vl; vl = vl->nnext) {
		if (strcmp(s, ((Cell *)(vl->narg[0]))->nval) == 0) {
			SYNTAX("duplicate argument %s", s);
			break;
		}
	}
}

/*** b.c ***/

#define	HAT	(NCHARS+2)	/* matches ^ in regular expr */
				/* NCHARS is 2**n */
#define MAXLIN 22

#define type(v)		(v)->nobj	/* badly overloaded here */
#define info(v)		(v)->ntype	/* badly overloaded here */
#define left(v)		(v)->narg[0]
#define right(v)	(v)->narg[1]
#define parent(v)	(v)->nnext

#define LEAF	case CCL: case NCCL: case CHAR: case DOT: case FINAL: case ALL:
#define ELEAF	case EMPTYRE:		/* empty string in regexp */
#define UNARY	case STAR: case PLUS: case QUEST:

/* encoding in tree Nodes:
	leaf (CCL, NCCL, CHAR, DOT, FINAL, ALL, EMPTYRE):
		left is index, right contains value or pointer to value
	unary (STAR, PLUS, QUEST): left is child, right is null
	binary (CAT, OR): left and right are children
	parent contains pointer to parent
*/


int	*setvec;
int	*tmpset;
int	maxsetvec = 0;

int	rtok;		/* next token in current re */
int	rlxval;
static uschar	*rlxstr;
static uschar	*prestr;	/* current position in current re */
static uschar	*lastre;	/* origin of last re */

static	int setcnt;
static	int poscnt;

char	*patbeg;
int	patlen;

#define	NFA	20	/* cache this many dynamic fa's */
fa	*fatab[NFA];
int	nfatab	= 0;	/* entries in fatab */

fa *makedfa(const char *s, int anchor)	/* returns dfa for reg expr s */
{
	int i, use, nuse;
	fa *pfa;
	static int now = 1;

	if (setvec == 0) {	/* first time through any RE */
		maxsetvec = MAXLIN;
		setvec = (int *) malloc(maxsetvec * sizeof(int));
		tmpset = (int *) malloc(maxsetvec * sizeof(int));
		if (setvec == 0 || tmpset == 0)
			overflo("out of space initializing makedfa");
	}

	if (compile_time)	/* a constant for sure */
		return mkdfa(s, anchor);
	for (i = 0; i < nfatab; i++)	/* is it there already? */
		if (fatab[i]->anchor == anchor
		  && strcmp((const char *) fatab[i]->restr, s) == 0) {
			fatab[i]->use = now++;
			return fatab[i];
		}
	pfa = mkdfa(s, anchor);
	if (nfatab < NFA) {	/* room for another */
		fatab[nfatab] = pfa;
		fatab[nfatab]->use = now++;
		nfatab++;
		return pfa;
	}
	use = fatab[0]->use;	/* replace least-recently used */
	nuse = 0;
	for (i = 1; i < nfatab; i++)
		if (fatab[i]->use < use) {
			use = fatab[i]->use;
			nuse = i;
		}
	freefa(fatab[nuse]);
	fatab[nuse] = pfa;
	pfa->use = now++;
	return pfa;
}

fa *mkdfa(const char *s, int anchor)	/* does the real work of making a dfa */
				/* anchor = 1 for anchored matches, else 0 */
{
	Node *p, *p1;
	fa *f;

	p = reparse(s);
	p1 = op2(CAT, op2(STAR, op2(ALL, NIL, NIL), NIL), p);
		/* put ALL STAR in front of reg.  exp. */
	p1 = op2(CAT, p1, op2(FINAL, NIL, NIL));
		/* put FINAL after reg.  exp. */

	poscnt = 0;
	penter(p1);	/* enter parent pointers and leaf indices */
	if ((f = (fa *) calloc(1, sizeof(fa) + poscnt*sizeof(rrow))) == NULL)
		overflo("out of space for fa");
	f->accept = poscnt-1;	/* penter has computed number of positions in re */
	cfoll(f, p1);	/* set up follow sets */
	freetr(p1);
	if ((f->posns[0] = (int *) calloc(1, *(f->re[0].lfollow)*sizeof(int))) == NULL)
			overflo("out of space in makedfa");
	if ((f->posns[1] = (int *) calloc(1, sizeof(int))) == NULL)
		overflo("out of space in makedfa");
	*f->posns[1] = 0;
	f->initstat = makeinit(f, anchor);
	f->anchor = anchor;
	f->restr = (uschar *) tostring(s);
	return f;
}

int makeinit(fa *f, int anchor)
{
	int i, k;

	f->curstat = 2;
	f->out[2] = 0;
	f->reset = 0;
	k = *(f->re[0].lfollow);
	xfree(f->posns[2]);			
	if ((f->posns[2] = (int *) calloc(1, (k+1)*sizeof(int))) == NULL)
		overflo("out of space in makeinit");
	for (i=0; i <= k; i++) {
		(f->posns[2])[i] = (f->re[0].lfollow)[i];
	}
	if ((f->posns[2])[1] == f->accept)
		f->out[2] = 1;
	for (i=0; i < NCHARS; i++)
		f->gototab[2][i] = 0;
	f->curstat = cgoto(f, 2, HAT);
	if (anchor) {
		*f->posns[2] = k-1;	/* leave out position 0 */
		for (i=0; i < k; i++) {
			(f->posns[0])[i] = (f->posns[2])[i];
		}

		f->out[0] = f->out[2];
		if (f->curstat != 2)
			--(*f->posns[f->curstat]);
	}
	return f->curstat;
}

void penter(Node *p)	/* set up parent pointers and leaf indices */
{
	switch (type(p)) {
	ELEAF
	LEAF
		info(p) = poscnt;
		poscnt++;
		break;
	UNARY
		penter(left(p));
		parent(left(p)) = p;
		break;
	case CAT:
	case OR:
		penter(left(p));
		penter(right(p));
		parent(left(p)) = p;
		parent(right(p)) = p;
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in penter", type(p));
		break;
	}
}

void freetr(Node *p)	/* free parse tree */
{
	switch (type(p)) {
	ELEAF
	LEAF
		xfree(p);
		break;
	UNARY
		freetr(left(p));
		xfree(p);
		break;
	case CAT:
	case OR:
		freetr(left(p));
		freetr(right(p));
		xfree(p);
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in freetr", type(p));
		break;
	}
}

/* in the parsing of regular expressions, metacharacters like . have */
/* to be seen literally;  \056 is not a metacharacter. */

int hexstr(uschar **pp)	/* find and eval hex string at pp, return new p */
{			/* only pick up one 8-bit byte (2 chars) */
	uschar *p;
	int n = 0;
	int i;

	for (i = 0, p = (uschar *) *pp; i < 2 && isxdigit(*p); i++, p++) {
		if (isdigit(*p))
			n = 16 * n + *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			n = 16 * n + *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			n = 16 * n + *p - 'A' + 10;
	}
	*pp = (uschar *) p;
	return n;
}

#define isoctdigit(c) ((c) >= '0' && (c) <= '7')	/* multiple use of arg */

int quoted(uschar **pp)	/* pick up next thing after a \\ */
			/* and increment *pp */
{
	uschar *p = *pp;
	int c;

	if ((c = *p++) == 't')
		c = '\t';
	else if (c == 'n')
		c = '\n';
	else if (c == 'f')
		c = '\f';
	else if (c == 'r')
		c = '\r';
	else if (c == 'b')
		c = '\b';
	else if (c == '\\')
		c = '\\';
	else if (c == 'x') {	/* hexadecimal goo follows */
		c = hexstr(&p);	/* this adds a null if number is invalid */
	} else if (isoctdigit(c)) {	/* \d \dd \ddd */
		int n = c - '0';
		if (isoctdigit(*p)) {
			n = 8 * n + *p++ - '0';
			if (isoctdigit(*p))
				n = 8 * n + *p++ - '0';
		}
		c = n;
	} /* else */
		/* c = c; */
	*pp = p;
	return c;
}

char *cclenter(const char *argp)	/* add a character class */
{
	int i, c, c2;
	uschar *p = (uschar *) argp;
	uschar *op, *bp;
	static uschar *buf = 0;
	static int bufsz = 100;

	op = p;
	if (buf == 0 && (buf = (uschar *) malloc(bufsz)) == NULL)
		FATAL("out of space for character class [%.10s...] 1", p);
	bp = buf;
	for (i = 0; (c = *p++) != 0; ) {
		if (c == '\\') {
			c = quoted(&p);
		} else if (c == '-' && i > 0 && bp[-1] != 0) {
			if (*p != 0) {
				c = bp[-1];
				c2 = *p++;
				if (c2 == '\\')
					c2 = quoted(&p);
				if (c > c2) {	/* empty; ignore */
					bp--;
					i--;
					continue;
				}
				while (c < c2) {
					if (!adjbuf((char **) &buf, &bufsz, bp-buf+2, 100, (char **) &bp, "cclenter1"))
						FATAL("out of space for character class [%.10s...] 2", p);
					*bp++ = ++c;
					i++;
				}
				continue;
			}
		}
		if (!adjbuf((char **) &buf, &bufsz, bp-buf+2, 100, (char **) &bp, "cclenter2"))
			FATAL("out of space for character class [%.10s...] 3", p);
		*bp++ = c;
		i++;
	}
	*bp = 0;
	dprintf( ("cclenter: in = |%s|, out = |%s|\n", op, buf) );
	xfree(op);
	return (char *) tostring((char *) buf);
}

void overflo(const char *s)
{
	FATAL("regular expression too big: %.30s...", s);
}

void cfoll(fa *f, Node *v)	/* enter follow set of each leaf of vertex v into lfollow[leaf] */
{
	int i;
	int *p;

	switch (type(v)) {
	ELEAF
	LEAF
		f->re[info(v)].ltype = type(v);
		f->re[info(v)].lval.np = right(v);
		while (f->accept >= maxsetvec) {	/* guessing here! */
			maxsetvec *= 4;
			setvec = (int *) realloc(setvec, maxsetvec * sizeof(int));
			tmpset = (int *) realloc(tmpset, maxsetvec * sizeof(int));
			if (setvec == 0 || tmpset == 0)
				overflo("out of space in cfoll()");
		}
		for (i = 0; i <= f->accept; i++)
			setvec[i] = 0;
		setcnt = 0;
		follow(v);	/* computes setvec and setcnt */
		if ((p = (int *) calloc(1, (setcnt+1)*sizeof(int))) == NULL)
			overflo("out of space building follow set");
		f->re[info(v)].lfollow = p;
		*p = setcnt;
		for (i = f->accept; i >= 0; i--)
			if (setvec[i] == 1)
				*++p = i;
		break;
	UNARY
		cfoll(f,left(v));
		break;
	case CAT:
	case OR:
		cfoll(f,left(v));
		cfoll(f,right(v));
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in cfoll", type(v));
	}
}

int first(Node *p)	/* collects initially active leaves of p into setvec */
			/* returns 0 if p matches empty string */
{
	int b, lp;

	switch (type(p)) {
	ELEAF
	LEAF
		lp = info(p);	/* look for high-water mark of subscripts */
		while (setcnt >= maxsetvec || lp >= maxsetvec) {	/* guessing here! */
			maxsetvec *= 4;
			setvec = (int *) realloc(setvec, maxsetvec * sizeof(int));
			tmpset = (int *) realloc(tmpset, maxsetvec * sizeof(int));
			if (setvec == 0 || tmpset == 0)
				overflo("out of space in first()");
		}
		if (type(p) == EMPTYRE) {
			setvec[lp] = 0;
			return(0);
		}
		if (setvec[lp] != 1) {
			setvec[lp] = 1;
			setcnt++;
		}
		if (type(p) == CCL && (*(char *) right(p)) == '\0')
			return(0);		/* empty CCL */
		else return(1);
	case PLUS:
		if (first(left(p)) == 0) return(0);
		return(1);
	case STAR:
	case QUEST:
		first(left(p));
		return(0);
	case CAT:
		if (first(left(p)) == 0 && first(right(p)) == 0) return(0);
		return(1);
	case OR:
		b = first(right(p));
		if (first(left(p)) == 0 || b == 0) return(0);
		return(1);
	}
	FATAL("can't happen: unknown type %d in first", type(p));	/* can't happen */
	return(-1);
}

void follow(Node *v)	/* collects leaves that can follow v into setvec */
{
	Node *p;

	if (type(v) == FINAL)
		return;
	p = parent(v);
	switch (type(p)) {
	case STAR:
	case PLUS:
		first(v);
		follow(p);
		return;

	case OR:
	case QUEST:
		follow(p);
		return;

	case CAT:
		if (v == left(p)) {	/* v is left child of p */
			if (first(right(p)) == 0) {
				follow(p);
				return;
			}
		} else		/* v is right child */
			follow(p);
		return;
	}
}

int member(int c, const char *sarg)	/* is c in s? */
{
	uschar *s = (uschar *) sarg;

	while (*s)
		if (c == *s++)
			return(1);
	return(0);
}

int match(fa *f, const char *p0)	/* shortest match ? */
{
	int s, ns;
	uschar *p = (uschar *) p0;

	s = f->reset ? makeinit(f,0) : f->initstat;
	if (f->out[s])
		return(1);
	do {
		/* assert(*p < NCHARS); */
		if ((ns = f->gototab[s][*p]) != 0)
			s = ns;
		else
			s = cgoto(f, s, *p);
		if (f->out[s])
			return(1);
	} while (*p++ != 0);
	return(0);
}

int pmatch(fa *f, const char *p0)	/* longest match, for sub */
{
	int s, ns;
	uschar *p = (uschar *) p0;
	uschar *q;
	int i, k;

	/* s = f->reset ? makeinit(f,1) : f->initstat; */
	if (f->reset) {
		f->initstat = s = makeinit(f,1);
	} else {
		s = f->initstat;
	}
	patbeg = (char *) p;
	patlen = -1;
	do {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			/* assert(*q < NCHARS); */
			if ((ns = f->gototab[s][*q]) != 0)
				s = ns;
			else
				s = cgoto(f, s, *q);
			if (s == 1) {	/* no transition */
				if (patlen >= 0) {
					patbeg = (char *) p;
					return(1);
				}
				else
					goto nextin;	/* no match */
			}
		} while (*q++ != 0);
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen >= 0) {
			patbeg = (char *) p;
			return(1);
		}
	nextin:
		s = 2;
		if (f->reset) {
			for (i = 2; i <= f->curstat; i++)
				xfree(f->posns[i]);
			k = *f->posns[0];			
			if ((f->posns[2] = (int *) calloc(1, (k+1)*sizeof(int))) == NULL)
				overflo("out of space in pmatch");
			for (i = 0; i <= k; i++)
				(f->posns[2])[i] = (f->posns[0])[i];
			f->initstat = f->curstat = 2;
			f->out[2] = f->out[0];
			for (i = 0; i < NCHARS; i++)
				f->gototab[2][i] = 0;
		}
	} while (*p++ != 0);
	return (0);
}

int nematch(fa *f, const char *p0)	/* non-empty match, for sub */
{
	int s, ns;
	uschar *p = (uschar *) p0;
	uschar *q;
	int i, k;

	/* s = f->reset ? makeinit(f,1) : f->initstat; */
	if (f->reset) {
		f->initstat = s = makeinit(f,1);
	} else {
		s = f->initstat;
	}
	patlen = -1;
	while (*p) {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			/* assert(*q < NCHARS); */
			if ((ns = f->gototab[s][*q]) != 0)
				s = ns;
			else
				s = cgoto(f, s, *q);
			if (s == 1) {	/* no transition */
				if (patlen > 0) {
					patbeg = (char *) p;
					return(1);
				} else
					goto nnextin;	/* no nonempty match */
			}
		} while (*q++ != 0);
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen > 0 ) {
			patbeg = (char *) p;
			return(1);
		}
	nnextin:
		s = 2;
		if (f->reset) {
			for (i = 2; i <= f->curstat; i++)
				xfree(f->posns[i]);
			k = *f->posns[0];			
			if ((f->posns[2] = (int *) calloc(1, (k+1)*sizeof(int))) == NULL)
				overflo("out of state space");
			for (i = 0; i <= k; i++)
				(f->posns[2])[i] = (f->posns[0])[i];
			f->initstat = f->curstat = 2;
			f->out[2] = f->out[0];
			for (i = 0; i < NCHARS; i++)
				f->gototab[2][i] = 0;
		}
		p++;
	}
	return (0);
}

Node *reparse(const char *p)	/* parses regular expression pointed to by p */
{			/* uses relex() to scan regular expression */
	Node *np;

	dprintf( ("reparse <%s>\n", p) );
	lastre = prestr = (uschar *) p;	/* prestr points to string to be parsed */
	rtok = relex();
	/* GNU compatibility: an empty regexp matches anything */
	if (rtok == '\0') {
		/* FATAL("empty regular expression"); previous */
		return(op2(EMPTYRE, NIL, NIL));
	}
	np = regexp();
	if (rtok != '\0')
		FATAL("syntax error in regular expression %s at %s", lastre, prestr);
	return(np);
}

Node *regexp(void)	/* top-level parse of reg expr */
{
	return (alt(concat(primary())));
}

Node *primary(void)
{
	Node *np;

	switch (rtok) {
	case CHAR:
		np = op2(CHAR, NIL, itonp(rlxval));
		rtok = relex();
		return (unary(np));
	case ALL:
		rtok = relex();
		return (unary(op2(ALL, NIL, NIL)));
	case EMPTYRE:
		rtok = relex();
		return (unary(op2(ALL, NIL, NIL)));
	case DOT:
		rtok = relex();
		return (unary(op2(DOT, NIL, NIL)));
	case CCL:
		np = op2(CCL, NIL, (Node*) cclenter((char *) rlxstr));
		rtok = relex();
		return (unary(np));
	case NCCL:
		np = op2(NCCL, NIL, (Node *) cclenter((char *) rlxstr));
		rtok = relex();
		return (unary(np));
	case '^':
		rtok = relex();
		return (unary(op2(CHAR, NIL, itonp(HAT))));
	case '$':
		rtok = relex();
		return (unary(op2(CHAR, NIL, NIL)));
	case '(':
		rtok = relex();
		if (rtok == ')') {	/* special pleading for () */
			rtok = relex();
			return unary(op2(CCL, NIL, (Node *) tostring("")));
		}
		np = regexp();
		if (rtok == ')') {
			rtok = relex();
			return (unary(np));
		}
		else
			FATAL("syntax error in regular expression %s at %s", lastre, prestr);
	default:
		FATAL("illegal primary in regular expression %s at %s", lastre, prestr);
	}
	return 0;	/*NOTREACHED*/
}

Node *concat(Node *np)
{
	switch (rtok) {
	case CHAR: case DOT: case ALL: case EMPTYRE: case CCL: case NCCL: case '$': case '(':
		return (concat(op2(CAT, np, primary())));
	}
	return (np);
}

Node *alt(Node *np)
{
	if (rtok == OR) {
		rtok = relex();
		return (alt(op2(OR, np, concat(primary()))));
	}
	return (np);
}

Node *unary(Node *np)
{
	switch (rtok) {
	case STAR:
		rtok = relex();
		return (unary(op2(STAR, np, NIL)));
	case PLUS:
		rtok = relex();
		return (unary(op2(PLUS, np, NIL)));
	case QUEST:
		rtok = relex();
		return (unary(op2(QUEST, np, NIL)));
	default:
		return (np);
	}
}

/*
 * Character class definitions conformant to the POSIX locale as
 * defined in IEEE P1003.1 draft 7 of June 2001, assuming the source
 * and operating character sets are both ASCII (ISO646) or supersets
 * thereof.
 *
 * Note that to avoid overflowing the temporary buffer used in
 * relex(), the expanded character class (prior to range expansion)
 * must be less than twice the size of their full name.
 */

/* Because isblank doesn't show up in any of the header files on any
 * system i use, it's defined here.  if some other locale has a richer
 * definition of "blank", define HAS_ISBLANK and provide your own
 * version.
 * the parentheses here are an attempt to find a path through the maze
 * of macro definition and/or function and/or version provided.  thanks
 * to nelson beebe for the suggestion; let's see if it works everywhere.
 */

/* #define HAS_ISBLANK */
#ifndef HAS_ISBLANK

int (xisblank)(int c)
{
	return c==' ' || c=='\t';
}

#endif

struct charclass {
	const char *cc_name;
	int cc_namelen;
	int (*cc_func)(int);
} charclasses[] = {
	{ "alnum",	5,	isalnum },
	{ "alpha",	5,	isalpha },
#ifndef HAS_ISBLANK
	{ "blank",	5,	isspace }, /* was isblank */
#else
	{ "blank",	5,	isblank },
#endif
	{ "cntrl",	5,	iscntrl },
	{ "digit",	5,	isdigit },
	{ "graph",	5,	isgraph },
	{ "lower",	5,	islower },
	{ "print",	5,	isprint },
	{ "punct",	5,	ispunct },
	{ "space",	5,	isspace },
	{ "upper",	5,	isupper },
	{ "xdigit",	6,	isxdigit },
	{ NULL,		0,	NULL },
};


int relex(void)		/* lexical analyzer for reparse */
{
	int c, n;
	int cflag;
	static uschar *buf = 0;
	static int bufsz = 100;
	uschar *bp;
	struct charclass *cc;
	int i;

	switch (c = *prestr++) {
	case '|': return OR;
	case '*': return STAR;
	case '+': return PLUS;
	case '?': return QUEST;
	case '.': return DOT;
	case '\0': prestr--; return '\0';
	case '^':
	case '$':
	case '(':
	case ')':
		return c;
	case '\\':
		rlxval = quoted(&prestr);
		return CHAR;
	default:
		rlxval = c;
		return CHAR;
	case '[': 
		if (buf == 0 && (buf = (uschar *) malloc(bufsz)) == NULL)
			FATAL("out of space in reg expr %.10s..", lastre);
		bp = buf;
		if (*prestr == '^') {
			cflag = 1;
			prestr++;
		}
		else
			cflag = 0;
		n = 2 * strlen((const char *) prestr)+1;
		if (!adjbuf((char **) &buf, &bufsz, n, n, (char **) &bp, "relex1"))
			FATAL("out of space for reg expr %.10s...", lastre);
		for (; ; ) {
			if ((c = *prestr++) == '\\') {
				*bp++ = '\\';
				if ((c = *prestr++) == '\0')
					FATAL("nonterminated character class %.20s...", lastre);
				*bp++ = c;
			/* } else if (c == '\n') { */
			/* 	FATAL("newline in character class %.20s...", lastre); */
			} else if (c == '[' && *prestr == ':') {
				/* POSIX char class names, Dag-Erling Smorgrav, des@ofug.org */
				for (cc = charclasses; cc->cc_name; cc++)
					if (strncmp((const char *) prestr + 1, (const char *) cc->cc_name, cc->cc_namelen) == 0)
						break;
				if (cc->cc_name != NULL && prestr[1 + cc->cc_namelen] == ':' &&
				    prestr[2 + cc->cc_namelen] == ']') {
					prestr += cc->cc_namelen + 3;
					for (i = 0; i < NCHARS; i++) {
						if (!adjbuf((char **) &buf, &bufsz, bp-buf+1, 100, (char **) &bp, "relex2"))
						    FATAL("out of space for reg expr %.10s...", lastre);
						if (cc->cc_func(i)) {
							*bp++ = i;
							n++;
						}
					}
				} else
					*bp++ = c;
			} else if (c == '\0') {
				FATAL("nonterminated character class %.20s", lastre);
			} else if (bp == buf) {	/* 1st char is special */
				*bp++ = c;
			} else if (c == ']') {
				*bp++ = 0;
				rlxstr = (uschar *) tostring((char *) buf);
				if (cflag == 0)
					return CCL;
				else
					return NCCL;
			} else
				*bp++ = c;
		}
	}
}

int cgoto(fa *f, int s, int c)
{
	int i, j, k;
	int *p, *q;

	assert(c == HAT || c < NCHARS);
	while (f->accept >= maxsetvec) {	/* guessing here! */
		maxsetvec *= 4;
		setvec = (int *) realloc(setvec, maxsetvec * sizeof(int));
		tmpset = (int *) realloc(tmpset, maxsetvec * sizeof(int));
		if (setvec == 0 || tmpset == 0)
			overflo("out of space in cgoto()");
	}
	for (i = 0; i <= f->accept; i++)
		setvec[i] = 0;
	setcnt = 0;
	/* compute positions of gototab[s,c] into setvec */
	p = f->posns[s];
	for (i = 1; i <= *p; i++) {
		if ((k = f->re[p[i]].ltype) != FINAL) {
			if ((k == CHAR && c == ptoi(f->re[p[i]].lval.np))
			 || (k == DOT && c != 0 && c != HAT)
			 || (k == ALL && c != 0)
			 || (k == EMPTYRE && c != 0)
			 || (k == CCL && member(c, (char *) f->re[p[i]].lval.up))
			 || (k == NCCL && !member(c, (char *) f->re[p[i]].lval.up) && c != 0 && c != HAT)) {
				q = f->re[p[i]].lfollow;
				for (j = 1; j <= *q; j++) {
					if (q[j] >= maxsetvec) {
						maxsetvec *= 4;
						setvec = (int *) realloc(setvec, maxsetvec * sizeof(int));
						tmpset = (int *) realloc(tmpset, maxsetvec * sizeof(int));
						if (setvec == 0 || tmpset == 0)
							overflo("cgoto overflow");
					}
					if (setvec[q[j]] == 0) {
						setcnt++;
						setvec[q[j]] = 1;
					}
				}
			}
		}
	}
	/* determine if setvec is a previous state */
	tmpset[0] = setcnt;
	j = 1;
	for (i = f->accept; i >= 0; i--)
		if (setvec[i]) {
			tmpset[j++] = i;
		}
	/* tmpset == previous state? */
	for (i = 1; i <= f->curstat; i++) {
		p = f->posns[i];
		if ((k = tmpset[0]) != p[0])
			goto different;
		for (j = 1; j <= k; j++)
			if (tmpset[j] != p[j])
				goto different;
		/* setvec is state i */
		f->gototab[s][c] = i;
		return i;
	  different:;
	}

	/* add tmpset to current set of states */
	if (f->curstat >= NSTATES-1) {
		f->curstat = 2;
		f->reset = 1;
		for (i = 2; i < NSTATES; i++)
			xfree(f->posns[i]);
	} else
		++(f->curstat);
	for (i = 0; i < NCHARS; i++)
		f->gototab[f->curstat][i] = 0;
	xfree(f->posns[f->curstat]);
	if ((p = (int *) calloc(1, (setcnt+1)*sizeof(int))) == NULL)
		overflo("out of space in cgoto");

	f->posns[f->curstat] = p;
	f->gototab[s][c] = f->curstat;
	for (i = 0; i <= setcnt; i++)
		p[i] = tmpset[i];
	if (setvec[f->accept])
		f->out[f->curstat] = 1;
	else
		f->out[f->curstat] = 0;
	return f->curstat;
}


void freefa(fa *f)	/* free a finite automaton */
{
	int i;

	if (f == NULL)
		return;
	for (i = 0; i <= f->curstat; i++)
		xfree(f->posns[i]);
	for (i = 0; i <= f->accept; i++) {
		xfree(f->re[i].lfollow);
		if (f->re[i].ltype == CCL || f->re[i].ltype == NCCL)
			xfree((f->re[i].lval.np));
	}
	xfree(f->restr);
	xfree(f);
}

/*** parse.c ***/

Node *nodealloc(int n)
{
	Node *x;

	x = (Node *) malloc(sizeof(Node) + (n-1)*sizeof(Node *));
	if (x == NULL)
		FATAL("out of space in nodealloc");
	x->nnext = NULL;
	x->lineno = lineno;
	return(x);
}

Node *exptostat(Node *a)
{
	a->ntype = NSTAT;
	return(a);
}

Node *node1(int a, Node *b)
{
	Node *x;

	x = nodealloc(1);
	x->nobj = a;
	x->narg[0]=b;
	return(x);
}

Node *node2(int a, Node *b, Node *c)
{
	Node *x;

	x = nodealloc(2);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	return(x);
}

Node *node3(int a, Node *b, Node *c, Node *d)
{
	Node *x;

	x = nodealloc(3);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	x->narg[2] = d;
	return(x);
}

Node *node4(int a, Node *b, Node *c, Node *d, Node *e)
{
	Node *x;

	x = nodealloc(4);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	x->narg[2] = d;
	x->narg[3] = e;
	return(x);
}

Node *stat1(int a, Node *b)
{
	Node *x;

	x = node1(a,b);
	x->ntype = NSTAT;
	return(x);
}

Node *stat2(int a, Node *b, Node *c)
{
	Node *x;

	x = node2(a,b,c);
	x->ntype = NSTAT;
	return(x);
}

Node *stat3(int a, Node *b, Node *c, Node *d)
{
	Node *x;

	x = node3(a,b,c,d);
	x->ntype = NSTAT;
	return(x);
}

Node *stat4(int a, Node *b, Node *c, Node *d, Node *e)
{
	Node *x;

	x = node4(a,b,c,d,e);
	x->ntype = NSTAT;
	return(x);
}

Node *op1(int a, Node *b)
{
	Node *x;

	x = node1(a,b);
	x->ntype = NEXPR;
	return(x);
}

Node *op2(int a, Node *b, Node *c)
{
	Node *x;

	x = node2(a,b,c);
	x->ntype = NEXPR;
	return(x);
}

Node *op3(int a, Node *b, Node *c, Node *d)
{
	Node *x;

	x = node3(a,b,c,d);
	x->ntype = NEXPR;
	return(x);
}

Node *op4(int a, Node *b, Node *c, Node *d, Node *e)
{
	Node *x;

	x = node4(a,b,c,d,e);
	x->ntype = NEXPR;
	return(x);
}

Node *celltonode(Cell *a, int b)
{
	Node *x;

	a->ctype = OCELL;
	a->csub = b;
	x = node1(0, (Node *) a);
	x->ntype = NVALUE;
	return(x);
}

Node *rectonode(void)	/* make $0 into a Node */
{
	extern Cell *literal0;
	return op1(INDIRECT, celltonode(literal0, CUNK));
}

Node *makearr(Node *p)
{
	Cell *cp;

	if (isvalue(p)) {
		cp = (Cell *) (p->narg[0]);
		if (isfcn(cp))
			SYNTAX( "%s is a function, not an array", cp->nval );
		else if (!isarr(cp)) {
			xfree(cp->sval);
			cp->sval = (char *) makesymtab(NSYMTAB);
			cp->tval = ARR;
		}
	}
	return p;
}

#define PA2NUM	50	/* max number of pat,pat patterns allowed */
int	paircnt;		/* number of them in use */
int	pairstack[PA2NUM];	/* state of each pat,pat */

Node *pa2stat(Node *a, Node *b, Node *c)	/* pat, pat {...} */
{
	Node *x;

	x = node4(PASTAT2, a, b, c, itonp(paircnt));
	if (paircnt++ >= PA2NUM)
		SYNTAX( "limited to %d pat,pat statements", PA2NUM );
	x->ntype = NSTAT;
	return(x);
}

Node *linkum(Node *a, Node *b)
{
	Node *c;

	if (errorflag)	/* don't link things that are wrong */
		return a;
	if (a == NULL)
		return(b);
	else if (b == NULL)
		return(a);
	for (c = a; c->nnext != NULL; c = c->nnext)
		;
	c->nnext = b;
	return(a);
}

void defn(Cell *v, Node *vl, Node *st)	/* turn on FCN bit in definition, */
{					/*   body of function, arglist */
	Node *p;
	int n;

	if (isarr(v)) {
		SYNTAX( "`%s' is an array name and a function name", v->nval );
		return;
	}
	if (isarg(v->nval) != -1) {
		SYNTAX( "`%s' is both function name and argument name", v->nval );
		return;
	}

	v->tval = FCN;
	v->sval = (char *) st;
	n = 0;	/* count arguments */
	for (p = vl; p; p = p->nnext)
		n++;
	v->fval = n;
	dprintf( ("defining func %s (%d args)\n", v->nval, n) );
}

int isarg(const char *s)		/* is s in argument list for current function? */
{			/* return -1 if not, otherwise arg # */
	extern Node *arglist;
	Node *p = arglist;
	int n;

	for (n = 0; p != 0; p = p->nnext, n++)
		if (strcmp(((Cell *)(p->narg[0]))->nval, s) == 0)
			return n;
	return -1;
}

int ptoi(void *p)	/* convert pointer to integer */
{
	return (int) (long) p;	/* swearing that p fits, of course */
}

Node *itonp(int i)	/* and vice versa */
{
	return (Node *) (long) i;
}


/*** proctab.c ***/

static char *printname[93] = {
	(char *) "FIRSTTOKEN",	/* 258 */
	(char *) "PROGRAM",	/* 259 */
	(char *) "PASTAT",	/* 260 */
	(char *) "PASTAT2",	/* 261 */
	(char *) "XBEGIN",	/* 262 */
	(char *) "XEND",	/* 263 */
	(char *) "NL",	/* 264 */
	(char *) "ARRAY",	/* 265 */
	(char *) "MATCH",	/* 266 */
	(char *) "NOTMATCH",	/* 267 */
	(char *) "MATCHOP",	/* 268 */
	(char *) "FINAL",	/* 269 */
	(char *) "DOT",	/* 270 */
	(char *) "ALL",	/* 271 */
	(char *) "CCL",	/* 272 */
	(char *) "NCCL",	/* 273 */
	(char *) "CHAR",	/* 274 */
	(char *) "OR",	/* 275 */
	(char *) "STAR",	/* 276 */
	(char *) "QUEST",	/* 277 */
	(char *) "PLUS",	/* 278 */
	(char *) "EMPTYRE",	/* 279 */
	(char *) "AND",	/* 280 */
	(char *) "BOR",	/* 281 */
	(char *) "APPEND",	/* 282 */
	(char *) "EQ",	/* 283 */
	(char *) "GE",	/* 284 */
	(char *) "GT",	/* 285 */
	(char *) "LE",	/* 286 */
	(char *) "LT",	/* 287 */
	(char *) "NE",	/* 288 */
	(char *) "IN",	/* 289 */
	(char *) "ARG",	/* 290 */
	(char *) "BLTIN",	/* 291 */
	(char *) "BREAK",	/* 292 */
	(char *) "CLOSE",	/* 293 */
	(char *) "CONTINUE",	/* 294 */
	(char *) "DELETE",	/* 295 */
	(char *) "DO",	/* 296 */
	(char *) "EXIT",	/* 297 */
	(char *) "FOR",	/* 298 */
	(char *) "FUNC",	/* 299 */
	(char *) "SUB",	/* 300 */
	(char *) "GSUB",	/* 301 */
	(char *) "IF",	/* 302 */
	(char *) "INDEX",	/* 303 */
	(char *) "LSUBSTR",	/* 304 */
	(char *) "MATCHFCN",	/* 305 */
	(char *) "NEXT",	/* 306 */
	(char *) "NEXTFILE",	/* 307 */
	(char *) "ADD",	/* 308 */
	(char *) "MINUS",	/* 309 */
	(char *) "MULT",	/* 310 */
	(char *) "DIVIDE",	/* 311 */
	(char *) "MOD",	/* 312 */
	(char *) "ASSIGN",	/* 313 */
	(char *) "ASGNOP",	/* 314 */
	(char *) "ADDEQ",	/* 315 */
	(char *) "SUBEQ",	/* 316 */
	(char *) "MULTEQ",	/* 317 */
	(char *) "DIVEQ",	/* 318 */
	(char *) "MODEQ",	/* 319 */
	(char *) "POWEQ",	/* 320 */
	(char *) "PRINT",	/* 321 */
	(char *) "PRINTF",	/* 322 */
	(char *) "SPRINTF",	/* 323 */
	(char *) "ELSE",	/* 324 */
	(char *) "INTEST",	/* 325 */
	(char *) "CONDEXPR",	/* 326 */
	(char *) "POSTINCR",	/* 327 */
	(char *) "PREINCR",	/* 328 */
	(char *) "POSTDECR",	/* 329 */
	(char *) "PREDECR",	/* 330 */
	(char *) "VAR",	/* 331 */
	(char *) "IVAR",	/* 332 */
	(char *) "VARNF",	/* 333 */
	(char *) "CALL",	/* 334 */
	(char *) "NUMBER",	/* 335 */
	(char *) "STRING",	/* 336 */
	(char *) "REGEXPR",	/* 337 */
	(char *) "GETLINE",	/* 338 */
	(char *) "RETURN",	/* 339 */
	(char *) "SPLIT",	/* 340 */
	(char *) "SUBSTR",	/* 341 */
	(char *) "WHILE",	/* 342 */
	(char *) "CAT",	/* 343 */
	(char *) "NOT",	/* 344 */
	(char *) "UMINUS",	/* 345 */
	(char *) "POWER",	/* 346 */
	(char *) "DECR",	/* 347 */
	(char *) "INCR",	/* 348 */
	(char *) "INDIRECT",	/* 349 */
	(char *) "LASTTOKEN",	/* 350 */
};


Cell *(*proctab[93])(Node **, int) = {
	nullproc,	/* FIRSTTOKEN */
	program,	/* PROGRAM */
	pastat,	/* PASTAT */
	dopa2,	/* PASTAT2 */
	nullproc,	/* XBEGIN */
	nullproc,	/* XEND */
	nullproc,	/* NL */
	array,	/* ARRAY */
	matchop,	/* MATCH */
	matchop,	/* NOTMATCH */
	nullproc,	/* MATCHOP */
	nullproc,	/* FINAL */
	nullproc,	/* DOT */
	nullproc,	/* ALL */
	nullproc,	/* CCL */
	nullproc,	/* NCCL */
	nullproc,	/* CHAR */
	nullproc,	/* OR */
	nullproc,	/* STAR */
	nullproc,	/* QUEST */
	nullproc,	/* PLUS */
	nullproc,	/* EMPTYRE */
	boolop,	/* AND */
	boolop,	/* BOR */
	nullproc,	/* APPEND */
	relop,	/* EQ */
	relop,	/* GE */
	relop,	/* GT */
	relop,	/* LE */
	relop,	/* LT */
	relop,	/* NE */
	instat,	/* IN */
	arg,	/* ARG */
	bltin,	/* BLTIN */
	jump,	/* BREAK */
	closefile,	/* CLOSE */
	jump,	/* CONTINUE */
	awkdelete,	/* DELETE */
	dostat,	/* DO */
	jump,	/* EXIT */
	forstat,	/* FOR */
	nullproc,	/* FUNC */
	sub,	/* SUB */
	gsub,	/* GSUB */
	ifstat,	/* IF */
	sindex,	/* INDEX */
	nullproc,	/* LSUBSTR */
	matchop,	/* MATCHFCN */
	jump,	/* NEXT */
	jump,	/* NEXTFILE */
	arith,	/* ADD */
	arith,	/* MINUS */
	arith,	/* MULT */
	arith,	/* DIVIDE */
	arith,	/* MOD */
	assign,	/* ASSIGN */
	nullproc,	/* ASGNOP */
	assign,	/* ADDEQ */
	assign,	/* SUBEQ */
	assign,	/* MULTEQ */
	assign,	/* DIVEQ */
	assign,	/* MODEQ */
	assign,	/* POWEQ */
	printstat,	/* PRINT */
	awkprintf,	/* PRINTF */
	awksprintf,	/* SPRINTF */
	nullproc,	/* ELSE */
	intest,	/* INTEST */
	condexpr,	/* CONDEXPR */
	incrdecr,	/* POSTINCR */
	incrdecr,	/* PREINCR */
	incrdecr,	/* POSTDECR */
	incrdecr,	/* PREDECR */
	nullproc,	/* VAR */
	nullproc,	/* IVAR */
	getnf,	/* VARNF */
	call,	/* CALL */
	nullproc,	/* NUMBER */
	nullproc,	/* STRING */
	nullproc,	/* REGEXPR */
	awkgetline,	/* GETLINE */
	jump,	/* RETURN */
	split,	/* SPLIT */
	substr,	/* SUBSTR */
	whilestat,	/* WHILE */
	cat,	/* CAT */
	boolop,	/* NOT */
	arith,	/* UMINUS */
	arith,	/* POWER */
	nullproc,	/* DECR */
	nullproc,	/* INCR */
	indirect,	/* INDIRECT */
	nullproc,	/* LASTTOKEN */
};

char *tokname(int n)
{
	static char buf[100];

	if (n < FIRSTTOKEN || n > LASTTOKEN) {
		sprintf(buf, "token %d", n);
		return buf;
	}
	return printname[n-FIRSTTOKEN];
}

/*** tran.c ***/

#define	FULLTAB	2	/* rehash when table gets this x full */
#define	GROWTAB 4	/* grow table by this factor */

Array	*symtab;	/* main symbol table */

char	**FS;		/* initial field sep */
char	**RS;		/* initial record sep */
char	**OFS;		/* output field sep */
char	**ORS;		/* output record sep */
char	**OFMT;		/* output format for numbers */
char	**CONVFMT;	/* format for conversions in getsval */
Awkfloat *NF;		/* number of fields in current record */
Awkfloat *NR;		/* number of current record */
Awkfloat *FNR;		/* number of current record in current file */
char	**FILENAME;	/* current filename argument */
Awkfloat *ARGC;		/* number of arguments from command line */
char	**SUBSEP;	/* subscript separator for a[i,j,k]; default \034 */
Awkfloat *RSTART;	/* start of re matched with ~; origin 1 (!) */
Awkfloat *RLENGTH;	/* length of same */

Cell	*fsloc;		/* FS */
Cell	*nrloc;		/* NR */
Cell	*nfloc;		/* NF */
Cell	*fnrloc;	/* FNR */
Array	*ARGVtab;	/* symbol table containing ARGV[...] */
Array	*ENVtab;	/* symbol table containing ENVIRON[...] */
Cell	*rstartloc;	/* RSTART */
Cell	*rlengthloc;	/* RLENGTH */
Cell	*symtabloc;	/* SYMTAB */

Cell	*nullloc;	/* a guaranteed empty cell */
Node	*nullnode;	/* zero&null, converted into a node for comparisons */
Cell	*literal0;

extern Cell **fldtab;

void syminit(void)	/* initialize symbol table with builtin vars */
{
	literal0 = setsymtab("0", "0", 0.0, NUM|STR|CON|DONTFREE, symtab);
	/* this is used for if(x)... tests: */
	nullloc = setsymtab("$zero&null", "", 0.0, NUM|STR|CON|DONTFREE, symtab);
	nullnode = celltonode(nullloc, CCON);

	fsloc = setsymtab("FS", " ", 0.0, STR|DONTFREE, symtab);
	FS = &fsloc->sval;
	RS = &setsymtab("RS", "\n", 0.0, STR|DONTFREE, symtab)->sval;
	OFS = &setsymtab("OFS", " ", 0.0, STR|DONTFREE, symtab)->sval;
	ORS = &setsymtab("ORS", "\n", 0.0, STR|DONTFREE, symtab)->sval;
	OFMT = &setsymtab("OFMT", "%.6g", 0.0, STR|DONTFREE, symtab)->sval;
	CONVFMT = &setsymtab("CONVFMT", "%.6g", 0.0, STR|DONTFREE, symtab)->sval;
	FILENAME = &setsymtab("FILENAME", "", 0.0, STR|DONTFREE, symtab)->sval;
	nfloc = setsymtab("NF", "", 0.0, NUM, symtab);
	NF = &nfloc->fval;
	nrloc = setsymtab("NR", "", 0.0, NUM, symtab);
	NR = &nrloc->fval;
	fnrloc = setsymtab("FNR", "", 0.0, NUM, symtab);
	FNR = &fnrloc->fval;
	SUBSEP = &setsymtab("SUBSEP", "\034", 0.0, STR|DONTFREE, symtab)->sval;
	rstartloc = setsymtab("RSTART", "", 0.0, NUM, symtab);
	RSTART = &rstartloc->fval;
	rlengthloc = setsymtab("RLENGTH", "", 0.0, NUM, symtab);
	RLENGTH = &rlengthloc->fval;
	symtabloc = setsymtab("SYMTAB", "", 0.0, ARR, symtab);
	symtabloc->sval = (char *) symtab;
}

void arginit(char **args)	/* set up ARGV and ARGC */
{
	Cell *cp;
	int i;
	char temp[50];

	cp = setsymtab("ARGV", "", 0.0, ARR, symtab);
	ARGVtab = makesymtab(NSYMTAB);	/* could be (int) ARGC as well */
	cp->sval = (char *) ARGVtab;
	setsymtab("0", "awk", 0.0, STR, ARGVtab);
	for (i = 0; args[i]; i++) {
		sprintf(temp, "%d", i+1);
		if (is_number(args[i]))
			setsymtab(temp, args[i], atof(args[i]), STR|NUM, ARGVtab);
		else
			setsymtab(temp, args[i], 0.0, STR, ARGVtab);
	}
	ARGC = &setsymtab("ARGC", "", (Awkfloat) (i+1), NUM, symtab)->fval;
}

void envinit(char **envp)	/* set up ENVIRON variable */
{
	Cell *cp;
	char *p;

	cp = setsymtab("ENVIRON", "", 0.0, ARR, symtab);
	ENVtab = makesymtab(NSYMTAB);
	cp->sval = (char *) ENVtab;
	for ( ; *envp; envp++) {
		if ((p = strchr(*envp, '=')) == NULL)
			continue;
		if( p == *envp ) /* no left hand side name in env string */
			continue;
		*p++ = 0;	/* split into two strings at = */
		if (is_number(p))
			setsymtab(*envp, p, atof(p), STR|NUM, ENVtab);
		else
			setsymtab(*envp, p, 0.0, STR, ENVtab);
		p[-1] = '=';	/* restore in case env is passed down to a shell */
	}
}

Array *makesymtab(int n)	/* make a new symbol table */
{
	Array *ap;
	Cell **tp;

	ap = (Array *) malloc(sizeof(Array));
	tp = (Cell **) calloc(n, sizeof(Cell *));
	if (ap == NULL || tp == NULL)
		FATAL("out of space in makesymtab");
	ap->nelem = 0;
	ap->size = n;
	ap->tab = tp;
	return(ap);
}

void freesymtab(Cell *ap)	/* free a symbol table */
{
	Cell *cp, *temp;
	Array *tp;
	int i;

	if (!isarr(ap))
		return;
	tp = (Array *) ap->sval;
	if (tp == NULL)
		return;
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp != NULL; cp = temp) {
			xfree(cp->nval);
			if (freeable(cp))
				xfree(cp->sval);
			temp = cp->cnext;	/* avoids freeing then using */
			free(cp); 
			tp->nelem--;
		}
		tp->tab[i] = 0;
	}
	if (tp->nelem != 0)
		WARNING("can't happen: inconsistent element count freeing %s", ap->nval);
	free(tp->tab);
	free(tp);
}

void freeelem(Cell *ap, const char *s)	/* free elem s from ap (i.e., ap["s"] */
{
	Array *tp;
	Cell *p, *prev = NULL;
	int h;
	
	tp = (Array *) ap->sval;
	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
		if (strcmp(s, p->nval) == 0) {
			if (prev == NULL)	/* 1st one */
				tp->tab[h] = p->cnext;
			else			/* middle somewhere */
				prev->cnext = p->cnext;
			if (freeable(p))
				xfree(p->sval);
			free(p->nval);
			free(p);
			tp->nelem--;
			return;
		}
}

Cell *setsymtab(const char *n, const char *s, Awkfloat f, unsigned t, Array *tp)
{
	int h;
	Cell *p;

	if (n != NULL && (p = lookup(n, tp)) != NULL) {
		   dprintf( ("setsymtab found %p: n=%s s=\"%s\" f=%g t=%o\n",
			(void*)p, NN(p->nval), NN(p->sval), p->fval, p->tval) );
		return(p);
	}
	p = (Cell *) malloc(sizeof(Cell));
	if (p == NULL)
		FATAL("out of space for symbol table at %s", n);
	p->nval = tostring(n);
	p->sval = s ? tostring(s) : tostring("");
	p->fval = f;
	p->tval = t;
	p->csub = CUNK;
	p->ctype = OCELL;
	tp->nelem++;
	if (tp->nelem > FULLTAB * tp->size)
		rehash(tp);
	h = hash(n, tp->size);
	p->cnext = tp->tab[h];
	tp->tab[h] = p;
	   dprintf( ("setsymtab set %p: n=%s s=\"%s\" f=%g t=%o\n",
		(void*)p, p->nval, p->sval, p->fval, p->tval) );
	return(p);
}

int hash(const char *s, int n)	/* form hash value for string s */
{
	unsigned hashval;

	for (hashval = 0; *s != '\0'; s++)
		hashval = (*s + 31 * hashval);
	return hashval % n;
}

void rehash(Array *tp)	/* rehash items in small table into big one */
{
	int i, nh, nsz;
	Cell *cp, *op, **np;

	nsz = GROWTAB * tp->size;
	np = (Cell **) calloc(nsz, sizeof(Cell *));
	if (np == NULL)		/* can't do it, but can keep running. */
		return;		/* someone else will run out later. */
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp; cp = op) {
			op = cp->cnext;
			nh = hash(cp->nval, nsz);
			cp->cnext = np[nh];
			np[nh] = cp;
		}
	}
	free(tp->tab);
	tp->tab = np;
	tp->size = nsz;
}

Cell *lookup(const char *s, Array *tp)	/* look for s in tp */
{
	Cell *p;
	int h;

	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; p = p->cnext)
		if (strcmp(s, p->nval) == 0)
			return(p);	/* found it */
	return(NULL);			/* not found */
}

Awkfloat setfval(Cell *vp, Awkfloat f)	/* set float val of a Cell */
{
	int fldno;

	if ((vp->tval & (NUM | STR)) == 0) 
		funnyvar(vp, "assign to");
	if (isfld(vp)) {
		donerec = 0;	/* mark $0 invalid */
		fldno = atoi(vp->nval);
		if (fldno > *NF)
			newfld(fldno);
		   dprintf( ("setting field %d to %g\n", fldno, f) );
	} else if (isrec(vp)) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	if (freeable(vp))
		xfree(vp->sval); /* free any previous string */
	vp->tval &= ~STR;	/* mark string invalid */
	vp->tval |= NUM;	/* mark number ok */
	if (f == -0)  /* who would have thought this possible? */
		f = 0;
	   dprintf( ("setfval %p: %s = %g, t=%o\n", (void*)vp, NN(vp->nval), f, vp->tval) );
	return vp->fval = f;
}

void funnyvar(Cell *vp, const char *rw)
{
	if (isarr(vp))
		FATAL("can't %s %s; it's an array name.", rw, vp->nval);
	if (vp->tval & FCN)
		FATAL("can't %s %s; it's a function.", rw, vp->nval);
	WARNING("funny variable %p: n=%s s=\"%s\" f=%g t=%o",
		vp, vp->nval, vp->sval, vp->fval, vp->tval);
}

char *setsval(Cell *vp, const char *s)	/* set string val of a Cell */
{
	char *t;
	int fldno;

	   dprintf( ("starting setsval %p: %s = \"%s\", t=%o, r,f=%d,%d\n", 
		(void*)vp, NN(vp->nval), s, vp->tval, donerec, donefld) );
	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "assign to");
	if (isfld(vp)) {
		donerec = 0;	/* mark $0 invalid */
		fldno = atoi(vp->nval);
		if (fldno > *NF)
			newfld(fldno);
		   dprintf( ("setting field %d to %s (%p)\n", fldno, s, s) );
	} else if (isrec(vp)) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	t = tostring(s);	/* in case it's self-assign */
	if (freeable(vp))
		xfree(vp->sval);
	vp->tval &= ~NUM;
	vp->tval |= STR;
	vp->tval &= ~DONTFREE;
	   dprintf( ("setsval %p: %s = \"%s (%p) \", t=%o r,f=%d,%d\n", 
		(void*)vp, NN(vp->nval), t,t, vp->tval, donerec, donefld) );
	return(vp->sval = t);
}

Awkfloat getfval(Cell *vp)	/* get float val of a Cell */
{
	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "read value of");
	if (isfld(vp) && donefld == 0)
		fldbld();
	else if (isrec(vp) && donerec == 0)
		recbld();
	if (!isnum(vp)) {	/* not a number */
		vp->fval = atof(vp->sval);	/* best guess */
		if (is_number(vp->sval) && !(vp->tval&CON))
			vp->tval |= NUM;	/* make NUM only sparingly */
	}
	   dprintf( ("getfval %p: %s = %g, t=%o\n",
		(void*)vp, NN(vp->nval), vp->fval, vp->tval) );
	return(vp->fval);
}

static char *get_str_val(Cell *vp, char **fmt)        /* get string val of a Cell */
{
	char s[100];	/* BUG: unchecked */
	double dtemp;

	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "read value of");
	if (isfld(vp) && donefld == 0)
		fldbld();
	else if (isrec(vp) && donerec == 0)
		recbld();
	if (isstr(vp) == 0) {
		if (freeable(vp))
			xfree(vp->sval);
		if (modf(vp->fval, &dtemp) == 0)	/* it's integral */
			sprintf(s, "%.30g", vp->fval);
		else
			sprintf(s, *fmt, vp->fval);
		vp->sval = tostring(s);
		vp->tval &= ~DONTFREE;
		vp->tval |= STR;
	}
	   dprintf( ("getsval %p: %s = \"%s (%p)\", t=%o\n",
		(void*)vp, NN(vp->nval), vp->sval, vp->sval, vp->tval) );
	return(vp->sval);
}

char *getsval(Cell *vp)       /* get string val of a Cell */
{
      return get_str_val(vp, CONVFMT);
}

char *getpssval(Cell *vp)     /* get string val of a Cell for print */
{
      return get_str_val(vp, OFMT);
}


char *tostring(const char *s)	/* make a copy of string s */
{
	char *p;

	p = (char *) malloc(strlen(s)+1);
	if (p == NULL)
		FATAL("out of space in tostring on %s", s);
	strcpy(p, s);
	return(p);
}

char *qstring(const char *is, int delim)	/* collect string up to next delim */
{
	const char *os = is;
	int c, n;
	uschar *s = (uschar *) is;
	uschar *buf, *bp;

	if ((buf = (uschar *) malloc(strlen(is)+3)) == NULL)
		FATAL( "out of space in qstring(%s)", s);
	for (bp = buf; (c = *s) != delim; s++) {
		if (c == '\n')
			SYNTAX( "newline in string %.20s...", os );
		else if (c != '\\')
			*bp++ = c;
		else {	/* \something */
			c = *++s;
			if (c == 0) {	/* \ at end */
				*bp++ = '\\';
				break;	/* for loop */
			}	
			switch (c) {
			case '\\':	*bp++ = '\\'; break;
			case 'n':	*bp++ = '\n'; break;
			case 't':	*bp++ = '\t'; break;
			case 'b':	*bp++ = '\b'; break;
			case 'f':	*bp++ = '\f'; break;
			case 'r':	*bp++ = '\r'; break;
			default:
				if (!isdigit(c)) {
					*bp++ = c;
					break;
				}
				n = c - '0';
				if (isdigit(s[1])) {
					n = 8 * n + *++s - '0';
					if (isdigit(s[1]))
						n = 8 * n + *++s - '0';
				}
				*bp++ = n;
				break;
			}
		}
	}
	*bp++ = 0;
	return (char *) buf;
}

/*** lib.c ***/

FILE	*infile	= NULL;
char	*file	= "";
char	*record;
int	recsize	= RECSIZE;
char	*fields;
int	fieldssize = RECSIZE;

Cell	**fldtab;	/* pointers to Cells */
char	inputFS[100] = " ";

#define	MAXFLD	2
int	nfields	= MAXFLD;	/* last allocated slot for $i */

int	donefld;	/* 1 = implies rec broken into fields */
int	donerec;	/* 1 = record is valid (no flds have changed) */

int	lastfld	= 0;	/* last used field */
int	argno	= 1;	/* current input argument number */
extern	Awkfloat *ARGC;

static Cell dollar0 = { OCELL, CFLD, NULL, "", 0.0, REC|STR|DONTFREE, NULL };
static Cell dollar1 = { OCELL, CFLD, NULL, "", 0.0, FLD|STR|DONTFREE, NULL };

void recinit(unsigned int n)
{
	if ( (record = (char *) malloc(n)) == NULL
	  || (fields = (char *) malloc(n+1)) == NULL
	  || (fldtab = (Cell **) malloc((nfields+1) * sizeof(Cell *))) == NULL
	  || (fldtab[0] = (Cell *) malloc(sizeof(Cell))) == NULL )
		FATAL("out of space for $0 and fields");
	*fldtab[0] = dollar0;
	fldtab[0]->sval = record;
	fldtab[0]->nval = tostring("0");
	makefields(1, nfields);
}

void makefields(int n1, int n2)		/* create $n1..$n2 inclusive */
{
	char temp[50];
	int i;

	for (i = n1; i <= n2; i++) {
		fldtab[i] = (Cell *) malloc(sizeof (struct Cell));
		if (fldtab[i] == NULL)
			FATAL("out of space in makefields %d", i);
		*fldtab[i] = dollar1;
		sprintf(temp, "%d", i);
		fldtab[i]->nval = tostring(temp);
	}
}

void initgetrec(void)
{
	int i;
	char *p;

	for (i = 1; i < *ARGC; i++) {
		p = getargv(i); /* find 1st real filename */
		if (p == NULL || *p == '\0') {  /* deleted or zapped */
			argno++;
			continue;
		}
		if (!isclvar(p)) {
			setsval(lookup("FILENAME", symtab), p);
			return;
		}
		setclvar(p);	/* a commandline assignment before filename */
		argno++;
	}
	infile = stdin;		/* no filenames, so use stdin */
}

static int firsttime = 1;

int getrec(char **pbuf, int *pbufsize, int isrecord)	/* get next input record */
{			/* note: cares whether buf == record */
	int c;
	char *buf = *pbuf;
	uschar saveb0;
	int bufsize = *pbufsize, savebufsize = bufsize;

	if (firsttime) {
		firsttime = 0;
		initgetrec();
	}
	   dprintf( ("RS=<%s>, FS=<%s>, ARGC=%g, FILENAME=%s\n",
		*RS, *FS, *ARGC, *FILENAME) );
	if (isrecord) {
		donefld = 0;
		donerec = 1;
	}
	saveb0 = buf[0];
	buf[0] = 0;
	while (argno < *ARGC || infile == stdin) {
		   dprintf( ("argno=%d, file=|%s|\n", argno, file) );
		if (infile == NULL) {	/* have to open a new file */
			file = getargv(argno);
			if (file == NULL || *file == '\0') {	/* deleted or zapped */
				argno++;
				continue;
			}
			if (isclvar(file)) {	/* a var=value arg */
				setclvar(file);
				argno++;
				continue;
			}
			*FILENAME = file;
			   dprintf( ("opening file %s\n", file) );
			if (*file == '-' && *(file+1) == '\0')
				infile = stdin;
			else if ((infile = fopen(file, "r")) == NULL)
				FATAL("can't open file %s", file);
			setfval(fnrloc, 0.0);
		}
		c = readrec(&buf, &bufsize, infile);
		if (c != 0 || buf[0] != '\0') {	/* normal record */
			if (isrecord) {
				if (freeable(fldtab[0]))
					xfree(fldtab[0]->sval);
				fldtab[0]->sval = buf;	/* buf == record */
				fldtab[0]->tval = REC | STR | DONTFREE;
				if (is_number(fldtab[0]->sval)) {
					fldtab[0]->fval = atof(fldtab[0]->sval);
					fldtab[0]->tval |= NUM;
				}
			}
			setfval(nrloc, nrloc->fval+1);
			setfval(fnrloc, fnrloc->fval+1);
			*pbuf = buf;
			*pbufsize = bufsize;
			return 1;
		}
		/* EOF arrived on this file; set up next */
		if (infile != stdin)
			fclose(infile);
		infile = NULL;
		argno++;
	}
	buf[0] = saveb0;
	*pbuf = buf;
	*pbufsize = savebufsize;
	return 0;	/* true end of file */
}

void nextfile(void)
{
	if (infile != NULL && infile != stdin)
		fclose(infile);
	infile = NULL;
	argno++;
}

int readrec(char **pbuf, int *pbufsize, FILE *inf)	/* read one record into buf */
{
	int sep, c;
	char *rr, *buf = *pbuf;
	int bufsize = *pbufsize;

	if (strlen(*FS) >= sizeof(inputFS))
		FATAL("field separator %.10s... is too long", *FS);
	/*fflush(stdout); avoids some buffering problem but makes it 25% slower*/
	strcpy(inputFS, *FS);	/* for subsequent field splitting */
	if ((sep = **RS) == 0) {
		sep = '\n';
		while ((c=getc(inf)) == '\n' && c != EOF)	/* skip leading \n's */
			;
		if (c != EOF)
			ungetc(c, inf);
	}
	for (rr = buf; ; ) {
		for (; (c=getc(inf)) != sep && c != EOF; ) {
			if (rr-buf+1 > bufsize)
				if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readrec 1"))
					FATAL("input record `%.30s...' too long", buf);
			*rr++ = c;
		}
		if (**RS == sep || c == EOF)
			break;
		if ((c = getc(inf)) == '\n' || c == EOF) /* 2 in a row */
			break;
		if (!adjbuf(&buf, &bufsize, 2+rr-buf, recsize, &rr, "readrec 2"))
			FATAL("input record `%.30s...' too long", buf);
		*rr++ = '\n';
		*rr++ = c;
	}
	if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readrec 3"))
		FATAL("input record `%.30s...' too long", buf);
	*rr = 0;
	   dprintf( ("readrec saw <%s>, returns %d\n", buf, c == EOF && rr == buf ? 0 : 1) );
	*pbuf = buf;
	*pbufsize = bufsize;
	return c == EOF && rr == buf ? 0 : 1;
}

char *getargv(int n)	/* get ARGV[n] */
{
	Cell *x;
	char *s, temp[50];
	extern Array *ARGVtab;

	sprintf(temp, "%d", n);
	if (lookup(temp, ARGVtab) == NULL)
		return NULL;
	x = setsymtab(temp, "", 0.0, STR, ARGVtab);
	s = getsval(x);
	   dprintf( ("getargv(%d) returns |%s|\n", n, s) );
	return s;
}

void setclvar(char *s)	/* set var=value from s */
{
	char *p;
	Cell *q;

	for (p=s; *p != '='; p++)
		;
	*p++ = 0;
	p = qstring(p, '\0');
	q = setsymtab(s, p, 0.0, STR, symtab);
	setsval(q, p);
	if (is_number(q->sval)) {
		q->fval = atof(q->sval);
		q->tval |= NUM;
	}
	   dprintf( ("command line set %s to |%s|\n", s, p) );
}


void fldbld(void)	/* create fields from current record */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	/* possibly with a final trailing \0 not associated with any field */
	char *r, *fr, sep;
	Cell *p;
	int i, j, n;

	if (donefld)
		return;
	if (!isstr(fldtab[0]))
		getsval(fldtab[0]);
	r = fldtab[0]->sval;
	n = strlen(r);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+2)) == NULL) /* possibly 2 final \0s */
			FATAL("out of space for fields in fldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	i = 0;	/* number of fields accumulated here */
	strcpy(inputFS, *FS);
	if (strlen(inputFS) > 1) {	/* it's a regular expression */
		i = refldbld(r, inputFS);
	} else if ((sep = *inputFS) == ' ') {	/* default whitespace */
		for (i = 0; ; ) {
			while (*r == ' ' || *r == '\t' || *r == '\n')
				r++;
			if (*r == 0)
				break;
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			do
				*fr++ = *r++;
			while (*r != ' ' && *r != '\t' && *r != '\n' && *r != '\0');
			*fr++ = 0;
		}
		*fr = 0;
	} else if ((sep = *inputFS) == 0) {		/* new: FS="" => 1 char/field */
		for (i = 0; *r != 0; r++) {
			char buf[2];
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			buf[0] = *r;
			buf[1] = 0;
			fldtab[i]->sval = tostring(buf);
			fldtab[i]->tval = FLD | STR;
		}
		*fr = 0;
	} else if (*r != 0) {	/* if 0, it's a null field */
		/* subtlecase : if length(FS) == 1 && length(RS > 0)
		 * \n is NOT a field separator (cf awk book 61,84).
		 * this variable is tested in the inner while loop.
		 */
		int rtest = '\n';  /* normal case */
		if (strlen(*RS) > 0)
			rtest = '\0';
		for (;;) {
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			while (*r != sep && *r != rtest && *r != '\0')	/* \n is always a separator */
				*fr++ = *r++;
			*fr++ = 0;
			if (*r++ == 0)
				break;
		}
		*fr = 0;
	}
	if (i > nfields)
		FATAL("record `%.30s...' has too many fields; can't happen", r);
	cleanfld(i+1, lastfld);	/* clean out junk from previous record */
	lastfld = i;
	donefld = 1;
	for (j = 1; j <= lastfld; j++) {
		p = fldtab[j];
		if(is_number(p->sval)) {
			p->fval = atof(p->sval);
			p->tval |= NUM;
		}
	}
	setfval(nfloc, (Awkfloat) lastfld);
	if (dbg) {
		for (j = 0; j <= lastfld; j++) {
			p = fldtab[j];
			printf("field %d (%s): |%s|\n", j, p->nval, p->sval);
		}
	}
}

void cleanfld(int n1, int n2)	/* clean out fields n1 .. n2 inclusive */
{				/* nvals remain intact */
	Cell *p;
	int i;

	for (i = n1; i <= n2; i++) {
		p = fldtab[i];
		if (freeable(p))
			xfree(p->sval);
		p->sval = "";
		p->tval = FLD | STR | DONTFREE;
	}
}

void newfld(int n)	/* add field n after end of existing lastfld */
{
	if (n > nfields)
		growfldtab(n);
	cleanfld(lastfld+1, n);
	lastfld = n;
	setfval(nfloc, (Awkfloat) n);
}

Cell *fieldadr(int n)	/* get nth field */
{
	if (n < 0)
		FATAL("trying to access out of range field %d", n);
	if (n > nfields)	/* fields after NF are empty */
		growfldtab(n);	/* but does not increase NF */
	return(fldtab[n]);
}

void growfldtab(int n)	/* make new fields up to at least $n */
{
	int nf = 2 * nfields;
	size_t s;

	if (n > nf)
		nf = n;
	s = (nf+1) * (sizeof (struct Cell *));  /* freebsd: how much do we need? */
	if (s / sizeof(struct Cell *) - 1 == nf) /* didn't overflow */
		fldtab = (Cell **) realloc(fldtab, s);
	else					/* overflow sizeof int */
		xfree(fldtab);	/* make it null */
	if (fldtab == NULL)
		FATAL("out of space creating %d fields", nf);
	makefields(nfields+1, nf);
	nfields = nf;
}

int refldbld(const char *rec, const char *fs)	/* build fields from reg expr in FS */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	char *fr;
	int i, tempstat, n;
	fa *pfa;

	n = strlen(rec);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+1)) == NULL)
			FATAL("out of space for fields in refldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	*fr = '\0';
	if (*rec == '\0')
		return 0;
	pfa = makedfa(fs, 1);
	   dprintf( ("into refldbld, rec = <%s>, pat = <%s>\n", rec, fs) );
	tempstat = pfa->initstat;
	for (i = 1; ; i++) {
		if (i > nfields)
			growfldtab(i);
		if (freeable(fldtab[i]))
			xfree(fldtab[i]->sval);
		fldtab[i]->tval = FLD | STR | DONTFREE;
		fldtab[i]->sval = fr;
		   dprintf( ("refldbld: i=%d\n", i) );
		if (nematch(pfa, rec)) {
			pfa->initstat = 2;	/* horrible coupling to b.c */
			   dprintf( ("match %s (%d chars)\n", patbeg, patlen) );
			strncpy(fr, rec, patbeg-rec);
			fr += patbeg - rec + 1;
			*(fr-1) = '\0';
			rec = patbeg + patlen;
		} else {
			   dprintf( ("no match %s\n", rec) );
			strcpy(fr, rec);
			pfa->initstat = tempstat;
			break;
		}
	}
	return i;		
}

void recbld(void)	/* create $0 from $1..$NF if necessary */
{
	int i;
	char *r, *p;

	if (donerec == 1)
		return;
	r = record;
	for (i = 1; i <= *NF; i++) {
		p = getsval(fldtab[i]);
		if (!adjbuf(&record, &recsize, 1+strlen(p)+r-record, recsize, &r, "recbld 1"))
			FATAL("created $0 `%.30s...' too long", record);
		while ((*r = *p++) != 0)
			r++;
		if (i < *NF) {
			if (!adjbuf(&record, &recsize, 2+strlen(*OFS)+r-record, recsize, &r, "recbld 2"))
				FATAL("created $0 `%.30s...' too long", record);
			for (p = *OFS; (*r = *p++) != 0; )
				r++;
		}
	}
	if (!adjbuf(&record, &recsize, 2+r-record, recsize, &r, "recbld 3"))
		FATAL("built giant record `%.30s...'", record);
	*r = '\0';
	   dprintf( ("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, (void*)fldtab[0]) );

	if (freeable(fldtab[0]))
		xfree(fldtab[0]->sval);
	fldtab[0]->tval = REC | STR | DONTFREE;
	fldtab[0]->sval = record;

	   dprintf( ("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, (void*)fldtab[0]) );
	   dprintf( ("recbld = |%s|\n", record) );
	donerec = 1;
}

int	errorflag	= 0;

void yyerror(const char *s)
{
	SYNTAX("%s", s);
}

void SYNTAX(const char *fmt, ...)
{
	extern char *cmdname, *curfname;
	static int been_here = 0;
	va_list varg;

	if (been_here++ > 2)
		return;
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	fprintf(stderr, " at source line %d", lineno);
	if (curfname != NULL)
		fprintf(stderr, " in function %s", curfname);
	if (compile_time == 1 && cursource() != NULL)
		fprintf(stderr, " source file %s", cursource());
	fprintf(stderr, "\n");
	errorflag = 2;
	eprint();
}

void fpecatch(int n)
{
	FATAL("floating point exception %d", n);
}

extern int bracecnt, brackcnt, parencnt;

void bracecheck(void)
{
	int c;
	static int beenhere = 0;

	if (beenhere++)
		return;
	while ((c = input()) != EOF && c != '\0')
		bclass(c);
	bcheck2(bracecnt, '{', '}');
	bcheck2(brackcnt, '[', ']');
	bcheck2(parencnt, '(', ')');
}

void bcheck2(int n, int c1, int c2)
{
	if (n == 1)
		fprintf(stderr, "\tmissing %c\n", c2);
	else if (n > 1)
		fprintf(stderr, "\t%d missing %c's\n", n, c2);
	else if (n == -1)
		fprintf(stderr, "\textra %c\n", c2);
	else if (n < -1)
		fprintf(stderr, "\t%d extra %c's\n", -n, c2);
}

void FATAL(const char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	fflush(stdout);
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	error();
	if (dbg > 1)		/* core dump if serious debugging on */
		abort();
	exit(2);
}

void WARNING(const char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	fflush(stdout);
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	error();
}

void error()
{
	extern Node *curnode;

	fprintf(stderr, "\n");
	if (compile_time != 2 && NR && *NR > 0) {
		fprintf(stderr, " input record number %d", (int) (*FNR));
		if (strcmp(*FILENAME, "-") != 0)
			fprintf(stderr, ", file %s", *FILENAME);
		fprintf(stderr, "\n");
	}
	if (compile_time != 2 && curnode)
		fprintf(stderr, " source line number %d", curnode->lineno);
	else if (compile_time != 2 && lineno)
		fprintf(stderr, " source line number %d", lineno);
	if (compile_time == 1 && cursource() != NULL)
		fprintf(stderr, " source file %s", cursource());
	fprintf(stderr, "\n");
	eprint();
}

void eprint(void)	/* try to print context around error */
{
	char *p, *q;
	int c;
	static int been_here = 0;
	extern char ebuf[], *ep;

	if (compile_time == 2 || compile_time == 0 || been_here++ > 0)
		return;
	p = ep - 1;
	if (p > ebuf && *p == '\n')
		p--;
	for ( ; p > ebuf && *p != '\n' && *p != '\0'; p--)
		;
	while (*p == '\n')
		p++;
	fprintf(stderr, " context is\n\t");
	for (q=ep-1; q>=p && *q!=' ' && *q!='\t' && *q!='\n'; q--)
		;
	for ( ; p < q; p++)
		if (*p)
			putc(*p, stderr);
	fprintf(stderr, " >>> ");
	for ( ; p < ep; p++)
		if (*p)
			putc(*p, stderr);
	fprintf(stderr, " <<< ");
	if (*ep)
		while ((c = input()) != '\n' && c != '\0' && c != EOF) {
			putc(c, stderr);
			bclass(c);
		}
	putc('\n', stderr);
	ep = ebuf;
}

void bclass(int c)
{
	switch (c) {
	case '{': bracecnt++; break;
	case '}': bracecnt--; break;
	case '[': brackcnt++; break;
	case ']': brackcnt--; break;
	case '(': parencnt++; break;
	case ')': parencnt--; break;
	}
}

double errcheck(double x, const char *s)
{

	if (errno == EDOM) {
		errno = 0;
		WARNING("%s argument out of domain", s);
		x = 1;
	} else if (errno == ERANGE) {
		errno = 0;
		WARNING("%s result out of range", s);
		x = 1;
	}
	return x;
}

int isclvar(const char *s)	/* is s of form var=something ? */
{
	const char *os = s;

	if (!isalpha((uschar) *s) && *s != '_')
		return 0;
	for ( ; *s; s++)
		if (!(isalnum((uschar) *s) || *s == '_'))
			break;
	return *s == '=' && s > os && *(s+1) != '=';
}

/* strtod is supposed to be a proper test of what's a valid number */
/* appears to be broken in gcc on linux: thinks 0x123 is a valid FP number */
/* wrong: violates 4.10.1.4 of ansi C standard */

int is_number(const char *s)
{
	double r;
	char *ep;
	errno = 0;
	r = strtod(s, &ep);
	if (ep == s || r == HUGE_VAL || errno == ERANGE)
		return 0;
	while (*ep == ' ' || *ep == '\t' || *ep == '\n')
		ep++;
	if (*ep == '\0')
		return 1;
	else
		return 0;
}

/*** run.c ***/


#define tempfree(x)	if (istemp(x)) tfree(x); else

/*
#undef tempfree

void tempfree(Cell *p) {
	if (p->ctype == OCELL && (p->csub < CUNK || p->csub > CFREE)) {
		WARNING("bad csub %d in Cell %d %s",
			p->csub, p->ctype, p->sval);
	}
	if (istemp(p))
		tfree(p);
}
*/

/* do we really need these? */
/* #ifdef _NFILE */
/* #ifndef FOPEN_MAX */
/* #define FOPEN_MAX _NFILE */
/* #endif */
/* #endif */
/*  */
/* #ifndef	FOPEN_MAX */
/* #define	FOPEN_MAX	40 */	/* max number of open files */
/* #endif */
/*  */
/* #ifndef RAND_MAX */
/* #define RAND_MAX	32767 */	/* all that ansi guarantees */
/* #endif */

jmp_buf env;
extern	int	pairstack[];
extern	Awkfloat	srand_seed;

Node	*winner = NULL;	/* root of parse tree */
Cell	*tmps;		/* free temporary cells for execution */

static Cell	truecell	={ OBOOL, BTRUE, 0, 0, 1.0, NUM, NULL };
Cell	*True	= &truecell;
static Cell	falsecell	={ OBOOL, BFALSE, 0, 0, 0.0, NUM, NULL };
Cell	*False	= &falsecell;
static Cell	breakcell	={ OJUMP, JBREAK, 0, 0, 0.0, NUM, NULL };
Cell	*jbreak	= &breakcell;
static Cell	contcell	={ OJUMP, JCONT, 0, 0, 0.0, NUM, NULL };
Cell	*jcont	= &contcell;
static Cell	nextcell	={ OJUMP, JNEXT, 0, 0, 0.0, NUM, NULL };
Cell	*jnext	= &nextcell;
static Cell	nextfilecell	={ OJUMP, JNEXTFILE, 0, 0, 0.0, NUM, NULL };
Cell	*jnextfile	= &nextfilecell;
static Cell	exitcell	={ OJUMP, JEXIT, 0, 0, 0.0, NUM, NULL };
Cell	*jexit	= &exitcell;
static Cell	retcell		={ OJUMP, JRET, 0, 0, 0.0, NUM, NULL };
Cell	*jret	= &retcell;
static Cell	tempcell	={ OCELL, CTEMP, 0, "", 0.0, NUM|STR|DONTFREE, NULL };

Node	*curnode = NULL;	/* the node being executed, for debugging */

/* buffer memory management */
int adjbuf(char **pbuf, int *psiz, int minlen, int quantum, char **pbptr,
	const char *whatrtn)
/* pbuf:    address of pointer to buffer being managed
 * psiz:    address of buffer size variable
 * minlen:  minimum length of buffer needed
 * quantum: buffer size quantum
 * pbptr:   address of movable pointer into buffer, or 0 if none
 * whatrtn: name of the calling routine if failure should cause fatal error
 *
 * return   0 for realloc failure, !=0 for success
 */
{
	if (minlen > *psiz) {
		char *tbuf;
		int rminlen = quantum ? minlen % quantum : 0;
		int boff = pbptr ? *pbptr - *pbuf : 0;
		/* round up to next multiple of quantum */
		if (rminlen)
			minlen += quantum - rminlen;
		tbuf = (char *) realloc(*pbuf, minlen);
		dprintf( ("adjbuf %s: %d %d (pbuf=%p, tbuf=%p)\n", whatrtn, *psiz, minlen, *pbuf, tbuf) );
		if (tbuf == NULL) {
			if (whatrtn)
				FATAL("out of memory in %s", whatrtn);
			return 0;
		}
		*pbuf = tbuf;
		*psiz = minlen;
		if (pbptr)
			*pbptr = tbuf + boff;
	}
	return 1;
}

void run(Node *a)	/* execution of parse tree starts here */
{
	extern void stdinit(void);

	stdinit();
	execute(a);
	closeall();
}

Cell *execute(Node *u)	/* execute a node of the parse tree */
{
	Cell *(*proc)(Node **, int);
	Cell *x;
	Node *a;

	if (u == NULL)
		return(True);
	for (a = u; ; a = a->nnext) {
		curnode = a;
		if (isvalue(a)) {
			x = (Cell *) (a->narg[0]);
			if (isfld(x) && !donefld)
				fldbld();
			else if (isrec(x) && !donerec)
				recbld();
			return(x);
		}
		if (notlegal(a->nobj))	/* probably a Cell* but too risky to print */
			FATAL("illegal statement");
		proc = proctab[a->nobj-FIRSTTOKEN];
		x = (*proc)(a->narg, a->nobj);
		if (isfld(x) && !donefld)
			fldbld();
		else if (isrec(x) && !donerec)
			recbld();
		if (isexpr(a))
			return(x);
		if (isjump(x))
			return(x);
		if (a->nnext == NULL)
			return(x);
		tempfree(x);
	}
}


Cell *program(Node **a, int n)	/* execute an awk program */
{				/* a[0] = BEGIN, a[1] = body, a[2] = END */
	Cell *x;

	if (setjmp(env) != 0)
		goto ex;
	if (a[0]) {		/* BEGIN */
		x = execute(a[0]);
		if (isexit(x))
			return(True);
		if (isjump(x))
			FATAL("illegal break, continue, next or nextfile from BEGIN");
		tempfree(x);
	}
	if (a[1] || a[2])
		while (getrec(&record, &recsize, 1) > 0) {
			x = execute(a[1]);
			if (isexit(x))
				break;
			tempfree(x);
		}
  ex:
	if (setjmp(env) != 0)	/* handles exit within END */
		goto ex1;
	if (a[2]) {		/* END */
		x = execute(a[2]);
		if (isbreak(x) || isnext(x) || iscont(x))
			FATAL("illegal break, continue, next or nextfile from END");
		tempfree(x);
	}
  ex1:
	return(True);
}

struct Frame {	/* stack frame for awk function calls */
	int nargs;	/* number of arguments in this call */
	Cell *fcncell;	/* pointer to Cell for function */
	Cell **args;	/* pointer to array of arguments after execute */
	Cell *retval;	/* return value */
};

#define	NARGS	50	/* max args in a call */

struct Frame *frame = NULL;	/* base of stack frames; dynamically allocated */
int	nframe = 0;		/* number of frames allocated */
struct Frame *fp = NULL;	/* frame pointer. bottom level unused */

Cell *call(Node **a, int n)	/* function call.  very kludgy and fragile */
{
	static Cell newcopycell = { OCELL, CCOPY, 0, "", 0.0, NUM|STR|DONTFREE, NULL };
	int i, ncall, ndef;
	int freed = 0; /* handles potential double freeing when fcn & param share a tempcell */
	Node *x;
	Cell *args[NARGS], *oargs[NARGS];	/* BUG: fixed size arrays */
	Cell *y, *z, *fcn;
	char *s;

	fcn = execute(a[0]);	/* the function itself */
	s = fcn->nval;
	if (!isfcn(fcn))
		FATAL("calling undefined function %s", s);
	if (frame == NULL) {
		fp = frame = (struct Frame *) calloc(nframe += 100, sizeof(struct Frame));
		if (frame == NULL)
			FATAL("out of space for stack frames calling %s", s);
	}
	for (ncall = 0, x = a[1]; x != NULL; x = x->nnext)	/* args in call */
		ncall++;
	ndef = (int) fcn->fval;			/* args in defn */
	   dprintf( ("calling %s, %d args (%d in defn), fp=%d\n", s, ncall, ndef, (int) (fp-frame)) );
	if (ncall > ndef)
		WARNING("function %s called with %d args, uses only %d",
			s, ncall, ndef);
	if (ncall + ndef > NARGS)
		FATAL("function %s has %d arguments, limit %d", s, ncall+ndef, NARGS);
	for (i = 0, x = a[1]; x != NULL; i++, x = x->nnext) {	/* get call args */
		   dprintf( ("evaluate args[%d], fp=%d:\n", i, (int) (fp-frame)) );
		y = execute(x);
		oargs[i] = y;
		   dprintf( ("args[%d]: %s %f <%s>, t=%o\n",
			   i, NN(y->nval), y->fval, isarr(y) ? "(array)" : NN(y->sval), y->tval) );
		if (isfcn(y))
			FATAL("can't use function %s as argument in %s", y->nval, s);
		if (isarr(y))
			args[i] = y;	/* arrays by ref */
		else
			args[i] = copycell(y);
		tempfree(y);
	}
	for ( ; i < ndef; i++) {	/* add null args for ones not provided */
		args[i] = gettemp();
		*args[i] = newcopycell;
	}
	fp++;	/* now ok to up frame */
	if (fp >= frame + nframe) {
		int dfp = fp - frame;	/* old index */
		frame = (struct Frame *)
			realloc((char *) frame, (nframe += 100) * sizeof(struct Frame));
		if (frame == NULL)
			FATAL("out of space for stack frames in %s", s);
		fp = frame + dfp;
	}
	fp->fcncell = fcn;
	fp->args = args;
	fp->nargs = ndef;	/* number defined with (excess are locals) */
	fp->retval = gettemp();

	   dprintf( ("start exec of %s, fp=%d\n", s, (int) (fp-frame)) );
	y = execute((Node *)(fcn->sval));	/* execute body */
	   dprintf( ("finished exec of %s, fp=%d\n", s, (int) (fp-frame)) );

	for (i = 0; i < ndef; i++) {
		Cell *t = fp->args[i];
		if (isarr(t)) {
			if (t->csub == CCOPY) {
				if (i >= ncall) {
					freesymtab(t);
					t->csub = CTEMP;
					tempfree(t);
				} else {
					oargs[i]->tval = t->tval;
					oargs[i]->tval &= ~(STR|NUM|DONTFREE);
					oargs[i]->sval = t->sval;
					tempfree(t);
				}
			}
		} else if (t != y) {	/* kludge to prevent freeing twice */
			t->csub = CTEMP;
			tempfree(t);
		} else if (t == y && t->csub == CCOPY) {
			t->csub = CTEMP;
			tempfree(t);
			freed = 1;
		}
	}
	tempfree(fcn);
	if (isexit(y) || isnext(y))
		return y;
	if (freed == 0) {
		tempfree(y);	/* don't free twice! */
	}
	z = fp->retval;			/* return value */
	   dprintf( ("%s returns %g |%s| %o\n", s, getfval(z), getsval(z), z->tval) );
	fp--;
	return(z);
}

Cell *copycell(Cell *x)	/* make a copy of a cell in a temp */
{
	Cell *y;

	y = gettemp();
	y->csub = CCOPY;	/* prevents freeing until call is over */
	y->nval = x->nval;	/* BUG? */
	if (isstr(x))
		y->sval = tostring(x->sval);
	y->fval = x->fval;
	y->tval = x->tval & ~(CON|FLD|REC|DONTFREE);	/* copy is not constant or field */
							/* is DONTFREE right? */
	return y;
}

Cell *arg(Node **a, int n)	/* nth argument of a function */
{

	n = ptoi(a[0]);	/* argument number, counting from 0 */
	   dprintf( ("arg(%d), fp->nargs=%d\n", n, fp->nargs) );
	if (n+1 > fp->nargs)
		FATAL("argument #%d of function %s was not supplied",
			n+1, fp->fcncell->nval);
	return fp->args[n];
}

Cell *jump(Node **a, int n)	/* break, continue, next, nextfile, return */
{
	Cell *y;

	switch (n) {
	case EXIT:
		if (a[0] != NULL) {
			y = execute(a[0]);
			errorflag = (int) getfval(y);
			tempfree(y);
		}
		longjmp(env, 1);
	case RETURN:
		if (a[0] != NULL) {
			y = execute(a[0]);
			if ((y->tval & (STR|NUM)) == (STR|NUM)) {
				setsval(fp->retval, getsval(y));
				fp->retval->fval = getfval(y);
				fp->retval->tval |= NUM;
			}
			else if (y->tval & STR)
				setsval(fp->retval, getsval(y));
			else if (y->tval & NUM)
				setfval(fp->retval, getfval(y));
			else		/* can't happen */
				FATAL("bad type variable %d", y->tval);
			tempfree(y);
		}
		return(jret);
	case NEXT:
		return(jnext);
	case NEXTFILE:
		nextfile();
		return(jnextfile);
	case BREAK:
		return(jbreak);
	case CONTINUE:
		return(jcont);
	default:	/* can't happen */
		FATAL("illegal jump type %d", n);
	}
	return 0;	/* not reached */
}

Cell *awkgetline(Node **a, int n)	/* get next line from specific input */
{		/* a[0] is variable, a[1] is operator, a[2] is filename */
	Cell *r, *x;
	extern Cell **fldtab;
	FILE *fp;
	char *buf;
	int bufsize = recsize;
	int mode;

	if ((buf = (char *) malloc(bufsize)) == NULL)
		FATAL("out of memory in getline");

	fflush(stdout);	/* in case someone is waiting for a prompt */
	r = gettemp();
	if (a[1] != NULL) {		/* getline < file */
		x = execute(a[2]);		/* filename */
		mode = ptoi(a[1]);
		if (mode == '|')		/* input pipe */
			mode = LE;	/* arbitrary flag */
		fp = openfile(mode, getsval(x));
		tempfree(x);
		if (fp == NULL)
			n = -1;
		else
			n = readrec(&buf, &bufsize, fp);
		if (n <= 0) {
			;
		} else if (a[0] != NULL) {	/* getline var <file */
			x = execute(a[0]);
			setsval(x, buf);
			tempfree(x);
		} else {			/* getline <file */
			setsval(fldtab[0], buf);
			if (is_number(fldtab[0]->sval)) {
				fldtab[0]->fval = atof(fldtab[0]->sval);
				fldtab[0]->tval |= NUM;
			}
		}
	} else {			/* bare getline; use current input */
		if (a[0] == NULL)	/* getline */
			n = getrec(&record, &recsize, 1);
		else {			/* getline var */
			n = getrec(&buf, &bufsize, 0);
			x = execute(a[0]);
			setsval(x, buf);
			tempfree(x);
		}
	}
	setfval(r, (Awkfloat) n);
	free(buf);
	return r;
}

Cell *getnf(Node **a, int n)	/* get NF */
{
	if (donefld == 0)
		fldbld();
	return (Cell *) a[0];
}

Cell *array(Node **a, int n)	/* a[0] is symtab, a[1] is list of subscripts */
{
	Cell *x, *y, *z;
	char *s;
	Node *np;
	char *buf;
	int bufsz = recsize;
	int nsub = strlen(*SUBSEP);

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in array");

	x = execute(a[0]);	/* Cell* for symbol table */
	buf[0] = 0;
	for (np = a[1]; np; np = np->nnext) {
		y = execute(np);	/* subscript */
		s = getsval(y);
		if (!adjbuf(&buf, &bufsz, strlen(buf)+strlen(s)+nsub+1, recsize, 0, "array"))
			FATAL("out of memory for %s[%s...]", x->nval, buf);
		strcat(buf, s);
		if (np->nnext)
			strcat(buf, *SUBSEP);
		tempfree(y);
	}
	if (!isarr(x)) {
		   dprintf( ("making %s into an array\n", NN(x->nval)) );
		if (freeable(x))
			xfree(x->sval);
		x->tval &= ~(STR|NUM|DONTFREE);
		x->tval |= ARR;
		x->sval = (char *) makesymtab(NSYMTAB);
	}
	z = setsymtab(buf, "", 0.0, STR|NUM, (Array *) x->sval);
	z->ctype = OCELL;
	z->csub = CVAR;
	tempfree(x);
	free(buf);
	return(z);
}

Cell *awkdelete(Node **a, int n)	/* a[0] is symtab, a[1] is list of subscripts */
{
	Cell *x, *y;
	Node *np;
	char *s;
	int nsub = strlen(*SUBSEP);

	x = execute(a[0]);	/* Cell* for symbol table */
	if (!isarr(x))
		return True;
	if (a[1] == 0) {	/* delete the elements, not the table */
		freesymtab(x);
		x->tval &= ~STR;
		x->tval |= ARR;
		x->sval = (char *) makesymtab(NSYMTAB);
	} else {
		int bufsz = recsize;
		char *buf;
		if ((buf = (char *) malloc(bufsz)) == NULL)
			FATAL("out of memory in adelete");
		buf[0] = 0;
		for (np = a[1]; np; np = np->nnext) {
			y = execute(np);	/* subscript */
			s = getsval(y);
			if (!adjbuf(&buf, &bufsz, strlen(buf)+strlen(s)+nsub+1, recsize, 0, "awkdelete"))
				FATAL("out of memory deleting %s[%s...]", x->nval, buf);
			strcat(buf, s);	
			if (np->nnext)
				strcat(buf, *SUBSEP);
			tempfree(y);
		}
		freeelem(x, buf);
		free(buf);
	}
	tempfree(x);
	return True;
}

Cell *intest(Node **a, int n)	/* a[0] is index (list), a[1] is symtab */
{
	Cell *x, *ap, *k;
	Node *p;
	char *buf;
	char *s;
	int bufsz = recsize;
	int nsub = strlen(*SUBSEP);

	ap = execute(a[1]);	/* array name */
	if (!isarr(ap)) {
		   dprintf( ("making %s into an array\n", ap->nval) );
		if (freeable(ap))
			xfree(ap->sval);
		ap->tval &= ~(STR|NUM|DONTFREE);
		ap->tval |= ARR;
		ap->sval = (char *) makesymtab(NSYMTAB);
	}
	if ((buf = (char *) malloc(bufsz)) == NULL) {
		FATAL("out of memory in intest");
	}
	buf[0] = 0;
	for (p = a[0]; p; p = p->nnext) {
		x = execute(p);	/* expr */
		s = getsval(x);
		if (!adjbuf(&buf, &bufsz, strlen(buf)+strlen(s)+nsub+1, recsize, 0, "intest"))
			FATAL("out of memory deleting %s[%s...]", x->nval, buf);
		strcat(buf, s);
		tempfree(x);
		if (p->nnext)
			strcat(buf, *SUBSEP);
	}
	k = lookup(buf, (Array *) ap->sval);
	tempfree(ap);
	free(buf);
	if (k == NULL)
		return(False);
	else
		return(True);
}


Cell *matchop(Node **a, int n)	/* ~ and match() */
{
	Cell *x, *y;
	char *s, *t;
	int i;
	fa *pfa;
	int (*mf)(fa *, const char *) = match, mode = 0;

	if (n == MATCHFCN) {
		mf = pmatch;
		mode = 1;
	}
	x = execute(a[1]);	/* a[1] = target text */
	s = getsval(x);
	if (a[0] == 0)		/* a[1] == 0: already-compiled reg expr */
		i = (*mf)((fa *) a[2], s);
	else {
		y = execute(a[2]);	/* a[2] = regular expr */
		t = getsval(y);
		pfa = makedfa(t, mode);
		i = (*mf)(pfa, s);
		tempfree(y);
	}
	tempfree(x);
	if (n == MATCHFCN) {
		int start = patbeg - s + 1;
		if (patlen < 0)
			start = 0;
		setfval(rstartloc, (Awkfloat) start);
		setfval(rlengthloc, (Awkfloat) patlen);
		x = gettemp();
		x->tval = NUM;
		x->fval = start;
		return x;
	} else if ((n == MATCH && i == 1) || (n == NOTMATCH && i == 0))
		return(True);
	else
		return(False);
}


Cell *boolop(Node **a, int n)	/* a[0] || a[1], a[0] && a[1], !a[0] */
{
	Cell *x, *y;
	int i;

	x = execute(a[0]);
	i = istrue(x);
	tempfree(x);
	switch (n) {
	case BOR:
		if (i) return(True);
		y = execute(a[1]);
		i = istrue(y);
		tempfree(y);
		if (i) return(True);
		else return(False);
	case AND:
		if ( !i ) return(False);
		y = execute(a[1]);
		i = istrue(y);
		tempfree(y);
		if (i) return(True);
		else return(False);
	case NOT:
		if (i) return(False);
		else return(True);
	default:	/* can't happen */
		FATAL("unknown boolean operator %d", n);
	}
	return 0;	/*NOTREACHED*/
}

Cell *relop(Node **a, int n)	/* a[0 < a[1], etc. */
{
	int i;
	Cell *x, *y;
	Awkfloat j;

	x = execute(a[0]);
	y = execute(a[1]);
	if (x->tval&NUM && y->tval&NUM) {
		j = x->fval - y->fval;
		i = j<0? -1: (j>0? 1: 0);
	} else {
		i = strcmp(getsval(x), getsval(y));
	}
	tempfree(x);
	tempfree(y);
	switch (n) {
	case LT:	if (i<0) return(True);
			else return(False);
	case LE:	if (i<=0) return(True);
			else return(False);
	case NE:	if (i!=0) return(True);
			else return(False);
	case EQ:	if (i == 0) return(True);
			else return(False);
	case GE:	if (i>=0) return(True);
			else return(False);
	case GT:	if (i>0) return(True);
			else return(False);
	default:	/* can't happen */
		FATAL("unknown relational operator %d", n);
	}
	return 0;	/*NOTREACHED*/
}

void tfree(Cell *a)	/* free a tempcell */
{
	if (freeable(a)) {
		   dprintf( ("freeing %s %s %o\n", NN(a->nval), NN(a->sval), a->tval) );
		xfree(a->sval);
	}
	if (a == tmps)
		FATAL("tempcell list is curdled");
	a->cnext = tmps;
	tmps = a;
}

Cell *gettemp(void)	/* get a tempcell */
{	int i;
	Cell *x;

	if (!tmps) {
		tmps = (Cell *) calloc(100, sizeof(Cell));
		if (!tmps)
			FATAL("out of space for temporaries");
		for(i = 1; i < 100; i++)
			tmps[i-1].cnext = &tmps[i];
		tmps[i-1].cnext = 0;
	}
	x = tmps;
	tmps = x->cnext;
	*x = tempcell;
	return(x);
}

Cell *indirect(Node **a, int n)	/* $( a[0] ) */
{
	Awkfloat val;
	Cell *x;
	int m;
	char *s;

	x = execute(a[0]);
	val = getfval(x);	/* freebsd: defend against super large field numbers */
	if ((Awkfloat)INT_MAX < val)
		FATAL("trying to access out of range field %s", x->nval);
	m = (int) val;
	if (m == 0 && !is_number(s = getsval(x)))	/* suspicion! */
		FATAL("illegal field $(%s), name \"%s\"", s, x->nval);
		/* BUG: can x->nval ever be null??? */
	tempfree(x);
	x = fieldadr(m);
	x->ctype = OCELL;	/* BUG?  why are these needed? */
	x->csub = CFLD;
	return(x);
}

Cell *substr(Node **a, int nnn)		/* substr(a[0], a[1], a[2]) */
{
	int k, m, n;
	char *s;
	int temp;
	Cell *x, *y, *z = 0;

	x = execute(a[0]);
	y = execute(a[1]);
	if (a[2] != 0)
		z = execute(a[2]);
	s = getsval(x);
	k = strlen(s) + 1;
	if (k <= 1) {
		tempfree(x);
		tempfree(y);
		if (a[2] != 0) {
			tempfree(z);
		}
		x = gettemp();
		setsval(x, "");
		return(x);
	}
	m = (int) getfval(y);
	if (m <= 0)
		m = 1;
	else if (m > k)
		m = k;
	tempfree(y);
	if (a[2] != 0) {
		n = (int) getfval(z);
		tempfree(z);
	} else
		n = k - 1;
	if (n < 0)
		n = 0;
	else if (n > k - m)
		n = k - m;
	   dprintf( ("substr: m=%d, n=%d, s=%s\n", m, n, s) );
	y = gettemp();
	temp = s[n+m-1];	/* with thanks to John Linderman */
	s[n+m-1] = '\0';
	setsval(y, s + m - 1);
	s[n+m-1] = temp;
	tempfree(x);
	return(y);
}

Cell *sindex(Node **a, int nnn)		/* index(a[0], a[1]) */
{
	Cell *x, *y, *z;
	char *s1, *s2, *p1, *p2, *q;
	Awkfloat v = 0.0;

	x = execute(a[0]);
	s1 = getsval(x);
	y = execute(a[1]);
	s2 = getsval(y);

	z = gettemp();
	for (p1 = s1; *p1 != '\0'; p1++) {
		for (q=p1, p2=s2; *p2 != '\0' && *q == *p2; q++, p2++)
			;
		if (*p2 == '\0') {
			v = (Awkfloat) (p1 - s1 + 1);	/* origin 1 */
			break;
		}
	}
	tempfree(x);
	tempfree(y);
	setfval(z, v);
	return(z);
}

#define	MAXNUMSIZE	50

int format(char **pbuf, int *pbufsize, const char *s, Node *a)	/* printf-like conversions */
{
	char *fmt;
	char *p, *t;
	const char *os;
	Cell *x;
	int flag = 0, n;
	int fmtwd; /* format width */
	int fmtsz = recsize;
	char *buf = *pbuf;
	int bufsize = *pbufsize;

	os = s;
	p = buf;
	if ((fmt = (char *) malloc(fmtsz)) == NULL)
		FATAL("out of memory in format()");
	while (*s) {
		adjbuf(&buf, &bufsize, MAXNUMSIZE+1+p-buf, recsize, &p, "format1");
		if (*s != '%') {
			*p++ = *s++;
			continue;
		}
		if (*(s+1) == '%') {
			*p++ = '%';
			s += 2;
			continue;
		}
		/* have to be real careful in case this is a huge number, eg, %100000d */
		fmtwd = atoi(s+1);
		if (fmtwd < 0)
			fmtwd = -fmtwd;
		adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format2");
		for (t = fmt; (*t++ = *s) != '\0'; s++) {
			if (!adjbuf(&fmt, &fmtsz, MAXNUMSIZE+1+t-fmt, recsize, &t, "format3"))
				FATAL("format item %.30s... ran format() out of memory", os);
			if (isalpha((uschar)*s) && *s != 'l' && *s != 'h' && *s != 'L')
				break;	/* the ansi panoply */
			if (*s == '*') {
				x = execute(a);
				a = a->nnext;
				sprintf(t-1, "%d", fmtwd=(int) getfval(x));
				if (fmtwd < 0)
					fmtwd = -fmtwd;
				adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format");
				t = fmt + strlen(fmt);
				tempfree(x);
			}
		}
		*t = '\0';
		if (fmtwd < 0)
			fmtwd = -fmtwd;
		adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format4");

		switch (*s) {
		case 'f': case 'e': case 'g': case 'E': case 'G':
			flag = 'f';
			break;
		case 'd': case 'i':
			flag = 'd';
			if(*(s-1) == 'l') break;
			*(t-1) = 'l';
			*t = 'd';
			*++t = '\0';
			break;
		case 'o': case 'x': case 'X': case 'u':
			flag = *(s-1) == 'l' ? 'd' : 'u';
			break;
		case 's':
			flag = 's';
			break;
		case 'c':
			flag = 'c';
			break;
		default:
			WARNING("weird printf conversion %s", fmt);
			flag = '?';
			break;
		}
		if (a == NULL)
			FATAL("not enough args in printf(%s)", os);
		x = execute(a);
		a = a->nnext;
		n = MAXNUMSIZE;
		if (fmtwd > n)
			n = fmtwd;
		adjbuf(&buf, &bufsize, 1+n+p-buf, recsize, &p, "format5");
		switch (flag) {
		case '?':	sprintf(p, "%s", fmt);	/* unknown, so dump it too */
			t = getsval(x);
			n = strlen(t);
			if (fmtwd > n)
				n = fmtwd;
			adjbuf(&buf, &bufsize, 1+strlen(p)+n+p-buf, recsize, &p, "format6");
			p += strlen(p);
			sprintf(p, "%s", t);
			break;
		case 'f':	sprintf(p, fmt, getfval(x)); break;
		case 'd':	sprintf(p, fmt, (long) getfval(x)); break;
		case 'u':	sprintf(p, fmt, (int) getfval(x)); break;
		case 's':
			t = getsval(x);
			n = strlen(t);
			if (fmtwd > n)
				n = fmtwd;
			if (!adjbuf(&buf, &bufsize, 1+n+p-buf, recsize, &p, "format7"))
				FATAL("huge string/format (%d chars) in printf %.30s... ran format() out of memory", n, t);
			sprintf(p, fmt, t);
			break;
		case 'c':
			if (isnum(x)) {
				if (getfval(x))
					sprintf(p, fmt, (int) getfval(x));
				else {
					*p++ = '\0'; /* explicit null byte */
					*p = '\0';   /* next output will start here */
				}
			} else
				sprintf(p, fmt, getsval(x)[0]);
			break;
		default:
			FATAL("can't happen: bad conversion %c in format()", flag);
		}
		tempfree(x);
		p += strlen(p);
		s++;
	}
	*p = '\0';
	free(fmt);
	for ( ; a; a = a->nnext)		/* evaluate any remaining args */
		execute(a);
	*pbuf = buf;
	*pbufsize = bufsize;
	return p - buf;
}

Cell *awksprintf(Node **a, int n)		/* sprintf(a[0]) */
{
	Cell *x;
	Node *y;
	char *buf;
	int bufsz=3*recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in awksprintf");
	y = a[0]->nnext;
	x = execute(a[0]);
	if (format(&buf, &bufsz, getsval(x), y) == -1)
		FATAL("sprintf string %.30s... too long.  can't happen.", buf);
	tempfree(x);
	x = gettemp();
	x->sval = buf;
	x->tval = STR;
	return(x);
}

Cell *awkprintf(Node **a, int n)		/* printf */
{	/* a[0] is list of args, starting with format string */
	/* a[1] is redirection operator, a[2] is redirection file */
	FILE *fp;
	Cell *x;
	Node *y;
	char *buf;
	int len;
	int bufsz=3*recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in awkprintf");
	y = a[0]->nnext;
	x = execute(a[0]);
	if ((len = format(&buf, &bufsz, getsval(x), y)) == -1)
		FATAL("printf string %.30s... too long.  can't happen.", buf);
	tempfree(x);
	if (a[1] == NULL) {
		/* fputs(buf, stdout); */
		fwrite(buf, len, 1, stdout);
		if (ferror(stdout))
			FATAL("write error on stdout");
	} else {
		fp = redirect(ptoi(a[1]), a[2]);
		/* fputs(buf, fp); */
		fwrite(buf, len, 1, fp);
		fflush(fp);
		if (ferror(fp))
			FATAL("write error on %s", filename(fp));
	}
	free(buf);
	return(True);
}

Cell *arith(Node **a, int n)	/* a[0] + a[1], etc.  also -a[0] */
{
	Awkfloat i, j = 0;
	double v;
	Cell *x, *y, *z;

	x = execute(a[0]);
	i = getfval(x);
	tempfree(x);
	if (n != UMINUS) {
		y = execute(a[1]);
		j = getfval(y);
		tempfree(y);
	}
	z = gettemp();
	switch (n) {
	case ADD:
		i += j;
		break;
	case MINUS:
		i -= j;
		break;
	case MULT:
		i *= j;
		break;
	case DIVIDE:
		if (j == 0)
			FATAL("division by zero");
		i /= j;
		break;
	case MOD:
		if (j == 0)
			FATAL("division by zero in mod");
		modf(i/j, &v);
		i = i - j * v;
		break;
	case UMINUS:
		i = -i;
		break;
	case POWER:
		if (j >= 0 && modf(j, &v) == 0.0)	/* pos integer exponent */
			i = ipow(i, (int) j);
		else
			i = errcheck(pow(i, j), "pow");
		break;
	default:	/* can't happen */
		FATAL("illegal arithmetic operator %d", n);
	}
	setfval(z, i);
	return(z);
}

double ipow(double x, int n)	/* x**n.  ought to be done by pow, but isn't always */
{
	double v;

	if (n <= 0)
		return 1;
	v = ipow(x, n/2);
	if (n % 2 == 0)
		return v * v;
	else
		return x * v * v;
}

Cell *incrdecr(Node **a, int n)		/* a[0]++, etc. */
{
	Cell *x, *z;
	int k;
	Awkfloat xf;

	x = execute(a[0]);
	xf = getfval(x);
	k = (n == PREINCR || n == POSTINCR) ? 1 : -1;
	if (n == PREINCR || n == PREDECR) {
		setfval(x, xf + k);
		return(x);
	}
	z = gettemp();
	setfval(z, xf);
	setfval(x, xf + k);
	tempfree(x);
	return(z);
}

Cell *assign(Node **a, int n)	/* a[0] = a[1], a[0] += a[1], etc. */
{		/* this is subtle; don't muck with it. */
	Cell *x, *y;
	Awkfloat xf, yf;
	double v;

	y = execute(a[1]);
	x = execute(a[0]);
	if (n == ASSIGN) {	/* ordinary assignment */
		if (x == y && !(x->tval & (FLD|REC)))	/* self-assignment: */
			;		/* leave alone unless it's a field */
		else if ((y->tval & (STR|NUM)) == (STR|NUM)) {
			setsval(x, getsval(y));
			x->fval = getfval(y);
			x->tval |= NUM;
		}
		else if (isstr(y))
			setsval(x, getsval(y));
		else if (isnum(y))
			setfval(x, getfval(y));
		else
			funnyvar(y, "read value of");
		tempfree(y);
		return(x);
	}
	xf = getfval(x);
	yf = getfval(y);
	switch (n) {
	case ADDEQ:
		xf += yf;
		break;
	case SUBEQ:
		xf -= yf;
		break;
	case MULTEQ:
		xf *= yf;
		break;
	case DIVEQ:
		if (yf == 0)
			FATAL("division by zero in /=");
		xf /= yf;
		break;
	case MODEQ:
		if (yf == 0)
			FATAL("division by zero in %%=");
		modf(xf/yf, &v);
		xf = xf - yf * v;
		break;
	case POWEQ:
		if (yf >= 0 && modf(yf, &v) == 0.0)	/* pos integer exponent */
			xf = ipow(xf, (int) yf);
		else
			xf = errcheck(pow(xf, yf), "pow");
		break;
	default:
		FATAL("illegal assignment operator %d", n);
		break;
	}
	tempfree(y);
	setfval(x, xf);
	return(x);
}

Cell *cat(Node **a, int q)	/* a[0] cat a[1] */
{
	Cell *x, *y, *z;
	int n1, n2;
	char *s;

	x = execute(a[0]);
	y = execute(a[1]);
	getsval(x);
	getsval(y);
	n1 = strlen(x->sval);
	n2 = strlen(y->sval);
	s = (char *) malloc(n1 + n2 + 1);
	if (s == NULL)
		FATAL("out of space concatenating %.15s... and %.15s...",
			x->sval, y->sval);
	strcpy(s, x->sval);
	strcpy(s+n1, y->sval);
	tempfree(x);
	tempfree(y);
	z = gettemp();
	z->sval = s;
	z->tval = STR;
	return(z);
}

Cell *pastat(Node **a, int n)	/* a[0] { a[1] } */
{
	Cell *x;

	if (a[0] == 0)
		x = execute(a[1]);
	else {
		x = execute(a[0]);
		if (istrue(x)) {
			tempfree(x);
			x = execute(a[1]);
		}
	}
	return x;
}

Cell *dopa2(Node **a, int n)	/* a[0], a[1] { a[2] } */
{
	Cell *x;
	int pair;

	pair = ptoi(a[3]);
	if (pairstack[pair] == 0) {
		x = execute(a[0]);
		if (istrue(x))
			pairstack[pair] = 1;
		tempfree(x);
	}
	if (pairstack[pair] == 1) {
		x = execute(a[1]);
		if (istrue(x))
			pairstack[pair] = 0;
		tempfree(x);
		x = execute(a[2]);
		return(x);
	}
	return(False);
}

Cell *split(Node **a, int nnn)	/* split(a[0], a[1], a[2]); a[3] is type */
{
	Cell *x = 0, *y, *ap;
	char *s, *origs;
	int sep;
	char *t, temp, num[50], *fs = 0;
	int n, tempstat, arg3type;

	y = execute(a[0]);	/* source string */
	origs = s = strdup(getsval(y));
	arg3type = ptoi(a[3]);
	if (a[2] == 0)		/* fs string */
		fs = *FS;
	else if (arg3type == STRING) {	/* split(str,arr,"string") */
		x = execute(a[2]);
		fs = getsval(x);
	} else if (arg3type == REGEXPR)
		fs = "(regexpr)";	/* split(str,arr,/regexpr/) */
	else
		FATAL("illegal type of split");
	sep = *fs;
	ap = execute(a[1]);	/* array name */
	freesymtab(ap);
	   dprintf( ("split: s=|%s|, a=%s, sep=|%s|\n", s, NN(ap->nval), fs) );
	ap->tval &= ~STR;
	ap->tval |= ARR;
	ap->sval = (char *) makesymtab(NSYMTAB);

	n = 0;
        if (arg3type == REGEXPR && strlen((char*)((fa*)a[2])->restr) == 0) {
		/* split(s, a, //); have to arrange that it looks like empty sep */
		arg3type = 0;
		fs = "";
		sep = 0;
	}
	if (*s != '\0' && (strlen(fs) > 1 || arg3type == REGEXPR)) {	/* reg expr */
		fa *pfa;
		if (arg3type == REGEXPR) {	/* it's ready already */
			pfa = (fa *) a[2];
		} else {
			pfa = makedfa(fs, 1);
		}
		if (nematch(pfa,s)) {
			tempstat = pfa->initstat;
			pfa->initstat = 2;
			do {
				n++;
				sprintf(num, "%d", n);
				temp = *patbeg;
				*patbeg = '\0';
				if (is_number(s))
					setsymtab(num, s, atof(s), STR|NUM, (Array *) ap->sval);
				else
					setsymtab(num, s, 0.0, STR, (Array *) ap->sval);
				*patbeg = temp;
				s = patbeg + patlen;
				if (*(patbeg+patlen-1) == 0 || *s == 0) {
					n++;
					sprintf(num, "%d", n);
					setsymtab(num, "", 0.0, STR, (Array *) ap->sval);
					pfa->initstat = tempstat;
					goto spdone;
				}
			} while (nematch(pfa,s));
			pfa->initstat = tempstat; 	/* bwk: has to be here to reset */
							/* cf gsub and refldbld */
		}
		n++;
		sprintf(num, "%d", n);
		if (is_number(s))
			setsymtab(num, s, atof(s), STR|NUM, (Array *) ap->sval);
		else
			setsymtab(num, s, 0.0, STR, (Array *) ap->sval);
  spdone:
		pfa = NULL;
	} else if (sep == ' ') {
		for (n = 0; ; ) {
			while (*s == ' ' || *s == '\t' || *s == '\n')
				s++;
			if (*s == 0)
				break;
			n++;
			t = s;
			do
				s++;
			while (*s!=' ' && *s!='\t' && *s!='\n' && *s!='\0');
			temp = *s;
			*s = '\0';
			sprintf(num, "%d", n);
			if (is_number(t))
				setsymtab(num, t, atof(t), STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, t, 0.0, STR, (Array *) ap->sval);
			*s = temp;
			if (*s != 0)
				s++;
		}
	} else if (sep == 0) {	/* new: split(s, a, "") => 1 char/elem */
		for (n = 0; *s != 0; s++) {
			char buf[2];
			n++;
			sprintf(num, "%d", n);
			buf[0] = *s;
			buf[1] = 0;
			if (isdigit((uschar)buf[0]))
				setsymtab(num, buf, atof(buf), STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, buf, 0.0, STR, (Array *) ap->sval);
		}
	} else if (*s != 0) {
		for (;;) {
			n++;
			t = s;
			while (*s != sep && *s != '\n' && *s != '\0')
				s++;
			temp = *s;
			*s = '\0';
			sprintf(num, "%d", n);
			if (is_number(t))
				setsymtab(num, t, atof(t), STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, t, 0.0, STR, (Array *) ap->sval);
			*s = temp;
			if (*s++ == 0)
				break;
		}
	}
	tempfree(ap);
	tempfree(y);
	free(origs);
	if (a[2] != 0 && arg3type == STRING) {
		tempfree(x);
	}
	x = gettemp();
	x->tval = NUM;
	x->fval = n;
	return(x);
}

Cell *condexpr(Node **a, int n)	/* a[0] ? a[1] : a[2] */
{
	Cell *x;

	x = execute(a[0]);
	if (istrue(x)) {
		tempfree(x);
		x = execute(a[1]);
	} else {
		tempfree(x);
		x = execute(a[2]);
	}
	return(x);
}

Cell *ifstat(Node **a, int n)	/* if (a[0]) a[1]; else a[2] */
{
	Cell *x;

	x = execute(a[0]);
	if (istrue(x)) {
		tempfree(x);
		x = execute(a[1]);
	} else if (a[2] != 0) {
		tempfree(x);
		x = execute(a[2]);
	}
	return(x);
}

Cell *whilestat(Node **a, int n)	/* while (a[0]) a[1] */
{
	Cell *x;

	for (;;) {
		x = execute(a[0]);
		if (!istrue(x))
			return(x);
		tempfree(x);
		x = execute(a[1]);
		if (isbreak(x)) {
			x = True;
			return(x);
		}
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
	}
}

Cell *dostat(Node **a, int n)	/* do a[0]; while(a[1]) */
{
	Cell *x;

	for (;;) {
		x = execute(a[0]);
		if (isbreak(x))
			return True;
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
		x = execute(a[1]);
		if (!istrue(x))
			return(x);
		tempfree(x);
	}
}

Cell *forstat(Node **a, int n)	/* for (a[0]; a[1]; a[2]) a[3] */
{
	Cell *x;

	x = execute(a[0]);
	tempfree(x);
	for (;;) {
		if (a[1]!=0) {
			x = execute(a[1]);
			if (!istrue(x)) return(x);
			else tempfree(x);
		}
		x = execute(a[3]);
		if (isbreak(x))		/* turn off break */
			return True;
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
		x = execute(a[2]);
		tempfree(x);
	}
}

Cell *instat(Node **a, int n)	/* for (a[0] in a[1]) a[2] */
{
	Cell *x, *vp, *arrayp, *cp, *ncp;
	Array *tp;
	int i;

	vp = execute(a[0]);
	arrayp = execute(a[1]);
	if (!isarr(arrayp)) {
		return True;
	}
	tp = (Array *) arrayp->sval;
	tempfree(arrayp);
	for (i = 0; i < tp->size; i++) {	/* this routine knows too much */
		for (cp = tp->tab[i]; cp != NULL; cp = ncp) {
			setsval(vp, cp->nval);
			ncp = cp->cnext;
			x = execute(a[2]);
			if (isbreak(x)) {
				tempfree(vp);
				return True;
			}
			if (isnext(x) || isexit(x) || isret(x)) {
				tempfree(vp);
				return(x);
			}
			tempfree(x);
		}
	}
	return True;
}

Cell *bltin(Node **a, int n)	/* builtin functions. a[0] is type, a[1] is arg list */
{
	Cell *x, *y;
	Awkfloat u;
	int t;
	Awkfloat tmp;
	char *p, *buf;
	Node *nextarg;
	FILE *fp;
	void flush_all(void);

	t = ptoi(a[0]);
	x = execute(a[1]);
	nextarg = a[1]->nnext;
	switch (t) {
	case FLENGTH:
		if (isarr(x))
			u = ((Array *) x->sval)->nelem;	/* GROT.  should be function*/
		else
			u = strlen(getsval(x));
		break;
	case FLOG:
		u = errcheck(log(getfval(x)), "log"); break;
	case FINT:
		modf(getfval(x), &u); break;
	case FEXP:
		u = errcheck(exp(getfval(x)), "exp"); break;
	case FSQRT:
		u = errcheck(sqrt(getfval(x)), "sqrt"); break;
	case FSIN:
		u = sin(getfval(x)); break;
	case FCOS:
		u = cos(getfval(x)); break;
	case FATAN:
		if (nextarg == 0) {
			WARNING("atan2 requires two arguments; returning 1.0");
			u = 1.0;
		} else {
			y = execute(a[1]->nnext);
			u = atan2(getfval(x), getfval(y));
			tempfree(y);
			nextarg = nextarg->nnext;
		}
		break;
	case FSYSTEM:
		fflush(stdout);		/* in case something is buffered already */
		u = (Awkfloat) system(getsval(x)) / 256;   /* 256 is unix-dep */
		break;
	case FRAND:
		/* in principle, rand() returns something in 0..RAND_MAX */
		u = (Awkfloat) (rand() % RAND_MAX) / RAND_MAX;
		break;
	case FSRAND:
		if (isrec(x))	/* no argument provided */
			u = time((time_t *)0);
		else
			u = getfval(x);
		tmp = u;
		srand((unsigned int) u);
		u = srand_seed;
		srand_seed = tmp;
		break;
	case FTOUPPER:
	case FTOLOWER:
		buf = tostring(getsval(x));
		if (t == FTOUPPER) {
			for (p = buf; *p; p++)
				if (islower((uschar) *p))
					*p = toupper((uschar)*p);
		} else {
			for (p = buf; *p; p++)
				if (isupper((uschar) *p))
					*p = tolower((uschar)*p);
		}
		tempfree(x);
		x = gettemp();
		setsval(x, buf);
		free(buf);
		return x;
	case FFLUSH:
		if (isrec(x) || strlen(getsval(x)) == 0) {
			flush_all();	/* fflush() or fflush("") -> all */
			u = 0;
		} else if ((fp = openfile(FFLUSH, getsval(x))) == NULL)
			u = EOF;
		else
			u = fflush(fp);
		break;
	default:	/* can't happen */
		FATAL("illegal function type %d", t);
		break;
	}
	tempfree(x);
	x = gettemp();
	setfval(x, u);
	if (nextarg != 0) {
		WARNING("warning: function has too many arguments");
		for ( ; nextarg; nextarg = nextarg->nnext)
			execute(nextarg);
	}
	return(x);
}

Cell *printstat(Node **a, int n)	/* print a[0] */
{
	Node *x;
	Cell *y;
	FILE *fp;

	if (a[1] == 0)	/* a[1] is redirection operator, a[2] is file */
		fp = stdout;
	else
		fp = redirect(ptoi(a[1]), a[2]);
	for (x = a[0]; x != NULL; x = x->nnext) {
		y = execute(x);
		fputs(getpssval(y), fp);
		tempfree(y);
		if (x->nnext == NULL)
			fputs(*ORS, fp);
		else
			fputs(*OFS, fp);
	}
	if (a[1] != 0)
		fflush(fp);
	if (ferror(fp))
		FATAL("write error on %s", filename(fp));
	return(True);
}

Cell *nullproc(Node **a, int n)
{
	return 0;
}


FILE *redirect(int a, Node *b)	/* set up all i/o redirections */
{
	FILE *fp;
	Cell *x;
	char *fname;

	x = execute(b);
	fname = getsval(x);
	fp = openfile(a, fname);
	if (fp == NULL)
		FATAL("can't open file %s", fname);
	tempfree(x);
	return fp;
}

struct files {
	FILE	*fp;
	const char	*fname;
	int	mode;	/* '|', 'a', 'w' => LE/LT, GT */
} *files;

int nfiles;

void stdinit(void)	/* in case stdin, etc., are not constants */
{
	nfiles = FOPEN_MAX;
	files = calloc(nfiles, sizeof(*files));
	if (files == NULL)
		FATAL("can't allocate file memory for %u files", nfiles);
        files[0].fp = stdin;
	files[0].fname = "/dev/stdin";
	files[0].mode = LT;
        files[1].fp = stdout;
	files[1].fname = "/dev/stdout";
	files[1].mode = GT;
        files[2].fp = stderr;
	files[2].fname = "/dev/stderr";
	files[2].mode = GT;
}

FILE *openfile(int a, const char *us)
{
	const char *s = us;
	int i, m;
	FILE *fp = 0;

	if (*s == '\0')
		FATAL("null file name in print or getline");
	for (i=0; i < nfiles; i++)
		if (files[i].fname && strcmp(s, files[i].fname) == 0) {
			if (a == files[i].mode || (a==APPEND && files[i].mode==GT))
				return files[i].fp;
			if (a == FFLUSH)
				return files[i].fp;
		}
	if (a == FFLUSH)	/* didn't find it, so don't create it! */
		return NULL;

	for (i=0; i < nfiles; i++)
		if (files[i].fp == 0)
			break;
	if (i >= nfiles) {
		struct files *nf;
		int nnf = nfiles + FOPEN_MAX;
		nf = realloc(files, nnf * sizeof(*nf));
		if (nf == NULL)
			FATAL("cannot grow files for %s and %d files", s, nnf);
		memset(&nf[nfiles], 0, FOPEN_MAX * sizeof(*nf));
		nfiles = nnf;
		files = nf;
	}
	fflush(stdout);	/* force a semblance of order */
	m = a;
	if (a == GT) {
		fp = fopen(s, "w");
	} else if (a == APPEND) {
		fp = fopen(s, "a");
		m = GT;	/* so can mix > and >> */
	} else if (a == '|') {	/* output pipe */
		fp = popen(s, "w");
	} else if (a == LE) {	/* input pipe */
		fp = popen(s, "r");
	} else if (a == LT) {	/* getline <file */
		fp = strcmp(s, "-") == 0 ? stdin : fopen(s, "r");	/* "-" is stdin */
	} else	/* can't happen */
		FATAL("illegal redirection %d", a);
	if (fp != NULL) {
		files[i].fname = tostring(s);
		files[i].fp = fp;
		files[i].mode = m;
	}
	return fp;
}

const char *filename(FILE *fp)
{
	int i;

	for (i = 0; i < nfiles; i++)
		if (fp == files[i].fp)
			return files[i].fname;
	return "???";
}

Cell *closefile(Node **a, int n)
{
	Cell *x;
	int i, stat;

	x = execute(a[0]);
	getsval(x);
	stat = -1;
	for (i = 0; i < nfiles; i++) {
		if (files[i].fname && strcmp(x->sval, files[i].fname) == 0) {
			if (ferror(files[i].fp))
				WARNING( "i/o error occurred on %s", files[i].fname );
			if (files[i].mode == '|' || files[i].mode == LE)
				stat = pclose(files[i].fp);
			else
				stat = fclose(files[i].fp);
			if (stat == EOF)
				WARNING( "i/o error occurred closing %s", files[i].fname );
			if (i > 2)	/* don't do /dev/std... */
				xfree(files[i].fname);
			files[i].fname = NULL;	/* watch out for ref thru this */
			files[i].fp = NULL;
		}
	}
	tempfree(x);
	x = gettemp();
	setfval(x, (Awkfloat) stat);
	return(x);
}

void closeall(void)
{
	int i, stat;

	for (i = 0; i < FOPEN_MAX; i++) {
		if (files[i].fp) {
			if (ferror(files[i].fp))
				WARNING( "i/o error occurred on %s", files[i].fname );
			if (files[i].mode == '|' || files[i].mode == LE)
				stat = pclose(files[i].fp);
			else
				stat = fclose(files[i].fp);
			if (stat == EOF)
				WARNING( "i/o error occurred while closing %s", files[i].fname );
		}
	}
}

void flush_all(void)
{
	int i;

	for (i = 0; i < nfiles; i++)
		if (files[i].fp)
			fflush(files[i].fp);
}

void backsub(char **pb_ptr, char **sptr_ptr);

Cell *sub(Node **a, int nnn)	/* substitute command */
{
	char *sptr, *pb, *q;
	Cell *x, *y, *result;
	char *t, *buf;
	fa *pfa;
	int bufsz = recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in sub");
	x = execute(a[3]);	/* target string */
	t = getsval(x);
	if (a[0] == 0)		/* 0 => a[1] is already-compiled regexpr */
		pfa = (fa *) a[1];	/* regular expression */
	else {
		y = execute(a[1]);
		pfa = makedfa(getsval(y), 1);
		tempfree(y);
	}
	y = execute(a[2]);	/* replacement string */
	result = False;
	if (pmatch(pfa, t)) {
		sptr = t;
		adjbuf(&buf, &bufsz, 1+patbeg-sptr, recsize, 0, "sub");
		pb = buf;
		while (sptr < patbeg)
			*pb++ = *sptr++;
		sptr = getsval(y);
		while (*sptr != 0) {
			adjbuf(&buf, &bufsz, 5+pb-buf, recsize, &pb, "sub");
			if (*sptr == '\\') {
				backsub(&pb, &sptr);
			} else if (*sptr == '&') {
				sptr++;
				adjbuf(&buf, &bufsz, 1+patlen+pb-buf, recsize, &pb, "sub");
				for (q = patbeg; q < patbeg+patlen; )
					*pb++ = *q++;
			} else
				*pb++ = *sptr++;
		}
		*pb = '\0';
		if (pb > buf + bufsz)
			FATAL("sub result1 %.30s too big; can't happen", buf);
		sptr = patbeg + patlen;
		if ((patlen == 0 && *patbeg) || (patlen && *(sptr-1))) {
			adjbuf(&buf, &bufsz, 1+strlen(sptr)+pb-buf, 0, &pb, "sub");
			while ((*pb++ = *sptr++) != 0)
				;
		}
		if (pb > buf + bufsz)
			FATAL("sub result2 %.30s too big; can't happen", buf);
		setsval(x, buf);	/* BUG: should be able to avoid copy */
		result = True;;
	}
	tempfree(x);
	tempfree(y);
	free(buf);
	return result;
}

Cell *gsub(Node **a, int nnn)	/* global substitute */
{
	Cell *x, *y;
	char *rptr, *sptr, *t, *pb, *q;
	char *buf;
	fa *pfa;
	int mflag, tempstat, num;
	int bufsz = recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in gsub");
	mflag = 0;	/* if mflag == 0, can replace empty string */
	num = 0;
	x = execute(a[3]);	/* target string */
	t = getsval(x);
	if (a[0] == 0)		/* 0 => a[1] is already-compiled regexpr */
		pfa = (fa *) a[1];	/* regular expression */
	else {
		y = execute(a[1]);
		pfa = makedfa(getsval(y), 1);
		tempfree(y);
	}
	y = execute(a[2]);	/* replacement string */
	if (pmatch(pfa, t)) {
		tempstat = pfa->initstat;
		pfa->initstat = 2;
		pb = buf;
		rptr = getsval(y);
		do {
			if (patlen == 0 && *patbeg != 0) {	/* matched empty string */
				if (mflag == 0) {	/* can replace empty */
					num++;
					sptr = rptr;
					while (*sptr != 0) {
						adjbuf(&buf, &bufsz, 5+pb-buf, recsize, &pb, "gsub");
						if (*sptr == '\\') {
							backsub(&pb, &sptr);
						} else if (*sptr == '&') {
							sptr++;
							adjbuf(&buf, &bufsz, 1+patlen+pb-buf, recsize, &pb, "gsub");
							for (q = patbeg; q < patbeg+patlen; )
								*pb++ = *q++;
						} else
							*pb++ = *sptr++;
					}
				}
				if (*t == 0)	/* at end */
					goto done;
				adjbuf(&buf, &bufsz, 2+pb-buf, recsize, &pb, "gsub");
				*pb++ = *t++;
				if (pb > buf + bufsz)	/* BUG: not sure of this test */
					FATAL("gsub result0 %.30s too big; can't happen", buf);
				mflag = 0;
			}
			else {	/* matched nonempty string */
				num++;
				sptr = t;
				adjbuf(&buf, &bufsz, 1+(patbeg-sptr)+pb-buf, recsize, &pb, "gsub");
				while (sptr < patbeg)
					*pb++ = *sptr++;
				sptr = rptr;
				while (*sptr != 0) {
					adjbuf(&buf, &bufsz, 5+pb-buf, recsize, &pb, "gsub");
					if (*sptr == '\\') {
						backsub(&pb, &sptr);
					} else if (*sptr == '&') {
						sptr++;
						adjbuf(&buf, &bufsz, 1+patlen+pb-buf, recsize, &pb, "gsub");
						for (q = patbeg; q < patbeg+patlen; )
							*pb++ = *q++;
					} else
						*pb++ = *sptr++;
				}
				t = patbeg + patlen;
				if (patlen == 0 || *t == 0 || *(t-1) == 0)
					goto done;
				if (pb > buf + bufsz)
					FATAL("gsub result1 %.30s too big; can't happen", buf);
				mflag = 1;
			}
		} while (pmatch(pfa,t));
		sptr = t;
		adjbuf(&buf, &bufsz, 1+strlen(sptr)+pb-buf, 0, &pb, "gsub");
		while ((*pb++ = *sptr++) != 0)
			;
	done:	if (pb < buf + bufsz)
			*pb = '\0';
		else if (*(pb-1) != '\0')
			FATAL("gsub result2 %.30s truncated; can't happen", buf);
		setsval(x, buf);	/* BUG: should be able to avoid copy + free */
		pfa->initstat = tempstat;
	}
	tempfree(x);
	tempfree(y);
	x = gettemp();
	x->tval = NUM;
	x->fval = num;
	free(buf);
	return(x);
}

void backsub(char **pb_ptr, char **sptr_ptr)	/* handle \\& variations */
{						/* sptr[0] == '\\' */
	char *pb = *pb_ptr, *sptr = *sptr_ptr;

	if (sptr[1] == '\\') {
		if (sptr[2] == '\\' && sptr[3] == '&') { /* \\\& -> \& */
			*pb++ = '\\';
			*pb++ = '&';
			sptr += 4;
		} else if (sptr[2] == '&') {	/* \\& -> \ + matched */
			*pb++ = '\\';
			sptr += 2;
		} else {			/* \\x -> \\x */
			*pb++ = *sptr++;
			*pb++ = *sptr++;
		}
	} else if (sptr[1] == '&') {	/* literal & */
		sptr++;
		*pb++ = *sptr++;
	} else				/* literal \ */
		*pb++ = *sptr++;

	*pb_ptr = pb;
	*sptr_ptr = sptr;
}


/*** lex.c ***/

extern YYSTYPE	yylval;
extern int	infunc;

int	lineno	= 1;
int	bracecnt = 0;
int	brackcnt  = 0;
int	parencnt = 0;

typedef struct Keyword {
	const char *word;
	int	sub;
	int	type;
} Keyword;

Keyword keywords[] ={	/* keep sorted: binary searched */
	{ "BEGIN",	XBEGIN,		XBEGIN },
	{ "END",	XEND,		XEND },
	{ "NF",		VARNF,		VARNF },
	{ "atan2",	FATAN,		BLTIN },
	{ "break",	BREAK,		BREAK },
	{ "close",	CLOSE,		CLOSE },
	{ "continue",	CONTINUE,	CONTINUE },
	{ "cos",	FCOS,		BLTIN },
	{ "delete",	DELETE,		DELETE },
	{ "do",		DO,		DO },
	{ "else",	ELSE,		ELSE },
	{ "exit",	EXIT,		EXIT },
	{ "exp",	FEXP,		BLTIN },
	{ "fflush",	FFLUSH,		BLTIN },
	{ "for",	FOR,		FOR },
	{ "func",	FUNC,		FUNC },
	{ "function",	FUNC,		FUNC },
	{ "getline",	GETLINE,	GETLINE },
	{ "gsub",	GSUB,		GSUB },
	{ "if",		IF,		IF },
	{ "in",		IN,		IN },
	{ "index",	INDEX,		INDEX },
	{ "int",	FINT,		BLTIN },
	{ "length",	FLENGTH,	BLTIN },
	{ "log",	FLOG,		BLTIN },
	{ "match",	MATCHFCN,	MATCHFCN },
	{ "next",	NEXT,		NEXT },
	{ "nextfile",	NEXTFILE,	NEXTFILE },
	{ "print",	PRINT,		PRINT },
	{ "printf",	PRINTF,		PRINTF },
	{ "rand",	FRAND,		BLTIN },
	{ "return",	RETURN,		RETURN },
	{ "sin",	FSIN,		BLTIN },
	{ "split",	SPLIT,		SPLIT },
	{ "sprintf",	SPRINTF,	SPRINTF },
	{ "sqrt",	FSQRT,		BLTIN },
	{ "srand",	FSRAND,		BLTIN },
	{ "sub",	SUB,		SUB },
	{ "substr",	SUBSTR,		SUBSTR },
	{ "system",	FSYSTEM,	BLTIN },
	{ "tolower",	FTOLOWER,	BLTIN },
	{ "toupper",	FTOUPPER,	BLTIN },
	{ "while",	WHILE,		WHILE },
};

#define	RET(x)	{ if(dbg)printf("lex %s\n", tokname(x)); return(x); }

int peekch(void)
{
	int c = input();
	unput(c);
	return c;
}

int gettok(char **pbuf, int *psz)	/* get next input token */
{
	int c, retc;
	char *buf = *pbuf;
	int sz = *psz;
	char *bp = buf;

	c = input();
	if (c == 0)
		return 0;
	buf[0] = c;
	buf[1] = 0;
	if (!isalnum(c) && c != '.' && c != '_')
		return c;

	*bp++ = c;
	if (isalpha(c) || c == '_') {	/* it's a varname */
		for ( ; (c = input()) != 0; ) {
			if (bp-buf >= sz)
				if (!adjbuf(&buf, &sz, bp-buf+2, 100, &bp, "gettok"))
					FATAL( "out of space for name %.10s...", buf );
			if (isalnum(c) || c == '_')
				*bp++ = c;
			else {
				*bp = 0;
				unput(c);
				break;
			}
		}
		*bp = 0;
		retc = 'a';	/* alphanumeric */
	} else {	/* maybe it's a number, but could be . */
		char *rem;
		/* read input until can't be a number */
		for ( ; (c = input()) != 0; ) {
			if (bp-buf >= sz)
				if (!adjbuf(&buf, &sz, bp-buf+2, 100, &bp, "gettok"))
					FATAL( "out of space for number %.10s...", buf );
			if (isdigit(c) || c == 'e' || c == 'E' 
			  || c == '.' || c == '+' || c == '-')
				*bp++ = c;
			else {
				unput(c);
				break;
			}
		}
		*bp = 0;
		strtod(buf, &rem);	/* parse the number */
		if (rem == buf) {	/* it wasn't a valid number at all */
			buf[1] = 0;	/* return one character as token */
			retc = buf[0];	/* character is its own type */
			unputstr(rem+1); /* put rest back for later */
		} else {	/* some prefix was a number */
			unputstr(rem);	/* put rest back for later */
			rem[0] = 0;	/* truncate buf after number part */
			retc = '0';	/* type is number */
		}
	}
	*pbuf = buf;
	*psz = sz;
	return retc;
}

int	word(char *);
int	string(void);
int	regexpr(void);
int	sc	= 0;	/* 1 => return a } right now */
int	reg	= 0;	/* 1 => return a REGEXPR now */

int yylex(void)
{
	int c;
	static char *buf = 0;
	static int bufsize = 5; /* BUG: setting this small causes core dump! */

	if (buf == 0 && (buf = (char *) malloc(bufsize)) == NULL)
		FATAL( "out of space in yylex" );
	if (sc) {
		sc = 0;
		RET('}');
	}
	if (reg) {
		reg = 0;
		return regexpr();
	}
	for (;;) {
		c = gettok(&buf, &bufsize);
		if (c == 0)
			return 0;
		if (isalpha(c) || c == '_')
			return word(buf);
		if (isdigit(c)) {
			yylval.cp = setsymtab(buf, tostring(buf), atof(buf), CON|NUM, symtab);
			/* should this also have STR set? */
			RET(NUMBER);
		}
	
		yylval.i = c;
		switch (c) {
		case '\n':	/* {EOL} */
			RET(NL);
		case '\r':	/* assume \n is coming */
		case ' ':	/* {WS}+ */
		case '\t':
			break;
		case '#':	/* #.* strip comments */
			while ((c = input()) != '\n' && c != 0)
				;
			unput(c);
			break;
		case ';':
			RET(';');
		case '\\':
			if (peekch() == '\n') {
				input();
			} else if (peekch() == '\r') {
				input(); input();	/* \n */
				lineno++;
			} else {
				RET(c);
			}
			break;
		case '&':
			if (peekch() == '&') {
				input(); RET(AND);
			} else 
				RET('&');
		case '|':
			if (peekch() == '|') {
				input(); RET(BOR);
			} else
				RET('|');
		case '!':
			if (peekch() == '=') {
				input(); yylval.i = NE; RET(NE);
			} else if (peekch() == '~') {
				input(); yylval.i = NOTMATCH; RET(MATCHOP);
			} else
				RET(NOT);
		case '~':
			yylval.i = MATCH;
			RET(MATCHOP);
		case '<':
			if (peekch() == '=') {
				input(); yylval.i = LE; RET(LE);
			} else {
				yylval.i = LT; RET(LT);
			}
		case '=':
			if (peekch() == '=') {
				input(); yylval.i = EQ; RET(EQ);
			} else {
				yylval.i = ASSIGN; RET(ASGNOP);
			}
		case '>':
			if (peekch() == '=') {
				input(); yylval.i = GE; RET(GE);
			} else if (peekch() == '>') {
				input(); yylval.i = APPEND; RET(APPEND);
			} else {
				yylval.i = GT; RET(GT);
			}
		case '+':
			if (peekch() == '+') {
				input(); yylval.i = INCR; RET(INCR);
			} else if (peekch() == '=') {
				input(); yylval.i = ADDEQ; RET(ASGNOP);
			} else
				RET('+');
		case '-':
			if (peekch() == '-') {
				input(); yylval.i = DECR; RET(DECR);
			} else if (peekch() == '=') {
				input(); yylval.i = SUBEQ; RET(ASGNOP);
			} else
				RET('-');
		case '*':
			if (peekch() == '=') {	/* *= */
				input(); yylval.i = MULTEQ; RET(ASGNOP);
			} else if (peekch() == '*') {	/* ** or **= */
				input();	/* eat 2nd * */
				if (peekch() == '=') {
					input(); yylval.i = POWEQ; RET(ASGNOP);
				} else {
					RET(POWER);
				}
			} else
				RET('*');
		case '/':
			RET('/');
		case '%':
			if (peekch() == '=') {
				input(); yylval.i = MODEQ; RET(ASGNOP);
			} else
				RET('%');
		case '^':
			if (peekch() == '=') {
				input(); yylval.i = POWEQ; RET(ASGNOP);
			} else
				RET(POWER);

		case '$':
			/* BUG: awkward, if not wrong */
			c = gettok(&buf, &bufsize);
			if (isalpha(c)) {
				if (strcmp(buf, "NF") == 0) {	/* very special */
					unputstr("(NF)");
					RET(INDIRECT);
				}
				c = peekch();
				if (c == '(' || c == '[' || (infunc && isarg(buf) >= 0)) {
					unputstr(buf);
					RET(INDIRECT);
				}
				yylval.cp = setsymtab(buf, "", 0.0, STR|NUM, symtab);
				RET(IVAR);
			} else if (c == 0) {	/*  */
				SYNTAX( "unexpected end of input after $" );
				RET(';');
			} else {
				unputstr(buf);
				RET(INDIRECT);
			}
	
		case '}':
			if (--bracecnt < 0)
				SYNTAX( "extra }" );
			sc = 1;
			RET(';');
		case ']':
			if (--brackcnt < 0)
				SYNTAX( "extra ]" );
			RET(']');
		case ')':
			if (--parencnt < 0)
				SYNTAX( "extra )" );
			RET(')');
		case '{':
			bracecnt++;
			RET('{');
		case '[':
			brackcnt++;
			RET('[');
		case '(':
			parencnt++;
			RET('(');
	
		case '"':
			return string();	/* BUG: should be like tran.c ? */
	
		default:
			RET(c);
		}
	}
}

int string(void)
{
	int c, n;
	char *s, *bp;
	static char *buf = 0;
	static int bufsz = 500;

	if (buf == 0 && (buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of space for strings");
	for (bp = buf; (c = input()) != '"'; ) {
		if (!adjbuf(&buf, &bufsz, bp-buf+2, 500, &bp, "string"))
			FATAL("out of space for string %.10s...", buf);
		switch (c) {
		case '\n':
		case '\r':
		case 0:
			SYNTAX( "non-terminated string %.10s...", buf );
			lineno++;
			if (c == 0)	/* hopeless */
				FATAL( "giving up" );
			break;
		case '\\':
			c = input();
			switch (c) {
			case '"': *bp++ = '"'; break;
			case 'n': *bp++ = '\n'; break;	
			case 't': *bp++ = '\t'; break;
			case 'f': *bp++ = '\f'; break;
			case 'r': *bp++ = '\r'; break;
			case 'b': *bp++ = '\b'; break;
			case 'v': *bp++ = '\v'; break;
			case 'a': *bp++ = '\007'; break;
			case '\\': *bp++ = '\\'; break;

			case '0': case '1': case '2': /* octal: \d \dd \ddd */
			case '3': case '4': case '5': case '6': case '7':
				n = c - '0';
				if ((c = peekch()) >= '0' && c < '8') {
					n = 8 * n + input() - '0';
					if ((c = peekch()) >= '0' && c < '8')
						n = 8 * n + input() - '0';
				}
				*bp++ = n;
				break;

			case 'x':	/* hex  \x0-9a-fA-F + */
			    {	char xbuf[100], *px;
				for (px = xbuf; (c = input()) != 0 && px-xbuf < 100-2; ) {
					if (isdigit(c)
					 || (c >= 'a' && c <= 'f')
					 || (c >= 'A' && c <= 'F'))
						*px++ = c;
					else
						break;
				}
				*px = 0;
				unput(c);
	  			sscanf(xbuf, "%x", (unsigned int *) &n);
				*bp++ = n;
				break;
			    }

			default: 
				*bp++ = c;
				break;
			}
			break;
		default:
			*bp++ = c;
			break;
		}
	}
	*bp = 0; 
	s = tostring(buf);
	*bp++ = ' '; *bp++ = 0;
	yylval.cp = setsymtab(buf, s, 0.0, CON|STR|DONTFREE, symtab);
	RET(STRING);
}


int binsearch(char *w, Keyword *kp, int n)
{
	int cond, low, mid, high;

	low = 0;
	high = n - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((cond = strcmp(w, kp[mid].word)) < 0)
			high = mid - 1;
		else if (cond > 0)
			low = mid + 1;
		else
			return mid;
	}
	return -1;
}

int word(char *w) 
{
	Keyword *kp;
	int c, n;

	n = binsearch(w, keywords, sizeof(keywords)/sizeof(keywords[0]));
/* BUG: this ought to be inside the if; in theory could fault (daniel barrett) */
	kp = keywords + n;
	if (n != -1) {	/* found in table */
		yylval.i = kp->sub;
		switch (kp->type) {	/* special handling */
		case BLTIN:
			if (kp->sub == FSYSTEM && safe)
				SYNTAX( "system is unsafe" );
			RET(kp->type);
		case FUNC:
			if (infunc)
				SYNTAX( "illegal nested function" );
			RET(kp->type);
		case RETURN:
			if (!infunc)
				SYNTAX( "return not in function" );
			RET(kp->type);
		case VARNF:
			yylval.cp = setsymtab("NF", "", 0.0, NUM, symtab);
			RET(VARNF);
		default:
			RET(kp->type);
		}
	}
	c = peekch();	/* look for '(' */
	if (c != '(' && infunc && (n=isarg(w)) >= 0) {
		yylval.i = n;
		RET(ARG);
	} else {
		yylval.cp = setsymtab(w, "", 0.0, STR|NUM|DONTFREE, symtab);
		if (c == '(') {
			RET(CALL);
		} else {
			RET(VAR);
		}
	}
}

void startreg(void)	/* next call to yylex will return a regular expression */
{
	reg = 1;
}

int regexpr(void)
{
	int c;
	static char *buf = 0;
	static int bufsz = 500;
	char *bp;

	if (buf == 0 && (buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of space for rex expr");
	bp = buf;
	for ( ; (c = input()) != '/' && c != 0; ) {
		if (!adjbuf(&buf, &bufsz, bp-buf+3, 500, &bp, "regexpr"))
			FATAL("out of space for reg expr %.10s...", buf);
		if (c == '\n') {
			SYNTAX( "newline in regular expression %.10s...", buf ); 
			unput('\n');
			break;
		} else if (c == '\\') {
			*bp++ = '\\'; 
			*bp++ = input();
		} else {
			*bp++ = c;
		}
	}
	*bp = 0;
	if (c == 0)
		SYNTAX("non-terminated regular expression %.10s...", buf);
	yylval.s = tostring(buf);
	unput('/');
	RET(REGEXPR);
}

/* low-level lexical stuff, sort of inherited from lex */

char	ebuf[300];
char	*ep = ebuf;
char	yysbuf[100];	/* pushback buffer */
char	*yysptr = yysbuf;
FILE	*yyin = 0;

int input(void)	/* get next lexical input character */
{
	int c;
	extern char *lexprog;

	if (yysptr > yysbuf)
		c = (uschar)*--yysptr;
	else if (lexprog != NULL) {	/* awk '...' */
		if ((c = (uschar)*lexprog) != 0)
			lexprog++;
	} else				/* awk -f ... */
		c = pgetc();
	if (c == '\n')
		lineno++;
	else if (c == EOF)
		c = 0;
	if (ep >= ebuf + sizeof ebuf)
		ep = ebuf;
	return *ep++ = c;
}

void unput(int c)	/* put lexical character back on input */
{
	if (c == '\n')
		lineno--;
	if (yysptr >= yysbuf + sizeof(yysbuf))
		FATAL("pushed back too much: %.20s...", yysbuf);
	*yysptr++ = c;
	if (--ep < ebuf)
		ep = ebuf + sizeof(ebuf) - 1;
}

void unputstr(const char *s)	/* put a string back on input */
{
	int i;

	for (i = strlen(s)-1; i >= 0; i--)
		unput(s[i]);
}


/*** main.c ***/

const char	*version = "version 20121220";

extern	char	**environ;
extern	int	nfields;

int	dbg	= 0;
Awkfloat	srand_seed = 1;
char	*cmdname;	/* gets argv[0] for error messages */
extern	FILE	*yyin;	/* lex input file */
char	*lexprog;	/* points to program argument if it exists */
extern	int errorflag;	/* non-zero if any syntax errors; set by yyerror */
int	compile_time = 2;	/* for error printing: */
				/* 2 = cmdline, 1 = compile, 0 = running */

#define	MAX_PFILE	20	/* max number of -f's */

char	*pfile[MAX_PFILE];	/* program filenames from -f's */
int	npfile = 0;	/* number of filenames */
int	curpfile = 0;	/* current filename */

int	safe	= 0;	/* 1 => "safe" mode */

GLOBALS(
  char *fsep;
  struct arg_list *v;
  struct arg_list *f;
)

void awk_main()
{
	struct arg_list *al;
	char **args = toys.optargs;
	const char *fs = NULL;

	cmdname = "awk";

	if (toys.optflags & FLAG_version) {
		xprintf("awk %s\n", version);
		return;
	}

	signal(SIGFPE, fpecatch);

	srand_seed = 1;
	srand(srand_seed);

	yyin = NULL;
	symtab = makesymtab(NSYMTAB/NSYMTAB);

	/* XXX: -safe, -d# */

	/* -f args go in pfile */
	/* -F arg goes in fs */

	if (TT.fsep) {
		if (TT.fsep[0] == 't' && TT.fsep[1] == 0) /* wart: t=>\t */
			fs = "\t";
		else if (TT.fsep[0] != 0)
			fs = &TT.fsep[0];
	}

	for (al = TT.v; al; al = al->next)
		setclvar(al->arg);

	if (TT.f) {
		for (al = TT.f; al; al = al->next) {
			if (npfile >= MAX_PFILE - 1)
				error_exit("too many -f options");
			pfile[npfile++] = al->arg;
		}
	}
	else {
		if (!args[0])
			error_exit("no program given");
		lexprog = args[0];
		args++;
	}

	recinit(recsize);
	syminit();
	compile_time = 1;
	arginit(args);
	if (!safe)
		envinit(environ);
	yyparse();
	setlocale(LC_NUMERIC, ""); /* back to whatever it is locally */
	if (fs)
		*FS = qstring(fs, '\0');
	   dprintf( ("errorflag=%d\n", errorflag) );
	if (errorflag == 0) {
		compile_time = 0;
		run(winner);
	} else
		bracecheck();

	/* XXX: return(errorflag); */
}

int pgetc(void)		/* get 1 character from awk program */
{
	int c;

	for (;;) {
		if (yyin == NULL) {
			if (curpfile >= npfile)
				return EOF;
			if (strcmp(pfile[curpfile], "-") == 0)
				yyin = stdin;
			else if ((yyin = fopen(pfile[curpfile], "r")) == NULL)
				FATAL("can't open file %s", pfile[curpfile]);
			lineno = 1;
		}
		if ((c = getc(yyin)) != EOF)
			return c;
		if (yyin != stdin)
			fclose(yyin);
		yyin = NULL;
		curpfile++;
	}
}

char *cursource(void)	/* current source file name */
{
	if (npfile > 0)
		return pfile[curpfile];
	else
		return NULL;
}
