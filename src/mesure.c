#define _POSIX_C_SOURCE 200809L

#include "mesure.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NS_PAR_SECONDE 1000000000LL

/*
 * cmp_int : comparateur de qsort pour des int.
 * On ne fait JAMAIS a - b : avec INT_MIN et INT_MAX la soustraction deborde
 * (comportement indefini). (a > b) - (a < b) vaut -1, 0 ou 1 sans risque.
 */
static int cmp_int(const void *a, const void *b) {
  int x = *(const int *)a;
  int y = *(const int *)b;
  return (x > y) - (x < y);
}

/* cmp_int64 : meme principe pour des int64_t (utilise par la mediane). */
static int cmp_int64(const void *a, const void *b) {
  int64_t x = *(const int64_t *)a;
  int64_t y = *(const int64_t *)b;
  return (x > y) - (x < y);
}

/*
 * mes_maintenant_ns : lit CLOCK_MONOTONIC et convertit en nanosecondes.
 * Retour : l'instant courant (int64_t). Le calcul se fait en 64 bits :
 * en 32 bits, secondes * 10^9 deborderait au bout de 2 secondes.
 * Si l'horloge est indisponible, le programme s'arrete avec un message.
 */
int64_t mes_maintenant_ns(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    perror("mes_maintenant_ns: clock_gettime");
    exit(EXIT_FAILURE);
  }
  return (int64_t)ts.tv_sec * NS_PAR_SECONDE + (int64_t)ts.tv_nsec;
}

/*
 * mes_temps_tri : chronometre un seul appel f(t, n).
 * Parametres : f (tri), t (tableau, trie en place), n (taille).
 * Retour : duree en nanosecondes. Rien d'autre n'est place entre les deux
 * lectures de l'horloge : copie, generation et verification restent dehors.
 */
int64_t mes_temps_tri(fonction_tri f, int *t, size_t n) {
  int64_t debut = mes_maintenant_ns();
  f(t, n);
  int64_t fin = mes_maintenant_ns();
  return fin - debut;
}

/*
 * mes_oracle : fabrique le resultat attendu (copie triee par qsort).
 * Parametres : oracle (sortie, n entiers), entree (non modifiee), n (taille).
 */
void mes_oracle(int *oracle, const int *entree, size_t n) {
  if (n == 0) {
    return;
  }
  memcpy(oracle, entree, n * sizeof(int));
  qsort(oracle, n, sizeof(int), cmp_int);
}

/*
 * mes_verifier : compare t a l'oracle case par case.
 * Une egalite totale prouve a la fois que t est trie et qu'il contient
 * les memes valeurs que l'entree.
 * Retour : 0 si identiques ; sinon (indice de la premiere case fausse) + 1.
 */
int mes_verifier(const int *t, const int *oracle, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (t[i] != oracle[i]) {
      return (int)(i + 1);
    }
  }
  return 0;
}

/*
 * mes_mediane : mediane de k valeurs (k > 0). Le tableau v est trie en place.
 * Retour : v[k/2]. Pour k = 11 c'est la 6e valeur, une vraie mesure.
 * Pour k pair, c'est la moyenne haute (pas de moyenne calculee).
 */
int64_t mes_mediane(int64_t *v, size_t k) {
  qsort(v, k, sizeof(int64_t), cmp_int64);
  return v[k / 2];
}
