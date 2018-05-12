#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "types.h"
#include "graphe.h"
#include "utils.h"


/*
	NOTE : tous les indices sont decales de -1 par rapport au fichier d'entree.
*/

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
				int n1 = g->noeuds[i].aretes[j]->noeud1->id, n2 = g->noeuds[i].aretes[j]->noeud2->id;
				//printf(">> %d ", g->noeuds[i].aretes[j]->poids);
				if( n1 >= i && n2 >= i && solution[n1] && solution[n2]){
					aretesTab[na] = *(g->noeuds[i].aretes[j]);
					na++;
				}
			}
		}
	}

	quickSort(aretesTab, 0, na - 1);

	/*
	for(int i = 0; i < na; i++){
		printf("%d %d %d\n", aretesTab[i].noeud1->id, aretesTab[i].noeud2->id, aretesTab[i].poids);
	}
    //getchar();
    */

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

int kruskal_partiel(graphe* g,int* solutionPartielle, /*sorties :*/ arete* aretesSol, int* nbAretesSol){
	//declaration des tableaux de parents et de rangs ( -> pour la detection de cycle)
	int* parents;
	int* rangs;
	parents = (int*) calloc(g->nbNoeuds, sizeof(int));
	rangs = (int*) calloc(g->nbNoeuds, sizeof(int));
	arete* aretesTab;
	aretesTab = (arete*) calloc(g->nbAretes, sizeof(arete));

	//important d'avoir une grande valeur pour M afin de ne pas prendre les solutions non-admissibles
	int r = kruskal2(g, solutionPartielle, 999999, parents, rangs, aretesTab, aretesSol, nbAretesSol);


/*
	printf("kruskal : aretes sol poids: %d arretes\n\t", *nbAretesSol);
	for(int i = 0; i < *nbAretesSol; i++)
		printf("%d  ", aretesSol[i].poids);
	puts("");

*/
	free(aretesTab);
	free(parents);
	free(rangs);

	return r;
}

//init les params nécessaires pour kruskal2 afin d'avoir un appel plus concis
int kruskal(graphe* g, /*sorties :*/ arete* aretesSol, int* nbAretesSol){
	//representation des individus avec les sommets terminaux
	int* solutionActuelle;
	solutionActuelle = (int*) calloc(g->nbNoeuds, sizeof(int));
	for(int i = 0; i < g->nbNoeuds; i++){
		solutionActuelle[i] = 1;
	}

	int r = kruskal_partiel(g, solutionActuelle, aretesSol, nbAretesSol);

	free(solutionActuelle);

	return r;
}


void generer_population_aleatoire(graphe* g, int** population, const int verbose){
	if(verbose) printf("Generation d'une population de taille %d.\n", TAILLE_POPULATION);
	//generation de la population
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		//population[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));
		double p = generer_uniforme(0.2,0.5);
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = generer_uniforme(0, 1) > p;
			//printf("%d \t %d\n", i, population[i][j]);
		}
		//puts("");
	}
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
		arete** voisins = g->noeuds[u].aretes;
		for(int j = 0; j < g->noeuds[u].nbAretes; j++) {
			arete arrete_uv = *voisins[j];
			int alt = dist[u] + arrete_uv.poids;

			int v=arrete_uv.noeud2->id;
			if(u == v) v = arrete_uv.noeud1->id;

			if(alt < dist[v]) {
				dist[v] = alt;
				prec[v] = u;
			}
		}
	}
	free(Q);
}

