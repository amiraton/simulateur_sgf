#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define FB 3 // Facteur de blocage
#define MAX_BLOCKS 1000
#define MAX_FILES 50
#define BLOC_LIBRE -1 // Indique qu'un bloc est libre
#define MAX_NOM 256
#define MAX_MODE 20

// Structure reprÃ©sentant un bloc de la mÃ©moire secondaire
typedef struct {
    int data[FB];    // DonnÃ©es du bloc
    int ne;          // Nombre d'enregistrements dans le bloc
    int nextBlock;   // Adresse du prochain bloc (mode chaÃ®nÃ©)
    bool libre;      // Indique si le bloc est libre ou occupÃ©
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
int nombreBlocs; // Nombre total de blocs dans la mÃ©moire secondaire
int nombreFichiersMeta = 0; // Nombre de fichiers
Metadata fichiers[MAX_FILES]; // Tableau des fichiers

// Initialisation de la mÃ©moire secondaire
void initialiserMemoireSecondaire(int nbBlocs) {
    if (nbBlocs > MAX_BLOCKS) {
        printf("Erreur : Nombre de blocs supÃ©rieur Ã  la capacitÃ© maximale (%d).\n", MAX_BLOCKS);
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
    printf("MÃ©moire secondaire initialisÃ©e avec %d blocs.\n", nombreBlocs);
}

void mettreAJourTableAllocation() {
    for (int i = 0; i < nombreBlocs; i++) {
        tableAllocation[i].libre = memoireSecondaire[i].libre;
        strcpy(tableAllocation[i].fichier, memoireSecondaire[i].fichier);
    }
}

void mettreAJourMetadonnees(Metadata *fichier) {
    int tailleEnregistrements = 0;
    int bloc_courant = fichier->premier_bloc;

    // Calculer la taille en enregistrements en fonction du mode global
    while (bloc_courant != -1) {
        tailleEnregistrements += memoireSecondaire[bloc_courant].ne;
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    // Mise ÃƒÂ  jour des mÃƒÂ©tadonnÃƒÂ©es
    fichier->taille_blocs = (tailleEnregistrements + FB - 1) / FB;
    printf("MÃƒÂ©tadonnÃƒÂ©es mises ÃƒÂ  jour pour le fichier '%s':\n", fichier->nom);
    printf("- Taille en blocs : %d\n", fichier->taille_blocs);
    printf("- Taille en enregistrements : %d\n", tailleEnregistrements);
    printf("- Premier bloc : %d\n", fichier->premier_bloc);
    printf("- Organisation globale : %s\n", fichier->organisation_globale);
    printf("- Organisation interne : %s\n", fichier->organisation_interne);
}

// Charger un fichier en mÃ©moire secondaire
int chargerFichier(Metadata *fichier) {
    int blocs_necessaires = fichier->taille_blocs;

    if (strcmp(fichier->organisation_globale, "contigue") == 0) {
        int premier_bloc_libre = -1, blocs_trouves = 0;
        int i ;

        for ( i = 0; i < nombreBlocs; i++) {
            if (memoireSecondaire[i].libre) {
                if (premier_bloc_libre == -1) 
                    premier_bloc_libre = i; // DÃ©but d'un espace libre
                blocs_trouves++;

                if (blocs_trouves == blocs_necessaires) 
                    break;
            } else {
                blocs_trouves = 0;
                premier_bloc_libre = -1;
            }
        }

        if (blocs_trouves < blocs_necessaires) {
            printf("Espace insuffisant pour charger le fichier '%s' en mode contigu.\n", fichier->nom);
            return -1;
        }

        // Allouer les blocs contigus trouvÃ©s
       
        for ( i = premier_bloc_libre; i < premier_bloc_libre + blocs_necessaires; i++) {
            memoireSecondaire[i].libre = false;
            strcpy(memoireSecondaire[i].fichier, fichier->nom);
            memoireSecondaire[i].ne = 0; // Initialiser Ã  0
        }

        fichier->premier_bloc = premier_bloc_libre;

    } else if (strcmp(fichier->organisation_globale, "chainee") == 0) {
        int premier_bloc = -1, precedent_bloc = -1, blocs_alloues = 0;
        int i ;
        for ( i = 0; i < nombreBlocs && blocs_alloues < blocs_necessaires; i++) {
            if (memoireSecondaire[i].libre) {
                memoireSecondaire[i].libre = false;
                strcpy(memoireSecondaire[i].fichier, fichier->nom);
                memoireSecondaire[i].ne = 0;

                if (premier_bloc == -1) 
                    premier_bloc = i; // DÃ©finir le premier bloc

                if (precedent_bloc != -1) 
                    memoireSecondaire[precedent_bloc].nextBlock = i;

                precedent_bloc = i;
                blocs_alloues++;
            }
        }

        if (blocs_alloues < blocs_necessaires) {
            printf("Espace insuffisant pour charger le fichier '%s' en mode chaÃ®nÃ©.\n", fichier->nom);
            return -1;
        }

        fichier->premier_bloc = premier_bloc;
    } else {
        printf("Mode d'organisation inconnu : %s\n", fichier->organisation_globale);
        return -1;
    }

    printf("Fichier '%s' chargÃ© avec succÃ¨s!\n", fichier->nom);
    mettreAJourTableAllocation();
    return 0;
}

// Fonction pour crÃ©er un fichier et le charger en mÃ©moire secondaire
void creerFichier() {
    if (nombreFichiersMeta >= MAX_FILES) {
        fprintf(stderr, "Erreur : Limite maximale de fichiers atteinte.\n");
        return;
    }

    Metadata fichier;
    printf("Entrez le nom du fichier : ");
    scanf("%s", fichier.nom);

    int nombreEnregistrements;
    printf("Entrez le nombre d'enregistrements : ");
    scanf("%d", &nombreEnregistrements);

    // Calcul du nombre de blocs nÃ©cessaires en fonction du facteur de blocage
    fichier.taille_blocs = (nombreEnregistrements + FB - 1) / FB;

    printf("Entrez le mode d'organisation globale (contigue ou chainee) : ");
    scanf("%s", fichier.organisation_globale);

    printf("Entrez le mode d'organisation interne (triee ou non) : ");
    scanf("%s", fichier.organisation_interne);

    // Charger le fichier en mÃ©moire secondaire
    if (chargerFichier(&fichier) == 0) {
        fichiers[nombreFichiersMeta] = fichier;
        nombreFichiersMeta++;
        printf("Fichier crÃ©Ã© et chargÃ© en mÃ©moire secondaire avec succÃ¨s.\n");
    }
}

void supprimerFichier() {
    printf("=== Suppression d'un Fichier ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante pour le supprimer :\n");
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

    // LibÃƒÂ©rer les blocs associÃƒÂ©s au fichier
    int bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
        memoireSecondaire[bloc_courant].libre = true;
        strcpy(memoireSecondaire[bloc_courant].fichier, "");
        memoireSecondaire[bloc_courant].ne = 0;

        int nextBloc = memoireSecondaire[bloc_courant].nextBlock;
        memoireSecondaire[bloc_courant].nextBlock = -1;
        bloc_courant = nextBloc;
    }
         mettreAJourTableAllocation();
    // Supprimer les mÃƒÂ©tadonnÃƒÂ©es du fichier
    
    for ( i = fichierIndex - 1; i < nombreFichiersMeta - 1; i++) {
        fichiers[i] = fichiers[i + 1];
    }
    nombreFichiersMeta--;

    printf("Fichier '%s' supprimÃƒÂ© avec succÃƒÂ¨s.\n", fichier->nom);
}

void renommerFichier() {
    printf("=== Renommer un Fichier ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante pour le renommer :\n");
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

    // Mettre ÃƒÂ  jour le nom dans les mÃƒÂ©tadonnÃƒÂ©es
    strcpy(fichier->nom, nouveauNom);

    // Mettre ÃƒÂ  jour le nom dans tous les blocs associÃƒÂ©s
    int bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
        strcpy(memoireSecondaire[bloc_courant].fichier, nouveauNom);
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }
          mettreAJourTableAllocation();
    // Mise ÃƒÂ  jour des mÃƒÂ©tadonnÃƒÂ©es
    mettreAJourMetadonnees(fichier);

    printf("Le fichier a ÃƒÂ©tÃƒÂ© renommÃƒÂ© en '%s'.\n", nouveauNom);
}


// Fonction pour insÃƒÂ©rer un enregistrement dans un fichier choisi
void insererEnregistrement() {
    // Demander Ã  l'utilisateur de choisir un fichier
    int fichierIndex;
    printf("SÃ©lectionnez un fichier Ã  partir de la liste suivante:\n");
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

    Metadata *fichier = &fichiers[fichierIndex - 1];  // AccÃ©der au fichier choisi

    // Demander les informations pour l'enregistrement
    Enregistrement enregistrement;
    printf("Entrez l'ID de l'enregistrement : ");
    scanf("%d", &enregistrement.id);
    printf("Entrez les donnÃ©es de l'enregistrement : ");
    scanf("%s", enregistrement.data);

    if (strcmp(fichier->organisation_globale, "contigue") == 0) {
        if (strcmp(fichier->organisation_interne, "triee") == 0) {
            // Mode contigu triÃ©
            int premierBloc = fichier->premier_bloc;
            int dernierBloc = premierBloc + fichier->taille_blocs;
            int i ;

            for ( i = premierBloc; i < dernierBloc; i++) {
                if (memoireSecondaire[i].ne < FB) {
                    // Trouver la bonne position dans le bloc triÃ©
                    int j;
                    for (j = 0; j < memoireSecondaire[i].ne; j++) {
                        if (memoireSecondaire[i].data[j] > enregistrement.id) {
                            break;
                        }
                    }
                      int k ;
                    // DÃ©caler les enregistrements si nÃ©cessaire
                    for ( k = memoireSecondaire[i].ne; k > j; k--) {
                        memoireSecondaire[i].data[k] = memoireSecondaire[i].data[k - 1];
                    }

                    // InsÃ©rer l'enregistrement Ã  la bonne position
                    memoireSecondaire[i].data[j] = enregistrement.id;
                    memoireSecondaire[i].ne++;
                    printf("Enregistrement insÃ©rÃ© dans le bloc %d Ã  la position %d\n", i, j);
                    return;
                }
            }
        } else {
            // Mode contigu non triÃ©
            int premierBloc = fichier->premier_bloc;
            int dernierBloc = premierBloc + fichier->taille_blocs;
             int i ;
            for ( i = premierBloc; i < dernierBloc; i++) {
                if (memoireSecondaire[i].ne < FB) {
                    // Ajouter l'enregistrement dans le premier espace disponible
                    memoireSecondaire[i].data[memoireSecondaire[i].ne] = enregistrement.id;
                    memoireSecondaire[i].ne++;
                    printf("Enregistrement insÃ©rÃ© dans le bloc %d\n", i);
                    return;
                }
            }
        }
    } else if (strcmp(fichier->organisation_globale, "chainee") == 0) {
        // Mode chaÃ®nÃ©
        int bloc_courant = fichier->premier_bloc;

        while (bloc_courant != -1) {
            if (memoireSecondaire[bloc_courant].ne < FB) {
                memoireSecondaire[bloc_courant].data[memoireSecondaire[bloc_courant].ne] = enregistrement.id;
                memoireSecondaire[bloc_courant].ne++;
                printf("Enregistrement insÃ©rÃ© dans le bloc %d\n", bloc_courant);
                return;
            }
            bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
        }
    } else {
        printf("Mode d'organisation inconnu : %s\n", fichier->organisation_globale);
    }

    printf("Aucun espace disponible pour insÃ©rer l'enregistrement.\n");
}

void rechercherEnregistrementParID() {
    printf("=== Recherche d'Enregistrement par ID ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante pour effectuer la recherche :\n");
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

    int id;
    printf("Entrez l'ID de l'enregistrement ÃƒÂ  rechercher : ");
    scanf("%d", &id);

    // Recherche en fonction du mode d'organisation globale et interne
    if (strcmp(fichier->organisation_globale, "contigue") == 0) {
        // Mode contiguÃƒÂ«
        int i ;
        for (i = fichier->premier_bloc; i < fichier->premier_bloc + fichier->taille_blocs; i++) {
            if (strcmp(fichier->organisation_interne, "triee") == 0) {
                // Recherche binaire (mode triÃƒÂ©)
                int gauche = 0, droite = memoireSecondaire[i].ne - 1;
                while (gauche <= droite) {
                    int milieu = (gauche + droite) / 2;
                    if (memoireSecondaire[i].data[milieu] == id) {
                        printf("Enregistrement trouvÃƒÂ© : Bloc %d, Position %d\n", i, milieu);
                        return;
                    } else if (memoireSecondaire[i].data[milieu] < id) {
                        gauche = milieu + 1;
                    } else {
                        droite = milieu - 1;
                    }
                }
            } else {
                // Recherche linÃƒÂ©aire (mode non triÃƒÂ©)
                int j ;
                for ( j = 0; j < memoireSecondaire[i].ne; j++) {
                    if (memoireSecondaire[i].data[j] == id) {
                        printf("Enregistrement trouvÃƒÂ© : Bloc %d, Position %d\n", i, j);
                        return;
                    }
                }
            }
        }
    } else if (strcmp(fichier->organisation_globale, "chainee") == 0) {
        // Mode chaÃƒÂ®nÃƒÂ©
        int bloc_courant = fichier->premier_bloc;
        while (bloc_courant != -1) {
            if (strcmp(fichier->organisation_interne, "triee") == 0) {
                // Recherche binaire (mode triÃƒÂ©)
                int gauche = 0, droite = memoireSecondaire[bloc_courant].ne - 1;
                while (gauche <= droite) {
                    int milieu = (gauche + droite) / 2;
                    if (memoireSecondaire[bloc_courant].data[milieu] == id) {
                        printf("Enregistrement trouvÃƒÂ© : Bloc %d, Position %d\n", bloc_courant, milieu);
                        return;
                    } else if (memoireSecondaire[bloc_courant].data[milieu] < id) {
                        gauche = milieu + 1;
                    } else {
                        droite = milieu - 1;
                    }
                }
            } else {
                // Recherche linÃƒÂ©aire (mode non triÃƒÂ©)
                int j ;
                for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
                    if (memoireSecondaire[bloc_courant].data[j] == id) {
                        printf("Enregistrement trouvÃƒÂ© : Bloc %d, Position %d\n", bloc_courant, j);
                        return;
                    }
                }
            }
            bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
        }
    } else {
        printf("Mode d'organisation globale inconnu : %s\n", fichier->organisation_globale);
        return;
    }

    printf("Enregistrement avec ID %d non trouvÃƒÂ©.\n", id);
}


