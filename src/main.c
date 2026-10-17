/*
 * main.c - Banc d'essai : les 6 tris sur les 5 jeux de donnees.
 * Un graphique par jeu (tous les tris ensemble), en log-log.
 * Tailles denses : 1000, 1500, 2000, ... jusqu'a 10 240 000 pour les
 * tris rapides, plafonnees a 32 000 pour les tris quadratiques (n_max).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "donnees.h"
#include "mesure.h"
#include "trace.h"
#include "tri.h"

#define GRAINE 12345

/* 28 tailles croissantes de 1000 a 10 240 000. Les tailles au-dela
 * de 32 000 ne concernent que les trois tris en O(n log n). */
static const size_t TAILLES[] = {
    1000,    1500,    2000,    3000,    4000,    6000,    8000,
    12000,   16000,   24000,   32000,   48000,   64000,   96000,
    128000,  192000,  256000,  384000,  512000,  768000,  1024000,
    1536000, 2048000, 3072000, 4096000, 6144000, 8192000, 10240000};
#define NB_TAILLES_TABLE (sizeof TAILLES / sizeof TAILLES[0])

/* 1 chauffe + NB_MESURES runs par point, on garde la mediane (spec §5.4).
 * Indispensable : une mesure unique capte le bruit de la machine, ce qui
 * deformait les courbes (ex. pic du tri par tas a 8M sur le jeu aleatoire). */
#define NB_CHAUFFES 1
#define NB_MESURES 11

/* Mesure un seul tri sur une taille : 0 si ok, -1 si n > n_max (skip).
 * Attribution : pt->y = mediane des NB_MESURES durees (ns). */
static int mesurer_un(const Jeu *jeu, const int *entree, const int *ref,
                      size_t n, int algo, Point *pt) {
  if (n > ALGOS[algo].n_max) {
    return -1;
  }
  size_t capacite = n > 0 ? n : 1;
  int *travail = malloc(capacite * sizeof *travail);
  if (travail == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  int64_t durees[NB_MESURES];
  for (int r = -NB_CHAUFFES; r < NB_MESURES; r++) {
    /* Le tri modifie travail : on repart de entree a chaque run. */
    array_copie(travail, entree, n);
    int64_t ns = mes_temps_tri(ALGOS[algo].f, travail, n);
    if (mes_verifier(travail, ref, n) != 0) {
      fprintf(stderr, "ERREUR : le tri %s est faux (jeu %s, n=%zu)\n",
              ALGOS[algo].nom, jeu->nom, n);
      free(travail);
      exit(EXIT_FAILURE);
    }
    if (r >= 0) {
      durees[r] = ns;
    }
  }
  int64_t ns = mes_mediane(durees, NB_MESURES);
  pt->x = (double)n;
  pt->y = (double)ns;
  free(travail);
  return 0;
}

static int tracer_jeu(FILE *gp, const Jeu *jeu) {
  Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES];
  const char *noms[NB_TRIS_PAR_GRAPHE];
  int nb_pts[NB_TRIS_PAR_GRAPHE] = {0};
  int nb_tris = (int)NB_ALGOS;

  if (nb_tris > NB_TRIS_PAR_GRAPHE) {
    nb_tris = NB_TRIS_PAR_GRAPHE;
  }
  for (int k = 0; k < nb_tris; k++) {
    noms[k] = ALGOS[k].nom;
  }

  /* Pour chaque taille : une seule entree + un oracle partages par
   * les 6 tris (spec §5.1 : meme tableau pour tous les algos). */
  for (size_t ti = 0; ti < NB_TAILLES_TABLE; ti++) {
    size_t n = TAILLES[ti];
    size_t capacite = n > 0 ? n : 1;
    int *entree = malloc(capacite * sizeof *entree);
    int *ref = malloc(capacite * sizeof *ref);
    if (entree == NULL || ref == NULL) {
      perror("malloc");
      free(entree);
      free(ref);
      exit(EXIT_FAILURE);
    }
    uint64_t etat = GRAINE;
    jeu->f(entree, n, &etat);
    mes_reference(ref, entree, n);

    for (int k = 0; k < nb_tris; k++) {
      Point pt;
      if (mesurer_un(jeu, entree, ref, n, k, &pt) == 0) {
        courbes[k][nb_pts[k]++] = pt;
      }
    }
    free(entree);
    free(ref);
  }

  char titre[200];
  char fichier_log[200];
  snprintf(titre, sizeof titre, "Temps de tri - jeu %s (comparaison)",
           jeu->nom);
  snprintf(fichier_log, sizeof fichier_log, "resultats/fig_%s_loglog.png",
           jeu->nom);

  /* Un seul graphique par jeu : log-log. Echelle ideale pour comparer
   * les complexites O(n^2) et O(n log n) sur un meme graphe. */
  gp_tracer_jeu(gp, titre, fichier_log, courbes, noms, nb_pts, nb_tris, 1, 0,
                0);
  printf("jeu %s : %s (%d points, max %zu)\n", jeu->nom, fichier_log,
         nb_pts[nb_tris - 1], TAILLES[NB_TAILLES_TABLE - 1]);
  return 0;
}

int main(void) {
  if (NB_ALGOS < 6) {
    fprintf(stderr, "main : il faut au moins 6 tris\n");
    return 1;
  }
  if (NB_JEUX < 5) {
    fprintf(stderr, "main : il faut les 5 jeux de donnees\n");
    return 1;
  }
  if (NB_TAILLES_TABLE > NB_TAILLES) {
    fprintf(stderr, "main : table des tailles trop grande (%zu > %d)\n",
            NB_TAILLES_TABLE, NB_TAILLES);
    return 1;
  }

  /* resultats/ doit exister avant le miroir dernier_script.gp. */
  mkdir("resultats", 0755);
  /* Nouveau script gnuplot a chaque campagne. */
  remove("resultats/dernier_script.gp");

  FILE *gp = gp_ouvrir();
  if (gp == NULL) {
    return 1;
  }

  for (size_t j = 0; j < NB_JEUX; j++) {
    tracer_jeu(gp, &JEUX[j]);
  }

  return gp_fermer(gp) == 0 ? 0 : 1;
}
