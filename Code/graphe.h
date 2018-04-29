#ifndef GRAPHE_H_
#define GRAPHE_H_
#include "types.h"

///trouve un arbre de steiner avec un algo genetique
/// time = temps max alloue, verbose = affichage des donnees de debug
void noeuds_steiner_gene(graphe* g, const int time, const int verbose, int* valeurSolution, int* nbAretes, arete* aretes_sol);

///trouve un arbre de steiner avec une recherche locale
/// time = temps max alloue, verbose = affichage des donnees de debug
void noeuds_steiner_local(graphe* g, const int time, const int verbose, int* valeurSolution, int* nbAretes, arete* aretes_sol);

#endif /* GRAPHE_H_ */
