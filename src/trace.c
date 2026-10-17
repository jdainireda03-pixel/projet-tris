#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "trace.h"

#include <stdio.h>
#include <stdlib.h>

/* Compatibilite Windows / Linux */
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define GP_VERIF "where gnuplot > NUL 2>&1"
#else
#define GP_VERIF "command -v gnuplot > /dev/null 2>&1"
#endif

/* Couleurs stables par algorithme (meme ordre que ALGOS : selection,
 * insertion, bulles, fusion, rapide, tas). */
static const char *COULEURS[NB_TRIS_PAR_GRAPHE] = {
    "#e41a1c", "#377eb8", "#4daf4a", "#984ea3", "#ff7f00", "#a65628"};

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

/* Recopie les commandes envoyees a gnuplot dans resultats/dernier_script.gp
 * (aide au debogage, exige par le cahier §2.4). */
static void gp_miroir(const char *s) {
  FILE *f = fopen("resultats/dernier_script.gp", "a");
  if (f == NULL) {
    return;
  }
  fputs(s, f);
  fclose(f);
}

static void gp_envoi(FILE *gp, const char *s) {
  fputs(s, gp);
  gp_miroir(s);
}

void gp_tracer_jeu(FILE *gp, const char *titre, const char *png,
                   Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                   const char *noms_tris[NB_TRIS_PAR_GRAPHE],
                   const int nb_pts[NB_TRIS_PAR_GRAPHE], int nb_tris,
                   int loglog, double xmin, double xmax) {
  if (gp == NULL || nb_tris < 1 || nb_tris > NB_TRIS_PAR_GRAPHE) {
    return;
  }
  for (int k = 0; k < nb_tris; k++) {
    if (nb_pts[k] < 0 || nb_pts[k] > NB_TAILLES) {
      return;
    }
  }

  char buf[256];
  snprintf(buf, sizeof buf,
           "set terminal pngcairo size 1400,900 enhanced font 'Sans,12'\n");
  gp_envoi(gp, buf);
  snprintf(buf, sizeof buf, "set output '%s'\n", png);
  gp_envoi(gp, buf);
  snprintf(buf, sizeof buf, "set title '%s'\n", titre);
  gp_envoi(gp, buf);
  gp_envoi(gp, "set xlabel \"Nombre d'elements\"\n");
  /* Temps converti ns -> ms a l'envoi : lisible de 0.001 ms a 10 000 ms. */
  gp_envoi(gp, "set ylabel 'Temps (ms)'\n");
  gp_envoi(gp, "set grid xtics ytics mxtics mytics lt 0 lw 0.5\n");
  gp_envoi(gp, "set key top left Left reverse samplen 2 spacing 1.2\n");
  /* Sans plage imposee on demarre a 900 : supprime le vide 0-900 qui
   * ecrasait les courbes quadratiques (plafonnees a 32k) contre l'axe y
   * quand le max rapide est a 10M. */
  if (xmax > xmin) {
    snprintf(buf, sizeof buf, "set xrange [%g:%g]\n", xmin, xmax);
    gp_envoi(gp, buf);
  } else {
    gp_envoi(gp, "set xrange [900:*]\n");
  }
  if (loglog) {
    gp_envoi(gp, "set logscale xy\n");
    /* X : etiquettes lisibles au lieu de 10^{%L}. */
    gp_envoi(gp, "set xtics ("
                 "'1k' 1000, '2k' 2000, '5k' 5000, "
                 "'10k' 10000, '20k' 20000, '50k' 50000, "
                 "'100k' 100000, '200k' 200000, '500k' 500000, "
                 "'1M' 1000000, '2M' 2000000, '5M' 5000000, "
                 "'10M' 10000000"
                 ") rotate by -45\n");
    /* Y : valeurs lisibles en ms au lieu de 10^{%L}. */
    gp_envoi(gp, "set ytics ("
                 "'0.001' 0.001, '0.01' 0.01, '0.1' 0.1, "
                 "'1' 1, '10' 10, '100' 100, "
                 "'1000' 1000, '10000' 10000"
                 ")\n");
    gp_envoi(gp, "set format x '%s'\nset format y '%g'\n");
  } else {
    gp_envoi(gp, "unset logscale xy\n");
    gp_envoi(gp, "set format x '%g'\nset format y '%g'\n");
  }

  /* Une courbe par tri, couleur et symbole stables (lc rgb + pt). */
  gp_envoi(gp, "plot ");
  for (int k = 0; k < nb_tris; k++) {
    snprintf(buf, sizeof buf,
             "%s'-' with linespoints lc rgb '%s' pt %d ps 1.5 lw 2 title '%s'",
             k > 0 ? ", " : "", COULEURS[k], k + 4, noms_tris[k]);
    gp_envoi(gp, buf);
  }
  gp_envoi(gp, "\n");

  for (int k = 0; k < nb_tris; k++) {
    for (int i = 0; i < nb_pts[k]; i++) {
      double x = courbes[k][i].x;
      /* ns -> ms pour l'axe y. En log, on saute les temps <= 0. */
      double y_ms = courbes[k][i].y / 1e6;
      if (loglog && y_ms <= 0.0) {
        continue;
      }
      snprintf(buf, sizeof buf, "%g %g\n", x, y_ms);
      gp_envoi(gp, buf);
    }
    gp_envoi(gp, "e\n");
  }

  gp_envoi(gp, "unset output\n");
  fflush(gp);
}

void gp_tracer_n_tris(FILE *gp, const char *titre, const char *png,
                      Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES],
                      const char *noms_tris[NB_TRIS_PAR_GRAPHE], int nb_tris,
                      int nb_tailles) {
  int nb_pts[NB_TRIS_PAR_GRAPHE];
  if (nb_tailles < 0) {
    nb_tailles = 0;
  }
  if (nb_tailles > NB_TAILLES) {
    nb_tailles = NB_TAILLES;
  }
  for (int k = 0; k < NB_TRIS_PAR_GRAPHE; k++) {
    nb_pts[k] = nb_tailles;
  }
  gp_tracer_jeu(gp, titre, png, courbes, noms_tris, nb_pts, nb_tris, 0, 0, 0);
}

int gp_fermer(FILE *gp) {
  if (gp == NULL) {
    return -1;
  }
  fflush(gp);
  return pclose(gp) == 0 ? 0 : -1;
}
