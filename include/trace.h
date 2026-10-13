#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>

/* Un graphique contient les 6 tris, avec jusqu'a NB_TAILLES points par courbe. */
#define NB_TRIS_PAR_GRAPHE 6
#define NB_TAILLES 28

/* Un point (taille du tableau, temps de tri en nanosecondes). */
typedef struct {
    double x;
    double y;
} Point;

FILE *gp_ouvrir(void);
/* Trace les 6 tris d'un jeu, en lineaire (loglog=0) ou log-log (loglog=1).
 * nb_pts[k] = nombre de points valides de la courbe k (0..NB_TAILLES).
 * y est en ns en entree, affiche en ms. Couleurs stables par algo.
 * xmin/xmax : plage x imposee (zoom) ; 0/0 = auto ([900:*]). */
void gp_tracer_jeu(FILE *gp, const char *titre, const char *png,
                   Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                   const char *noms_tris[NB_TRIS_PAR_GRAPHE],
                   const int nb_pts[NB_TRIS_PAR_GRAPHE],
                   int nb_tris, int loglog, double xmin, double xmax);
/* Compatibilite : trace avec un seul nb_tailles commun, echelle lineaire. */
void gp_tracer_n_tris(FILE *gp, const char *titre, const char *png,
                      Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                      const char *noms_tris[NB_TRIS_PAR_GRAPHE],
                      int nb_tris, int nb_tailles);
int gp_fermer(FILE *gp);

#endif
