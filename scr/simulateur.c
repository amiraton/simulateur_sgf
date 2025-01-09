/*
Projet: Simulateur Simplifié d’un Système de Gestion de Fichiers (SGF)



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
//fonction pour renomer un fichier 
void renommerFichier() {
    printf("=== Renommer un Fichier ===\n");

    // Afficher la liste des fichiers
    printf("Selectionnez un fichier a  partir de la liste suivante pour le renommer :\n");
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

    printf("Entrez le nouveau nom pour le fichier '%s' : ", fichier->nom);
    char nouveauNom[MAX_NOM];
    scanf("%s", nouveauNom);

    // Mettre a  jour le nom dans les metadonnees
    strcpy(fichier->nom, nouveauNom);

    // Mettre a  jour le nom dans tous les blocs associees
    int bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
        strcpy(memoireSecondaire[bloc_courant].fichier, nouveauNom);
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }
          mettreAJourTableAllocation();
    // Mise a  jour des metadonnees
    mettreAJourMetadonnees(fichier);

    printf("Le fichier a ete renomme en '%s'.\n", nouveauNom);
}

// Fonction pour inserer un enregistrement dans un fichier choisi
void insererEnregistrement() {
    // Demander a  l'utilisateur de choisir un fichier
    int fichierIndex;
    printf("Selectionnez un fichier a  partir de la liste suivante:\n");
    int i ;
    for ( i = 0; i < nombreFichiersMeta; i++) {
        printf("%d. %s\n", i + 1, fichiers[i].nom);
    }
    printf("Votre choix : ");
    scanf("%d", &fichierIndex);

    if (fichierIndex < 1 || fichierIndex > nombreFichiersMeta) {
        printf("Choix invalide.\n");
        return;
    }

    Metadata *fichier = &fichiers[fichierIndex - 1];  // Acceder au fichier choisi

    // Demander les informations pour l'enregistrement
    Enregistrement enregistrement;
    printf("Entrez l'ID de l'enregistrement : ");
    scanf("%d", &enregistrement.id);
    printf("Entrez les donnees de l'enregistrement : ");
    scanf("%s", enregistrement.data);

    if (strcmp(fichier->organisation_globale, "contigue") == 0) {
        if (strcmp(fichier->organisation_interne, "triee") == 0) {
            // Mode contigu triee
            int premierBloc = fichier->premier_bloc;
            int dernierBloc = premierBloc + fichier->taille_blocs;
            int i ;

            for ( i = premierBloc; i < dernierBloc; i++) {
                if (memoireSecondaire[i].ne < FB) {
                    // Trouver la bonne position dans le bloc triee
                    int j;
                    for (j = 0; j < memoireSecondaire[i].ne; j++) {
                        if (memoireSecondaire[i].data[j] > enregistrement.id) {
                            break;
                        }
                    }
                      int k ;
                    // Decaler les enregistrements si necessaire
                    for ( k = memoireSecondaire[i].ne; k > j; k--) {
                        memoireSecondaire[i].data[k] = memoireSecondaire[i].data[k - 1];
                    }

                    // Inserer l'enregistrement a  la bonne position
                    memoireSecondaire[i].data[j] = enregistrement.id;
                    memoireSecondaire[i].ne++;
                    printf("Enregistrement insere dans le bloc %d a  la position %d\n", i, j);
                    return;
                }
            }
        } else {
            // Mode contigu non triee
            int premierBloc = fichier->premier_bloc;
            int dernierBloc = premierBloc + fichier->taille_blocs;
             int i ;
            for ( i = premierBloc; i < dernierBloc; i++) {
                if (memoireSecondaire[i].ne < FB) {
                    // Ajouter l'enregistrement dans le premier espace disponible
                    memoireSecondaire[i].data[memoireSecondaire[i].ne] = enregistrement.id;
                    memoireSecondaire[i].ne++;
                    printf("Enregistrement insere dans le bloc %d\n", i);
                    return;
                }
            }
        }
    } else if (strcmp(fichier->organisation_globale, "chainee") == 0) {
        // Mode chaine
        int bloc_courant = fichier->premier_bloc;

        while (bloc_courant != -1) {
            if (memoireSecondaire[bloc_courant].ne < FB) {
                memoireSecondaire[bloc_courant].data[memoireSecondaire[bloc_courant].ne] = enregistrement.id;
                memoireSecondaire[bloc_courant].ne++;
                printf("Enregistrement insere dans le bloc %d\n", bloc_courant);
                return;
            }
            bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
        }
    } else {
        printf("Mode d'organisation inconnu : %s\n", fichier->organisation_globale);
    }

    printf("Aucun espace disponible pour inserer l'enregistrement.\n");
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
                mettreAJourTableAllocation();    // Met a jour la table d'allocation.
                mettreAJourMetadonnees(fichier); // Met a jour les métadonnées.
                return;
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock; // Passe au bloc suivant.
    }

    // Si l'ID n'a pas été trouvé.
    printf("Enregistrement non trouve.\n");
}

// fonction pour afficher metadonnee dans un tableau 
void afficherMetadonnees() {
    printf("=== Metadonnees des Fichiers ===\n");

    if (nombreFichiersMeta == 0) {
        printf("Aucun fichier n'est actuellement enregistrÃƒÂ©.\n");
        return;
    }

    // En-tete du tableau
    printf("+----------------------+------------+------------+-------------------+-------------------+------------+\n");
    printf("| %-20s | %-10s | %-10s | %-17s | %-17s | %-10s |\n", 
           "Nom du fichier", "Blocs", "Enreg.", "Org. Globale", "Org. Interne", "Premier Bloc");
    printf("+----------------------+------------+------------+-------------------+-------------------+------------+\n");

    // Affichage des metadonnees pour chaque fichier
    int i ;
    for ( i = 0; i < nombreFichiersMeta; i++) {
        Metadata *fichier = &fichiers[i];
        printf("| %-20s | %-10d | %-10d | %-17s | %-17s | %-10d |\n", 
               fichier->nom, 
               fichier->taille_blocs, 
               fichier->taille_blocs * FB, // Taille en enregistrements
               fichier->organisation_globale, 
               fichier->organisation_interne, 
               fichier->premier_bloc);
    }

    // Pied du tableau
    printf("+----------------------+------------+------------+-------------------+-------------------+------------+\n");
}
