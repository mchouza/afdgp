/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Wayne A. Christopher
**********/

/*
 * The main routine for spice 3 and nutmeg.
 */

#include "spice.h"
#include "misc.h"
#include "ifsim.h"
#include "inpdefs.h"
#include "iferrmsg.h"
#include "cpdefs.h"
#include "ftedefs.h"
#include "ftedev.h"
#include "ftedebug.h"
#include "const.h"
#include <setjmp.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>

#include "patchlev.h"
#include "suffix.h"

/* POR MMC */
#include "../memfile.h"
#include <assert.h>


int Patch_Level = PATCHLEVEL;
char *ft_rawfile = "rawspice.raw";

char *errRtn;
char *errMsg;
char *cp_program;

bool ft_servermode = false;
bool ft_intrpt = false;     /* Set by the (void) signal handlers. */
bool ft_setflag = false;    /* Don't abort after an interrupt. */

struct variable *(*if_getparam)( );

DISPDEVICE device[ ] = {
	{"", 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0 }
	};
DISPDEVICE *dispdev = device;
bool ft_nopage = false;
bool ft_nomod = false;
bool ft_parsedb = false;
bool ft_evdb = false;
bool ft_vecdb = false;
bool ft_grdb = false;
bool ft_gidb = false;
bool ft_controldb = false;
bool ft_asyncdb = false;

bool ft_debug = false;
bool ft_batchmode = true;
bool ft_nospiceadd = true;
bool ft_simdb = false;
static char *usage = "Usage: %s [ - ] [ -r rawfile ] [ file ... ]\n";
FILE *cp_curin = 0;
FILE *cp_curout = 0;
FILE *cp_curerr = 0;
FILE *cp_in = 0;
FILE *cp_out = 0;
FILE *cp_err = 0;
bool gi_endpause = true;

bool ft_listprint = true;
bool ft_optsprint = false;
bool ft_nodesprint = false;
bool ft_acctprint = false;
jmp_buf jbuf; /* POR MMC */

struct plot *plot_list = NULL;
int plot_num = 1;

static started = false;
IFsimulator *ft_sim = 0;

extern IFsimulator SIMinfo;

bool ft_nutmeg = false;
struct comm *cp_coms = NULL; /* POR MMC */
extern int OUTpBeginPlot(), OUTpData(), OUTwBeginPlot(), OUTwReference();
extern int OUTwData(), OUTwEnd(), OUTendPlot(), OUTbeginDomain();
extern int OUTendDomain(), OUTstopnow(), OUTerror(), OUTattributes();
static IFfrontEnd nutmeginfo = {
    IFnewUid,
    IFdelUid,
    OUTstopnow,
    seconds,
    OUTerror,
    OUTpBeginPlot,
    OUTpData,
    OUTwBeginPlot,
    OUTwReference,
    OUTwData,
    OUTwEnd,
    OUTendPlot,
    OUTbeginDomain,
    OUTendDomain,
    OUTattributes
};

char *hlp_filelist[] = { "spice", 0 };

/* Hace el trabajo que normalmente haría main */
/* Estoy tratando de probar el funcionamiento del spice, eventualmente
   le voy a mandar los datos en binario y voy a recibirlos de igual forma */
