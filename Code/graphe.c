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

///algorithme de kruskal
int kruskal2(graphe* g, int* solution,
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
				int n1 = g->noeuds[i].aretes[j].noeud1->id, n2 = g->noeuds[i].aretes[j].noeud2->id;
				if( n1 >= i && n2 >= i && solution[n1] && solution[n2]){
					aretesTab[na] = g->noeuds[i].aretes[j];
					na++;
				}
			}
		}
	}

	quickSort(aretesTab, 0, na - 1);

//	for(int i = 0; i < na; i++){
//		printf("%d %d\n", aretesTab[i].noeud1->id, aretesTab[i].noeud2->id);
//	}
//getchar();

	int k = 0;
	int parentX;
	int parentY;
	//parcours des aretes
	while( k < na && *nbAretesSol < nbNoeudsSol - 1 ){
		//test cycle
		parentX = trouverParent(aretesTab[k].noeud1->id, parents);
		parentY = trouverParent(aretesTab[k].noeud2->id, parents);

		if(parentX != parentY){
			aretesSol[*nbAretesSol] = aretesTab[k];
			*nbAretesSol+=1;        
			valeurSol += aretesTab[k].poids;
			unionEnsembles(parentX, parentY, parents, rangs);
		}
		k++;
	}
	valeurSol += (nbNoeudsSol - 1 - *nbAretesSol) * M;

	return valeurSol;
}


//init les params nécessaires pour kruskal2 afin d'avoir un appel plus concis
int kruskal(graphe* g, /*sorties :*/ arete* aretesSol, int* nbAretesSol){
	int sol = 0;	//solution actuelle
	//representation des individus avec les sommets terminaux
	int* solutionActuelle;
	solutionActuelle = (int*) calloc(g->nbNoeuds, sizeof(int));
	for(int i = 0; i < g->nbNoeuds; i++){
		solutionActuelle[i] = 1;
	}
	//declaration des tableaux de parents et de rangs ( -> pour la detection de cycle)
	int* parents;
	int* rangs;
	parents = (int*) calloc(g->nbNoeuds, sizeof(int));
	rangs = (int*) calloc(g->nbNoeuds, sizeof(int));
	arete* aretesTab;
	aretesTab = (arete*) calloc(g->nbAretes, sizeof(arete));

	return kruskal2(g, solutionActuelle, 0, parents, rangs, aretesTab, aretesSol, nbAretesSol);
}


void generate_population(graphe* g, int** population, const int verbose){
	if(verbose) printf("Generation d'une population de taille %d.\n", TAILLE_POPULATION);
	//generation de la population
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		population[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));
		double p = generer_uniforme(0.2,0.5);
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = generer_uniforme(0, 1) > p;
			//printf("%d \t %d\n", i, population[i][j]);
		}
		//puts("");
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

void dijkstra(graphe* g, int depart, /*out:*/ int* dist, int* prec){
/*	int* dist;
	dist = (int*)calloc(g->nbNoeuds,sizeof(int));

	int* prec;
	prec = (int*)calloc(g->nbNoeuds,sizeof(int));
*/
	int* Q; //boolean (visités)
	Q = (int*)calloc(g->nbNoeuds,sizeof(int)); 

	//init
	for(int i = 0; i < g->nbNoeuds; i++) {
		dist[i] = INT_MAX;
		prec[i] = -1;
		Q[i] = 0;
	}
	dist[depart] = 0;

	for(int i = 0; i < g->nbNoeuds; i++) //on ferme 1 sommet à chaque iteration
	{ 
		int u = argminQ(dist, Q, g->nbNoeuds);
		//printf("dijkstra: %d plus proche à distance %d\n", u, dist[u]);
		Q[u] = 1;
		arete* voisins = g->noeuds[u].aretes;
		for(int j = 0; j < g->noeuds[u].nbAretes; j++) {
			arete arrete_uv = voisins[j];
			int alt = dist[u] + arrete_uv.poids;

			int v=arrete_uv.noeud2->id;
			if(u == v) v = arrete_uv.noeud1->id;

			if(alt < dist[v]) {
				dist[v] = alt;
				prec[v] = u;
			}
		}
	}
}

