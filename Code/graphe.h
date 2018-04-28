#ifndef GRAPHE_H_
#define GRAPHE_H_
#include "types.h"

///trouve un arbre de steiner avec un algo genetique
/// time = temps max alloue, verbose = affichage des donnees de debug
int* noeuds_steiner_gene(graphe* g, int time, int verbose);

///trouve un arbre de steiner avec une recherche locale
/// time = temps max alloue, verbose = affichage des donnees de debug
int* noeuds_steiner_local(graphe* g, int time, int verbose);

#endif /* GRAPHE_H_ */