void generer_population_heuristique_PCC_one(graphe* g,  int* noeudsactifs, const int verbose){
	//ETAPES G1 ET G2
	graphe* g2;
	g2 = (graphe*)calloc(1,sizeof(graphe));

	//creation des noeuds
	g2->nbNoeuds = g->nbTerminaux;
	g2->noeuds = (noeud*)calloc(g->nbTerminaux,sizeof(noeud));
	for(int i = 0; i < g2->nbNoeuds; i++){
		g2->noeuds[i].id = i;
		//g2->noeuds[i].est_terminal = g2->noeuds[i].est_terminal;
	}
	//creation des aretes (graphe complet)
	int n = g2->nbNoeuds;
	g2->nbAretes = n*(n-1)/2;
	//printf("%d aretes\n", g2->nbAretes);
	g2->aretes = (arete*)calloc(g2->nbAretes,sizeof(arete));

	int* dist;
	dist = (int*)calloc(g->nbNoeuds,sizeof(int));

	int** prec; // tableau 2d des précédents
	prec = (int**)calloc(g->nbNoeuds,sizeof(int*));
	for(int i=0; i < g->nbNoeuds; i++){
		prec[i] = (int*)calloc(g->nbNoeuds,sizeof(int));
	}

	//ac : compteur d'aretes
	int ac = 0;
	//pour la valeur des arretes il faut les PCC => dijkstra
	for(int i = 0; i < g2->nbNoeuds; i++) {

		//dijkstra depuis le sommet i
		dijkstra(g, g->terminaux[i]->id, /*out:*/ dist, prec[i]);

		g2->noeuds[i].id = i;//g->terminaux[i]->id;
		for(int j = i+1; j < g2->nbNoeuds; j++) {
			g2->aretes[ac].noeud1 = &g2->noeuds[i];
			g2->aretes[ac].noeud2 = &g2->noeuds[j];
			g2->aretes[ac].poids = dist[g->terminaux[j]->id];

			g2->noeuds[i].nbAretes++;
			g2->noeuds[j].nbAretes++;
			//associer les aretes aux noeuds -> tableau a realloc a chaque fois
			g2->noeuds[i].aretes = (arete**)realloc(g2->noeuds[i].aretes, g2->noeuds[i].nbAretes * sizeof(arete*));
			g2->noeuds[i].aretes[g2->noeuds[i].nbAretes - 1] = &g2->aretes[ac];
			g2->noeuds[j].aretes = (arete**)realloc(g2->noeuds[j].aretes, g2->noeuds[j].nbAretes * sizeof(arete*));
			g2->noeuds[j].aretes[g2->noeuds[j].nbAretes - 1] = &g2->aretes[ac];
			ac++;
			//printf("(%d, %d) %d\n", g2->aretes[ac-1].noeud1->id+1, g->terminaux[j]->id+1, dist[g->terminaux[j]->id]);
		}
	}
	//kruskal pour ACPM
	arete* aretesSol = (arete*) calloc(g2->nbAretes, sizeof(arete));
	int nbAretesSol = 0;

	int val = kruskal(g2, /*sorties :*/ aretesSol, &nbAretesSol);
/*
	if(verbose){
		printf("solution kruskal: %d aretes, val %d\n", nbAretesSol, val);
		for(int i=0; i< nbAretesSol; i++) {
			printf("%d %d %d\n", g->terminaux[aretesSol[i].noeud1->id]->id+1, g->terminaux[aretesSol[i].noeud2->id]->id+1, aretesSol[i].poids);
		}
	}
*/
	//etape G3
	graphe* g3;
	g3 = (graphe*)calloc(1,sizeof(graphe));
	g3->aretes = (arete*)calloc(g->nbAretes,sizeof(arete));
	//creation des noeuds
	g3->nbNoeuds = 0;//g2->nbNoeuds;// on ne connait pas le nombre de noeuds a l'avance
	g3->noeuds = (noeud*)calloc(g->nbNoeuds,sizeof(noeud));
	for(int i = 0; i < g3->nbNoeuds; i++){
		g3->noeuds[i].id = i;
	}

	//tableau permettant de faire correspondre les noeuds de g3 a ceux de g
	int* g3_to_g;
	g3_to_g = (int*)calloc(g->nbNoeuds,sizeof(int));
	int* g_to_g3;
	g_to_g3 = (int*)calloc(g->nbNoeuds,sizeof(int));
	for(int i = 0; i < g->nbNoeuds; i++){
		g_to_g3[i] = -1;
		g3_to_g[i] = -1;
	}
/*
	for(int i = 0; i < g3->nbNoeuds; i++){
		for(int j = 0; j < g->nbNoeuds; j++){
			printf("%d\t",prec[i][j]);
		}
		printf("\n");
	}
*/
	ac = 0;
	//parcours des arrêtes dans G2:
	for(int i=0; i< nbAretesSol; i++) {
		//printf("%d %d %d\n", aretesSol[i].noeud1->id+1, aretesSol[i].noeud2->id+1, aretesSol[i].poids);
		// retrouver le PCC de noeud1 vers noeud2
		// on à construit depuis 0 donc prendre le plus grand pour avoir les précédents
		int noeudfin = g->terminaux[aretesSol[i].noeud1->id]->id; //noeud de départ (donc le dernier)
		int noeudbut = aretesSol[i].noeud2->id;	//noeud vers lequel on cherche a revenir
		if(aretesSol[i].noeud1->id < aretesSol[i].noeud2->id){
			noeudfin = g->terminaux[aretesSol[i].noeud2->id]->id;
			noeudbut = aretesSol[i].noeud1->id;
		}

		//printf("je pars du noeud %d, but: %d\n", noeudfin+1, g->terminaux[noeudbut]->id+1);
		int noeudprec = noeudfin;//prec[noeudbut][noeudfin]; // ou l'inverse?
		//printf("\tnoeudprec : %d", noeudprec+1);
		int noeudprecprec;
		while(noeudprec != g->terminaux[noeudbut]->id) {
			noeudprecprec = noeudprec;	// --> noeud suivant dans le chemin
			noeudprec = prec[noeudbut][noeudprec];
				//printf(" <- %d \n",  noeudprec+1);
				//getchar();

			int e = 1;

			//creation du noeud s'il n'existe pas déjà
			if(g_to_g3[noeudprecprec] == -1){
				g3->noeuds[g3->nbNoeuds].id = g3->nbNoeuds;
				g3->noeuds[g3->nbNoeuds].nbAretes = 0;
				g_to_g3[noeudprecprec] = g3->nbNoeuds;
				g3_to_g[g3->nbNoeuds] = noeudprecprec;
				g3->nbNoeuds++;
				e=0;
			}

			if(g_to_g3[noeudprec] == -1){
				g3->noeuds[g3->nbNoeuds].id = g3->nbNoeuds;
				g3->noeuds[g3->nbNoeuds].nbAretes = 0;
				g_to_g3[noeudprec] = g3->nbNoeuds;
				g3_to_g[g3->nbNoeuds] = noeudprec;
				g3->nbNoeuds++;
				e=0;
			}

			//test si l'arete n'existe pas deja
			if(e == 1){
				e = 0;
				for (int k = 0; k < g3->noeuds[g_to_g3[noeudprec]].nbAretes; k++){
					int n1 = g3_to_g[g3->noeuds[g_to_g3[noeudprec]].aretes[k]->noeud1->id], n2 = g3_to_g[g3->noeuds[g_to_g3[noeudprec]].aretes[k]->noeud2->id];
					if (( /*n1 == noeudprec &&*/ n2 == noeudprecprec ) || ( n1 == noeudprecprec /*&& n2 == noeudprec*/ )){
						e = 1;
						break;
					}
				}
			}


			//creation de l'arete du chemin dans g3
			if(!e){

				//recuperation du poids de l'arete originale
				int val3 = 0;
				for (int k = 0; k < g->noeuds[noeudprec].nbAretes; k++){
					int n1 = g->noeuds[noeudprec].aretes[k]->noeud1->id, n2 = g->noeuds[noeudprec].aretes[k]->noeud2->id;
					if (n2 == noeudprecprec  ||  n1 == noeudprecprec){//((n1 == noeudprec || n1 == noeudprecprec) && (n2 == noeudprec || n2 == noeudprecprec)){
						val3 = g->noeuds[noeudprec].aretes[k]->poids;
						break;
					}
				}


				val = g_to_g3[noeudprec];
				int val2 = g_to_g3[noeudprecprec];

				//valeur des aretes
				//noeud1, noeud2, poids
				g3->aretes[ac].noeud1 = &g3->noeuds[val];
				g3->aretes[ac].noeud2 = &g3->noeuds[val2];
				g3->aretes[ac].poids = val3;

				g3->noeuds[val].nbAretes++;
				g3->noeuds[val2].nbAretes++;
				//associer les aretes aux noeuds -> tableau a realloc a chaque fois
				g3->noeuds[val].aretes = (arete**)realloc(g3->noeuds[val].aretes, g3->noeuds[val].nbAretes * sizeof(arete*));
				g3->noeuds[val].aretes[g3->noeuds[val].nbAretes - 1] = &g3->aretes[ac];
				g3->noeuds[val2].aretes = (arete**)realloc(g3->noeuds[val2].aretes, g3->noeuds[val2].nbAretes * sizeof(arete*));
				g3->noeuds[val2].aretes[g3->noeuds[val2].nbAretes - 1] = &g3->aretes[ac];

				g3->nbAretes++;
				ac++;
			}
		}
	}
/*
	puts("g3 :");
	for(int i = 0; i<g3->nbAretes; i++){
		printf("%d %d %d\n", g3_to_g[g3->aretes[i].noeud1->id]+1, g3_to_g[g3->aretes[i].noeud2->id]+1, g3->aretes[i].poids);
	}
	puts("");
*/

	//kruskal pour ACPM
	arete* aretesSol2;
	aretesSol2 = (arete*) calloc(g3->nbAretes, sizeof(arete));
	int* solution;
	solution = (int*) calloc(g->nbNoeuds, sizeof(int));
	nbAretesSol = 0;

	int valsol = kruskal(g3, /*sorties :*/ aretesSol2, &nbAretesSol);


/*
	int s = 0;
	for(int i = 0; i<g->nbNoeuds; i++){
		s+=g->noeuds[i].nbAretes;
	}
	printf("nb aretes g3 : %d \t nb aretes solution : %d \t c : %d\n",g3->nbAretes, nbAretesSol,s/2);
*/

/*
	puts("solution (arbre G4) :");
	for(int i = 0; i<nbAretesSol; i++){
		printf("(%d %d)\n", g3_to_g[aretesSol2[i].noeud1->id]+1, g3_to_g[aretesSol2[i].noeud2->id]+1);
	}
*/

	for(int i = 0; i<nbAretesSol; i++){
		//if(!g->noeuds[g3_to_g[aretesSol2[i].noeud1->id]].est_terminal)
			solution[g3_to_g[aretesSol2[i].noeud1->id]] ++;
		//if(!g->noeuds[g3_to_g[aretesSol2[i].noeud2->id]].est_terminal)
			solution[g3_to_g[aretesSol2[i].noeud2->id]] ++;
	}

	//retirer les noeuds non terminaux isoles (etape 5)
	for(int i = 0; i<nbAretesSol; i++){
		noeud n = *(aretesSol2[i].noeud1), n2 = *(aretesSol2[i].noeud2);
		if( solution[g3_to_g[n2.id]] == 1 && !g->noeuds[g3_to_g[n2.id]].est_terminal ){
			n = n2;
			n2 = *(aretesSol2[i].noeud1);
		}

		if( solution[g3_to_g[n.id]] == 1 && !g->noeuds[g3_to_g[n.id]].est_terminal ){
			if(verbose)
				printf("Suppression du noeud %d (%d %d) (%d aretes)\n", g3_to_g[n.id]+1, g3_to_g[aretesSol2[i].noeud1->id]+1, g3_to_g[aretesSol2[i].noeud2->id]+1, n.nbAretes);

			//retirer aretesSol2[i] de la solution
			solution[g3_to_g[n.id]] = 0;
			solution[g3_to_g[n2.id]]--;


			//TODO ... boucler

			//impossible de trouver une instance avec un tel noeud a supprimer

		}
	}

	/*
	puts("solution : ");
	for(int i = 0; i < g->nbNoeuds; i++)
		printf("%d  ", !!solution[i]); // !! renvoi 0 si 0 et 1 si >=1
	puts("");
	*/
/*
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		int c = 0;
		for(int j = 0; j < g->nbNoeuds; j++){
			if(!g->noeuds[j].est_terminal) {
				population[i][c] =  !!solution[j];
				c++;
			}
		}
		//0 1 0 1 sur l'exemple
		printf("individu:\t");
		for(int j = 0; j < g->nbNonTerminaux; j++)
			printf("%d  ",population[i][j]);
		puts("");
	}
*/
/*
	//0 1 0 1 sur l'exemple (ou 1  0  1  0  1  1  1 si on affiche tout)
	printf("\tindividu: ");
	for(int j = 0; j < g->nbNoeuds; j++)
		printf("%d  ",!!solution[j]);
	puts("");
*/

	//if(verbose) printf("\tindividu généré de valeur %d\n",valsol);
	//on copie la solution dans noeudsactifs
	for(int i = 0; i < g->nbNoeuds; i++)
		noeudsactifs[i] = !!solution[i]; // !! renvoie 0 si 0 et 1 si != 0

	//liberation memoire

	free(aretesSol);
	free(aretesSol2);
	free(g3_to_g);
	free(g_to_g3);
	free(solution);
	free(dist);

	for(int i=0; i < g->nbNoeuds; i++){
		free(prec[i]);
	}
	free(prec);

	freeGraphe(g2);
	freeGraphe(g3);

}


