#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "types.h"

#define TAILLE_POPULATION 100
#define CHANCE_MUTATION 0.02

/*
	NOTE : tous les indices sont decales de -1 par rapport au fichier d'entree.
*/

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


	for(int i = 0; i < na; i++){
		printf("%d %d\n", aretesTab[i].noeud1->id, aretesTab[i].noeud2->id);
	}
getchar();
//TODO : retirer les doublons ou changer le tri des aretes


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
void noeuds_steiner_gene(graphe* g, const int maxTime, const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes)
{

	srand(time(NULL));
	clock_t debut = clock();
	*valeurSolution = INT_MAX;	//meilleure solution
	*nbAretes = 0;
	int sol = 0;	//solution actuelle
	int nbAretesSol;
	int M = 0;		//majorant du cout d'une solution

	//representation des individus avec les sommets terminaux
	int* solutionActuelle;
	solutionActuelle = (int*) calloc(g->nbNoeuds, sizeof(int));

	for(int i = 0; i < g->nbTerminaux; i++){
		solutionActuelle[g->terminaux[i]->id] = 1;
	}

	//declaration des tableaux de parents et de rangs ( -> pour la detection de cycle)
	int* parents;
	int* rangs;
	parents = (int*) calloc(g->nbNoeuds, sizeof(int));
	rangs = (int*) calloc(g->nbNoeuds, sizeof(int));
	int* solutions;
	solutions = (int*) calloc(TAILLE_POPULATION, sizeof(int));
	arete* aretesSol;
	arete* aretesTab;
	aretesSol = (arete*) calloc(g->nbAretes, sizeof(arete));
	aretesTab = (arete*) calloc(g->nbAretes, sizeof(arete));

	//calcul de la valeur pénalisant une solution non admissible
	for(int i = 0; i < g->nbAretes; i++){
		M += g->aretes[i].poids;
	}

	if(verbose) printf("Generation d'une population de taille %d.\n", TAILLE_POPULATION);
	//generation de la population
	int** population;
	int** populationEnfants;
	population = (int**) calloc(TAILLE_POPULATION, sizeof(int*));
	populationEnfants = (int**) calloc(TAILLE_POPULATION, sizeof(int*));
	for(int i = 0; i < TAILLE_POPULATION; i++){
		population[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));
		populationEnfants[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));
		double p = generer_uniforme(0.2,0.5);
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = generer_uniforme(0, 1) > p;
			//printf("%d ", population[i][j]);
		}
		//puts("");
	}


	int gen = 0;
	do{

		/*
		//affichage des sommets actuellement inclus dans la solution
		puts("Sommets :");
		for(int i = 0; i < g->nbNonTerminaux; i++){
			//test
			sommets[i] = 1;

			printf("%d:%d ", g->nonTerminaux[i]->id, sommets[i]);
		}
		//sommets[0] = 1;
		puts("");
		*/


		for(int i = 0; i < TAILLE_POPULATION; i++){
			//creation d'un tableau de solutions comprenant les sommets terminaux
			for(int j = 0; j < g->nbNonTerminaux; j++){
				solutionActuelle[g->nonTerminaux[j]->id] = population[i][j];


				sol = evaluerSolution(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);
				solutions[i] = sol;

				//amelioration trouvee
				if(sol < *valeurSolution){
					if(verbose) printf("Amelioration trouvee de valeur %d (generation %d).\n", sol, gen);
					*valeurSolution = sol;
					*nbAretes = nbAretesSol;
					for(int k = 0; k < nbAretesSol; k++){
						aretes[k] = aretesSol[k];
					}
				}
			}
		}

		//calcul nouvelle solution

		for(int i = 0; i < TAILLE_POPULATION; i++){
			//selection de deux individus
			//on prend un individu au hasard, puis on determine s'il est selectionne en fonction de la valeur de sa solution
			int parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( solutions[parent1] / (double)(*valeurSolution) < generer_uniforme(0, 1)){
				parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			}

			int parent2 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( solutions[parent2] / (double)(*valeurSolution) < generer_uniforme(0, 1)){
				parent2 = (int)generer_uniforme(0, TAILLE_POPULATION);
			}

			int p = (int)generer_uniforme(0, g->nbNonTerminaux);
			//croisement des deux parents
			for(int j = 0; j < p; j++){
				populationEnfants[i][j] = population[parent1][j];
				//printf("%d ", populationEnfants[i][j]);
				//printf("%d ", population[parent1][j]);
			}
			for(int j = p; j < g->nbNonTerminaux; j++){
				//printf("%d ", population[parent2][j]);
				populationEnfants[i][j] = population[parent2][j];
			}
			if( CHANCE_MUTATION >= generer_uniforme(0, 1)){
				int r = (int)generer_uniforme(0, TAILLE_POPULATION);
				populationEnfants[i][r] = !populationEnfants[i][r];
			}
		}

		//remplacement des parents par les enfants
		for(int i = 0; i < TAILLE_POPULATION; i++){
			population[i] = populationEnfants[i];
		}

		gen++;

	}while((clock() - debut) / (double)CLOCKS_PER_SEC < (double)maxTime);
}

///algo de recherche locale
void noeuds_steiner_local(graphe* g,const int time,const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes){

	//TODO...
}
