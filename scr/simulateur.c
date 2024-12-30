/*
Projet: Simulateur Simplifié d’un Système de Gestion de Fichiers (SGF)

Intégration finale des contributions des étudiants :

Lyna : Conception et Structure de la Mémoire Secondaire (MS)
Hadjer : Gestion des Fichiers de Données
Amira : Opérations sur les Fichiers
Anfel : Gestion des Métadonnées
Sirine : Menu Principal et Interface Utilisateur
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* ---------------------------- Lyna : Conception et Structure de la Mémoire Secondaire ---------------------------- */
#define FB 3 // Facteur de blocage
#define MAX_BLOCKS 1000
#define MAX_FILES 50

// Structure représentant un bloc de la mémoire secondaire
typedef struct {
    int data[FB];    // Données du bloc
    int ne;          // Nombre d'enregistrements dans le bloc
    int nextBlock;   // Adresse du prochain bloc (mode chaîné)
    bool libre;      // Indique si le bloc est libre ou occupé
    char fichier[50]; // Nom du fichier occupant ce bloc
} Bloc;

Bloc memoireSecondaire[MAX_BLOCKS];
int allocationTable[MAX_BLOCKS]; // Table d'allocation pour les blocs chaînés
int nombreBlocs; // Nombre total de blocs dans la mémoire secondaire

// Initialisation de la mémoire secondaire
void initialiserMemoireSecondaire(int nbBlocs) {
    nombreBlocs = nbBlocs;
    int i;
    for (i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        strcpy(memoireSecondaire[i].fichier, "");
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
    }
    printf("Secondary memory initialized with %d blocks.\n", nombreBlocs);
}

// Fonction pour afficher les blocs en mode contigu
void afficherBlocsContigus() {
	int i;
	int j;
    printf("Affichage des blocs (mode contigu) :\n");
    for ( i = 0; i < nombreBlocs; i++) {
        if (!memoireSecondaire[i].libre) {
            printf("Bloc %d (Fichier : %s, Enregistrements : %d) : ",
                   i + 1, memoireSecondaire[i].fichier, memoireSecondaire[i].ne);
            for (j = 0; j < memoireSecondaire[i].ne; j++) {
                printf("%d ", memoireSecondaire[i].data[j]);
            }
            printf("\n");
        }
    }
}

// Fonction pour afficher les blocs en mode cha�n�
void afficherBlocsChaines() {
	int i;
	int j;
    printf("Affichage des blocs (mode cha�n�) :\n");
    for (i = 0; i < nombreBlocs; i++) {
        if (!memoireSecondaire[i].libre) {
            printf("Bloc %d (Fichier : %s) : ", i + 1, memoireSecondaire[i].fichier);
            for ( j = 0; j < memoireSecondaire[i].ne; j++) {
                printf("%d ", memoireSecondaire[i].data[j]);
            }
            printf(" -> Bloc suivant : %d\n", memoireSecondaire[i].nextBlock + 1);
        }
    }
}

// Fonction pour g�n�rer la table d'allocation
void genererTableAllocation() {
	int i;
    for ( i = 0; i < nombreBlocs - 1; i++) {
        allocationTable[i] = i + 1; // Chaque bloc pointe vers le suivant
    }
    allocationTable[nombreBlocs - 1] = -1; // Dernier bloc marque la fin
    printf("Table d'allocation g�n�r�e.\n");
}

// Compactage de la mémoire secondaire
void compactageMemoireSecondaire() {
    int writeIndex = 0;
    int readIndex;
    for (readIndex = 0; readIndex < nombreBlocs; readIndex++) {
        if (!memoireSecondaire[readIndex].libre) {
            memoireSecondaire[writeIndex] = memoireSecondaire[readIndex];
            writeIndex++;
        }
    }
    int i;

    for (i = writeIndex; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        strcpy(memoireSecondaire[i].fichier, "");
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
    }

    printf("Secondary memory compacted. All free blocks are moved to the end.\n");
}

// Vider la mémoire secondaire
void viderMemoireSecondaire() {
    initialiserMemoireSecondaire(nombreBlocs);
    printf("Secondary memory cleared. All data has been erased.\n");
}

