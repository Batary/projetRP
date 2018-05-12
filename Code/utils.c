#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "graphe.h"
#include "types.h"
#include "utils.h"


void freeGraphe(graphe* g){

	for(int i = 0; i < g->nbNoeuds; i++)
	{
		free(g->noeuds[i].aretes);
	}

    free(g->terminaux);
    free(g->nonTerminaux);
    free(g->aretes);
	free(g->noeuds);
	free(g);
}


double generer_uniforme(double x, double y){
	return ( rand()/(double)RAND_MAX ) * (y-x) + x;
}

///trouve le parent (= l'id de l'ensemble qui le contient) d'un element
int trouverParent(const int x, int* parents){
	if(parents[x] != x)
		parents[x] = trouverParent(parents[x], parents);
	return parents[x];
}

///fusionne deux ensembles de sommets
/// attention parentX et parentY doivent être les parents des noeuds à fusionner
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

//idem pour trier les meilleurs individus
int partitionnerPopulation(int** pop, int* tableau, int p, int r) {
    int pivot = tableau[p], i = p-1, j = r+1;
    int temp;
    int* temp2;
    while (1) {
        do
            j--;
        while (tableau[j] > pivot);
        do
            i++;
        while (tableau[i] < pivot);
        if (i < j) {
            temp = tableau[i];
            tableau[i] = tableau[j];
            tableau[j] = temp;
            temp2 = pop[i];
            pop[i] = pop[j];
            pop[j] = temp2;
        }
        else
            return j;
    }
}

void quickSort_pop(int** pop, int* tableau, int p, int r) {
	if(p > TAILLE_POPULATION) return; //pas besoin de trier la 2e partie
    int q;
    if (p < r) {
        q = partitionnerPopulation(pop, tableau, p, r);
        quickSort_pop(pop, tableau, p, q);
        quickSort_pop(pop, tableau, q+1, r);
    }
}

int argminQ(int* tab, int* Q, int taille){
	int min = INT_MAX;
	int argmin = -1;
	for(int i = 0; i < taille; i++) {
		if(tab[i] < min && Q[i] == 0) {
			min = tab[i];
			argmin = i;
		}
	}
	return argmin;
}

//renvoi 1 si il existe une arete entre ces deux sommets, 0 sinon
// version optimisée
int isarete(graphe* g, int n1, int n2) {
	for(int i = 0; i < g->noeuds[n1].nbAretes; i++) {
		int noeud1 = g->noeuds[n1].aretes[i]->noeud1->id;
		int noeud2 = g->noeuds[n1].aretes[i]->noeud2->id;
		if(noeud1 == n1 && noeud2 == n2)
			return 1;
		if(noeud1 == n2 && noeud2 == n1)
			return 1;
	}
	return 0; // rien trouvé
}


//convertit un encodage des sommmets non-terminaux en une solution de taille g->nbnoeuds
void convertpartielletofullsolution(graphe* g,int* solS, int* solfull){
	int cptnonterminaux = 0;
	for(int i = 0; i < g->nbNoeuds; i++){
		if(g->noeuds[i].est_terminal){
			//printf("t \n");
			solfull[i] = 1;
		}
		else { //non-terminal
			solfull[i] = solS[cptnonterminaux];
			//printf("nt %d %d %d\n", i, cptnonterminaux, solS[cptnonterminaux]);
			cptnonterminaux++;
		}
	}
	//puts("");
}
void convertfulltopartiellesolution(graphe* g, int* solfull, int* solpartielle){
	for(int i = 0; i < g->nbNonTerminaux; i++)
		solpartielle[i] = solfull[g->nonTerminaux[i]->id];
}

void printtabint(int * tab, int size){
	for(int i = 0; i < size; i++){
		printf("%d  ", tab[i]);
	}
	puts("");
}
