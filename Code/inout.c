#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"



///lit le fichier contenant le graphe
///on suppose ici que le fichier est bien formé
graphe* lireFichier(const String fichier, const int verbose)
{
    graphe* g;
    g = (graphe*)calloc(1,sizeof(graphe));
    int  ret, val, val2, val3, ac = 0, tc = 0;
    char line[50];
    FILE *f = NULL;
    if ((f = fopen(fichier, "r")) == NULL) return NULL; // fichier mal lu

    if(verbose) puts("");

    while((ret = fscanf (f,"%s", line)) != EOF && ret == 1)
    {


		//creation des noeuds
        if(!strcmp(line,"Nodes") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d", val);
			g->nbNoeuds = val;
			g->noeuds = (noeud*)calloc(val,sizeof(noeud));
			for(int i = 0; i<val; i++){
				g->noeuds[i].id = i;
				g->noeuds[i].est_terminal = 0;
			}
        }

        //creation des aretes
        else if(!strcmp(line,"Edges") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d", val);
			g->nbAretes = val;
			g->aretes = (arete*)calloc(val,sizeof(arete));
        }

        //lecture de la valeur des aretes
        else if(!strcmp(line,"E") && fscanf(f, "%d %d %d", &val, &val2, &val3) == 3){
			//noeud1, noeud2, poids
			if(verbose) printf("%d %d %d", val, val2, val3);
			val--; val2--;
			g->aretes[ac].noeud1 = &g->noeuds[val];
			g->aretes[ac].noeud2 = &g->noeuds[val2];
			g->aretes[ac].poids = val3;

			g->noeuds[val].nbAretes++;
			g->noeuds[val2].nbAretes++;
			//associer les aretes aux noeuds -> tableau a realloc a chaque fois
			g->noeuds[val].aretes = (arete**)realloc(g->noeuds[val].aretes, g->noeuds[val].nbAretes * sizeof(arete*));
			g->noeuds[val].aretes[g->noeuds[val].nbAretes - 1] = &g->aretes[ac];
			g->noeuds[val2].aretes = (arete**)realloc(g->noeuds[val2].aretes, g->noeuds[val2].nbAretes * sizeof(arete*));
			g->noeuds[val2].aretes[g->noeuds[val2].nbAretes - 1] = &g->aretes[ac];

			ac++;
        }

        //ajout des noeuds terminaux
        else if(!strcmp(line,"Terminals") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d", val);
			g->nbTerminaux = val;
			g->terminaux = (noeud**)calloc(val, sizeof(noeud*));

			g->nbNonTerminaux = g->nbNoeuds - val;
			g->nonTerminaux = (noeud**)calloc(g->nbNonTerminaux, sizeof(noeud*));
        }

        //lecture de la valeur des noeuds terminaux
        else if(!strcmp(line,"T") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d", val);
			val--;
			g->noeuds[val].est_terminal = 1;
			g->terminaux[tc] = &g->noeuds[val];
			tc++;
        }
        else continue;
		if(verbose) printf(" %s\n", line);
        //if(verbose) puts("");
    }

    //ajout de la liste de noeuds non terminaux
    int c = 0;
    for(int i = 0; i < g->nbNoeuds; i++){
		if(!g->noeuds[i].est_terminal){
			g->nonTerminaux[c] = &g->noeuds[i];
			c++;
		}
    }

	fclose(f);
    //tri des aretes par poids croissant pour preparer l'algorithme de Kruskal
    /*
    for(int i = 0; i<g->nbAretes; i++){
		printf("%d %d %d\n", g->aretes[i].noeud1->id, g->aretes[i].noeud2->id, g->aretes[i].poids);
    }
    */
    /*
	quickSort(g->aretes, 0, g->nbAretes - 1);
	if(verbose){
		puts("\nAffichage des aretes triees (avec ajustement de l'index) :");
		for(int i = 0; i<g->nbAretes; i++){
			printf("%d %d %d\n", g->aretes[i].noeud1->id, g->aretes[i].noeud2->id, g->aretes[i].poids);
		}
		puts("");
    }
	*/


    return g;
}

//NOTE : tous les indices sont decales de -1 par rapport au fichier d'entree.
//A ne pas oublier a la sortie.


// écrit l'output en mode 'pas de temps' et complète si il manque des données (en mode histograme cumulé)
void writeoutput(FILE* f, double tempslancement, double* tempsprecedent, double debut,
	int* oldbestval, int newbestval, int* lastvalwritten, int* lasttimewritten) {
	double tempscourant = ((clock() - debut) / (double)CLOCKS_PER_SEC) + tempslancement;
	int pas = 1; // 1 = 1 seconde //todo: passer en double pour permettre des pas de 0.5 par exemple ?
	//printf("if( %f >= (int)(%f + %d) ) ? --> ", tempscourant, *tempsprecedent, pas);
	//printf("if( %f >= (int)(%d + %d) ) ? --> ", tempscourant, *lasttimewritten, pas);
	// TODO :si on veut un pas en double: tronquer avec le modulo avec le pas de temps au lieu de caster en int

	// on test si il est temps d'écrire
	//if(((int)tempscourant) >= (int)(*tempsprecedent + pas) || *lastvalwritten == -1) {  // *lastvalwritten == -1 -> force 1ere ecriture
	if(((int)tempscourant) >= (int)(*lasttimewritten + pas) || *lastvalwritten == -1) {  // *lastvalwritten == -1 -> force 1ere ecriture
		//printf(" *OUI* \n");
		// si on à raté des étapes on remplit avec la dernière meilleure valeur
		//if( ((int)tempscourant) - (int)(*tempsprecedent + pas) > 0) {
		if( ((int)tempscourant) - (int)(*lasttimewritten + pas) > 0) {
			//printf("tempscourant = %d,  tempsprecedent=%d, il faut rattraper.\n", (int)tempscourant, (int)*tempsprecedent);
			//double variation = tempscourant - (*tempsprecedent + pas );
			double variation = tempscourant - (*lasttimewritten + pas );
			//printf("variation: %d seconde(s) à rattraper\n", (int)variation);
			//for(int i = (int)*tempsprecedent + pas; i <= (int)(tempscourant - pas /*+ 1*/); i+=pas) {
			for(int i = (int)*lasttimewritten + pas; i <= (int)(tempscourant - pas /*+ 1*/); i+=pas) {
				//printf("write %d \t %d\n", (int)(i+tempslancement), *oldbestval);
				//fprintf(f, "%d \t %d\n", (int)(i+tempslancement), *oldbestval);
				printf("write %d \t %d\n", (int)(i), *oldbestval);
				fprintf(f, "%d \t %d\n", (int)(i), *oldbestval);
			}
		}
		// puis on ecrit la nouvelle meilleure valeur
		fprintf(f, "%d \t %d\n", (int)(tempscourant), newbestval);
		printf("write %d \t %d\n", (int)(tempscourant), newbestval);
		*tempsprecedent = (int)tempscourant;
		*oldbestval = newbestval;
		*lastvalwritten = newbestval;
		*lasttimewritten = (int)(tempscourant);
	} //else printf(" NON \n");
	//sinon on ecrit rien
}




