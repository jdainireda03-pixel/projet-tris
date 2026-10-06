#ifndef MESURE_H
#define MESURE_H

#include <stddef.h>
#include <stdint.h>

#include "tri.h"

/*
 * mes_maintenant_ns : lit l'horloge monotone.
 * Retour : l'instant courant en nanosecondes (int64_t). Ne recule jamais.
 */
int64_t mes_maintenant_ns(void);

/*
 * mes_temps_tri : chronometre un seul appel f(t, n).
 * Parametres : f (tri a mesurer), t (tableau a trier, modifie en place), n
 * (taille). Retour : duree de l'appel en nanosecondes. Seul f(t, n) est
 * chronometre.
 */
int64_t mes_temps_tri(fonction_tri f, int *t, size_t n);

/*
 * mes_reference : fabrique le resultat attendu avec qsort.
 * Parametres : reference (sortie, n entiers), entree (tableau d'origine, non
 * modifie), n (taille). Retour : aucun. A appeler avant le tri, hors de la zone
 * chronometree.
 */
void mes_reference(int *reference, const int *entree, size_t n);

/*
 * mes_verifier : compare le resultat d'un tri a la reference, case par case.
 * Parametres : t (resultat du tri), reference (resultat attendu), n (taille).
 * Retour : 0 si identiques ; sinon (indice de la premiere case fausse) + 1.
 */
int mes_verifier(const int *t, const int *reference, size_t n);

/*
 * mes_mediane : mediane de k valeurs.
 * Parametres : v (valeurs, TRIEES EN PLACE par la fonction), k (nombre de
 * valeurs, > 0). Retour : la valeur du milieu (la 6e pour k = 11).
 */
int64_t mes_mediane(int64_t *v, size_t k);

#endif /* MESURE_H */