/* Le mando los datos como string */
int processNetList(const char* netlist_in_memory,
				   const char* output_filename, FILE* nulFP, int binaryResults)
{
	/* Archivo de entrada */
	FILE*	file;
	/* Archivo de salida */
	MEMFILE* mfp;

	/* Marca que se inició */
    started = true;

	/* Pone o no el rawfile en binario */
	AsciiRawFile = !binaryResults;

    /* Inicializa variables de entorno */
	ivars();

	/* Le da valor a los file pointers de salida */
	/*
    cp_in = stdin;
    cp_out = stdout;
    cp_err = stderr;
    cp_curin = stdin;
    cp_curout = stdout;
    cp_curerr = stderr;
	*/
    cp_in = stdin;
    cp_out = nulFP;
    cp_err = nulFP;
    cp_curin = stdin;
    cp_curout = nulFP;
    cp_curerr = nulFP;

	/* Limpio el circuito actual (necesario por la reutilización */
	ft_curckt = NULL;

    /* Inicializa el simulador y lo conecta con el front-end */
	if (SIMinit(&nutmeginfo,&ft_sim) != OK)
		return EXIT_BAD;

    /* Obtiene el nombre del programa */
	cp_program = ft_sim->simulator;

	/* Inicializa los números aleatorios */
	srandom((unsigned int)time(0));

    /* Es la función que devuelve parámetros del circuito */
	if_getparam = spif_getparam;

    /* Indico que la ejecucuión no es interactiva */
	cp_interactive = false;

	/* Carga la netlist */
	/*inp_spsource(file, false, netlist_filename);*/
	inp_spsource_from_mem(netlist_in_memory, false);

    /* Realiza la simulación */
	if (ft_dorun(output_filename))
		return EXIT_BAD;

    /* Todo OK */
	return EXIT_NORMAL;
}

/* allocate space for global constants in 'CONST.h' */

double CONSTroot2;
double CONSTvt0;
double CONSTKoverQ;
double CONSTe;
IFfrontEnd *SPfrontEnd = NULL;

int
SPIinit(frtEnd,description)
    IFfrontEnd *frtEnd;
    IFsimulator **description;
{

    SPfrontEnd = frtEnd;
    *description = &SIMinfo;
    CONSTroot2 = sqrt(2.);
    CONSTvt0 = CONSTboltz * (27 /* deg c */ + CONSTCtoK ) / CHARGE;
    CONSTKoverQ = CONSTboltz / CHARGE;
    CONSTe = exp((double)1.0);
    return(OK);
}

/* XXX SIMinit and SPIinit ?? */

int SIMinit(frontEnd,simulator)
    IFfrontEnd *frontEnd;
    IFsimulator **simulator;
{
    return(SPIinit(frontEnd,simulator));
}


/*
 * Incredibly Ugly
 */

/* Now some misc junk that we need to fake */

bool cp_nocc = true;
bool cp_debug = false;
int cp_maxhistlength = 0;
char cp_chars[128];

MFBHalt( ) { }


/* ARGSUSED */ char *cp_tildexpand(s) char *s; { return (s); }
/* ARGSUSED */ struct dvec *vec_fromplot(w, p) char *w; struct plot *p;
        { return (NULL); }
void vec_new() {}
void plot_new() {}
void plot_setcur() {}
/* ARGSUSED */ struct plot *plot_alloc(name) char *name; { return (NULL); }
struct plot *plot_cur = NULL;
/* ARGSUSED */ void plot_docoms(wl) wordlist *wl; {}
void out_init() {}

void
out_printf(fmt, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10)
    char *fmt, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9, *s10;
{
    printf(fmt, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10);
}

/* ARGSUSED */ void out_send(s) char *s; {}
char out_pbuf[1];
/* ARGSUSED */ struct dvec *vec_get(word) char *word; { return (NULL); }

/* ARGSUSED */ int cp_usrset(v, i) struct variable *v; bool i; {return(US_OK);}
void cp_pushcontrol() {}
void cp_popcontrol() {}
/* ARGSUSED */ void cp_addkword(class, word) int class; char *word; {}
struct circ *ft_circuits = 0, *ft_curckt = 0;
/* ARGSUSED */ char *cp_kwswitch(c, t) int c; char *t; { return (NULL); }
/* ARGSUSED */ bool ft_bpcheck(r, i) struct plot *r; int i; { return (true); }
/* ARGSUSED */ void cp_ccon(b) bool b; {}
/* ARGSUSED */ wordlist *cp_cctowl(stuff) char *stuff; { return NULL; }

int currentgraph = 0;
/* ARGSUSED */ void Input(p, q) char *p, *q; {}
/* ARGSUSED */ void DevSwitch(i) int i; {}
/* ARGSUSED */ int *CopyGraph(i) int i; {}
/* ARGSUSED */ void DestroyGraph(i) int i; {}
/* ARGSUSED */ void NewViewport(i) int i; {}

