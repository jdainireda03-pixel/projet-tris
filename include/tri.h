/*
 * tri.h - Module de tri : algorithmes, compteurs et table des algorithmes.
 *
 * Tous les tris ont la meme signature : void tri_xxx(int *t, size_t n);
 * Ils trient le tableau t (n entiers) dans l'ordre croissant, en place
 * (sauf le tri fusion qui utilise un tampon de taille n).
 */
#ifndef TRI_H
#define TRI_H

#include <stddef.h>

/* ------------------------------------------------------------------ */
/* Compteurs                                                           */
/* ------------------------------------------------------------------ */
/*
 * Ces macros n'existent vraiment que si on compile avec -DCOMPTAGE.
 * Sinon elles ne font rien : le tri chronometre n'est donc pas ralenti.
 *
 * Regle de comptage :
 *  - INC_CMP : une comparaison entre deux elements du tableau ;
 *  - INC_ECH : un echange de deux elements compte 1 ;
 *              une copie ou un decalage d'un element compte 1.
 */
#ifdef COMPTAGE
extern unsigned long long g_comparaisons, g_echanges;
#define INC_CMP() (g_comparaisons++)
#define INC_ECH() (g_echanges++)
#else
#define INC_CMP() ((void)0)
#define INC_ECH() ((void)0)
#endif

/* ------------------------------------------------------------------ */
/* Table des algorithmes (pointeurs de fonction)                       */
/* ------------------------------------------------------------------ */
typedef void (*fonction_tri)(int *t, size_t n);

typedef struct {
    const char *nom;   /* "insertion" */
    fonction_tri f;    /* pointeur vers le tri */
    size_t n_max;      /* plus grande taille autorisee */
} AlgoTri;

extern const AlgoTri ALGOS[];
extern const size_t NB_ALGOS;

/* ------------------------------------------------------------------ */
/* Algorithmes de tri                                                  */
/* ------------------------------------------------------------------ */
void tri_selection(int *t, size_t n);  /* O(n^2), au plus n-1 echanges   */
void tri_insertion(int *t, size_t n);  /* O(n^2), O(n) si deja trie      */
void tri_bulles(int *t, size_t n);     /* O(n^2), arret anticipe         */
void tri_fusion(int *t, size_t n);     /* O(n log n), stable, tampon n   */
void tri_rapide(int *t, size_t n);     /* O(n log n) en moyenne          */

#endif /* TRI_H */
