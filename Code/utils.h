#ifndef UTILS_H_
#define UTILS_H_

#define TAILLE_POPULATION 100
#define CHANCE_MUTATION 0.02

void freeGraphe(graphe* g);

double generer_uniforme(double x, double y);

int trouverParent(const int x, int* parents);
void unionEnsembles(int parentX, int parentY, int* parents, int* rangs);
int partitionner(arete* tableau, int p, int r);
void quickSort(arete* tableau, int p, int r);
int partitionnerPopulation(int** pop, int* tableau, int p, int r);
void quickSort_pop(int** pop, int* tableau, int p, int r);

int argminQ(int* tab, int* Q, int taille);

//renvoie 1 si il existe une arete entre ces deux sommets, 0 sinon
// version optimisée
int isarete(graphe* g, int n1, int n2);

//convertit un encodage des sommmets non-terminaux en une solution de taille g->nbnoeuds
void convertpartielletofullsolution(graphe* g,int* solS, int* solfull);
void convertfulltopartiellesolution(graphe* g, int* solfull, int* solpartielle);

void printtabint(int * tab, int size);

void resetIndividu(graphe* g, int* indi);
void copieIndividu(graphe* g, int* source, int* cible);

#endif /* UTILS_H_ */

