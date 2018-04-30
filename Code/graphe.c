#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "types.h"

/*
	NOTE : tous les indices sont decales de -1 par rapport au fichier d'entree.
*/

///trouve le parent (= l'id de l'ensemble qui le contient) d'un element
int trouverParent(const int x, int* parents){
	if(parents[x] != x)
		parents[x] = trouverParent(parents[x], parents);
	return parents[x];
}

///fusionne deux ensembles de sommets
void unionEnsembles(int parentX, int parentY, int* parents, int* rangs){
	if(parentX == parentY) return;	//les ensembles sont deja les memes

	//on prend le parent de rang le plus eleve pour reduire la profondeur de l'arbre
	if(rangs[parentX] < rangs[parentY]){
		int temp = parentX;
		parentX = parentY;
		parentY = temp;
	}

	//on fusionne et on augmente le rang si les deux ont le meme
	parents[parentY] = parentX;
	if(rangs[parentX] == rangs[parentY]) rangs[parentX]++;
}

//une implementation du quicksort pour le tri des aretes, adaptee de wikipedia
int partitionner(arete* tableau, int p, int r) {
    int pivot = tableau[p].poids, i = p-1, j = r+1;
    arete temp;
    while (1) {
        do
            j--;
        while (tableau[j].poids > pivot);
        do
            i++;
        while (tableau[i].poids < pivot);
        if (i < j) {
            temp = tableau[i];
            tableau[i] = tableau[j];
            tableau[j] = temp;
        }
        else
            return j;
    }
}

void quickSort(arete* tableau, int p, int r) {
    int q;
    if (p < r) {
        q = partitionner(tableau, p, r);
        quickSort(tableau, p, q);
        quickSort(tableau, q+1, r);
    }
}

///algorithme de kruskal
int evaluerSolution(graphe* g, int* solution,
						/*constantes et tableaux conteneurs :*/ const int M, int* parents, int* rangs, arete* aretesTab,
						/*sorties :*/ arete* aretesSol, int* nbAretesSol)
{
	int valeurSol = 0;
	*nbAretesSol = 0; //nombre d'aretes du sous-graphe, doit etre egal au nombre de sommets - 1

	for(int i = 0; i < g->nbNoeuds; i++){
		parents[i] = i;
	}

	//récupération des aretes disponibles
	//on parcourt les sommets choisis et on regarde si les autres extremites des aretes sont egalement dans le sous-graphe
	int na = 0, nbNoeudsSol = 0;
	for(int i = 0; i < g->nbNoeuds; i++){
		if(solution[i]){
			nbNoeudsSol++;
			for(int j = 0; j < g->noeuds[i].nbAretes; j++){
				if( solution[g->noeuds[i].aretes[j].noeud1->id] && solution[g->noeuds[i].aretes[j].noeud2->id]){
					aretesTab[na] = g->noeuds[i].aretes[j];
					na++;
				}
			}
		}
	}

	quickSort(aretesTab, 0, na - 1);

	int k = 0;
	//parcours des aretes
	while( k < na && *nbAretesSol < nbNoeudsSol - 1 ){
		//test cycle
		if(trouverParent(aretesTab[k].noeud1->id, parents) != trouverParent(aretesTab[k].noeud2->id, parents)){
			aretesSol[*nbAretesSol] = aretesTab[k];
			*nbAretesSol+=1;
			valeurSol += aretesTab[k].poids;
			unionEnsembles(aretesTab[k].noeud1->id, aretesTab[k].noeud2->id, parents, rangs);
		}
		k++;
	}

	valeurSol += (nbNoeudsSol - 1 - *nbAretesSol) * M;

	return valeurSol;
}

///algo genetique
void noeuds_steiner_gene(graphe* g, const int time, const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes)
{
	*valeurSolution = INT_MAX;	//meilleure solution
	*nbAretes = 0;
	int sol = 0;	//solution actuelle
	int nbAretesSol;
	int M = 0;		//majorant du cout d'une solution

	//representation des individus
	int* sommets;			//sans les sommets terminaux
	sommets = (int*) calloc(g->nbNonTerminaux, sizeof(int));
	int* solutionActuelle;	//avec les sommets terminaux
	solutionActuelle = (int*) calloc(g->nbNoeuds, sizeof(int));

	for(int i = 0; i < g->nbTerminaux; i++){
		solutionActuelle[g->terminaux[i]->id] = 1;
	}


	//declaration des tableaux de parents et de rangs (pour la detection de cycle)
	int* parents;
	int* rangs;
	parents = (int*) calloc(g->nbNoeuds, sizeof(int));
	rangs = (int*) calloc(g->nbNoeuds, sizeof(int));
	arete* aretesSol;
	arete* aretesTab;
	aretesSol = (arete*) calloc(g->nbAretes, sizeof(arete));
	aretesTab = (arete*) calloc(g->nbAretes, sizeof(arete));

	//calcul de la valeur pénalisant une solution non admissible
	for(int i = 0; i < g->nbAretes; i++){
		M += g->aretes[i].poids;
	}



	//while()...

	//TODO : calcul nouvelle solution



	//affichage des sommets actuellement inclus dans la solution
	puts("Sommets :");
	for(int i = 0; i < g->nbNonTerminaux; i++){
		//test
		//sommets[i] = 1;

		printf("%d:%d ", g->nonTerminaux[i]->id, sommets[i]);
	}
	//sommets[0] = 1;
	puts("");

	//creation d'un tableau de solutions comprenant les sommets terminaux
	for(int i = 0; i < g->nbNonTerminaux; i++){
		solutionActuelle[g->nonTerminaux[i]->id] = sommets[i];
	}

	sol = evaluerSolution(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);

	//amelioration trouvee
	if(sol < *valeurSolution){
		if(verbose) printf("Amelioration trouvee de valeur %d.\n", sol);
		*valeurSolution = sol;
		*nbAretes = nbAretesSol;
		for(int i = 0; i < nbAretesSol; i++){
			aretes[i] = aretesSol[i];
		}
	}

}

///algo de recherche locale
void noeuds_steiner_local(graphe* g,const int time,const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes){

	//TODO...
}
