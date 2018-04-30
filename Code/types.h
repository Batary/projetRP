#ifndef TYPES_H_
#define TYPES_H_

///Contient tous les types nécessaires au programme.

typedef char *String;

typedef struct _arete arete;

typedef struct {
	int id;
	int est_terminal;
	int nbAretes;
	arete* aretes;
	//on peut eventuellement ajouter ici les noeuds connectes si besoin (meme si on peut y acceder indirectement)
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
	int nbNonTerminaux;

	// listes d'elements que l'on pourra parcourir
	noeud* noeuds;
	// la liste de noeuds terminaux pointe sur la liste de noeuds ci dessus
	noeud** terminaux;
	noeud** nonTerminaux;

	arete* aretes;

} graphe;

/*
typedef struct{
	int parent;
	int rang;
}set_aretes;
*/
#endif /* TYPES_H_ */
