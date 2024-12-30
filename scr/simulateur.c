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