void generer_population_heuristique_PCC(graphe* g, int** population, double alea, const int verbose){
	if(verbose) printf("\n** génération de population via heuristique 1 (PCC) **\n");
	int* poids_origines;
	poids_origines = (int*)calloc(g->nbAretes, sizeof(int));

	//on copie les valeurs des poids de g pour les remettre à la fin
	for(int i = 0; i < g->nbAretes; i++){
		poids_origines[i] = g->aretes[i].poids;
	}
	//printpoidsaretes(g);

	int* noeudsactifs = (int*)calloc(g->nbNoeuds, sizeof(int)); // 1 si le noeud est présent, 0 sinon
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		for(int j = 0; j < g->nbNoeuds; j++){
			noeudsactifs[j] = 0;
		}
		//modification aleatoire de g
		// printf(">>> %f\n", generer_uniforme(-alea, alea));
		for(int k = 0; k < g->nbAretes; k++){
			g->aretes[k].poids = poids_origines[k] + poids_origines[k] * generer_uniforme(-alea, alea);
		}
		//printtabint(g->aretes);
		//printpoidsaretes(g);
		// génération d'un individu depuis g
		generer_population_heuristique_PCC_one(g, noeudsactifs, verbose);
		// affichage des noeuds actifs
		/*	printf("noeuds actifs : ");
			for(int i = 0; i < g->nbNoeuds; i++){
				printf("%d ", noeudsactifs[i]);
			}
			puts("\n");
			*/
		//fin affichage
		//on copie cet individu dans la population
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = noeudsactifs[g->nonTerminaux[j]->id];
		}
	}
	//on remet les poids d'origine sur g
	for(int i = 0; i < g->nbAretes; i++){
		g->aretes[i].poids = poids_origines[i];
	}
	//printpoidsaretes(g);

	free(noeudsactifs);
	free(poids_origines);

	if(verbose) printf("*** population via heuristique PCC terminée. ***\n\n");
}

