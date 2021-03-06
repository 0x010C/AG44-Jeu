/*
	PROGRAMME :
		AG44LevelGraph
	DEVELOPPEUR :
		Antoine LAMIELLE <antoine.lamielle@utbm.fr>
	PROJECT START DATE :
		September, 19th 2014
	LICENCE :
		AG44LevelGraph is a project realised as a part of the AG44-lessons at the UTBM school, dealing with Graph theory.
		Copyright © 2014 Antoine LAMIELLE

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program; if not, write to the Free Software Foundation, Inc.,
		51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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

int fifo_count(int *actuel, int nbValeurs)
{
	int i;
	for(i=0;i<nbValeurs;i++)
		if(actuel[i] == -1)
			return i;
	return nbValeurs;
}

int main(int argc, char **argv)
{
	int **A = NULL;
	int **N = NULL;
	int **PLC = NULL;
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
		printf("=====================================\n Research starting from the place %2d \n=====================================\n", i);
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
			printf("--> Places in the '+' list :\n    ");
			for(j=0;j<fifo_count(listePositif, nbPlaces);j++)
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
			printf("--> Places in the '-' list :\n    ");
                        for(j=0;j<fifo_count(listeNegatif, nbPlaces);j++)
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
			printf("\n--> Saving them in the new level n°%2d\n\n", nbLevels);
			nbLevels++;

			/* Vidage des deux listes */
			for(j=0;j<nbPlaces;j++)
			{
				listePositif[j] = -1;
				listeNegatif[j] = -1;
			}
		}
		else
			printf("--> Already in the level n°%2d\n\n", GL[i]);
	}

	printf("\n\n\n========\n Levels\n========\n");
	for(i=0;i<nbPlaces;i++)
		printf("--> Place %2d is in the Level n°%2d\n", i, GL[i]);

	printf("\n");
	for(i=0;i<nbLevels;i++)
	{
		k = 0;
		printf("{");
		for(j=0; j<nbPlaces; j++)
			if(GL[j] == i)
			{
				if(k == 1)
					printf(";");
				else
					k = 1;
				printf("%2d", j);
			}
		printf(" } ");
	}
	printf("\n");


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

	printf("\n================\n Reduced Matrix\n================\n");
	for(i=0; i<nbLevels; i++)
	{
		for(j=0; j<nbLevels; j++)
			printf("%2d ", N[i][j]);
		printf("\n");
	}


	/*
	 * Calcul du plus long chemin entre le premier et le dernier niveau
	 */

	/* Allocation et Reallocation d'une partie de mémoire précédement utilisé et mise à zéro */
	listeAVisiter = (int*) realloc(listeAVisiter, sizeof(int)*nbLevels);
	PLC = (int**) malloc(sizeof(int*)*nbLevels);
	for(i=0; i<nbLevels; i++)
	{
		listeAVisiter[i] = -1;
		PLC[i] = (int*) malloc(sizeof(int)*nbLevels);
		for(j=0; j<nbLevels; j++)
			PLC[i][j] = -1;
	}	

	/* Application de l'algo de calcul du plus long chemin */
	fifo_add(listeAVisiter, nbLevels, GL[0]);
	while(fifo_isEmpty(listeAVisiter, nbLevels) == 0)
	{
		buf = fifo_pop(listeAVisiter, nbLevels);
		for(i=0; i<nbLevels; i++)
			if(N[buf][i] >= 1 && buf != i)
			{
				if(fifo_count(PLC[buf], nbLevels)+1 > fifo_count(PLC[i], nbLevels))
				{
					for(j=0; j<nbLevels; j++)
						PLC[i][j] = PLC[buf][j];
					fifo_add(PLC[i], nbLevels, buf);
					fifo_add(listeAVisiter, nbLevels, i);
				}
			}

	}

	printf("\n==============\n Longest path\n==============\n");
	if(fifo_isEmpty(PLC[GL[nbPlaces-1]], nbLevels) == 0)
		k = GL[nbPlaces-1];
	else
	{
		k = 0;
		for(i=1; i<nbLevels; i++)
			if(fifo_count(PLC[i], nbLevels) > fifo_count(PLC[k], nbLevels))
				k = i;
	}
	printf("--> { ");
	for(i=0; i<nbLevels && PLC[k][i] != -1; i++)
	{
		printf("%d ; ", PLC[k][i]);
	}
	printf("%d }\n", k);


	/*
	 * Generation d'une image du graph (idée par Jérome BOURSIER, merci à lui !)
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
        if(fork() == 0)
                execlp("eog", "eog", "graph.png", NULL);


	/*
	 * Libération de la mémoire allouée
	 */

	for(i=0; i<nbPlaces; i++)
		free(A[i]);
	free(A);
	for(i=0; i<nbLevels; i++)
	{
		free(N[i]);
		free(PLC[i]);
	}
	free(N);
	free(PLC);
	free(GL);
	free(listePositif);
	free(listeNegatif);
	free(listeAVisiter);

	return 0;
}
