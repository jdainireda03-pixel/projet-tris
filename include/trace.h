#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>

/* Nombre de tris dessines par graphique et de points par courbe. */
#define NB_TRIS_PAR_GRAPHE 3
#define NB_TAILLES 11

/* Un point (taille du tableau, temps de tri en nanosecondes). */
typedef struct {
    double x;
    double y;
} Point;

FILE *gp_ouvrir(void);
void gp_tracer_n_tris(FILE *gp, const char *titre, const char *png,
                      Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                      const char *noms_tris[NB_TRIS_PAR_GRAPHE],
                      int nb_tris);
int gp_fermer(FILE *gp);

#endif
