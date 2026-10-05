#ifndef TRI_H
#define TRI_H

#include <stddef.h>

/* Type commun a tous les tris : trie t[0..n-1] en place, ordre croissant. */
typedef void (*fonction_tri)(int *t, size_t n);

/* Une ligne de la table des algorithmes. */
typedef struct {
  const char *nom; /* nom utilise dans les CSV, sans accent : "insertion" */
  fonction_tri f;  /* pointeur vers le tri */
  size_t n_max;    /* plus grande taille autorisee pour ce tri */
} AlgoTri;

/*
 * Compteurs : ces macros n'existent vraiment qu'avec -DCOMPTAGE.
 * Sans cette option elles ne font rien, donc le tri chronometre n'est pas
 * ralenti. INC_CMP() precede chaque comparaison entre deux elements du tableau.
 * INC_ECH() accompagne chaque echange ou deplacement d'un element.
 */
#ifdef COMPTAGE
extern unsigned long long g_comparaisons, g_echanges;
#define INC_CMP() (g_comparaisons++)
#define INC_ECH() (g_echanges++)
#else
#define INC_CMP() ((void)0)
#define INC_ECH() ((void)0)
#endif

/* Les tris. Chacun accepte n = 0, 1, 2 et tous les jeux de donnees. */
void tri_selection(int *t, size_t n);
void tri_insertion(int *t, size_t n);
void tri_bulles(int *t, size_t n);
void tri_fusion(int *t, size_t n);
void tri_rapide(int *t, size_t n);
void tri_tas(int *t, size_t n);

/* Table des algorithmes (definie dans tri.c) et son nombre d'elements. */
extern const AlgoTri ALGOS[];
extern const size_t NB_ALGOS;

#endif /* TRI_H */
