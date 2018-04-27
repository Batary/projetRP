#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>

#include "inout.h"
#include "types.h"

#define OUT "../Output/"

/**
Options actuellement disponibles :
-print : affiche le resultat du programme pour chaque instance
-verbose : detaille l'execution du programme
-time <X> : limite le temps a X secondes par instance (5 min = 300 sec)
-file <source_file> : le nom du fichier d'entree a ouvrir
-out <dest_file> : le fichier de sortie, par défaut, ../Output/ ( + <nomfichier>.out )
**/

int main(int argc, const char *argv[])
{

    puts("Demarrage du programme. \n");

    double time = 0;
    int verbose = 0, print = 0, t=-1;					//options de lancement
    String source = NULL, dest = OUT;

    clock_t begin = clock(), current = clock();
    double time_spent, lastTime;

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
        if(!strcmp(argv[i], "-out") && i+1 < argc){
			dest = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(dest, argv[i+1]);
        }
    }

    if(!source)
    {
        puts("No source file has been specified. Please specify a source file with -file");
        return EXIT_FAILURE;
    }

    printf("Arguments : print = %d, verbose = %d, time = %d, file = %s, out = %s\n",print,verbose,t,source,dest);
    if(t == -1) time = -1;
    else time = (double)t;	//Comptage en secondes

    graphe* g = lireFichier(source);
    if(g == NULL){
		puts("Fichier non trouve ou fichier vide. \n");
		return EXIT_FAILURE;
    }

//Code commenté extrait d'un solveur de sudoku que j'ai fait l'annee derniere, qu'on pourra reutiliser

    /*
    	grille* grilles = lireGrille(SOURCE, &nbGrilles);

    	if(grilles == NULL || nbGrilles == 0){
    		puts("Fichier non trouve ou fichier vide. \n");
    		return EXIT_FAILURE;
    	}
    	int i; double execTime;
    	for (i = 0; i < nbGrilles; i++) {
    		if(print) printf("Grille %d : \n", i + 1);
    		if(print) afficherGrille(grilles[i]);
    		lastTime = time;

    		execTime = resoudre(grilles[i], &time, verbose);

    		time_spent = (double)(clock() - current) / CLOCKS_PER_SEC;
    		current = clock();
    		if(time != -1){time = lastTime - time_spent;}

    		if((execTime == -1 || time <= 0) && time != -1){		//temps écoulé
    			if(print)printf("Temps ecoule ! Arret du programme.\n");
    			break;
    		}
    		else{
    			printf("Temps d'execution (grille %d) : %f ms.\n", i+1, time_spent*1000);
    		}
    		if(print){
    			printf("Grille %d resolue : \n", i + 1);
    			afficherGrille(grilles[i]);
    		}
    	}

    	time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    	printf("Temps total : %f ms.\n", time_spent*1000);
    	if(print && time > 0)printf("Temps restant : %f ms.\n", time*1000);

    	if(ecrireGrilles(DEST, grilles, i) == -1){
    		printf("ERREUR ! Impossible d'ecrire dans le fichier \"%s\" ! \n", DEST);
    		return EXIT_FAILURE;
    	}
    	*/

    puts("Programme termine avec succes. \n");
    return EXIT_SUCCESS;
}
