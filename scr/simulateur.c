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
#include <string.h>
#include <stdbool.h>


/* ---------------------------- Lyna : Conception et Structure de la Mémoire Secondaire ---------------------------- */
#define FB 3 // Facteur de blocage
#define MAX_BLOCKS 1000
#define MAX_FILES 50
#define BLOC_LIBRE -1 // Indique qu'un bloc est libre
#define MAX_NOM 256
#define MAX_MODE 20

// Structure représentant un bloc de la mémoire secondaire
typedef struct {
    int data[FB];    // Données du bloc
    int ne;          // Nombre d'enregistrements dans le bloc
    int nextBlock;   // Adresse du prochain bloc (mode chaîné)
    bool libre;      // Indique si le bloc est libre ou occupé
    char fichier[50]; // Nom du fichier occupant ce bloc
} Bloc;

typedef struct {
    char nom[MAX_NOM];
    int taille_blocs;
    char organisation_globale[MAX_MODE];
    char organisation_interne[MAX_MODE];
    int premier_bloc;
} Metadata;

typedef struct {
    int id;
    char data[100];
} Enregistrement;

typedef struct {
    int indexBloc;
    bool libre;
    char fichier[50]; // Nom du fichier occupant le bloc, vide si le bloc est libre
} Allocation;

Allocation tableAllocation[MAX_BLOCKS];

Bloc memoireSecondaire[MAX_BLOCKS];
int nombreBlocs; // Nombre total de blocs dans la mémoire secondaire
int nombreFichiersMeta = 0; // Nombre de fichiers
Metadata fichiers[MAX_FILES]; // Tableau des fichiers



// Initialisation de la mémoire secondaire
// Initialisation de la mémoire secondaire
void initialiserMemoireSecondaire(int nbBlocs) {
    if (nbBlocs > MAX_BLOCKS) {
        printf("Erreur : Nombre de blocs supérieur à la capacité maximale (%d).\n", MAX_BLOCKS);
        return;
    }

    nombreBlocs = nbBlocs;
    int i ;
    for ( i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = BLOC_LIBRE;
        memoireSecondaire[i].libre = true;
        strcpy(memoireSecondaire[i].fichier, "");
        // Initialiser la table d'allocation
        tableAllocation[i].indexBloc = i;
        tableAllocation[i].libre = true;
        strcpy(tableAllocation[i].fichier, "");
    }
    printf("Mémoire secondaire initialisée avec %d blocs.\n", nombreBlocs);
}


// fonction pour la mise a jour de la table d'allocation

void mettreAJourTableAllocation() {
    for (int i = 0; i < nombreBlocs; i++) {
        tableAllocation[i].libre = memoireSecondaire[i].libre;
        strcpy(tableAllocation[i].fichier, memoireSecondaire[i].fichier);
    }
}

// Compactage de la mémoire secondaire
void compactageMemoire() {
    int i, j;
    int prochainBlocLibre = 0;  // Premier bloc libre disponible pour le compactage
    
    // Réinitialiser les blocs libres
    for (i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        strcpy(memoireSecondaire[i].fichier, "");
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
    }

    // Recharger les fichiers en mémoire après le compactage
    for (i = 0; i < nombreFichiersMeta; i++) {
        Metadata *fichier = &fichiers[i];
        int premierBlocCompacte = -1, precedentBloc = -1;
        int blocsAlloues = 0;

        // Allouer les blocs de manière contiguë
        for (j = 0; j < fichier->taille_blocs; j++) {
            if (prochainBlocLibre < nombreBlocs) {
                // Allouer un bloc pour le fichier
                memoireSecondaire[prochainBlocLibre].libre = false;
                strcpy(memoireSecondaire[prochainBlocLibre].fichier, fichier->nom);
                memoireSecondaire[prochainBlocLibre].ne = 0;

                if (premierBlocCompacte == -1) {
                    premierBlocCompacte = prochainBlocLibre;  // Définir le premier bloc
                }
                if (precedentBloc != -1) {
                    memoireSecondaire[precedentBloc].nextBlock = prochainBlocLibre;
                }

                precedentBloc = prochainBlocLibre;
                blocsAlloues++;
                prochainBlocLibre++;
            } else {
                printf("Espace insuffisant pour effectuer le compactage.\n");
                return;
            }
        }

        // Mettre à jour les métadonnées du fichier
        fichier->premier_bloc = premierBlocCompacte;
        mettreAJourMetadonnees(fichier);
    }
        
        mettreAJourTableAllocation() ;
    printf("Le compactage de la mémoire secondaire a été effectué avec succès.\n");
}

