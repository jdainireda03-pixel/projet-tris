/*
 * main.c - Compare les 6 tris sur 4 jeux de donnees.
 * Produit des graphiques separes pour les tris lents et un pour les tris rapides.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "donnees.h"
#include "mesure.h"
#include "trace.h"
#include "tri.h"

#define NB_TRIS 6
#define NB_TRIS_LENTS 3
#define NB_TRIS_RAPIDES 3

#define NB_JEUX_TRACES 4
#define TAILLE_MAX_RAPIDES 100000
#define TAILLE_MAX_LENTS 32000
#define GRAINE 12345

/* Mesure les algorithmes du groupe pour une taille donnee. */
static void mesurer_taille(const Jeu *jeu, size_t n, int indice,
                           int debut_algo, int nb_tris,
                           Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES]) {
    /* malloc(0) peut retourner NULL : allouer au moins une case. */
    size_t capacite = n > 0 ? n : 1;
    int *entree = malloc(capacite * sizeof *entree);
    int *travail = malloc(capacite * sizeof *travail);
    int *ref = malloc(capacite * sizeof *ref);
    if (entree == NULL || travail == NULL || ref == NULL) {
        perror("malloc");
        free(entree);
        free(travail);
        free(ref);
        exit(EXIT_FAILURE);
    }

    uint64_t etat = GRAINE;
    jeu->f(entree, n, &etat);
    mes_reference(ref, entree, n);

    for (int k = 0; k < nb_tris; k++) {
        int algo = debut_algo + k;
        array_copie(travail, entree, n);
        courbes[k][indice].x = (double)n;
        courbes[k][indice].y = (double)mes_temps_tri(ALGOS[algo].f, travail, n);
        if (mes_verifier(travail, ref, n) != 0) {
            fprintf(stderr, "ERREUR : le tri %s est faux (jeu %s, n=%zu)\n",
                    ALGOS[algo].nom, jeu->nom, n);
            free(entree);
            free(travail);
            free(ref);
            exit(EXIT_FAILURE);
        }
    }

    free(entree);
    free(travail);
    free(ref);
}

static int tracer_groupe(FILE *gp, const Jeu *jeu, int debut_algo,
                         int nb_tris, size_t taille_max, const char *suffixe) {
    Point courbes[NB_TRIS_PAR_GRAPHE][NB_TAILLES];
    const char *noms[NB_TRIS_PAR_GRAPHE];

    for (int k = 0; k < nb_tris; k++) {
        noms[k] = ALGOS[debut_algo + k].nom;
    }

    for (int i = 0; i < NB_TAILLES; i++) {
        /* NB_TAILLES points regulierement repartis de 0 a taille_max inclus. */
        size_t n = (size_t)i * taille_max / (NB_TAILLES - 1);
        mesurer_taille(jeu, n, i, debut_algo, nb_tris, courbes);
    }

    char titre[160];
    char fichier[160];
    snprintf(titre, sizeof titre, "Tris %s - jeu %s", suffixe, jeu->nom);
    snprintf(fichier, sizeof fichier, "fig_%s_%s.png", jeu->nom, suffixe);
    gp_tracer_n_tris(gp, titre, fichier, courbes, noms, nb_tris);
    printf("jeu %s : graphique %s termine (0 a %zu)\n", jeu->nom, suffixe, taille_max);
    return 0;
}

int main(void) {
    if (NB_TAILLES < 2) {
        fprintf(stderr, "main : NB_TAILLES doit etre au moins egal a 2\n");
        return 1;
    }
   if (NB_ALGOS < 6 || NB_JEUX < NB_JEUX_TRACES) {
        fprintf(stderr, "main : il faut au moins 6 tris et 4 jeux de donnees\n");
        return 1;
    }

    FILE *gp = gp_ouvrir();
    if (gp == NULL) {
        return 1;
    }

    for (int j = 0; j < NB_JEUX_TRACES; j++) {
        /* Sur le jeu inverse (indice 2), les trois tris lents restent ensemble. */
        if (j == 2) {
            tracer_groupe(gp, &JEUX[j], 0, 3, TAILLE_MAX_LENTS, "lents");
        } else {
            /* Selection seule pour garder une echelle lisible. */
            tracer_groupe(gp, &JEUX[j], 0, 1, TAILLE_MAX_LENTS, "selection");
            /* Insertion et bulles sur un graphique commun. */
            tracer_groupe(gp, &JEUX[j], 1, 2, TAILLE_MAX_LENTS, "insertion_bulles");
        }
        /* Tris plus efficaces : fusion, rapide, tas. */
        tracer_groupe(gp, &JEUX[j], 3, 3, TAILLE_MAX_RAPIDES, "rapides");
    }

    return gp_fermer(gp) == 0 ? 0 : 1;
}
