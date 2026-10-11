#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "trace.h"

#include <stdio.h>
#include <stdlib.h>

/* Compatibilite Windows / Linux */
#ifdef _WIN32
  #define popen  _popen
  #define pclose _pclose
  #define GP_VERIF "where gnuplot > NUL 2>&1"
#else
  #define GP_VERIF "command -v gnuplot > /dev/null 2>&1"
#endif

FILE *gp_ouvrir(void) {
    if (system(GP_VERIF) != 0) {
        fprintf(stderr, "gp_ouvrir : gnuplot est introuvable\n");
        return NULL;
    }
    FILE *gp = popen("gnuplot", "w");
    if (gp == NULL) {
        perror("gp_ouvrir: popen");
    }
    return gp;
}

/* Trace un, deux ou trois algorithmes sur une echelle lineaire. */
void gp_tracer_n_tris(FILE *gp, const char *titre, const char *png,
                      Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                      const char *noms_tris[NB_TRIS_PAR_GRAPHE],
                      int nb_tris) {
    if (gp == NULL || nb_tris < 1 || nb_tris > NB_TRIS_PAR_GRAPHE) {
        return;
    }

    fprintf(gp, "set terminal pngcairo size 1200,800\n");
    fprintf(gp, "set output '%s'\n", png);
    fprintf(gp, "set title '%s'\n", titre);
    fprintf(gp, "set xlabel 'Taille du tableau (n)'\n");
    fprintf(gp, "set ylabel 'Temps (ns)'\n");
    fprintf(gp, "set grid\nset key top left\n");
    fprintf(gp, "unset logscale xy\n");

    fprintf(gp, "plot ");
    for (int k = 0; k < nb_tris; k++) {
        fprintf(gp, "%s'-' with linespoints title '%s'",
                k > 0 ? ", " : "", noms_tris[k]);
    }
    fprintf(gp, "\n");

    for (int k = 0; k < nb_tris; k++) {
        for (int i = 0; i < NB_TAILLES; i++) {
            fprintf(gp, "%g %g\n", courbes[k][i].x, courbes[k][i].y);
        }
        fprintf(gp, "e\n");
    }

    fprintf(gp, "unset output\n");
    fflush(gp);
}

int gp_fermer(FILE *gp) {
    if (gp == NULL) {
        return -1;
    }
    fflush(gp);
    return pclose(gp) == 0 ? 0 : -1;
}
