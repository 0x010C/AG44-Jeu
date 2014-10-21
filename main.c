#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void fifo_add(int *actuel, int nbValeurs, int valeur)
{
	int i;
	for(i=0;i<nbValeurs;i++)
	{
		if(actuel[i] == valeur)
			return;
		else if(actuel[i] == -1)
		{
			actuel[i] = valeur;
			return;
		}
	}
}

int fifo_pop(int *actuel, int nbValeurs)
{
	int valeur = actuel[0];
	int i;
	for(i=0;i<nbValeurs-1;i++)
	{
		actuel[i] = actuel[i+1];
	}
	actuel[nbValeurs-1] = -1;

	return valeur;
}

int fifo_search(int *actuel, int nbValeurs, int valeur)
{
	int i;
	for(i=0;i<nbValeurs;i++)
		if(actuel[i] == valeur)
			return i;
	return -1;
}

int fifo_isEmpty(int *actuel, int nbValeurs)
{
	int i;
	for(i=0;i<nbValeurs;i++)
		if(actuel[i] != -1)
			return 0;
	return 1;
}

int main(int argc, char **argv)
{
	int **A = NULL;
	int **N = NULL;
	int *GL = NULL;
	FILE *fichier = NULL;
	int nbPlaces=0, nbLevels=0;
	int *listePositif = NULL;
	int *listeNegatif = NULL;
	int *listeAVisiter = NULL;
	int i, j, k, buf;

	/* 
	 * Récupération de la Matrice
	 */

	/* Ouverture du fichier */
	fichier = fopen("input.data", "r");
	if(fichier == NULL)
		fprintf(stderr,"Erreur : Ouverture du fichier impossible :'(\n");

	/* Récupération du nombre de zones */
	fscanf(fichier, "%d", &nbPlaces);

	/* Allocation de la mémoire pour contenir la matrice */
	A = (int**) malloc(sizeof(int*)*nbPlaces);
	for(i=0; i<nbPlaces; i++)
		A[i] = (int*) malloc(sizeof(int)*nbPlaces);

	/* Récupération des données et stoquage dans la matrice */
	for(i=0; i<nbPlaces; i++)
		for(j=0; j<nbPlaces; j++)
			fscanf(fichier, "%d", &A[i][j]);

	/* Fermeture du fichier */
	fclose(fichier);


	/*
	 * Recherche des composants fortement connectés
	 */

	/* Allocation de la mémoire pour enregistrer les niveaux et pour contenir des tableaux temporaires */
	GL = (int*) malloc(sizeof(int)*nbPlaces);
	listePositif = (int*) malloc(sizeof(int)*nbPlaces);
	listeNegatif = (int*) malloc(sizeof(int)*nbPlaces);
	listeAVisiter = (int*) malloc(sizeof(int)*nbPlaces);
	for(i=0; i<nbPlaces; i++)
	{
		listePositif[i] = -1;
		listeNegatif[i] = -1;
		listeAVisiter[i] = -1;
		GL[i] = -1;
	}

	/* On va effectuer la recherche depuis chaque point du graph (sauf si elle est déjà dans un level) */
	for(i=0; i<nbPlaces; i++)
	{
		printf("================================\n Recherche à partir du noeud %2d \n================================\n", i);
		if(GL[i] == -1)
		{
			/* Remplissage de la liste positive */
			fifo_add(listeAVisiter, nbPlaces, i);
			while(fifo_isEmpty(listeAVisiter, nbPlaces) == 0)
			{
				buf = fifo_pop(listeAVisiter, nbPlaces);
				fifo_add(listePositif, nbPlaces, buf);
				for(j=0;j<nbPlaces;j++)
					if(A[buf][j] == 1)
					{
						if(fifo_search(listePositif, nbPlaces, j) == -1)
						{
							fifo_add(listeAVisiter, nbPlaces, j);
						}
					}
			}
			printf("--> Elements dans la liste positive :\n    ");
			for(j=0;j<nbPlaces;j++)
				printf("%2d ", listePositif[j]);
			printf("\n");

			/* Remplissage de la liste negative */
			fifo_add(listeAVisiter, nbPlaces, i);
			while(fifo_isEmpty(listeAVisiter, nbPlaces) == 0)
			{
				buf = fifo_pop(listeAVisiter, nbPlaces);
				fifo_add(listeNegatif, nbPlaces, buf);
				for(j=0;j<nbPlaces;j++)
					if(A[j][buf] == 1 && fifo_search(listeNegatif, nbPlaces, j) == -1)
						fifo_add(listeAVisiter, nbPlaces, j);

			}
			printf("--> Elements dans la liste negative :\n    ");
			for(j=0;j<nbPlaces;j++)
				printf("%2d ", listeNegatif[j]);
			printf("\n");

			/* Recherche d'elements presents dans les deux listes */
			printf("--> Intersection :\n    ");
			for(j=0;j<nbPlaces;j++)
			{
				for(k=0;k<nbPlaces;k++)
					if(listePositif[j] == listeNegatif[k] && listePositif[j] != -1)
					{
						GL[listePositif[j]] = nbLevels;
						printf("%2d ", listePositif[j]);
					}
			}
			printf("\n--> Enregistrement du niveau : %2d\n\n", nbLevels);
			nbLevels++;

			/* Vidage des deux listes */
			for(j=0;j<nbPlaces;j++)
			{
				listePositif[j] = -1;
				listeNegatif[j] = -1;
			}
		}
		else
			printf("--> Déjà dans le niveau : %2d\n\n", GL[i]);
	}

	printf("\n\n========\n Levels\n========\n");
	for(i=0;i<nbPlaces;i++)
		printf("--> Place %2d est dans le Level %2d\n", i, GL[i]);


	/*
	 * Création de la matrice réduite
	 */

	/* Allocation et initialisation de la matrice réduite */
	N = (int**) malloc(sizeof(int*)*nbLevels);
	for(i=0; i<nbLevels; i++)
	{
		N[i] = (int*) malloc(sizeof(int)*nbLevels);
		for(j=0; j<nbLevels; j++)
			N[i][j] = 0;
	}

	/* Remplissage de la-dite matrice */
	for(i=0; i<nbPlaces; i++)
	{
		for(j=0; j<nbPlaces; j++)
		{
			if(A[i][j] == 1)
			{
				N[GL[i]][GL[j]]++;
			}
		}
	}

	printf("\n\n=================\n Matrice reduite\n=================\n");
	for(i=0; i<nbLevels; i++)
	{
		for(j=0; j<nbLevels; j++)
			printf("%2d ", N[i][j]);
		printf("\n");
	}


	/*
	 * Calcul du plus long chemin
	 */

	/* TODO */


	/*
	 * Generation du graph
	 */

	/* Création du fichier */
	fichier = fopen("graph.dot","w");
	fprintf(fichier, "digraph graphAG44\n{\n");

        /* Enregistrement des Niveaux */
	srand(time(NULL));
	for(i=0; i<nbLevels; i++)
	{
		fprintf(fichier, "\tsubgraph sub%d\n\t{\n\t\tnode [style=filled,color=\"#%2x%2x%2x\"];\n", i,rand()%225, rand()%225, rand()%225);
		for(j=0; j<nbPlaces; j++)
			if(GL[j] == i)
				fprintf(fichier, "\t\t%d;\n", j);
		fprintf(fichier, "\t}\n\n");
	}

	/* Enregistrement des Places */
	for(i=0; i<nbPlaces; i++)
		for(j=0; j<nbPlaces; j++)
			if(A[i][j] == 1)
				fprintf(fichier, "\t%d -> %d;\n", i, j);
	fprintf(fichier, "}");
	fclose(fichier);
	
	/* Appel au programme dot pour generer l'image a partir du fichier */
	if(fork() == 0)
		execlp("dot", "dot", "-Tpng", "graph.dot", "-o", "graph.png", NULL);


	/*
	 * Libération de la mémoire allouée
	 */

	for(i=0; i<nbPlaces; i++)
		free(A[i]);
	free(A);
	for(i=0; i<nbLevels; i++)
		free(N[i]);
	free(N);
	free(GL);
        free(listePositif);
        free(listeNegatif);
        free(listeAVisiter);

	return 0;
}
