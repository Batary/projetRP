#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>
#include <dirent.h> 

#include "inout.h"
#include "graphe.h"
#include "types.h"
#include "utils.h"

#define OUT "../Output/"

/**
Options actuellement disponibles :
-print : affiche le resultat du programme pour chaque instance
-verbose : detaille l'execution du programme
-time <X> : limite le temps a X secondes par instance (5 min = 300 sec)
-file <source_file> : le nom du fichier d'entree a ouvrir
-out <dest_file> : le fichier de sortie, par défaut, ../Output/ ( + <nomfichier>.out )
-gene : algo genetique seul
-local : algo de recherche locale seul
**/

int main(int argc, const char *argv[])
{

    puts("Demarrage du programme. \n");

    double time = 0;
    int verbose = 0, print = 0, t = -1, gene = 1, local = 1;			//options de lancement
    String source = NULL, dest = OUT;
    int heuristique = 0;

    //lecture des parametres
    char *ptr;
    for (int i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i], "-print")) print = 1;
        if(!strcmp(argv[i], "-verbose")) verbose = 1;
        if(!strcmp(argv[i], "-time") && i+1 < argc) t = (int)strtol(argv[i+1], &ptr, 10);
        if(!strcmp(argv[i], "-file") && i+1 < argc){
            source = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(source, argv[i+1]);
        }
        else if(!strcmp(argv[i], "-dir") && i+1 < argc) {
          analysedossier(argv[i+1]);
        }
        if(!strcmp(argv[i], "-out") && i+1 < argc){
			dest = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(dest, argv[i+1]);
        }
        if(!strcmp(argv[i], "-gene")) local = 0;
        if(!strcmp(argv[i], "-local")) gene = 0;
        if(!strcmp(argv[i], "-pop")) heuristique = (int)strtol(argv[i+1], &ptr, 10);
    }

    if(!source)
    {
        puts("No source file has been specified. Please specify a source file with -file");
        return EXIT_FAILURE;
    }
    if(heuristique == 0) {
        heuristique = 1; // heuristique par défaut
        printf("pas d'heuristique choisie, choix par défaut de l'heuristique %d\n", heuristique);
    }
    if(heuristique < 1 || heuristique > 3 ) {
        printf("mauvaise heuristique de population");
    }

    if(!local && !gene){
		local = 1;
		gene = 1;
    }

    if(verbose) printf("Arguments : print = %d, verbose = %d, pop = %d, time = %d, file = %s, out = %s, local = %d, gene = %d\n", print, verbose, heuristique, t, source, dest, local, gene);
    time = (double)t;	//Comptage en secondes

	//lecture du fichier graphe
    graphe* g = lireFichier(source, verbose);
    if(g == NULL){
		puts("Fichier non trouve ou fichier vide. \n");
		return EXIT_FAILURE;
    }

    clock_t current = clock();
    double time_spent = time;
    int coutSolution, nbAretes;
    arete* solution;
    solution = (arete*) calloc(g->nbAretes, sizeof(arete));

	if(local){
		if(print) puts("Demarrage de l'algorithme de recherche locale.");

		noeuds_steiner_local(g, heuristique, time, verbose, &coutSolution, &nbAretes, solution);

		time_spent = (double)(clock() - current) / CLOCKS_PER_SEC;
		if(print){
			printf("Recherche locale terminee en %f sec.\n", time_spent);
			printf("\nSolution trouvee de valeur %d (%d aretes). Affichage des aretes solution :\n", coutSolution, nbAretes);
			for(int i = 0; i < nbAretes; i++){
				printf("%d %d %d\n", solution[i].noeud1->id, solution[i].noeud2->id, solution[i].poids);
			}
			puts("");
		}
		//TODO : write solution on disk

	}

	if(gene){
		current = clock();
		if(print) puts("Demarrage de l'algorithme genetique.");

		noeuds_steiner_gene(g, heuristique, time, verbose, &coutSolution, &nbAretes, solution);

		time_spent = (double)(clock() - current) / CLOCKS_PER_SEC;
		if(print){
			printf("Algorithme genetique termine en %f sec.\n", time_spent);
			printf("\nSolution trouvee de valeur %d (%d aretes). Affichage des aretes solution :\n", coutSolution, nbAretes);
			for(int i = 0; i<nbAretes; i++){
				printf("%d %d %d\n", solution[i].noeud1->id+1, solution[i].noeud2->id+1, solution[i].poids);
			}
			puts("");
		}
	}

	free(source);
	free(solution);
	freeGraphe(g);

    puts("Programme termine avec succes. \n");
    return EXIT_SUCCESS;
}



void analysedossier(String dname){
    printf("analyse d'un dossier:\n");
    DIR *d;
    struct dirent *dir;
    d = opendir(dname);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) //strcmp: 0 si identique
                printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}