/* ARGSUSED */ void cp_remkword() { }
/* ARGSUSED */ void cp_resetcontrol() { }

bool gr_init() {}
void gr_redraw() {}
/* ARGSUSED */ void gr_iplot(pl) struct plot *pl; {}
/* ARGSUSED */ void gr_end_iplot(pl) struct plot *pl; {}
/* ARGSUSED */ void gr_pmsg(text, more) char *text; bool more; {}
/* ARGSUSED */ void gr_clean(intr) bool intr; {}
/* ARGSUSED */ int cp_evloop(string) char *string; { return (0); }
void ft_graf() {}
void ft_trquery() {}
/* ARGSUSED */ void ft_newcirc(ckt) struct circ *ckt; {}
void cp_doquit() { exit(0); }
/* ARGSUSED */ void cp_usrvars(v1, v2) struct variable **v1, **v2; { return; }
/* ARGSUSED */ struct variable * cp_enqvar(word) char *word; { return (NULL); }
/* ARGSUSED */ void cp_ccom(w, b, e) wordlist *w; char *b; bool e; { return; }
/* ARGSUSED */ void com_resume( ) { return; }
/* ARGSUSED */ int if_sens_run( ) { return 0; }
/* ARGSUSED */ int ft_xgraph( ) { return 0; }
/* ARGSUSED */ int gr_resize( ) { return 0; }

/* This is from options.c */

/* Extract the .option cards from the deck... */

struct line *
inp_getopts(deck)
    struct line *deck;
{
    struct line *last = NULL, *opts = NULL, *dd, *next = NULL;

    for (dd = deck->li_next; dd; dd = next) {
        next = dd->li_next;
        if (ciprefix(".opt", dd->li_line)) {
            inp_casefix(dd->li_line);
            if (last)
                last->li_next = dd->li_next;
            else
                deck->li_next = dd->li_next;
            dd->li_next = opts;
            opts = dd;
        } else
            last = dd;
    }
    return (opts);
}


/* This is from dotcards.c -- we don't want to include the whole file. */

static wordlist *gettoks();

static wordlist *
gettoks(s)
    char *s;
{
    char *t, *r, buf[64];
    wordlist *wl = NULL, *end = NULL;
    bool iflag;

    while (t = gettok(&s)) {
        if (*t == '(' /* ) */) {
            /* This is a (upper, lower) thing -- ignore. */
            continue;
        } else if (!index(t, '(' /*)*/ )) {
            if (end) {
                end->wl_next = alloc(struct wordlist);
                end->wl_next->wl_prev = end;
                end = end->wl_next;
            } else
                wl = end = alloc(struct wordlist);
            end->wl_word = copy(t);
        } else if (!index(t, ',')) {
            iflag = ((*t == 'i') || (*t == 'I')) ? true : false;
            while (*t != '(' /*)*/)
                t++;
            t++;
            for (r = t; *r && *r != /*(*/ ')'; r++)
                ;
            *r = '\0';
            if (end) {
                end->wl_next = alloc(struct wordlist);
                end->wl_next->wl_prev = end;
                end = end->wl_next;
            } else
                wl = end = alloc(struct wordlist);
            if (iflag) {
                (void) sprintf(buf, "%s#branch", t);
                t = buf;
            }
            end->wl_word = copy(t);
        } else {
            /* The painful case... */
            while (*t != '(' /*)*/)
                t++;
            t++;
            for (r = t; *r && *r != ','; r++)
                ;
            *r = '\0';
            if (end) {
                end->wl_next = alloc(struct wordlist);
                end->wl_next->wl_prev = end;
                end = end->wl_next;
            } else
                wl = end = alloc(struct wordlist);
            end->wl_word = copy(t);
            t = r + 1;
            for (r = t; *r && *r != /*(*/ ')'; r++)
                ;
            *r = '\0';
            if (end) {
                end->wl_next = alloc(struct wordlist);
                end->wl_next->wl_prev = end;
                end = end->wl_next;
            } else
                wl = end = alloc(struct wordlist);
            end->wl_word = copy(t);
        }
    }
    return (wl);
}