void printpoidsaretes(graphe* g){
	printf("poids aretes : ");
	for(int i = 0; i < g->nbAretes; i++){
		printf("%d ", g->aretes[i].poids);
	}
	puts("");
}

void generer_population_heuristique_ACPM(graphe* g, int** population, double alea, const int verbose){
	if(verbose) printf("\n** génération de population via heuristique 2 (ACPM) **\n");
	int* poids_origines = (int*)calloc(g->nbAretes, sizeof(int));
	//on copie les valeurs des poids de g pour les remettre à la fin
	for(int i = 0; i < g->nbAretes; i++){
		poids_origines[i] = g->aretes[i].poids;
	}
	//printpoidsaretes(g);

		int* noeudsactifs = (int*)calloc(g->nbNoeuds, sizeof(int)); // 1 si le noeud est présent, 0 sinon
	for(int i = 0; i < TAILLE_POPULATION * 2; i++){
		for(int j = 0; j < g->nbNoeuds; j++){
			noeudsactifs[j] = 0;
		}
		//modification aleatoire de g
		// printf(">>> %f\n", generer_uniforme(-alea, alea));
		for(int k = 0; k < g->nbAretes; k++){
			g->aretes[k].poids = poids_origines[k] + poids_origines[k] * generer_uniforme(-alea, alea);
		}
		//printpoidsaretes(g);
		// génération d'un individu depuis g
		generer_population_heuristique_ACPM_one(g, noeudsactifs, verbose);
		// affichage des noeuds actifs
		/*	printf("noeuds actifs : ");
			for(int i = 0; i < g->nbNoeuds; i++){
				printf("%d ", noeudsactifs[i]);
			}
			puts("\n");
			*/
		//fin affichage
		//on copie cet individu dans la population
		for(int j = 0; j < g->nbNonTerminaux; j++){
			population[i][j] = noeudsactifs[g->nonTerminaux[j]->id];
		}
	}
	//on remet les poids d'origine sur g
	for(int i = 0; i < g->nbAretes; i++){
		g->aretes[i].poids = poids_origines[i];
	}
	//printpoidsaretes(g);

	free(noeudsactifs);
	free(poids_origines);


	if(verbose) printf("*** population via heuristique ACPM terminée. ***\n\n");
}
void generer_population_heuristique_ACPM_one(graphe* g, int* noeudsactifs, const int verbose){

	//au début la solution "partielle" contient tout le graphe, on a encore supprimé aucun noeud
	int* solutionPartielle;
	solutionPartielle = (int*) calloc(g->nbNoeuds, sizeof(int));
	for(int i = 0; i < g->nbNoeuds; i++){
		solutionPartielle[i] = 1;
	}

	arete* aretesSol = (arete*) calloc(g->nbAretes, sizeof(arete));
	while (1) { // quand on aura fini on fera un return
		//kruskal pour ACPM
		int nbAretesSol = 0;

		int val = kruskal_partiel(g, solutionPartielle, /*sorties :*/ aretesSol, &nbAretesSol);
		if(verbose){
			//printf("solution kruskal: %d aretes, val %d\n", nbAretesSol, val);
			/*for(int i=0; i< nbAretesSol; i++) {
				printf("%d %d %d terminal? %d %d\n", aretesSol[i].noeud1->id+1, aretesSol[i].noeud2->id+1, aretesSol[i].poids, aretesSol[i].noeud1->est_terminal, aretesSol[i].noeud2->est_terminal);
			}*/
		}

		/*
		// TODO SUPPRIMER SI USELESS NOW ??
		//on recupere le noeud max dans les arêtes
		int noeudmax = 0;
		for (int i = 0 ; i < nbAretesSol; i++){
			int noeud1 = aretesSol[i].noeud1->id;
			int noeud2 = aretesSol[i].noeud2->id;
			//printf("adding %d and %d to vector\n", noeud1, noeud2);
			if(noeud1 > noeudmax) noeudmax = noeud1;
			if(noeud2 > noeudmax) noeudmax = noeud2;
		}
		//if(verbose) printf("noeudmax = %d \n", noeudmax);
		*/

		//on cherche les noeuds actifs dans l'ACPM
		//int* noeudsactifs = (int)calloc(noeudmax, sizeof(int)); // 1 si le noeud est présent, 0 sinon
		//reinitialise noeudsactifs
		for(int i = 0; i < g->nbNoeuds; i++)
			noeudsactifs[i] = 0;

		for (int i = 0 ; i < nbAretesSol; i++){
			int noeud1 = aretesSol[i].noeud1->id;
			int noeud2 = aretesSol[i].noeud2->id;
			//if(verbose) printf("\tadding %d and %d to vector\n", noeud1+1, noeud2+1);
			noeudsactifs[noeud1] = 1;
			noeudsactifs[noeud2] = 1;
		}
		int nbnoeudsacpm = 0;
		//for (int i = 0 ; i < noeudmax; i++)
		for(int i =0; i < g->nbNoeuds; i++)
			if(noeudsactifs[i]) nbnoeudsacpm++;

		/*
		if(verbose) printf("noeuds actifs ACPM : %d noeuds\n\t", nbnoeudsacpm);
		for (int i = 0 ; i < noeudmax; i++)
			if(verbose) printf("%d ",noeudsactifs[i]);
		if(verbose) puts("");
		*/

		//maintenant on connait le nombre de noeuds et les noeuds de cet ACPM
		// fonction qui retourne 1 si le noeud est une feuille (arité 1), 0 sinon
		int isfeuille(int noeud) {
			//on parcours toutes les aretes et on compte le nombre de fois qu'on observe ce noeud
			int c = 0;
			for (int i = 0 ; i < nbAretesSol; i++){
				int noeud1 = aretesSol[i].noeud1->id;
				int noeud2 = aretesSol[i].noeud2->id;
				if(noeud1 == noeud) c++;
				if(noeud2 == noeud) c++;
				//si on l'a vu 2 fois on sait déjà que ce n'est pas une feuille
				if(c >= 2) return 0;
			}
			//if(verbose) printf("\tfeuille de l'ACPM: noeud %d\n", noeud+1);
			if(c==1) return 1; // c'est une feuille !
			else return 0;
		}
		//tester si toutes les feuilles de l'arbre sont des sommets terminaux
		// les feuilles sont les sommets d'arité 1
		int allterm = 1; //boolean = 1 si toutes les feuilles sont des sommets terminaux
		for(int i=0; i< g->nbNoeuds && allterm; i++) {
			if(noeudsactifs[i]) { // test si c'est un sommet de l'ACPM
				if( isfeuille(i) ) { // test si c'est bien un sommet feuille dans l'ACPM
					if( ! g->noeuds[i].est_terminal ) { // test si sommet feuille ACPM est NON terminal dans g
						//if(verbose) printf("\tnoeud %d est une feuille de l'ACPM mais n'est pas terminal\n", i+1);
						allterm = 0;
						solutionPartielle[i] = 0; // on élimine si c'est un noeud feuille (degré 1) non terminal
					}
				}
			}
		}
		//if(verbose) printf("\tallterm = %d\n", allterm);

		if(allterm) { // si toutes les feuilles sont des noeuds terminaux, retourner l'arbre obtenu (dans population)
			/* // maintenant on ne fait plus qu'un individu dans cette fonction, retourné dans noeudsactifs
			for(int i = 0; i < TAILLE_POPULATION * 2; i++){
				for(int j = 0; j < g->nbNonTerminaux; j++){
					population[i][j] = noeudsactifs[g->nonTerminaux[j]->id];
				}
			}*/
			/*
			int coutacpm = 0;
			printf("aretessol poids :");
			for (int i = 0 ; i < nbAretesSol; i++){
				printf("%d ", aretesSol[i].poids);
				coutacpm += aretesSol[i].poids;
			}
			puts("");
			if(verbose) printf("heuristique ACPM: individu de coût %d. \n", coutacpm);
			*/

			free(solutionPartielle);
			free(aretesSol);
			return; // on retourne l'arbre obtenu dans la population --> on sort

		}
		//on boucle via le while(1)
	}

	free(solutionPartielle);
    free(aretesSol);

}


