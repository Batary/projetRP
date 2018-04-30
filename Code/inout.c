#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if(verbose) printf("%s ", line);

		//creation des noeuds
        if(!strcmp(line,"Nodes") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d ", val);
			g->nbNoeuds = val;
			g->noeuds = (noeud*)calloc(val,sizeof(noeud));
			for(int i = 0; i<val; i++){
				g->noeuds[i].id = i;
				g->noeuds[i].est_terminal = 0;
			}
        }

        //creation des aretes
        else if(!strcmp(line,"Edges") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d ", val);
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
			//associer les aretes aux noeuds -> tableau a realloc a chaque fois
			g->noeuds[val].aretes = (arete*)realloc(g->noeuds[val].aretes, g->noeuds[val].nbAretes * sizeof(arete));
			g->noeuds[val].aretes[g->noeuds[val].nbAretes - 1] = g->aretes[ac];

			ac++;
        }

        //ajout des noeuds terminaux
        else if(!strcmp(line,"Terminals") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d ", val);
			g->nbTerminaux = val;
			g->terminaux = (noeud**)calloc(val, sizeof(noeud*));

			g->nbNonTerminaux = g->nbNoeuds - val;
			g->nonTerminaux = (noeud**)calloc(g->nbNonTerminaux, sizeof(noeud*));
        }

        //lecture de la valeur des noeuds terminaux
        else if(!strcmp(line,"T") && fscanf(f, "%d", &val) == 1){
			if(verbose) printf("%d ", val);
			val--;
			g->noeuds[val].est_terminal = 1;
			g->terminaux[tc] = &g->noeuds[val];
			tc++;
        }

        if(verbose) puts("");
    }

    //ajout de la liste de noeuds non terminaux
    int c = 0;
    for(int i = 0; i < g->nbNoeuds; i++){
		if(!g->noeuds[i].est_terminal){
			g->nonTerminaux[c] = &g->noeuds[i];
			c++;
		}
    }

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

    fclose(f);
    return g;
}

//NOTE : tous les indices sont decales de -1 par rapport au fichier d'entree.
//A ne pas oublier a la sortie.




