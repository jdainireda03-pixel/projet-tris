/*
 * tri.c - Implementation des algorithmes de tri.
 *
 * Algorithmes : selection, insertion, bulles, fusion, rapide.
 * Chaque comparaison est precedee de INC_CMP(), chaque echange ou
 * deplacement d'element est accompagne de INC_ECH() (voir tri.h).
 */
#include "tri.h"

#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Constantes                                                          */
/* ------------------------------------------------------------------ */
#define N_MAX_QUADRATIQUE 32000UL   /* tailles max des tris en O(n^2)   */
#define N_MAX_LINEARITHM  1024000UL /* tailles max des tris en O(n log n) */

/* ------------------------------------------------------------------ */
/* Compteurs (definis seulement si -DCOMPTAGE)                         */
/* ------------------------------------------------------------------ */
#ifdef COMPTAGE
unsigned long long g_comparaisons = 0;
unsigned long long g_echanges = 0;
#endif

/* ------------------------------------------------------------------ */
/* Outil commun                                                        */
/* ------------------------------------------------------------------ */

/*
 * echanger : echange les valeurs pointees par a et b (compte 1 echange).
 */
static inline void echanger(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
    INC_ECH();
}

/* ------------------------------------------------------------------ */
/* Tri par selection                                                   */
/* ------------------------------------------------------------------ */

/*
 * tri_selection : tri par selection classique.
 * Parametres : t le tableau, n sa taille.
 * Invariant : apres i tours, t[0..i-1] contient les i plus petits
 *             elements, tries.
 * Fait exactement n(n-1)/2 comparisons et au plus n-1 echanges.
 */
