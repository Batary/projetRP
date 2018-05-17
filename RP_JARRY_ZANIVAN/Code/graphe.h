#ifndef GRAPHE_H_
#define GRAPHE_H_
#include "types.h"

///trouve un arbre de steiner avec un algo genetique
/// time = temps max alloue, verbose = affichage des donnees de debug
void noeuds_steiner_gene(graphe* g, int heuristique, String dest, String filename, const int time, const int verbose, int* valeurSolution, int* nbAretes, arete* aretes_sol);

///trouve un arbre de steiner avec une recherche locale
/// time = temps max alloue, verbose = affichage des donnees de debug
void noeuds_steiner_local(graphe* g, int heuristique, String dest,String filename, const int time, const int verbose, int* valeurSolution, int* nbAretes, arete* aretes_sol);



void generer_population_heuristique_ACPM_one(graphe* g, int* noeudsactifs, const int verbose);

void noeuds_steiner_local_one(graphe* g, int heuristique, String dest, String filename,FILE *f, double tempslancement, const double maxTime,double alea,int bestsol, const int verbose,
/*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes, int* lastvalwritten, int* lasttimewritten);


#endif /* GRAPHE_H_ */
