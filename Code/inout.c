#include <stdio.h>
#include <stdlib.h>

#include "types.h"
/*
void afficherGrille(grille g){
	printf("\n");
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			printf("%d ",g[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

//pour le debug
void afficherGrille3D(grille3D g, int val){
	printf("\n");
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			printf("%d ",g[i][j][val]);
		}
		printf("\n");
	}
	printf("\n");
}

//lecture du fichier contenant les grilles
grille* lireGrille(const String fichier, int* nbGrilles){
	grille* tabGrilles;
	tabGrilles = (grille*)malloc(MAXGRILLES * sizeof(grille));

	FILE *f;
	int idGrille = 0, i = 0, j = 0;

	if ((f = fopen(fichier, "r")) == NULL) return NULL; // fichier mal lu

	while(!feof(f) && (fscanf(f, "%1d", &tabGrilles[idGrille][i][j]) == 1)){
		j++;
		if(j >= 9){
			j = 0;
			i++;
		}
		if(i >= 9){
			i = 0;
			j = 0;
			idGrille++;
		}

	}
	fclose(f);

	*nbGrilles = idGrille;

	return tabGrilles;
}

//ecriture des grilles dans le fichier texte
int ecrireGrilles(const String fichier, grille* grilles, int nbGrilles){
	FILE *f;
	if ((f = fopen(fichier, "w")) == NULL) { //fichier inaccessible
		return -1;
	}
	for (int g = 0; g < nbGrilles; g++) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				fprintf(f,"%d",grilles[g][i][j]);
			}
		}
		fprintf(f,"\n");
	}

	return 0;
}
*/