void tri_selection(int *t, size_t n)
{
    if (n < 2) {
        return;
    }
    for (size_t i = 0; i + 1 < n; i++) {
        size_t min = i;
        for (size_t j = i + 1; j < n; j++) {
            INC_CMP();
            if (t[j] < t[min]) {
                min = j;
            }
        }
        if (min != i) {
            echanger(&t[i], &t[min]);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Tri par insertion                                                   */
/* ------------------------------------------------------------------ */

/*
 * tri_insertion : tri par insertion, version par decalage.
 * Parametres : t le tableau, n sa taille.
 * Invariant : au debut du tour i, t[0..i-1] est trie et contient les
 *             elements d'origine de ces cases.
 * On decale les elements plus grands que x d'une case vers la droite
 * (1 deplacement chacun), puis on place x (1 deplacement s'il a bouge).
 * Tableau trie : n-1 comparaisons. Tableau inverse : n(n-1)/2.
 */
void tri_insertion(int *t, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        int x = t[i];
        size_t j = i;
        while (j > 0) {
            INC_CMP();
            if (t[j - 1] <= x) {
                break;
            }
            t[j] = t[j - 1];
            INC_ECH();
            j--;
        }
        if (j != i) {
            t[j] = x;
            INC_ECH();
        }
    }
}

/* ------------------------------------------------------------------ */
/* Tri a bulles                                                        */
/* ------------------------------------------------------------------ */

/*
 * tri_bulles : tri a bulles avec arret anticipe.
 * Parametres : t le tableau, n sa taille.
 * Invariant : apres k passes, les k plus grands elements sont a leur
 *             place definitive, a la fin du tableau.
 * Arret anticipe : si une passe ne fait aucun echange, le tableau est
 * trie (n-1 comparaisons sur un tableau deja trie).
 */
void tri_bulles(int *t, size_t n)
{
    if (n < 2) {
        return;
    }
    size_t fin = n;     /* t[fin..n-1] est deja a sa place */
    int echange;
    do {
        echange = 0;
        for (size_t j = 1; j < fin; j++) {
            INC_CMP();
            if (t[j - 1] > t[j]) {
                echanger(&t[j - 1], &t[j]);
                echange = 1;
            }
        }
        fin--;
    } while (echange && fin > 1);
}

/* ------------------------------------------------------------------ */
/* Tri fusion                                                          */
/* ------------------------------------------------------------------ */

/*
 * fusionner : fusionne les deux moities triees t[debut..milieu-1] et
 * t[milieu..fin-1] grace au tampon tmp.
 * Stable : en cas d'egalite, on prend l'element de gauche.
 * Invariant : les elements deja ecrits dans tmp sont les plus petits
 *             des deux moities, dans l'ordre.
 */
static void fusionner(int *t, int *tmp, size_t debut, size_t milieu, size_t fin)
{
    size_t i = debut;
    size_t j = milieu;
    size_t k = debut;

    while (i < milieu && j < fin) {
        INC_CMP();
        if (t[i] <= t[j]) {
            tmp[k++] = t[i++];
        } else {
            tmp[k++] = t[j++];
        }
        INC_ECH();
    }
    while (i < milieu) {
        tmp[k++] = t[i++];
        INC_ECH();
    }
    while (j < fin) {
        tmp[k++] = t[j++];
        INC_ECH();
    }
    for (k = debut; k < fin; k++) {
        t[k] = tmp[k];
        INC_ECH();
    }
}

/*
 * fusion_rec : tri fusion recursif (descendant) de t[debut..fin-1].
 * Le tampon tmp est fourni par l'appelant (alloue une seule fois).
 */
static void fusion_rec(int *t, int *tmp, size_t debut, size_t fin)
{
    if (fin - debut < 2) {
        return;
    }
    size_t milieu = debut + (fin - debut) / 2;
    fusion_rec(t, tmp, debut, milieu);
    fusion_rec(t, tmp, milieu, fin);
    fusionner(t, tmp, debut, milieu, fin);
}

/*
 * tri_fusion : tri fusion, stable, avec un seul tampon de taille n
 * alloue par appel (l'allocation fait partie du cout mesure).
 * Parametres : t le tableau, n sa taille.
 * En cas d'echec de malloc, le programme s'arrete avec un message.
 */
void tri_fusion(int *t, size_t n)
{
    if (n < 2) {
        return;
    }
    int *tmp = malloc(n * sizeof *tmp);
    if (tmp == NULL) {
        perror("tri_fusion: malloc");
        exit(EXIT_FAILURE);
    }
    fusion_rec(t, tmp, 0, n);
    free(tmp);
}

/* ------------------------------------------------------------------ */
/* Tri rapide                                                          */
/* ------------------------------------------------------------------ */

/*
 * partition_hoare : choisit le pivot par mediane de trois (premier,
 * milieu, dernier), puis partitionne t[lo..hi] (bornes incluses) avec
 * le schema de Hoare.
 * Retour : un indice j tel que lo <= j < hi, et tout element de
 *          t[lo..j] est <= tout element de t[j+1..hi].
 * Les indices sont de type ptrdiff_t car j peut descendre sous 0.
 * Les elements egaux au pivot arretent les deux balayages : les
 * tableaux a doublons restent bien partages en deux.
 */
static ptrdiff_t partition_hoare(int *t, ptrdiff_t lo, ptrdiff_t hi)
{
    ptrdiff_t mid = lo + (hi - lo) / 2;

    /* Mediane de trois : on ordonne t[lo] <= t[mid] <= t[hi]. */
    INC_CMP();
    if (t[mid] < t[lo]) {
        echanger(&t[lo], &t[mid]);
    }
    INC_CMP();
    if (t[hi] < t[lo]) {
        echanger(&t[lo], &t[hi]);
    }
    INC_CMP();
    if (t[hi] < t[mid]) {
        echanger(&t[mid], &t[hi]);
    }
    int pivot = t[mid];

    ptrdiff_t i = lo - 1;
    ptrdiff_t j = hi + 1;
    for (;;) {
        do {
            i++;
            INC_CMP();
        } while (t[i] < pivot);
        do {
            j--;
            INC_CMP();
        } while (t[j] > pivot);
        if (i >= j) {
            return j;
        }
        echanger(&t[i], &t[j]);
    }
}

/*
 * rapide_rec : tri rapide sur t[lo..hi] (bornes incluses).
 * On rappelle la fonction sur la plus petite partie et on boucle sur
 * la plus grande : la pile reste en O(log n).
 * Invariant : apres la partition, tout element de la partie gauche est
 *             inferieur ou egal a tout element de la partie droite.
 */
static void rapide_rec(int *t, ptrdiff_t lo, ptrdiff_t hi)
{
    while (lo < hi) {
        ptrdiff_t j = partition_hoare(t, lo, hi);
        if (j - lo + 1 < hi - j) {          /* gauche plus petite */
            rapide_rec(t, lo, j);
            lo = j + 1;
        } else {                            /* droite plus petite */
            rapide_rec(t, j + 1, hi);
            hi = j;
        }
    }
}

/*
 * tri_rapide : tri rapide, pivot median de trois, partition de Hoare.
 * Parametres : t le tableau, n sa taille.
 */
void tri_rapide(int *t, size_t n)
{
    if (n < 2) {
        return;
    }
    rapide_rec(t, 0, (ptrdiff_t)n - 1);
}

/* ------------------------------------------------------------------ */
/* Table des algorithmes                                               */
/* ------------------------------------------------------------------ */
/* Ajouter un tri = ajouter une ligne dans cette table. */
const AlgoTri ALGOS[] = {
    { "selection", tri_selection, N_MAX_QUADRATIQUE },
    { "insertion", tri_insertion, N_MAX_QUADRATIQUE },
    { "bulles",    tri_bulles,    N_MAX_QUADRATIQUE },
    { "fusion",    tri_fusion,    N_MAX_LINEARITHM },
    { "rapide",    tri_rapide,    N_MAX_LINEARITHM },
};

const size_t NB_ALGOS = sizeof ALGOS / sizeof ALGOS[0];