// fonction defragmentation apres suppression 
void deFragmenterFichier(char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fichier == NULL || temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier pour la défragmentation");
        return;
    }

    Enregistrement enregistrement;
    int recordsMoved = 0;

    // Parcourir tous les enregistrements pour ignorer ceux marqués comme supprimés
    while (fread(&enregistrement, sizeof(Enregistrement), 1, fichier) == 1) {
        if (enregistrement.id != -1) { // Ignorer les enregistrements supprimés logiquement
            fwrite(&enregistrement, sizeof(Enregistrement), 1, temp);
            recordsMoved++;
        }
    }

    fclose(fichier);
    fclose(temp);

    // Remplacer l'ancien fichier par le fichier défragmenté
    remove(nomFichier);
    rename("temp.dat", nomFichier);

    printf("Defragmentation completed. %d records remain in file '%s'.\n", recordsMoved, nomFichier);
}

/* ---------------------------- Amira : Opérations sur les Fichiers ---------------------------- */

/**
 * @brief Recherche un enregistrement par ID dans un fichier donné.
 * 
 * @param nomFichier Nom du fichier contenant les enregistrements.
 * @param id ID de l'enregistrement à rechercher.
 * 
 * @details Cette fonction parcourt le fichier à la recherche d'un enregistrement 
 *          correspondant à l'ID donné. Si trouvé, il affiche ses détails, sinon, 
 *          informe l'utilisateur que l'enregistrement n'existe pas.
 */
void rechercherEnregistrement(char *nomFichier, int id) {
    FILE *fichier = fopen(nomFichier, "rb");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    Enregistrement enregistrement;
    int position = 0;
    bool found = false;

    while (fread(&enregistrement, sizeof(Enregistrement), 1, fichier) == 1) {
        if (enregistrement.id == id) {
            printf("Enregistrement trouvé : ID: %d, Nom: %s, Adresse: %s, Position: %d\n",
                   id, enregistrement.nom, enregistrement.adresse, position);
            found = true;
            break;
        }
        position++;
    }

    if (!found) {
        printf("Aucun enregistrement avec l'ID %d trouvé dans le fichier '%s'.\n", id, nomFichier);
    }

    fclose(fichier);
}

/**
 * @brief Supprime logiquement un enregistrement en le marquant comme supprimé.
 * 
 * @param nomFichier Nom du fichier contenant les enregistrements.
 * @param id ID de l'enregistrement à supprimer logiquement.
 * 
 * @details L'enregistrement est marqué avec un ID de -1 pour indiquer qu'il est supprimé.
 */
void supprimerEnregistrementLogique(char *nomFichier, int id) {
    FILE *fichier = fopen(nomFichier, "r+b");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    Enregistrement enregistrement;
    bool found = false;

    while (fread(&enregistrement, sizeof(Enregistrement), 1, fichier) == 1) {
        if (enregistrement.id == id) {
            enregistrement.id = -1; // Marquer comme supprimé
            fseek(fichier, -sizeof(Enregistrement), SEEK_CUR);
            fwrite(&enregistrement, sizeof(Enregistrement), 1, fichier);
            printf("Enregistrement avec l'ID %d supprimé logiquement.\n", id);
            found = true;
            break;
        }
    }

    if (!found) {
        printf("Aucun enregistrement avec l'ID %d trouvé pour suppression logique.\n", id);
    }

    fclose(fichier);
}

/**
 * @brief Supprime physiquement un enregistrement en recréant le fichier sans cet enregistrement.
 * 
 * @param nomFichier Nom du fichier contenant les enregistrements.
 * @param id ID de l'enregistrement à supprimer physiquement.
 * 
 * @details Les enregistrements non supprimés sont transférés dans un fichier temporaire, 
 *          puis le fichier original est remplacé.
 */