// Vider la mémoire secondaire
void viderMemoire() {
    printf("=== Vider la MÃ©moire Secondaire ===\n");

    // RÃ©initialiser tous les blocs de mÃ©moire secondaire
    int i ;
    for (i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
        strcpy(memoireSecondaire[i].fichier, "");
    }
      mettreAJourTableAllocation();
    // RÃ©initialiser toutes les mÃ©tadonnÃ©es
    nombreFichiersMeta = 0;
    printf("MÃ©moire secondaire vidÃ©e avec succÃ¨s. Tous les fichiers et blocs ont Ã©tÃ© rÃ©initialisÃ©s.\n");
}

// fonction defragmentation apres suppression
void defragmenterFichier() {
    printf("=== DÃ©fragmentation ===\n");

    // Afficher la liste des fichiers
    printf("SÃ©lectionnez un fichier Ã  partir de la liste suivante:\n");
    int i ;
    for ( i = 0; i < nombreFichiersMeta; i++) {
        printf("%d. %s\n", i + 1, fichiers[i].nom);
    }

    int fichierIndex;
    printf("Votre choix : ");
    scanf("%d", &fichierIndex);

    if (fichierIndex < 1 || fichierIndex > nombreFichiersMeta) {
        printf("Choix invalide.\n");
        return;
    }

    Metadata *fichier = &fichiers[fichierIndex - 1];

    int bloc_courant = fichier->premier_bloc;
    int temp[FB * MAX_BLOCKS];
    int pos = 0;

    // Collecter tous les enregistrements valides
    while (bloc_courant != -1) {
    	int j ;
        for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
            if (memoireSecondaire[bloc_courant].data[j] != -1) {
                temp[pos++] = memoireSecondaire[bloc_courant].data[j];
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    // RÃ©Ã©crire les enregistrements
    bloc_courant = fichier->premier_bloc;
    pos = 0;
    while (bloc_courant != -1) {
        memoireSecondaire[bloc_courant].ne = 0;
        int j ;
        for ( j = 0; j < FB && pos < fichier->taille_blocs * FB; j++) {
            memoireSecondaire[bloc_courant].data[j] = temp[pos++];
            memoireSecondaire[bloc_courant].ne++;
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    printf("DÃ©fragmentation terminÃ©e pour '%s'.\n", fichier->nom);
    mettreAJourTableAllocation();
    mettreAJourMetadonnees(fichier); // Mise Ã  jour des mÃ©tadonnÃ©es
}

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------- Amira : Opérations sur les Fichiers ---------------------------- */

/// Fonction : rechercherEnregistrementParID
// Description : Recherche un enregistrement par son ID dans un fichier sélectionné.
// Exemple d'utilisation :
// Supposons que le fichier sélectionné "fichier1" contienne les blocs suivants :
// Bloc 0 : [101, 102, 103]
// Bloc 1 : [201, 202, 203]
// Recherche ID : 202
// Résultat attendu : "Enregistrement trouvé : Bloc 1, Position 1"

void rechercherEnregistrementParID()
{
    printf("=== Recherche d'Enregistrement par ID ===\n");

    // Affiche une liste des fichiers disponibles pour permettre à l'utilisateur d'en choisir un.
    printf("Sélectionnez un fichier à partir de la liste suivante pour effectuer la recherche :\n");
    int i; // Déclaration de la variable pour la boucle.
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        // Affiche chaque fichier avec son index (commençant à 1 pour plus de clarté).
        printf("%d. %s\n", i + 1, fichiers[i].nom);
    }

    // Demande à l'utilisateur de choisir un fichier dans la liste.
    int fichierIndex;
    printf("Votre choix : ");
    scanf("%d", &fichierIndex);

    // Vérifie si le choix est valide (index dans les limites de la liste).
    if (fichierIndex < 1 || fichierIndex > nombreFichiersMeta)
    {
        printf("Choix invalide.\n");
        return; // Termine la fonction si le choix est invalide.
    }

    // Récupère les métadonnées du fichier sélectionné.
    Metadata *fichier = &fichiers[fichierIndex - 1];

    // Demande à l'utilisateur d'entrer l'ID à rechercher.
    int id;
    printf("Entrez l'ID de l'enregistrement à rechercher : ");
    scanf("%d", &id);

    // Vérifie le mode d'organisation globale pour déterminer comment effectuer la recherche.
    if (strcmp(fichier->organisation_globale, "contigue") == 0)
    {
        // Mode d'organisation contiguë.
        int i; // Variable pour parcourir les blocs.
        for (i = fichier->premier_bloc; i < fichier->premier_bloc + fichier->taille_blocs; i++)
        {
            if (strcmp(fichier->organisation_interne, "triee") == 0)
            {
                // Mode interne trié - utilise une recherche binaire.
                int gauche = 0, droite = memoireSecondaire[i].ne - 1;
                while (gauche <= droite)
                {
                    int milieu = (gauche + droite) / 2;
                    if (memoireSecondaire[i].data[milieu] == id)
                    {
                        // Enregistrement trouvé, affiche sa position.
                        printf("Enregistrement trouvé : Bloc %d, Position %d\n", i, milieu);
                        return;
                    }
                    else if (memoireSecondaire[i].data[milieu] < id)
                    {
                        gauche = milieu + 1; // Continue la recherche dans la moitié droite.
                    }
                    else
                    {
                        droite = milieu - 1; // Continue la recherche dans la moitié gauche.
                    }
                }
            }
            else
            {
                // Mode interne non trié - utilise une recherche linéaire.
                int j; // Variable pour parcourir les enregistrements.
                for (j = 0; j < memoireSecondaire[i].ne; j++)
                {
                    if (memoireSecondaire[i].data[j] == id)
                    {
                        // Enregistrement trouvé, affiche sa position.
                        printf("Enregistrement trouvé : Bloc %d, Position %d\n", i, j);
                        return;
                    }
                }
            }
        }
    }
    else if (strcmp(fichier->organisation_globale, "chainee") == 0)
    {
        // Mode d'organisation chaînée.
        int bloc_courant = fichier->premier_bloc; // Début de la chaîne.
        while (bloc_courant != -1)
        {
            if (strcmp(fichier->organisation_interne, "triee") == 0)
            {
                // Mode interne trié - utilise une recherche binaire.
                int gauche = 0, droite = memoireSecondaire[bloc_courant].ne - 1;
                while (gauche <= droite)
                {
                    int milieu = (gauche + droite) / 2;
                    if (memoireSecondaire[bloc_courant].data[milieu] == id)
                    {
                        // Enregistrement trouvé, affiche sa position.
                        printf("Enregistrement trouvé : Bloc %d, Position %d\n", bloc_courant, milieu);
                        return;
                    }
                    else if (memoireSecondaire[bloc_courant].data[milieu] < id)
                    {
                        gauche = milieu + 1; // Continue la recherche dans la moitié droite.
                    }
                    else
                    {
                        droite = milieu - 1; // Continue la recherche dans la moitié gauche.
                    }
                }
            }
            else
            {
                // Mode interne non trié - utilise une recherche linéaire.
                int j; // Variable pour parcourir les enregistrements.
                for (j = 0; j < memoireSecondaire[bloc_courant].ne; j++)
                {
                    if (memoireSecondaire[bloc_courant].data[j] == id)
                    {
                        // Enregistrement trouvé, affiche sa position.
                        printf("Enregistrement trouvé : Bloc %d, Position %d\n", bloc_courant, j);
                        return;
                    }
                }
            }
            // Passe au bloc suivant dans la chaîne.
            bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
        }
    }
    else
    {
        // Mode d'organisation inconnu.
        printf("Mode d'organisation globale inconnu : %s\n", fichier->organisation_globale);
        return;
    }

    // Si aucun enregistrement correspondant n'est trouvé.
    printf("Enregistrement avec ID %d non trouvé.\n", id);
}

// Fonction : supprimerEnregistrementLogique
// Description : Marque un enregistrement comme supprimé en définissant son ID à -1 (suppression logique).
// Exemple d'utilisation :
// Supposons que le fichier sélectionné "fichier1" contienne les blocs suivants :
// Bloc 0 : [101, 102, 103]
// Bloc 1 : [201, 202, 203]
// Suppression ID : 102
// Résultat attendu : Bloc 0 : [101, -1, 103]

void supprimerEnregistrementLogique()
{
    printf("=== Suppression Logique ===\n");

    // Affiche une liste des fichiers disponibles pour permettre à l'utilisateur d'en choisir un.
    printf("Sélectionnez un fichier à partir de la liste suivante:\n");
    int i; // Déclaration de la variable pour la boucle.
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        // Affiche chaque fichier avec son index (commençant à 1).
        printf("%d. %s\n", i + 1, fichiers[i].nom);
    }

    // Demande à l'utilisateur de choisir un fichier.
    int fichierIndex;
    printf("Votre choix : ");
    scanf("%d", &fichierIndex);

    // Vérifie si le choix est valide.
    if (fichierIndex < 1 || fichierIndex > nombreFichiersMeta)
    {
        printf("Choix invalide.\n");
        return;
    }

    // Récupère les métadonnées du fichier sélectionné.
    Metadata *fichier = &fichiers[fichierIndex - 1];

    // Affiche tous les enregistrements disponibles dans le fichier.
    printf("Enregistrements disponibles dans '%s':\n", fichier->nom);
    int bloc_courant = fichier->premier_bloc; // Commence à partir du premier bloc.
    while (bloc_courant != -1)
    {
        int j; // Parcourt les enregistrements dans le bloc courant.
        for (j = 0; j < memoireSecondaire[bloc_courant].ne; j++)
        {
            if (memoireSecondaire[bloc_courant].data[j] != -1)
            { // Ignore les enregistrements déjà supprimés.
                printf("Bloc %d, Position %d: ID = %d\n", bloc_courant, j, memoireSecondaire[bloc_courant].data[j]);
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock; // Passe au bloc suivant.
    }

    // Demande à l'utilisateur de saisir l'ID à supprimer.
    int id;
    printf("Entrez l'ID de l'enregistrement à supprimer : ");
    scanf("%d", &id);

    // Parcourt les blocs pour trouver et supprimer l'enregistrement.
    bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1)
    {
        int j;
        for (j = 0; j < memoireSecondaire[bloc_courant].ne; j++)
        {
            if (memoireSecondaire[bloc_courant].data[j] == id)
            {
                // Marque l'enregistrement comme supprimé.
                memoireSecondaire[bloc_courant].data[j] = -1;
                printf("Enregistrement %d supprimé logiquement du bloc %d\n", id, bloc_courant);
                mettreAJourTableAllocation();    // Met à jour la table d'allocation.
                mettreAJourMetadonnees(fichier); // Met à jour les métadonnées.
                return;
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock; // Passe au bloc suivant.
    }

    // Si l'ID n'a pas été trouvé.
    printf("Enregistrement non trouvé.\n");
}

// Fonction : supprimerEnregistrementPhysique
// Description : Supprime physiquement un enregistrement en le retirant complètement du fichier.
// Exemple d'utilisation :
// Supposons que le fichier sélectionné "fichier1" contienne les blocs suivants :
// Bloc 0 : [101, 102, 103]
// Bloc 1 : [201, 202, 203]
// Suppression ID : 103
// Résultat attendu : Bloc 0 : [101, 102]

void supprimerEnregistrementPhysique()
{
    printf("=== Suppression Physique ===\n");

    // Affiche une liste des fichiers disponibles pour permettre à l'utilisateur d'en choisir un.
    printf("Sélectionnez un fichier à partir de la liste suivante:\n");
    int i; // Déclaration de la variable pour la boucle.
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        // Affiche chaque fichier avec son index (commençant à 1).
        printf("%d. %s\n", i + 1, fichiers[i].nom);
    }

    // Demande à l'utilisateur de choisir un fichier.
    int fichierIndex;
    printf("Votre choix : ");
    scanf("%d", &fichierIndex);

    // Vérifie si le choix est valide.
    if (fichierIndex < 1 || fichierIndex > nombreFichiersMeta)
    {
        printf("Choix invalide.\n");
        return;
    }

    // Récupère les métadonnées du fichier sélectionné.
    Metadata *fichier = &fichiers[fichierIndex - 1];

    // Affiche tous les enregistrements disponibles dans le fichier.
    printf("Enregistrements disponibles dans '%s':\n", fichier->nom);
    int bloc_courant = fichier->premier_bloc; // Commence à partir du premier bloc.
    while (bloc_courant != -1)
    {
        int j; // Parcourt les enregistrements dans le bloc courant.
        for (j = 0; j < memoireSecondaire[bloc_courant].ne; j++)
        {
            if (memoireSecondaire[bloc_courant].data[j] != -1)
            { // Ignore les enregistrements déjà supprimés.
                printf("Bloc %d, Position %d: ID = %d\n", bloc_courant, j, memoireSecondaire[bloc_courant].data[j]);
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock; // Passe au bloc suivant.
    }

    // Demande à l'utilisateur de saisir l'ID à supprimer.
    int id;
    printf("Entrez l'ID de l'enregistrement à supprimer : ");
    scanf("%d", &id);

    // Parcourt les blocs pour trouver et supprimer l'enregistrement physiquement.
    bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1)
    {
        int j, k;
        for (j = 0; j < memoireSecondaire[bloc_courant].ne; j++)
        {
            if (memoireSecondaire[bloc_courant].data[j] == id)
            {
                // Décale les enregistrements pour combler l'espace laissé par la suppression.
                for (k = j; k < memoireSecondaire[bloc_courant].ne - 1; k++)
                {
                    memoireSecondaire[bloc_courant].data[k] = memoireSecondaire[bloc_courant].data[k + 1];
                }
                memoireSecondaire[bloc_courant].ne--; // Réduit le nombre d'enregistrements dans le bloc.
                printf("Enregistrement %d supprimé physiquement du bloc %d\n", id, bloc_courant);
                mettreAJourTableAllocation();    // Met à jour la table d'allocation.
                mettreAJourMetadonnees(fichier); // Met à jour les métadonnées.
                return;
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock; // Passe au bloc suivant.
    }

    // Si l'ID n'a pas été trouvé.
    printf("Enregistrement non trouvé.\n");
}

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

typedef struct
{
    char nom_fichier[50];       // Nom du fichier
    int taille_blocs;           // Nombre de blocs utilisÃ©s
    int taille_enregistrements; // Nombre total d'enregistrements
    int adresse_premier_bloc;   // Adresse du premier bloc
    char mode_organisation[20]; // Mode d'organisation des donnÃ©es
    char mode_interne[10];      // Mode interne (triÃ© ou non)
} Metadonnees;

Metadonnees fichiersMeta[MAX_FILES]; // Tableau des mÃ©tadonnÃ©es
int nombreFichiersMeta = 0;          // Nombre de fichiers enregistrÃ©s dans les mÃ©tadonnÃ©es

// Ajouter des mÃ©tadonnÃ©es pour un nouveau fichier
void ajouterMetadonnees(char *nom, int taille_blocs, int taille_enreg, int adresse_bloc, char *mode_globale, char *mode_interne)
{
    if (nombreFichiersMeta >= MAX_FILES)
    {
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
void afficherMetadonnees()
{
    if (nombreFichiersMeta == 0)
    {
        printf("No metadata available.\n");
        return;
    }
    int i;
    for (i = 0; i < nombreFichiersMeta; i++)
    {
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
void miseAJourMetadonneesRenommage(char *ancienNom, char *nouveauNom)
{
    int i;
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        if (strcmp(fichiersMeta[i].nom_fichier, ancienNom) == 0)
        {
            strcpy(fichiersMeta[i].nom_fichier, nouveauNom);
            printf("Metadata updated: file '%s' renamed to '%s'.\n", ancienNom, nouveauNom);
            return;
        }
    }
    fprintf(stderr, "Error: No metadata found for file '%s'. Cannot update.\n", ancienNom);
}
Fonction pour effectuer une suppression logique d'un fichier void MiseSuppressionLogique(char *nomFichier)
{
    int i;
    // Parcours des métadonnées pour trouver le fichier à supprimer
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0)
        {
            strcpy(fichiersMeta[i].nom_fichier, "SUPPRIME"); // Marquer comme supprimÃ©
            printf("Logical deletion: file '%s' marked as deleted.\n", nomFichier);
            return;
        }
    }
    // Si le fichier n'est pas trouvé dans les métadonnées
    fprintf(stderr, "Error: No metadata found for file '%s'. Cannot perform logical deletion.\n", nomFichier);
}
// Fonction pour effectuer une suppression physique d'un fichier
void MisSuppressionPhysique(char *nomFichier)
{
    int i;
    int j;
    // Parcours des métadonnées pour trouver le fichier à supprimer
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0)
        {
            // DÃ©calage pour Ã©craser l'entrÃ©e supprimÃ©e
            for (j = i; j < nombreFichiersMeta - 1; j++)
            {
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
void miseAJourMetadonneesApresCompactage()
{
    int writeIndex = 0;
    // Mise à jour des métadonnées après compactage
    int i;
    int j;
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        // Vérification du premier bloc utilisé par le fichier
        if (fichiersMeta[i].adresse_premier_bloc >= writeIndex)
        {
            // Si l'adresse du premier bloc a changé, il faut la mettre à jour
            fichiersMeta[i].adresse_premier_bloc = -1; // On va devoir recalculer l'adresse du premier bloc
            for (j = 0; j < nombreBlocs; j++)
            {
                if (!memoireSecondaire[j].libre && strcmp(memoireSecondaire[j].fichier, fichiersMeta[i].nom_fichier) == 0)
                {
                    fichiersMeta[i].adresse_premier_bloc = j; // Recalcul de l'adresse
                    break;
                }
            }
            printf("Metadata updated for file '%s': First block address is now %d.\n", fichiersMeta[i].nom_fichier, fichiersMeta[i].adresse_premier_bloc);
        }
    }
}
// Mis a jour metadonnee apres insertion
void miseAJourMetadonneesApresInsertion(char *nomFichier, int blocsUtilises, int nouveauxEnregistrements, char *modeOrganisation)
{
    int i;
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        if (strcmp(fichiersMeta[i].nom_fichier, nomFichier) == 0)
        {
            fichiersMeta[i].taille_enregistrements += nouveauxEnregistrements;

            if (strcmp(modeOrganisation, "chaînée") == 0)
            {
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
void MisDefragmentation()
{
    int indexLibre = 0;
    int i;
    // Parcours des m�tadonn�es pour r�organiser les fichiers non supprim�s
    for (i = 0; i < nombreFichiersMeta; i++)
    {
        if (strcmp(fichiersMeta[i].nom_fichier, "SUPPRIME") != 0)
        {
            // Copier les données valides à l'index libre
            if (i != indexLibre)
            {
                fichiersMeta[indexLibre] = fichiersMeta[i];
            }
            indexLibre++;
        }
    }
    // Mise � jour du nombre r�el de fichiers apr�s la d�fragmentation
    nombreFichiersMeta = indexLibre; // Met à jour le nombre réel de fichiers
    printf("Defragmentation completed. %d file(s) retained.\n", nombreFichiersMeta);
}