void generate_population_heuristique_PCC(graphe* g, int** population, const int verbose){
	/*
	if(verbose) printf("Generation d'une population de taille %d.\n", TAILLE_POPULATION);
	//generation de la population
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		population[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));
		double p = generer_uniforme(0.2,0.5);
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = generer_uniforme(0, 1) > p;
			//printf("%d \t %d\n", i, population[i][j]);
		}
		//puts("");
	}
	*/
	graphe* g2;
	g2 = (graphe*)calloc(1,sizeof(graphe));

	//creation des noeuds
	g2->nbNoeuds = g->nbNoeuds;
	g2->noeuds = (noeud*)calloc(g->nbNoeuds,sizeof(noeud));
	for(int i = 0; i<g2->nbNoeuds; i++){
		g2->noeuds[i].id = i;
		//g2->noeuds[i].est_terminal = g2->noeuds[i].est_terminal;
	}
	//creation des arretes (graphe complet)
	int n = g2->nbNoeuds;
	g2->nbAretes = n*(n-1)/2;
	//printf("%d arretes\n", g2->nbAretes);
	g2->aretes = (arete*)calloc(g2->nbAretes,sizeof(arete));

	int* dist;
	dist = (int*)calloc(g->nbNoeuds,sizeof(int));

	int** prec; // tableau 2d des précédents 
	prec = (int**)calloc(g->nbNoeuds,sizeof(int*));
	for(int i=0; i <g->nbNoeuds; i++)
		prec[i] = (int*)calloc(g->nbNoeuds,sizeof(int));

	int ac = 0;
	//pour la valeur des arretes il faut les PCC => dijkstra
	for(int i = 0; i < g2->nbNoeuds; i++) {
		//dijkstra depuis le sommet i
		dijkstra(g, i, /*out:*/ dist, prec[i]);


		for(int j = i+1; j < g2->nbNoeuds; j++) {
			g2->aretes[ac].noeud1 = &g2->noeuds[i];
			g2->aretes[ac].noeud2 = &g2->noeuds[j];
			g2->aretes[ac].poids = dist[j];

			g2->noeuds[i].nbAretes++;
			g2->noeuds[j].nbAretes++;
			//associer les aretes aux noeuds -> tableau a realloc a chaque fois
			g2->noeuds[i].aretes = (arete*)realloc(g2->noeuds[i].aretes, g2->noeuds[i].nbAretes * sizeof(arete));
			g2->noeuds[i].aretes[g2->noeuds[i].nbAretes - 1] = g2->aretes[ac];
			g2->noeuds[j].aretes = (arete*)realloc(g2->noeuds[j].aretes, g2->noeuds[j].nbAretes * sizeof(arete));
			g2->noeuds[j].aretes[g2->noeuds[j].nbAretes - 1] = g2->aretes[ac];
			ac++;
			//printf("(%d, %d) %d\n", i+1, j+1, dist[j]);
		}
	}

	//kruskal pour ACPM
	arete* aretesSol = (arete*) calloc(g2->nbAretes, sizeof(arete));
	int nbAretesSol = 0;
	//kruskal pour ACPM
	int val = kruskal(g2, /*sorties :*/ aretesSol, &nbAretesSol);

	/*printf("solution kruskal: %d arretes, val %d\n", nbAretesSol, val);
	for(int i=0; i< nbAretesSol; i++) {
		printf("%d %d %d\n", aretesSol[i].noeud1->id+1, aretesSol[i].noeud2->id+1, aretesSol[i].poids);
	}*/

	//etape G3
	graphe* g3;
	g3 = (graphe*)calloc(1,sizeof(graphe));
	//creation des noeuds
	g3->nbNoeuds = g->nbNoeuds;
	g3->noeuds = (noeud*)calloc(g->nbNoeuds,sizeof(noeud));
	for(int i = 0; i < g3->nbNoeuds; i++){
		g3->noeuds[i].id = i;
		//g2->noeuds[i].est_terminal = g2->noeuds[i].est_terminal;
	}

	for(int i = 0; i < g3->nbNoeuds; i++){
		for(int j = 0; j < g3->nbNoeuds; j++){
			printf("%d\t",prec[i][j]);
		}
		printf("\n");
	}

	//parcours des arrêtes dans G2:
	for(int i=0; i< nbAretesSol; i++) {
		//printf("%d %d %d\n", aretesSol[i].noeud1->id+1, aretesSol[i].noeud2->id+1, aretesSol[i].poids);
		// retrouver le PCC de noeud1 vers noeud2
		// on à construit depuis 0 donc prendre le plus grand pour avoir les précédents
		int noeudfin = aretesSol[i].noeud1->id; //noeud de départ (final)
		int noeudbut = aretesSol[i].noeud2->id;
		if(aretesSol[i].noeud2->id > noeudfin){
			noeudfin = aretesSol[i].noeud2->id;
			noeudbut = aretesSol[i].noeud1->id;
		}
		/*int noeudfin = 6;
		int noeudbut = 0; // pour tester */
		//printf("je pars du noeud %d, but: %d\n", noeudfin+1, noeudbut+1);
		int noeudprec = prec[noeudbut][noeudfin]; // ou l'inverse?
		//printf("\tnoeudprec : %d", noeudprec+1);
		int noeudprecprec;
		while(noeudprec != noeudbut) {
			noeudprecprec = noeudprec;
			noeudprec = prec[noeudbut][noeudprec];
			//	printf(" <-%d ",  noeudprec+1);

/*
			//************************
			//************************
			//   TODO  -> creation des aretes du chemin dans g3
			//************************
			//************************

			//creation des aretes du chemin dans g3
			g->nbAretes = val;
			g->aretes = (arete*)calloc(val,sizeof(arete));

	        //valeur des aretes
			//noeud1, noeud2, poids
			val--; val2--;
			g->aretes[ac].noeud1 = &g->noeuds[val];
			g->aretes[ac].noeud2 = &g->noeuds[val2];
			g->aretes[ac].poids = val3;

			g->noeuds[val].nbAretes++;
			g->noeuds[val2].nbAretes++;
			//associer les aretes aux noeuds -> tableau a realloc a chaque fois
			g->noeuds[val].aretes = (arete*)realloc(g->noeuds[val].aretes, g->noeuds[val].nbAretes * sizeof(arete));
			g->noeuds[val].aretes[g->noeuds[val].nbAretes - 1] = g->aretes[ac];
			g->noeuds[val2].aretes = (arete*)realloc(g->noeuds[val2].aretes, g->noeuds[val2].nbAretes * sizeof(arete));
			g->noeuds[val2].aretes[g->noeuds[val2].nbAretes - 1] = g->aretes[ac];
			
			ac++;
			*/
	        
		}
		//puts("");

	}





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
	solutions = (int*) calloc(TAILLE_POPULATION * 2, sizeof(int));
	arete* aretesSol;
	arete* aretesTab;
	aretesSol = (arete*) calloc(g->nbAretes, sizeof(arete));
	aretesTab = (arete*) calloc(g->nbAretes, sizeof(arete));

	//calcul de la valeur pénalisant une solution non admissible
	for(int i = 0; i < g->nbAretes; i++){
		M += g->aretes[i].poids;
	}

	//if(verbose) printf("Generation d'une population de taille %d.\n", TAILLE_POPULATION);
	//generation de la population
	int** population;
	population = (int**) calloc(TAILLE_POPULATION * 2, sizeof(int*));	//on double la taille pour simplifier le tri a la selection


/*********/
/*
	//test de dijkstra
	int* dist;
	dist = (int*)calloc(g->nbNoeuds,sizeof(int));

	int* prec;
	prec = (int*)calloc(g->nbNoeuds,sizeof(int));

	dijkstra(g, 0, dist, prec);

	printf("dist:\n");
	for(int i=0; i<g->nbNoeuds; i++) {
		printf("\t %d", dist[i]);
	}
	printf("\n");

	printf("prec:\n");
	for(int i=0; i<g->nbNoeuds; i++) {
		printf("\t %d", prec[i]);
	}
	printf("\n");

	printf("\n\n\n");
*/
/*******/

	//TODO: switcher de fonction avec un param
	//generate_population(g, population, verbose);
	generate_population_heuristique_PCC(g, population, verbose);
	//fin todo

	int gen = 0;

	for(int i = 0; i < TAILLE_POPULATION; i++){
		//creation d'un tableau de solutions comprenant les sommets terminaux
		for(int j = 0; j < g->nbNonTerminaux; j++){
			solutionActuelle[g->nonTerminaux[j]->id] = population[i][j];

			sol = kruskal2(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);
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

	do{

		//calcul nouvelle solution

		for(int i = 0; i < TAILLE_POPULATION; i++){
			//selection de deux individus
			//on prend un individu au hasard, puis on determine s'il est selectionne en fonction de la valeur de sa solution
			int parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( solutions[parent1] / (double)(*valeurSolution) < generer_uniforme(0, 1)){
				parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			}

			int parent2 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( parent1 == parent2 || solutions[parent2] / (double)(*valeurSolution) < generer_uniforme(0, 1)){
				parent2 = (int)generer_uniforme(0, TAILLE_POPULATION);
			}

			int p = (int)generer_uniforme(0, g->nbNonTerminaux);
			//croisement des deux parents
			for(int j = 0; j < p; j++){
				population[i+TAILLE_POPULATION][j] = population[parent1][j];
				//printf("%d ", population[parent1][j]);
			}
			for(int j = p; j < g->nbNonTerminaux; j++){
				//printf("%d ", population[parent2][j]);
				population[i+TAILLE_POPULATION][j] = population[parent2][j];
			}
			if( CHANCE_MUTATION >= generer_uniforme(0, 1)){
				int r = (int)generer_uniforme(0, g->nbNonTerminaux);
				population[i+TAILLE_POPULATION][r] = !population[i+TAILLE_POPULATION][r];
			}
		}

		//evaluation des enfants
		for(int i = TAILLE_POPULATION; i < TAILLE_POPULATION * 2; i++){
			//creation d'un tableau de solutions comprenant les sommets terminaux
			for(int j = 0; j < g->nbNonTerminaux; j++){
				solutionActuelle[g->nonTerminaux[j]->id] = population[i][j];


				sol = kruskal2(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);
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


		quickSort_pop(population, solutions, 0, TAILLE_POPULATION * 2 - 1);

		gen++;

	}while(maxTime == -1 || (clock() - debut) / (double)CLOCKS_PER_SEC < (double)maxTime);
}

///algo de recherche locale
void noeuds_steiner_local(graphe* g,const int time,const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes){
	
}
