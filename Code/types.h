#ifndef TYPES_H_
#define TYPES_H_

///Contient tous les types nécessaires au programme.

typedef char *String;

typedef struct _arete arete;

typedef struct {
	int id;
	int est_terminal;
	int nbAretes;
	arete* aretes; //non implemente, realoc necessaire.
	//on peut eventuellement ajouter ici les noeuds connectes si besoin
} noeud;

typedef struct _arete{
	int poids;
	//les noeuds que cette arete connecte
	noeud* noeud1;
	noeud* noeud2;
}arete;

typedef struct{
	int nbNoeuds;
	int nbAretes;
	int nbTerminaux;

	// listes d'elements que l'on pourra parcourir
	noeud* noeuds;
	// la liste de noeuds terminaux pointe sur la liste de noeuds ci dessus
	noeud** terminaux;
	// on pourrait ajouter ici les noeuds non terminaux

	arete* aretes;

} graphe;

#endif /* TYPES_H_ */
