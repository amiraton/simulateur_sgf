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
 * @brief Défragmente un fichier en supprimant les enregistrements marqués comme supprimés logiquement.
 * 
 * @param nomFichier Nom du fichier à défragmenter.
 * 
 * @details Tous les enregistrements valides sont transférés dans un fichier temporaire, 
 *          puis le fichier original est remplacé.
 */
void deFragmenterFichier(char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fichier == NULL || temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier pour la défragmentation");
        return;
    }

    Enregistrement enregistrement;
    int recordsMoved = 0;

    while (fread(&enregistrement, sizeof(Enregistrement), 1, fichier) == 1) {
        if (enregistrement.id != -1) { // Ignorer les enregistrements supprimés
            fwrite(&enregistrement, sizeof(Enregistrement), 1, temp);
            recordsMoved++;
        }
    }

    fclose(fichier);
    fclose(temp);

    if (recordsMoved > 0) {
        remove(nomFichier);
        rename("temp.dat", nomFichier);
        printf("Défragmentation terminée : %d enregistrements restants dans le fichier '%s'.\n", recordsMoved, nomFichier);
    } else {
        printf("Aucun enregistrement valide trouvé pour la défragmentation dans '%s'.\n", nomFichier);
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
void renommerFichier(char *ancienNom, char *nouveauNom) {
    if (rename(ancienNom, nouveauNom) == 0) {
        printf("Fichier '%s' renommé en '%s' avec succès.\n", ancienNom, nouveauNom);
    } else {
        perror("Erreur lors du renommage du fichier");
    }
}
