#ifndef INOUT_H_
#define INOUT_H_
#include "types.h"

graphe* lireFichier(const String fichier, const int verbose);

void writeoutput(FILE* f, double tempslancement, double* tempsprecedent, double debut, int *oldbestval,
	int newbestval, int* lastvalwritten, int* lasttimewritten);

#endif /* INOUT_H_ */