///algo genetique
void noeuds_steiner_gene(graphe* g, int heuristique, String dest, const int maxTime, const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes)
{
	srand(time(NULL));
	clock_t debut = clock();
	double tempscourant = 0;
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
	for(int i = 0; i < TAILLE_POPULATION * 2; i++)
		population[i] = (int*) calloc(g->nbNonTerminaux, sizeof(int));

	//heuristique
	if(heuristique == 1)
		generer_population_aleatoire(g, population, verbose);
	if(heuristique == 2)
		generer_population_heuristique_PCC(g, population, 0.2, verbose); //0.2 = 20% de random sur les poids
	if(heuristique == 3)
		generer_population_heuristique_ACPM(g, population, 0.2, verbose); //0.2 = 20% de random sur les poids
	//fin heuristique

	int gen = 0;

	for(int i = 0; i < TAILLE_POPULATION; i++){
		//creation d'un tableau de solutions comprenant les sommets terminaux
		for(int j = 0; j < g->nbNonTerminaux; j++){
			/*solution actuelle est codé sur TOUS les sommets, ceux terminaux sont déjà à 1*/
			/* donc on copie ceux non-terminaux dedans */
			solutionActuelle[g->nonTerminaux[j]->id] = population[i][j];
		}

		sol = kruskal2(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);
		solutions[i] = sol;
		//amelioration trouvee
		if(sol < *valeurSolution){
			tempscourant = (clock() - debut) / (double)CLOCKS_PER_SEC ;
			if(verbose) printf("Amelioration trouvee de valeur %d (generation %d) après %f sec.\n", sol, gen, tempscourant);
			*valeurSolution = sol;
			*nbAretes = nbAretesSol;
			for(int k = 0; k < nbAretesSol; k++){
				aretes[k] = aretesSol[k];
			}
		}
	}
	gen++;

	do{

		//calcul nouvelle solution

		for(int i = 0; i < TAILLE_POPULATION; i++){
			if(maxTime > -1 && (clock() - debut) / (double)CLOCKS_PER_SEC > (double)maxTime) break;

			//selection de deux individus
			//on prend un individu au hasard, puis on determine s'il est selectionne en fonction de la valeur de sa solution
			int parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( *valeurSolution / (double)(solutions[parent1]) < generer_uniforme(0, 1)){
				parent1 = (int)generer_uniforme(0, TAILLE_POPULATION);
			}

			int parent2 = (int)generer_uniforme(0, TAILLE_POPULATION);
			while( parent1 == parent2 || *valeurSolution / (double)(solutions[parent2]) < generer_uniforme(0, 1)){
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
			if(maxTime > -1 && (clock() - debut) / (double)CLOCKS_PER_SEC > (double)maxTime) break;

			//creation d'un tableau de solutions comprenant les sommets terminaux
			for(int j = 0; j < g->nbNonTerminaux; j++){
				solutionActuelle[g->nonTerminaux[j]->id] = population[i][j];


				sol = kruskal2(g, solutionActuelle, M, parents, rangs, aretesTab, aretesSol, &nbAretesSol);
				solutions[i] = sol;

				//amelioration trouvee
				if(sol < *valeurSolution){
					tempscourant = (clock() - debut) / (double)CLOCKS_PER_SEC;
					if(verbose) printf("Amelioration trouvee de valeur %d (generation %d) après %f sec.\n", sol, gen, tempscourant);
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


	for(int i = 0; i < TAILLE_POPULATION * 2; i++)
		free(population[i]);
	free(population);

	free(aretesSol);
	free(aretesTab);
	free(solutionActuelle);
	free(parents);
	free(rangs);
	free(solutions);

	if(verbose){printf("\nNombre de generations au total : %d.\n", gen);}
}

//renvoi 1 si il existe une arete entre ces deux sommets, 0 sinon
/*
int isarete(graphe* g, int n1, int n2) {
	for(int i = 0; i < g->nbAretes; i++) {
		int noeud1 = g->aretes[i].noeud1->id;
		int noeud2 = g->aretes[i].noeud2->id;

		if(noeud1 == n1 && noeud2 == n2)
			return 1;
		if(noeud1 == n2 && noeud2 == n1)
			return 1;
	}
	return 0; // rien trouvé
}
*/

///algo de recherche locale 
void noeuds_steiner_local(graphe* g, int heuristique, String dest, const int maxTime,const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes){
	srand(time(NULL));
	clock_t debut = clock();
	*valeurSolution = INT_MAX;	//meilleure solution
	int tempsrestant = maxTime;
	double tempscourant = 0;
	double alea = 0.2; // 0.2 = 20%
	int gen = 0;

	//variables temporaires
	int valeurSolutionTemp;
	int nbAretesTemp;
	arete* aretesTemp = (arete*) calloc(g->nbAretes, sizeof(arete));

	while(tempsrestant > 0) {
		if(gen == 0) // alea de 0 pour le premier
			noeuds_steiner_local_one(g, heuristique, tempsrestant, 0, *valeurSolution, verbose, /*sorties :*/ &valeurSolutionTemp, &nbAretesTemp, aretesTemp);
		else
			noeuds_steiner_local_one(g, heuristique, tempsrestant, alea, *valeurSolution, verbose, /*sorties :*/ &valeurSolutionTemp, &nbAretesTemp, aretesTemp);
		gen++;
		//si meilleure solution on met à jour
		if(valeurSolutionTemp < *valeurSolution) {
			*nbAretes = nbAretesTemp;
			*valeurSolution = valeurSolutionTemp;
			*nbAretes = nbAretesTemp;
			for(int k = 0; k < nbAretesTemp; k++){
				aretes[k] = aretesTemp[k];
			}
			tempscourant = (clock() - debut) / (double)CLOCKS_PER_SEC;
			if(verbose) printf("\tAmelioration trouvee de valeur %d après %f sec\n", valeurSolutionTemp, tempscourant);
		}
		//recalcule du temps restant
		tempscourant = (clock() - debut) / (double)CLOCKS_PER_SEC;
		tempsrestant = maxTime - tempscourant;
		//si le temps est dépassé on quitte
		if(maxTime > -1 && (clock() - debut) / (double)CLOCKS_PER_SEC > (double)maxTime) break;
	}

	free(aretesTemp);

}
// fait une seule recherche locale
void noeuds_steiner_local_one(graphe* g, int heuristique, const int maxTime, double alea, int bestsol, const int verbose, /*sorties :*/ int* valeurSolution, int* nbAretes, arete* aretes){
	clock_t debut = clock();
	*valeurSolution = INT_MAX;	//meilleure solution
	//double alea = 0; // 0.2 = 20%

	//recupération d'une solution initiale
	int* individu = (int*) calloc(g->nbNonTerminaux, sizeof(int));
	int* solutionfull = (int*) calloc(g->nbNoeuds, sizeof(int));


    int* poids_origines = (int*)calloc(g->nbAretes, sizeof(int));
	//on copie les valeurs des poids de g pour les remettre à la fin
	for(int i = 0; i < g->nbAretes; i++){
		poids_origines[i] = g->aretes[i].poids;
	}
	//printpoidsaretes(g);

	//modification aleatoire de g
	// printf(">>> %f\n", generer_uniforme(-alea, alea));
	for(int k = 0; k < g->nbAretes; k++){
		g->aretes[k].poids = poids_origines[k] + poids_origines[k] * generer_uniforme(-alea, alea);
	}
	//printpoidsaretes(g);
	// génération d'un individu depuis g
	
	//heuristique
	if(heuristique == 1) {
		double p = generer_uniforme(0.2,0.5);
		for(int j = 0; j < g->nbNonTerminaux; j++){
			solutionfull[j] = generer_uniforme(0, 1) > p;
			//printf("%d \t %d\n", i, population[i][j]);
		}
	}
	if(heuristique == 2)
		generer_population_heuristique_PCC_one(g, solutionfull, verbose); //0.2 = 20% de random sur les poids
	if(heuristique == 3)
		generer_population_heuristique_ACPM_one(g, solutionfull, verbose); //0.2 = 20% de random sur les poids
	//fin heuristique


	//on remet les poids d'origine sur g
	for(int i = 0; i < g->nbAretes; i++){
		g->aretes[i].poids = poids_origines[i];
	}
	////////////////////////////////


	convertfulltopartiellesolution(g, solutionfull, individu);
/*
	printf("solution initiale: ");
	for(int i = 0; i < g->nbNonTerminaux; i++)
		printf("%d ", individu[i]);
	puts("");

*/
	//convertpartielletofullsolution(g, individu, solutionfull);
	//printf("solutionfull : "); printtabint(solutionfull, g->nbNoeuds);

	// pour avoir l'arbre de steiner on applique kruskal sur l'individu
	arete* aretesSol = (arete*) calloc(g->nbAretes, sizeof(arete));
	int nbAretesSol = 0;
	convertpartielletofullsolution(g, individu, solutionfull);
	int val = kruskal_partiel(g, solutionfull, /*sorties :*/ aretesSol, &nbAretesSol);

	//on prend un return de base
	*valeurSolution = val;
	*nbAretes = nbAretesSol;
	for(int k = 0; k < nbAretesSol; k++){
		aretes[k] = aretesSol[k];
	}
	//fin return de base

/*
	printf("cet individu donne une solution de %d aretes et de coût %d:\n", nbAretesSol, val);
	for(int i = 0; i < nbAretesSol; i++) {
		printf("\t%d %d %d\n", aretesSol[i].noeud1->id+1, aretesSol[i].noeud2->id+1, aretesSol[i].poids);
	}
	puts("");
*/

	void resetIndividu(int* indi){
		for(int i = 0; i < g->nbNonTerminaux; i++)
			indi[i] = 0;
	}
	void copieIndividu(int* source, int* cible){
		for(int i = 0; i < g->nbNonTerminaux; i++)
			cible[i] = source[i];
	}

	int ameliore = 1; // bool
	int gen = 0;
	int* newindividu = (int*) calloc(g->nbNonTerminaux, sizeof(int));
	while(ameliore) { // tant qu'on améliore la solution on continue
		//si on à pas dépassé le temps imparti
		if(maxTime > -1 && (clock() - debut) / (double)CLOCKS_PER_SEC > (double)maxTime) break;

		//remarque: dans cette recherche locale on selectionne le premier mouvement améliorant
		ameliore = 0;

		// tester les solutions par insertion
		for(int i = 0; i < g->nbNonTerminaux && !ameliore; i++) {
			copieIndividu(individu, newindividu); // copie individu dans newindividu
			if(individu[i] == 0) { //si le noeud est inactif on tente de l'insérer
				newindividu[i] = 1;
				/* on compte le nombre d'aretes qui connecte le nouveau sommet s au noeuds de {S}U{T}
				   si on trouve 0   -> graphe induit pas connexe
				   si on trouve 1   -> noeud de degré 1 donc inutile
				   si on trouve >=2 -> c'est bon et on lance kruskal pour ACPM
				   */
				int cptaretes = 0;
				int idcourant = g->nonTerminaux[i]->id;
				//printf("test d'insertion du sommet %d\n", idcourant+1);
				// aretes avec S
				for(int i2 = 0; i2 < g->nbNonTerminaux; i2++) {
					if(individu[i2] == 1) { // si c'est bien un sommet de S
						int idpair = g->nonTerminaux[i2]->id; //recup son id
						int valarete = isarete(g, idcourant, idpair);
						//printf("\tarete entre %d et %d nonterminal ?? -> %d\n", idcourant+1, idpair+1, valarete);
						if(valarete) cptaretes++;
					}
				}

				// aretes avec T
				for(int i2 = 0; i2 < g->nbTerminaux; i2++) { // si c'est bien un sommet de T
					int idpair = g->terminaux[i2]->id; //recup son id
					int valarete = isarete(g, idcourant, idpair);
					//printf("\tarete entre %d et %d terminal ?? -> %d\n", idcourant+1, idpair+1, valarete);
					if(valarete) cptaretes++;
				}

				//printf("le sommet %d possède %d aretes avec {S}U{T}\n", idcourant+1, cptaretes);
			//	printtabint(newindividu, g->nbNonTerminaux);
				//si tests OK, on lance kruskal pour avoir l'ACPM
				if(cptaretes >= 2) {

//					printf("test d'insertion de l'arete %d\n", idcourant+1);
					convertpartielletofullsolution(g, newindividu, solutionfull);
					//printf("solutionfull : "); printtabint(solutionfull, g->nbNoeuds);
					val = kruskal_partiel(g, solutionfull, /*sorties :*/ aretesSol, &nbAretesSol);
					//printf("kruskal : %d\n", val);
					if(val < *valeurSolution) {
						//if(verbose && val < bestsol) printf("\tAmelioration trouvee (par insertion de l'arête %d) \n\t\tde valeur %d (amélioration n°%d).\n", idcourant+1, val, gen);
						nbAretesSol = nbAretesSol;
						*valeurSolution = val;
						*nbAretes = nbAretesSol;
						for(int k = 0; k < nbAretesSol; k++){
							aretes[k] = aretesSol[k];
						}
						gen++;
						ameliore = 1;
						copieIndividu(newindividu, individu); //copie newindividu dans individu
					}
				}
			}
		}

		// tester les solutions par élimination
		for(int i = 0; i < g->nbNonTerminaux && !ameliore; i++) {
			//printf("individu : "); printtabint(individu, g->nbNonTerminaux);
			copieIndividu(individu, newindividu); // copie individu dans newindividu
			if(individu[i] == 1) { //si le noeud est actif on tente de l'éliminer
				newindividu[i] = 0;
				int idcourant = g->nonTerminaux[i]->id;
			//	printf("test d'élimination de l'arete %d\n", idcourant+1);
				convertpartielletofullsolution(g, newindividu, solutionfull);
				//printf("solutionfull : "); printtabint(solutionfull, g->nbNoeuds);
				val = kruskal_partiel(g, solutionfull, /*sorties :*/ aretesSol, &nbAretesSol);
				if(val < *valeurSolution) {
					//if(verbose && val < bestsol) printf("\tAmelioration trouvee (par elimination de l'arete %d) \n\t\tde valeur %d (amélioration n°%d).\n", idcourant+1, val, gen);
					nbAretesSol = nbAretesSol;
					*valeurSolution = val;
					*nbAretes = nbAretesSol;
					for(int k = 0; k < nbAretesSol; k++){
						aretes[k] = aretesSol[k];
					}
					gen++;
					ameliore = 1;
					copieIndividu(newindividu, individu); //copie newindividu dans individu
				}
			}
		}
	}

	free(newindividu);
	free(aretesSol);
	free(individu);
	free(solutionfull);
	free(poids_origines);
}