void supprimerEnregistrementPhysique(char *nomFichier, int id) {
    FILE *fichier = fopen(nomFichier, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fichier == NULL || temp == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    Enregistrement enregistrement;
    bool recordFound = false;

    while (fread(&enregistrement, sizeof(Enregistrement), 1, fichier) == 1) {
        if (enregistrement.id == id) {
            recordFound = true; // Ignorer cet enregistrement
        } else {
            fwrite(&enregistrement, sizeof(Enregistrement), 1, temp);
        }
    }

    fclose(fichier);
    fclose(temp);

    if (recordFound) {
        remove(nomFichier);
        rename("temp.dat", nomFichier);
        printf("Enregistrement avec l'ID %d supprimé physiquement.\n", id);
    } else {
        printf("Aucun enregistrement avec l'ID %d trouvé pour suppression physique.\n", id);
        remove("temp.dat");
    }
}



/**
 * @brief Renomme un fichier.
 * 
 * @param ancienNom Nom actuel du fichier.
 * @param nouveauNom Nouveau nom pour le fichier.
 * 
 * @details Vérifie si le fichier peut être renommé et met à jour les métadonnées.
 */
// Fonction pour renommer un fichier
void renommerFichier(char *ancienNom, char *nouveauNom) {
    // VÃ©rifier si le fichier existe avant de renommer
    if (rename(ancienNom, nouveauNom) == 0) {
        printf("File '%s' successfully renamed to '%s'.\n", ancienNom, nouveauNom);
    } else {
        perror("Erreur lors du renommage du fichier");
    }
}
typedef struct {
    char nom_fichier[50];          // Nom du fichier
    int taille_blocs;              // Nombre de blocs utilisÃ©s
    int taille_enregistrements;    // Nombre total d'enregistrements
    int adresse_premier_bloc;      // Adresse du premier bloc
    char mode_organisation[20];    // Mode d'organisation des donnÃ©es
    char mode_interne[10];         // Mode interne (triÃ© ou non)
} Metadonnees;

Metadonnees fichiersMeta[MAX_FILES]; // Tableau des mÃ©tadonnÃ©es
int nombreFichiersMeta = 0;          // Nombre de fichiers enregistrÃ©s dans les mÃ©tadonnÃ©es

// Ajouter des mÃ©tadonnÃ©es pour un nouveau fichier
void ajouterMetadonnees(char *nom, int taille_blocs, int taille_enreg, int adresse_bloc, char *mode_globale, char *mode_interne) {
    if (nombreFichiersMeta >= MAX_FILES) {
        fprintf(stderr, "Error: Metadata limit reached. Cannot add metadata for file '%s'.\n", nom);
        return;
    }

    Metadonnees *meta = &fichiersMeta[nombreFichiersMeta++];
    strcpy(meta->nom_fichier, nom);
    meta->taille_blocs = taille_blocs;
    meta->taille_enregistrements = taille_enreg;
    meta->adresse_premier_bloc = adresse_bloc;
    strcpy(meta->mode_organisation, mode_globale);
    strcpy(meta->mode_interne, mode_interne);

    printf("Metadata added for file '%s'.\n", nom);
}

// Afficher toutes les mÃ©tadonnÃ©es
void afficherMetadonnees() {
    if (nombreFichiersMeta == 0) {
        printf("No metadata available.\n");
        return;
    }
    int i;
    for ( i = 0; i < nombreFichiersMeta; i++) {
        printf("File: %s\nBlocks: %d\nRecords: %d\nFirst Block Address: %d\nGlobal Mode: %s\nInternal Mode: %s\n---\n",
               fichiersMeta[i].nom_fichier,
               fichiersMeta[i].taille_blocs,
               fichiersMeta[i].taille_enregistrements,
               fichiersMeta[i].adresse_premier_bloc,
               fichiersMeta[i].mode_organisation,
               fichiersMeta[i].mode_interne);
    }
}

// Mettre Ã  jour les mÃ©tadonnÃ©es lors du renommage d'un fichier
void miseAJourMetadonneesRenommage(char *ancienNom, char *nouveauNom) {
	int i;
    for (i = 0; i < nombreFichiersMeta; i++) {
        if (strcmp(fichiersMeta[i].nom_fichier, ancienNom) == 0) {
            strcpy(fichiersMeta[i].nom_fichier, nouveauNom);
            printf("Metadata updated: file '%s' renamed to '%s'.\n", ancienNom, nouveauNom);
            return;
        }
    }
    fprintf(stderr, "Error: No metadata found for file '%s'. Cannot update.\n", ancienNom);
}
Fonction pour effectuer une suppression logique d'un fichier
void MiseSuppressionLogique(char *nomFichier) {
 int i ;
  // Parcours des métadonnées pour trouver le fichier à supprimer
    for ( i = 0; i < nombreFichiersMeta; i++) {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0) {
            strcpy(fichiersMeta[i].nom_fichier, "SUPPRIME"); // Marquer comme supprimÃ©
            printf("Logical deletion: file '%s' marked as deleted.\n", nomFichier);
            return;
        }
    }
    // Si le fichier n'est pas trouvé dans les métadonnées
    fprintf(stderr, "Error: No metadata found for file '%s'. Cannot perform logical deletion.\n", nomFichier);
}
// Fonction pour effectuer une suppression physique d'un fichier
void MisSuppressionPhysique(char *nomFichier) {
 int i ; int j ;
 // Parcours des métadonnées pour trouver le fichier à supprimer
    for ( i = 0; i < nombreFichiersMeta; i++) {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0) {
            // DÃ©calage pour Ã©craser l'entrÃ©e supprimÃ©e
            for ( j = i; j < nombreFichiersMeta - 1; j++) {
                fichiersMeta[j] = fichiersMeta[j + 1];
            }
            nombreFichiersMeta--;
            printf("Physical deletion: Metadata for file '%s' has been removed.\n", nomFichier);
            return;
        }
    }
    fprintf(stderr, "Error: No metadata found for file '%s'. Cannot perform physical deletion.\n", nomFichier);
}
// Fonction pour mettre à jour les métadonnées après un compactage des fichiers
void miseAJourMetadonneesApresCompactage() {
    int writeIndex = 0;
    // Mise à jour des métadonnées après compactage
    int i ; int j ;
    for ( i = 0; i < nombreFichiersMeta; i++) {
        // Vérification du premier bloc utilisé par le fichier
        if (fichiersMeta[i].adresse_premier_bloc >= writeIndex) {
            // Si l'adresse du premier bloc a changé, il faut la mettre à jour
            fichiersMeta[i].adresse_premier_bloc = -1; // On va devoir recalculer l'adresse du premier bloc
            for ( j = 0; j < nombreBlocs; j++) {
                if (!memoireSecondaire[j].libre && strcmp(memoireSecondaire[j].fichier, fichiersMeta[i].nom_fichier) == 0) {
                    fichiersMeta[i].adresse_premier_bloc = j; // Recalcul de l'adresse
                    break;
                }
            }
            printf("Metadata updated for file '%s': First block address is now %d.\n", fichiersMeta[i].nom_fichier, fichiersMeta[i].adresse_premier_bloc);
        }
    }
}
//Mis a jour metadonnee apres insertion 
void miseAJourMetadonneesApresInsertion(char *nomFichier, int blocsUtilises, int nouveauxEnregistrements, char *modeOrganisation) {
    int i;
    for (i = 0; i < nombreFichiersMeta; i++) {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0) {
            fichiersMeta[i].taille_enregistrements += nouveauxEnregistrements;

            if (strcmp(modeOrganisation, "chaînée") == 0) {
                fichiersMeta[i].taille_blocs += blocsUtilises; // Mise à jour uniquement pour mode chaîné
            }

            printf("Mise à jour des métadonnées : fichier '%s', +%d blocs, +%d enregistrements.\n",
                   nomFichier, blocsUtilises, nouveauxEnregistrements);
            return;
        }
    }
    fprintf(stderr, "Erreur : Métadonnées introuvables pour le fichier '%s'.\n", nomFichier);
}
// Fonction pour effectuer la d�fragmentation des m�tadonn�es en r�organisant les fichiers
void MisDefragmentation() {
    int indexLibre = 0; int i ;
    // Parcours des m�tadonn�es pour r�organiser les fichiers non supprim�s
    for ( i = 0; i < nombreFichiersMeta; i++) {
        if (strcmp(fichiersMeta[i].nom_fichier, "SUPPRIME") != 0) {
            // Copier les données valides à l'index libre
            if (i != indexLibre) {
                fichiersMeta[indexLibre] = fichiersMeta[i];
            }
            indexLibre++;
        }
    }
    // Mise � jour du nombre r�el de fichiers apr�s la d�fragmentation
    nombreFichiersMeta = indexLibre; // Met à jour le nombre réel de fichiers
    printf("Defragmentation completed. %d file(s) retained.\n", nombreFichiersMeta);
} 


