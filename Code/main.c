#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>
#include <dirent.h>
#include <libgen.h>

#include "inout.h"
#include "graphe.h"
#include "types.h"
#include "utils.h"

#if SC_PLATFORM == SC_PLATFORM_LINUX
#include <errno.h>
#endif

/**
Options actuellement disponibles :
-print : affiche le resultat du programme pour chaque instance
-verbose : detaille l'execution du programme
-time <X> : limite le temps a X secondes par instance (5 min = 300 sec)
-file <source_file> : le nom du fichier d'entree a ouvrir
-out <dest_folder> : le dossier de sortie, par exemple ../Output/B/
-gene : algo genetique seul
-local : algo de recherche locale seul
**/


void analysefichier(int print, int verbose, int heuristique, int maxtime, String source, String dest, int local, int gene){
    //lecture du fichier graphe
    graphe* g = lireFichier(source, verbose);
    if(g == NULL){
        printf("Fichier %s non trouve ou fichier vide. \n\n", source);
        return;
    }

    clock_t current = clock();
    double time_spent = 0;
    int coutSolution, nbAretes;
    arete* solution;
    solution = (arete*) calloc(g->nbAretes, sizeof(arete));

    String filename = NULL;

    if(dest) {
        filename = (String)malloc((strlen(basename(source)) + 1) * sizeof(char));
        strcpy(filename,basename(source));
        printf(">>> filename \"%s\" from %s\n", filename, source);
    }

    if(local){
        if(print) puts("Demarrage de l'algorithme de recherche locale.");

        noeuds_steiner_local(g, heuristique, dest, filename, maxtime, verbose, &coutSolution, &nbAretes, solution);

        time_spent = (double)(clock() - current) / CLOCKS_PER_SEC;
        if(print){
            printf("Recherche locale terminee en %f sec.\n", time_spent);
            printf("\nSolution trouvee de valeur %d (%d aretes). Affichage des aretes solution :\n", coutSolution, nbAretes);
            for(int i = 0; i < nbAretes; i++){
                printf("%d %d %d\n", solution[i].noeud1->id+1, solution[i].noeud2->id+1, solution[i].poids);
            }
            puts("");
        }
        //TODO : write solution on disk

    }

    if(gene){
        current = clock();
        if(print) puts("Demarrage de l'algorithme genetique.");

        noeuds_steiner_gene(g, heuristique, dest, filename, maxtime, verbose, &coutSolution, &nbAretes, solution);

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

    free(solution);
    freeGraphe(g);
}


int main(int argc, const char *argv[])
{

    puts("Demarrage du programme. \n");

    int verbose = 0, print = 0, maxtime = -1, gene = 1, local = 1;			//options de lancement
    String source = NULL, dest = NULL, dname = NULL;
    int heuristique = 0;

    //lecture des parametres
    char *ptr;
    for (int i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i], "-print")) print = 1;
        if(!strcmp(argv[i], "-verbose")) verbose = 1;
        if(!strcmp(argv[i], "-time") && i+1 < argc) maxtime = (int)strtol(argv[i+1], &ptr, 10);
        if(!strcmp(argv[i], "-file") && i+1 < argc){
            source = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(source, argv[i+1]);
        }
        else if(!strcmp(argv[i], "-dir") && i+1 < argc) {
            dname = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(dname, argv[i+1]);
        }
        if(!strcmp(argv[i], "-out") && i+1 < argc){
			dest = (String)malloc((strlen(argv[i+1]) + 1) * sizeof(char));
            strcpy(dest, argv[i+1]);

            if(opendir(dest)) {} else if (ENOENT == errno){
                /* Directory does not exist. */
                printf("error: output directory does not exist.\n");
                return EXIT_FAILURE;
            }

        }
        if(!strcmp(argv[i], "-gene")) local = 0;
        if(!strcmp(argv[i], "-local")) gene = 0;
        if(!strcmp(argv[i], "-pop")) heuristique = (int)strtol(argv[i+1], &ptr, 10);
    }

    if(!source && !dname)
    {
        puts("Aucune source specifiee, veuillez specifier la source avec -file ou -dir.");
        return EXIT_FAILURE;
    }
    if(heuristique < 0 || heuristique > 3 ) {
        printf("Mauvaise heuristique de population\n");
        heuristique = 0;
    }
    if(heuristique == 0) {
        heuristique = 1; // heuristique par défaut
        printf("Pas d'heuristique choisie, choix par défaut de l'heuristique %d.\n1 = aleatoire\n2 = Plus Court Chemin (PCC)\n3 = Arbre Couvrant de Poids Minimum (ACPM)\n", heuristique);
    }

    if(!local && !gene){
		local = 1;
		gene = 1;
    }

    if(verbose) printf("Arguments : print = %d, verbose = %d, pop = %d, maxtime = %d, file = %s, dir = %s, out = %s, local = %d, gene = %d\n", print, verbose, heuristique, maxtime, source, dname, dest, local, gene);

    if(dname) {
        printf("analyse d'un dossier:\n");
        DIR *d;
        struct dirent *dir;
        d = opendir(dname);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){ //strcmp: 0 si identique
                    int size = strlen(dname)+strlen(dir->d_name);
                    String fullfilename = (String)malloc((size + 1) * sizeof(char));
                    strcpy(fullfilename, dname);
                    strcat(fullfilename, dir->d_name);
                    //printf("%s%s\n", dname,dir->d_name);
                    printf("analyse de %s\n", fullfilename);
                    analysefichier(print, verbose, heuristique, maxtime, fullfilename, dest, local, gene);
                }
            }
            closedir(d);
        }
    }
    else analysefichier(print, verbose, heuristique, maxtime, source, dest, local, gene);

    free(source);

    puts("Programme termine avec succes. \n");
    return EXIT_SUCCESS;
}