void supprimerEnregistrementLogique() {
    printf("=== Suppression Logique ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante:\n");
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

    // Afficher les enregistrements disponibles
    printf("Enregistrements disponibles dans '%s':\n", fichier->nom);
    int bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
    	int j ;
        for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
            if (memoireSecondaire[bloc_courant].data[j] != -1) {
                printf("Bloc %d, Position %d: ID = %d\n", bloc_courant, j, memoireSecondaire[bloc_courant].data[j]);
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    int id;
    printf("Entrez l'ID de l'enregistrement ÃƒÂ  supprimer : ");
    scanf("%d", &id);

    // Suppression logique
    bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
    	int j ;
        for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
            if (memoireSecondaire[bloc_courant].data[j] == id) {
                memoireSecondaire[bloc_courant].data[j] = -1; // Suppression logique
                printf("Enregistrement %d supprimÃƒÂ© logiquement du bloc %d\n", id, bloc_courant);
                mettreAJourTableAllocation();
                mettreAJourMetadonnees(fichier); // Mise ÃƒÂ  jour des mÃƒÂ©tadonnÃƒÂ©es
                return;
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    printf("Enregistrement non trouvÃƒÂ©.\n");
}

void supprimerEnregistrementPhysique() {
    printf("=== Suppression Physique ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante:\n");
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

    // Afficher les enregistrements disponibles
    printf("Enregistrements disponibles dans '%s':\n", fichier->nom);
    int bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
    	int j ;
        for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
            if (memoireSecondaire[bloc_courant].data[j] != -1) {
                printf("Bloc %d, Position %d: ID = %d\n", bloc_courant, j, memoireSecondaire[bloc_courant].data[j]);
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    int id;
    printf("Entrez l'ID de l'enregistrement ÃƒÂ  supprimer : ");
    scanf("%d", &id);

    // Suppression physique
    bloc_courant = fichier->premier_bloc;
    while (bloc_courant != -1) {
    	int j ; int k ;
        for ( j = 0; j < memoireSecondaire[bloc_courant].ne; j++) {
            if (memoireSecondaire[bloc_courant].data[j] == id) {
                for ( k = j; k < memoireSecondaire[bloc_courant].ne - 1; k++) {
                    memoireSecondaire[bloc_courant].data[k] = memoireSecondaire[bloc_courant].data[k + 1];
                }
                memoireSecondaire[bloc_courant].ne--;
                printf("Enregistrement %d supprimÃƒÂ© physiquement du bloc %d\n", id, bloc_courant);
                mettreAJourTableAllocation();
                mettreAJourMetadonnees(fichier); // Mise ÃƒÂ  jour des mÃƒÂ©tadonnÃƒÂ©es
                return;
            }
        }
        bloc_courant = memoireSecondaire[bloc_courant].nextBlock;
    }

    printf("Enregistrement non trouvÃƒÂ©.\n");
}

void defragmenterFichier() {
    printf("=== DÃƒÂ©fragmentation ===\n");

    // Afficher la liste des fichiers
    printf("SÃƒÂ©lectionnez un fichier ÃƒÂ  partir de la liste suivante:\n");
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

    // RÃƒÂ©ÃƒÂ©crire les enregistrements
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

    printf("DÃƒÂ©fragmentation terminÃƒÂ©e pour '%s'.\n", fichier->nom);
    mettreAJourTableAllocation();
    mettreAJourMetadonnees(fichier); // Mise ÃƒÂ  jour des mÃƒÂ©tadonnÃƒÂ©es
}

void viderMemoire() {
    printf("=== Vider la MÃƒÂ©moire Secondaire ===\n");

    // RÃƒÂ©initialiser tous les blocs de mÃƒÂ©moire secondaire
    int i ;
    for (i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
        strcpy(memoireSecondaire[i].fichier, "");
    }
      mettreAJourTableAllocation();
    // RÃƒÂ©initialiser toutes les mÃƒÂ©tadonnÃƒÂ©es
    nombreFichiersMeta = 0;
    printf("MÃƒÂ©moire secondaire vidÃƒÂ©e avec succÃƒÂ¨s. Tous les fichiers et blocs ont ÃƒÂ©tÃƒÂ© rÃƒÂ©initialisÃƒÂ©s.\n");
}
void compactageMemoire() {
    int i, j;
    int prochainBlocLibre = 0;  // Premier bloc libre disponible pour le compactage
    
    // RÃ©initialiser les blocs libres
    for (i = 0; i < nombreBlocs; i++) {
        memoireSecondaire[i].libre = true;
        strcpy(memoireSecondaire[i].fichier, "");
        memoireSecondaire[i].ne = 0;
        memoireSecondaire[i].nextBlock = -1;
    }

    // Recharger les fichiers en mÃ©moire aprÃ¨s le compactage
    for (i = 0; i < nombreFichiersMeta; i++) {
        Metadata *fichier = &fichiers[i];
        int premierBlocCompacte = -1, precedentBloc = -1;
        int blocsAlloues = 0;

        // Allouer les blocs de maniÃ¨re contiguÃ«
        for (j = 0; j < fichier->taille_blocs; j++) {
            if (prochainBlocLibre < nombreBlocs) {
                // Allouer un bloc pour le fichier
                memoireSecondaire[prochainBlocLibre].libre = false;
                strcpy(memoireSecondaire[prochainBlocLibre].fichier, fichier->nom);
                memoireSecondaire[prochainBlocLibre].ne = 0;

                if (premierBlocCompacte == -1) {
                    premierBlocCompacte = prochainBlocLibre;  // DÃ©finir le premier bloc
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

        // Mettre Ã  jour les mÃ©tadonnÃ©es du fichier
        fichier->premier_bloc = premierBlocCompacte;
        mettreAJourMetadonnees(fichier);
    }
        
        mettreAJourTableAllocation() ;
    printf("Le compactage de la mÃ©moire secondaire a Ã©tÃ© effectuÃ© avec succÃ¨s.\n");
}


void afficherEtatMemoire() {
    printf("\nMemory State:\n");
    printf("[Free = \033[32mGreen\033[0m, Occupied = \033[31mRed\033[0m]\n");
    int i ;
    for ( i = 0; i < nombreBlocs; i++) {
        if (memoireSecondaire[i].libre) {
            printf("\033[32m[Free]\033[0m ");
        } else {
            printf("\033[31m[%s: %d records]\033[0m ", memoireSecondaire[i].fichier, memoireSecondaire[i].ne);
        }
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void afficherMetadonnees() {
    printf("=== MÃƒÂ©tadonnÃƒÂ©es des Fichiers ===\n");

    if (nombreFichiersMeta == 0) {
        printf("Aucun fichier n'est actuellement enregistrÃƒÂ©.\n");
        return;
    }

    // En-tÃƒÂªte du tableau
    printf("+----------------------+------------+------------+-------------------+-------------------+------------+\n");
    printf("| %-20s | %-10s | %-10s | %-17s | %-17s | %-10s |\n", 
           "Nom du fichier", "Blocs", "Enreg.", "Org. Globale", "Org. Interne", "Premier Bloc");
    printf("+----------------------+------------+------------+-------------------+-------------------+------------+\n");

    // Affichage des mÃƒÂ©tadonnÃƒÂ©es pour chaque fichier
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


int main() {
    int choix;
    do {
         printf("\n=== Menu Principal ===\n");
        printf("1. Initialiser la memoire secondaire\n");
        printf("2. Creer un fichier\n");
        printf("3. Afficher l'etat de la memoire secondaire\n");
        printf("4. Afficher les metadonnes des fichiers\n");
        printf("5. Rechercher un enregistrement par ID\n");
        printf("6. Inserer un nouvel enregistrement dans un fichier\n");
        printf("7. Supprimer un enregistrement (logique)\n");
        printf("8. Supprimer un enregistrement (physique)\n");
        printf("9. Defragmenter un fichier\n");
        printf("10. Supprimer un fichier\n");
        printf("11. Renommer un fichier\n");
        printf("12. Compactage de la memoire secondaire\n");
        printf("13. Vider la memoire secondaire\n");
        printf("14. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("Nombre de blocs dans la mÃƒÂ©moire secondaire : ");
                int nbBlocs;
                scanf("%d", &nbBlocs);
                initialiserMemoireSecondaire(nbBlocs);
                break;

            case 2:
                creerFichier();
                break;

            case 3:
                 afficherEtatMemoire();
                break;

            case 4:
                afficherMetadonnees();
                break;
            case 5:
            	rechercherEnregistrementParID();
            	break;
            case 6:
				insererEnregistrement();
				break;
			case 7:
				supprimerEnregistrementLogique();
				break;
			case 8:
				supprimerEnregistrementPhysique();
				break;
			case 9:
				defragmenterFichier();
				break;
			case 10:
				supprimerFichier();
				break;
			case 11 :
				renommerFichier();
				break;
			case 12:
				compactageMemoire();
				break;
			case 13 :
				viderMemoire();
				break;
			case 14:
			 printf("Au revoir !\n");
			 break;	
            default:
                printf("Option invalide. Veuillez rÃƒÂ©essayer.\n");
        }
    } while (choix != 15);

    return 0;
}
