#include "grid.h"
#include "SimpleButton.h"
#include "RadioButton.h"
#include "zonedetexte.h"
#include "BoiteDialogue.h"
#include "frame.h"

void clearBox(GtkWidget* box) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(box));
    GList* iter = children;
    while (iter != NULL) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
        iter = g_list_next(iter);
    }

    g_list_free(children);
}

void fini_Jeu()
{
    if (New_game.Players[New_game.tour_Joueur]->fini == 1 && New_game.Players[1 - New_game.tour_Joueur]->fini == 1)
    {
        New_game.fini = 2;
        dialogue_ajouter(window.window);
    }
    else
    {
        if (New_game.Players[1 - New_game.tour_Joueur]->fini == 1)
        {
            New_game.vainqueur = New_game.tour_Joueur;
            dialogue_ajouter(window.window);
        }
        else
        {
            New_game.vainqueur = 1 - New_game.tour_Joueur;
            dialogue_ajouter(window.window);
        }
    }

}

void gtk_sleep(int milliseconds) {
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }

    gint64 start_time = g_get_monotonic_time();
    gint64 end_time = start_time + (gint64)milliseconds * 1000;

    while (g_get_monotonic_time() < end_time) {
        gtk_main_iteration_do(FALSE);
    }
}

Cell* create_new_cell(Cell* source_cell)
{
    Cell* new_cell = malloc(sizeof(Cell));
    if (!new_cell)
    {
        printf("echec d'allocation");
        exit(0);
    }
    new_cell->row = source_cell->row;
    new_cell->col = source_cell->col;
    new_cell->button = source_cell->button;
    return ((Cell*)new_cell);
}

void init_tab(tableau* tab)
{
    if (!tab) return;
    tab->nbreElem = -1;
}

Liste_piece* cree_piece(int x, int y)
{
    Liste_piece* NE = (Liste_piece*)malloc(sizeof(Liste_piece));
    if (!NE)
    {
        printf("echec d'allocation");
        exit(0);
    }
    NE->x = x;
    NE->y = y;
    NE->is_dame = 0;
    NE->svt = NULL;
    return ((Liste_piece*)NE);
}

void afficher_liste(Liste_piece* liste)
{
    Liste_piece* tmp = NULL;
    if (!liste)
    {
        printf("la liste est vide\n");
        exit(0);
    }
    tmp = liste;
    while (tmp->svt)
    {
        printf("(%d,%d) %d -> ", tmp->x, tmp->y, tmp->is_dame);
        tmp = tmp->svt;
    }
    printf("(%d,%d) %d ", tmp->x, tmp->y, tmp->is_dame);
}

Liste_piece* chercher_pion(Liste_piece* Liste, int x, int y)
{
    Liste_piece* tmp = Liste;
    while (tmp)
    {
        if (tmp->x == x && tmp->y == y) return tmp;
        tmp = tmp->svt;
    }
    return NULL;
}

/*
   Description : Initialise les joueurs et leurs pièces dans un jeu de
                    plateau.
   Entrée : game - Pointeur vers la structure du jeu de plateau.
   Sortie : Pointeur vers la structure du jeu de plateau initialisée.
*/

Board_Jeu* init_Joueur(Board_Jeu* game)
{
    int i, j; // variables pour parcourir la boucle

    // Initialiser les Joueurs
    for (i = 0; i < 2; i++)
    {
        // Allouer de la mémoire pour un joueur
        game->Players[i] = (Joueur*)malloc(sizeof(Joueur)); 

        // Vérifier l'allocation mémoire
        if (!game->Players[i])
        {
            printf("echec d'allocation\n");
            exit(0);
        }

        game->Players[i]->type = i; // Définir le type du joueur
        game->Players[i]->fini = 0; // Définir le statut de fini du joueur
        //Initialiser la liste de pièces du joueur à NULL
        game->Players[i]->Mes_pieces = NULL; 
        game->Players[i]->score = 12; // Initialiser le score du joueur à 12
    }

    // Initialiser les pièces de chaque joueur
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            // Ajouter les pions du joueur en haut du plateau
            if (game->board[i][j] == PLAYER1 || game->board[i][j] == PLAYER2)
            {
                // Créer une pièce à la position (i, j)
                Liste_piece* pion = cree_piece(i, j); 
                // Ajouter la pièce à la liste de pièces du joueur
                pion->svt = game->Players[game->board[i][j] - 1]->Mes_pieces; 
                // Mettre à jour la tête de la liste de pièces du joueur
                game->Players[game->board[i][j] - 1]->Mes_pieces = pion; 
            }
        }
    }

    return ((Board_Jeu*)game); // Retourner le jeu de plateau initialisé
}


/*
   Description : Initialise le jeu de plateau en remplissant la matrice 
                 avec les pièces des joueurs et en initialisant d'autres 
                 variables du jeu.
   Entrée : game - Pointeur vers la structure du jeu de plateau.
   Sortie : Aucune (void).
*/
void init_Jeu(Board_Jeu* game)
{
    int i, j;
    // Remplissage de la matrice
    for (i = 0; i < Colonne; i++)
    {
        for (j = 0; j < ligne; j++)
        {
            if ((i + j) % 2 == 1)
            {
                // Place les pièces du joueur 1 dans les trois premières lignes
                if (i < 3)	    game->board[i][j] = PLAYER1; 
                // Place les pièces du joueur 2 dans les trois dernières lignes
                else if (i > 4) game->board[i][j] = PLAYER2; 
                // Cases vides pour les autres cases
                     else      game->board[i][j] = EMPTY; 
            }
            else game->board[i][j] = EMPTY; //Cases vides pour les cases impaires
        }
    }

    // Initialisation des autres variables du jeu
    game->fini = 0; // Le jeu n'est pas terminé au départ
    game->vainqueur = -1; // Pas de vainqueur déterminé initialement
    //game->tour_Joueur = 0; // Le joueur 0 commence
    game->Score = 0; // Score initial à zéro

    // Appel de la fonction pour initialiser les joueurs et leurs pièces
    game = init_Joueur(game);
}


void print_board(Board_Jeu* game)
{
    int i, j;
    char piece;
    printf("  0 1 2 3 4 5 6 7\n"); // Print column headers
    for (i = 0; i < 8; i++)
    {
        printf("%d ", i); // Print row header
        for (j = 0; j < 8; j++)
        {
            piece = game->board[i][j];
            switch (piece)
            {
            case EMPTY:         printf("  "); break;
            case PLAYER1:       printf("O "); break;
            case PLAYER2:       printf("X "); break;
            case PLAYER1_KING:  printf("K "); break;
            case PLAYER2_KING:  printf("Y "); break;
            }
        }
        printf("%d", i); // Print row footer
        printf("\n"); // Start new line
    }
    printf("  0 1 2 3 4 5 6 7\n"); // Print column footers
}

//fonctions de dames

/*
   Description : Vérifie si une pièce donnée est une pièce adverse en fonction du tour actuel.
   Entrées : piece - La pièce à vérifier.
             tour - Le tour actuel (1 ou 2).
   Sortie : 1 si la pièce est une pièce adverse, 0 sinon.
*/
int is_opposing_piece(piece_t piece, int tour)
{
    // Une case vide n'est pas considérée comme une pièce adverse
    if (piece == EMPTY)  return 0; 
    // Si c'est le tour du joueur 1, vérifie si la pièce est PLAYER1 ou PLAYER1_KING
    if (tour == 1)       return (piece == PLAYER1 || piece == PLAYER1_KING);
    // Si c'est le tour du joueur 2, vérifie si la pièce est PLAYER2 ou PLAYER2_KING
    return (piece == PLAYER2 || piece == PLAYER2_KING); 
}

/*
   Description : Vérifie si un mouvement donné est un mouvement de capture
                 dans le jeu de plateau.
   Entrées : x1, y1 - Coordonnées de départ du mouvement.
             x2, y2 - Coordonnées de destination du mouvement.
             Jeu - La structure du jeu de plateau.
   Sortie : 1 si le mouvement est un mouvement de capture, 0 sinon,
            -1 en cas d'erreur.
*/
int is_capturing_move(int x1, int y1, int x2, int y2, Board_Jeu Jeu)
{
    int dx = x2 - x1, 
        dy = y2 - y1, 
        mid_x = (x1 + x2) / 2, 
        mid_y = (y1 + y2) / 2;
    piece_t mid_piece;

    if (abs(dx) != 2 || abs(dy) != 2 || Jeu.board[mid_x][mid_y] == EMPTY)
        return 0; // Le mouvement ne capture pas une pièce

    // Vérifie que la case intermédiaire contient une pièce opposée
    mid_piece = Jeu.board[mid_x][mid_y];

    // Le mouvement capture une pièce opposée
    if (((mid_piece == PLAYER1 || mid_piece == PLAYER1_KING) &&
          Jeu.tour_Joueur == 1) ||
        ((mid_piece == PLAYER2 || mid_piece == PLAYER2_KING) &&
            Jeu.tour_Joueur == 0))
        return 1;

    return -1; // Si c'est une capture de meme pion du meme joueur
}


/*
   Description : Vérifie si une pièce dames peut gagner en effectuant 
                une prise ou un mouvement.
   Entrées : game - La structure du jeu de plateau.
             posx, posy - Les coordonnées de la pièce dames à vérifier.
   Sortie : 1 si la pièce dames peut gagner, 0 sinon.
*/
int peut_gagner_dames(Board_Jeu game, int posx, int posy)
{
    int i, j, k;

    // Vérification des prises possibles en diagonale
    for (i = -1; i <= 1; i += 2)
    {
        for (j = -1; j <= 1; j += 2)
        {
            for (k = 1; k < BOARD_SIZE; k++)
            {
                /*Sortie du plateau, pas de prise possible dans cette 
                    direction ou case non vide, pas de prise possible dans
                    cette direction*/
                if (posx + i * k < 0 || posx + i * k >= BOARD_SIZE ||
                    posy + j * k < 0 || posy + j * k >= BOARD_SIZE ||
                    game.board[posx + i * k][posy + j * k] != EMPTY)
                    break;
            }
        }
    }

    // Vérification des mouvements possibles en diagonale
    for (i = -1; i <= 1; i += 2)
    {
        for (j = -1; j <= 1; j += 2)
        {
            for (k = 1; k < BOARD_SIZE; k++)
            {
                // Sortie du plateau, pas de mouvement possible dans cette direction
                if (posx + i * k < 0 || posx + i * k >= BOARD_SIZE ||
                    posy + j * k < 0 || posy + j * k >= BOARD_SIZE ||
                    game.board[posx + i * k][posy + j * k] != EMPTY)
                    break;
                if (k == 1)  return (int)1; // Mouvement possible en diagonale
            }
        }
    }
    return (int)0; // La pièce ne peut pas gagner
}


/*
   Description : Vérifie si un mouvement donné pour une pièce dames
                est valide.
   Entrées : x, y - Coordonnées de départ de la pièce dames.
             new_x, new_y - Coordonnées de destination de la pièce dames.
             is_capturing - Indicateur si le mouvement est une prise.
             game - La structure du jeu de plateau.
   Sortie : 1 si le mouvement est valide, 0 sinon.
*/
int is_valide_dame(int x, int y, int new_x, int new_y, int is_capturing,
                    Board_Jeu game)
{
    int deltaY, deltaX,//pour déterminer la direction du mouvement(haut,bas,gauche,droite)
        tmp_x, tmp_y;

    deltaY = (new_x > x) ? 1 : -1;
    deltaX = (new_y > y) ? 1 : -1;
    tmp_x = x + deltaY;
    tmp_y = y + deltaX;

    /*tant que les coordonnées temporaires ne sont pas égales
            aux coordonnées de destination*/
    while (tmp_x != new_x && tmp_y != new_y)
    {
        /*Comparer la couleur du pion sur la case intermédiaire avec
                celle du joueur en cours*/
        if ((game.tour_Joueur == 0 && (game.board[tmp_x][tmp_y] == PLAYER1 ||
                game.board[tmp_x][tmp_y] == PLAYER1_KING)) || 
            (game.tour_Joueur == 1 && (game.board[tmp_x][tmp_y] == PLAYER2 ||
                game.board[tmp_x][tmp_y] == PLAYER2_KING)))
        {
            // La rafle capture un pion du joueur en cours
            return (int)0;
        }
        //Si la pièce sur la case intermédiaire est une pièce adverse
        if ((game.tour_Joueur == 0 && (game.board[tmp_x][tmp_y] == PLAYER2 ||
                game.board[tmp_x][tmp_y] == PLAYER2_KING)) ||
            (game.tour_Joueur == 1 && (game.board[tmp_x][tmp_y] == PLAYER1 || 
                game.board[tmp_x][tmp_y] == PLAYER1_KING)))
        {
            if (game.board[tmp_x + deltaY][tmp_y + deltaX] != EMPTY)
                return (int)0;
        }

        tmp_x += deltaY;
        tmp_y += deltaX;
    }

    return (int)1; // Le mouvement est valide
}



/*
   Description : Vérifie si une entrée de mouvement donnée est valide.
   Entrées : x, y - Coordonnées de départ du mouvement.
             new_x, new_y - Coordonnées de destination du mouvement.
             is_capturing - Indicateur si le mouvement est une prise.
             game - La structure du jeu de plateau.
   Sortie : 1 si l'entrée est valide, 0 sinon.
*/
int is_valid_input(int x, int y, int new_x, int new_y, 
                    int is_capturing, Board_Jeu game)
{
    int test, est_dame, trouve = 0;
    Liste_piece* tmp = NULL, * liste_tmp=NULL;
    // Vérifier si les coordonnées se trouvent dans les limites du plateau
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || new_x < 0 ||
        new_x >= BOARD_SIZE || new_y < 0 || new_y >= BOARD_SIZE)
    {
        printf(" \n vous avez dépassé le plateau \n ");
        return (int)0;
    }
    //Chercher le pion de la source 
    
    liste_tmp = game.Players[game.tour_Joueur]->Mes_pieces;
    while (liste_tmp)
    {
        if (liste_tmp->x == x && liste_tmp->y == y)
        {
            est_dame = liste_tmp->is_dame;
            trouve = 1;
        }
        liste_tmp = liste_tmp->svt;
    }

    // Vérifier si le joueur essaie de déplacer la pièce de l'adversaire
    if (trouve == 0)
    {
        printf(" \n ce n'est pas votre pièce \n");
        return (int)0;
    }

    if (is_capturing == -1)
    {
        printf(" \nC'est votre propre piece \n ");
        return (int)0;
    }

    /* Vérifier si les nouvelles coordonnées sont différentes
            des coordonnées actuelles*/
    if (x == new_x && y == new_y)
    {
        printf(" \n même position \n ");
        return (int)0;
    }

    // Vérifier si le joueur essaie de se déplacer vers une case occupée
    if (game.board[new_x][new_y] != EMPTY)
    {
        printf("\ncase occupée contient %d\n", game.board[new_x][new_y]);
        return (int)0;
    }

    if (est_dame == 0 && is_capturing == 0 && 
        (new_x != x - 1 && new_x != x + 1))
    {
        printf("\ncase occupée contient %d\n", game.board[new_x][new_y]);
        return (int)0;
    }
    /*Verifie si le pion est une dame puis fait appel a la fonction qui 
            effectue une vérification spécifique pour les pièces dames*/
    if (est_dame == 1)
    {
        return (int)is_valide_dame(x, y, new_x, new_y, is_capturing, game);
    }

    // Vérifier si le joueur essaie de se déplacer trop loin
    if ((est_dame == 0) && (abs(new_x - x) > 2))
    {
        printf(" \n trop loin \n ");
        return (int)0;
    }
    /*Vérifie si le joueur essaie de se déplacer dans une mauvaise 
            direction pour une pièce non - roi*/
    if ((est_dame == 0) && ((game.tour_Joueur == 1 && new_x > x) ||
        (game.tour_Joueur == 0 && new_x < x) ||
        abs(new_x - x) != abs(new_y - y)))
    {
        printf(" \n mauvaise direction, pas roi \n ");
        return (int)0;
    }
    /*vérifie si le joueur essaie de se déplacer dans une mauvaise 
        direction pour une pièce roi*/
    if ((est_dame == 1) && abs(new_x - x) != abs(new_y - y))
    {
        printf(" \n mauvaise direction, roi \n ");
        return (int)0;
    }

    return (int)1; // L'entrée est valide
}


/*
   Description : Teste si un joueur peut effectuer une capture avec l'une
            de ses pièces.
   Entrée : game - La structure du jeu de plateau.
   Sortie : Un pointeur vers la pièce qui peut effectuer une capture, ou NULL
        si aucune pièce ne peut capturer.
*/
Liste_piece* tester_gain(Board_Jeu game)
{
    Liste_piece* tmp = NULL, * nf = NULL;
    int new_x = 0, new_y = 0, is_capturing = 0, i, res;
    int indices[2][2][2] =
    {
      {
            {2,-2},
            {2,2},
      },
      {
            {-2,-2},
            {-2,2},
      }
    };

    tmp = game.Players[game.tour_Joueur]->Mes_pieces;

    while (tmp)
    {
        if (tmp->is_dame == 1)
        {
            // Vérifier si une pièce dames peut capturer
            res = peut_gagner_dames(game, tmp->x, tmp->y);
            if (res == 1 && is_valid_input(tmp->x, tmp->y, new_x, new_y, is_capturing, game))
            {
                nf = tmp;
                return (Liste_piece*)nf;
            }
        }
        else
        {
            // Vérifier si une pièce ordinaire peut capturer
            for (i = 0; i < 2; i++)
            {
                new_x = tmp->x + indices[game.tour_Joueur][i][0];
                new_y = tmp->y - indices[game.tour_Joueur][i][1];
                //Verifier si ce mouvement est une capture ou non
                is_capturing = is_capturing_move(tmp->x, tmp->y, new_x, new_y, game);
                //Si la capture est valide 
                if (is_capturing == 1 && is_valid_input(tmp->x, tmp->y, new_x, new_y, is_capturing, game) == 1)
                {
                    nf = tmp;
                    return (Liste_piece*)nf;
                }
            }
        }
        tmp = tmp->svt;
    }

    return (Liste_piece*)nf; //Retourne la liste
}

Liste_piece* supprimer_val(Liste_piece* Maliste, int x, int y)
{
    Liste_piece* ptcrt, * preced, * tmp;
    //la liste n'existe pas  on sort
    if (!Maliste)	exit(0);
    //la valeur est le début de la liste en supprimer le début
    if (Maliste->x == x && Maliste->y == y)
    {
        Maliste = Maliste->svt;
        return ((Liste_piece*)Maliste);
    }
    //les pointeurs de parcours de la liste
    preced = Maliste;
    ptcrt = Maliste->svt;
    //tant qu'il des éléments dans la liste
    while (ptcrt != NULL)
    {
        //on n'a trouver valeur à supprimer
        if (ptcrt->x == x && ptcrt->y == y)
        {
            //on supprime la cellule
            tmp = preced->svt;
            preced->svt = ptcrt->svt;
            free(tmp);
            return ((Liste_piece*)Maliste);
        }
        ptcrt = ptcrt->svt;
        preced = preced->svt;
    }
    return ((Liste_piece*)Maliste);
}

GtkWidget* create_resized_image(const gchar* filename, gint width, gint height);

/*
   Description : Promouvoir une pièce ordinaire en une pièce dame 
                lorsqu'elle atteint la dernière rangée du plateau.
   Entrée : x, y - Les coordonnées de la pièce à promouvoir.
            liste - La liste des pièces du joueur en cours.
   Sortie : Un pointeur vers la pièce promue, ou NULL si la pièce n'a pas été trouvée.
*/
Liste_piece* promote_to_king(int x, int y, Liste_piece* liste)
{
    Liste_piece* tmp = liste;

    while (tmp)
    {
        if (tmp->x == x && tmp->y == y)
        {
            GtkWidget* button = gtk_grid_get_child_at(gridd->grid, y, x);

            //Promouvoir la pièce en pièce dame selon le joueur en cours
            if (New_game.tour_Joueur == 0)
            {
                New_game.board[x][y] = PLAYER1_KING;
                gtk_button_set_image(GTK_BUTTON(button), 
                    create_resized_image("piece4.png", 50, 50));
                /*gtk_button_set_image(GTK_BUTTON(button),
                create_resized_image(New_game.imagePaths[4], 50, 50));*/
                
            }
            else

            {
                New_game.board[x][y] = PLAYER2_KING;
                gtk_button_set_image(GTK_BUTTON(button), 
                    create_resized_image("piece3.png", 50, 50));
                /*gtk_button_set_image(GTK_BUTTON(button),
                create_resized_image(New_game.imagePaths[3], 50, 50));*/
                
            }

            tmp->is_dame = 1; // Marquer la pièce comme une pièce dame (ou roi)
            break;
        }
        tmp = tmp->svt;
    }
    // Renvoyer un pointeur vers la pièce promue (ou NULL si la pièce n'a pas été trouvée)
    return (Liste_piece*)tmp; 
}


int is_dame(int x, int y, Board_Jeu game)
{

    // Si la pièce appartient au joueur 1 et se trouve sur la dernière rangée, elle est une dame
    //ou  Si la pièce appartient au joueur 2 et se trouve sur la première rangée, elle est une dame
    if (game.board[x][y] == PLAYER1 && x == BOARD_SIZE - 1 || game.board[x][y] == PLAYER2 && x == 0)
    {
        return 1;
    }
    // Si la pièce ne remplit aucune des conditions ci-dessus, elle n'est pas une dame
    // Si la pièce n'existe pas ou n'appartient à aucun joueur, elle n'est pas une dame
    return 0;
}

int taille_liste(Liste_piece* Maliste)
{
    Liste_piece* tmp = Maliste;
    int compteur = 0;
    while (tmp)
    {
        compteur++;
        tmp = tmp->svt;
    }
    return ((int)compteur);
}


void switch_tour(Board_Jeu* game)
{
    game->tour_Joueur = 1 - game->tour_Joueur;
}

void insere_tab(tableau* tab, int x, int y)
{
    int i;
    couple coupleMove;
    coupleMove.x = x;
    coupleMove.y = y;

    if (tab->nbreElem == -1)
    {
        tab->nbreElem++;
        tab->tableau[tab->nbreElem] = coupleMove;
    }
    else
    {
        for (i = 0; i <= tab->nbreElem; i++)
        {
            if (tab->tableau[i].x == x && tab->tableau[i].y == y) return;
        }
        tab->nbreElem++;
        tab->tableau[tab->nbreElem] = coupleMove;
    }

}

void swap_button_labels();

void init_Jeu2(GtkWidget* grid, Board_Jeu Jeu);

void close_dialog(GtkWidget* widget, gpointer data) {
    gtk_widget_destroy(GTK_WIDGET(data));
}

// Fonction pour afficher une boîte de dialogue de fin de jeu de dames
void show_game_over_dialog(int winner) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_NONE,
        "Le jeu de dames est terminé.\nLe vainqueur est : %d",
        winner);

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Fermer", GTK_RESPONSE_CLOSE);
    g_signal_connect(dialog, "response", G_CALLBACK(close_dialog), dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void make_move(int posx, int posy, int new_x, int new_y, Board_Jeu* game)
{

    int adversaire = 1 - game->tour_Joueur;
    int c = game->tour_Joueur;
    int tst_dame = 0;

    Liste_piece* tmp = NULL;
    swap_button_labels();
    game->board[posx][posy] = EMPTY;


    Liste_piece* liste_tmp = game->Players[game->tour_Joueur]->Mes_pieces;
    while (liste_tmp)
    {
        if (liste_tmp->x == posx && liste_tmp->y == posy)
        {
            tst_dame = liste_tmp->is_dame;
            break;
        }
        liste_tmp = liste_tmp->svt;
    }


    //supprimer
    game->Players[game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[game->tour_Joueur]->Mes_pieces, posx, posy);

    //ajouter
    Liste_piece* pion = cree_piece(new_x, new_y);
    pion->is_dame = tst_dame;
    pion->svt = game->Players[game->tour_Joueur]->Mes_pieces;
    game->Players[game->tour_Joueur]->Mes_pieces = pion;

    if (tst_dame == 1)
    {
        int deltaY = (new_x > posx) ? 1 : -1;
        int deltaX = (new_y > posy) ? 1 : -1;

        int tmp_x = posx + deltaY;
        int tmp_y = posy + deltaX;

        while (tmp_x != new_x && tmp_y != new_y) {
            // Comparer la couleur du pion sur la case intermédiaire avec celle du joueur en cours
            game->board[tmp_x][tmp_y] = EMPTY;
            game->Players[1 - game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[1 - game->tour_Joueur]->Mes_pieces, tmp_x, tmp_y);
            GtkWidget* btn2 = gtk_grid_get_child_at(gridd->grid, tmp_y, tmp_x);
            gtk_button_set_image(GTK_BUTTON(btn2), create_resized_image(NULL, 50, 50));
            tmp_x += deltaY;
            tmp_y += deltaX;
        }

    }
    else
    {
        // Si c'est une prise, retirer la pièce prise
        if (abs(new_x - posx) == 2)
        {
            int x_mid = (posx + new_x) / 2;
            int y_mid = (posy + new_y) / 2;
            game->board[x_mid][y_mid] = EMPTY;
            game->Players[1 - c]->Mes_pieces = supprimer_val(game->Players[1 - c]->Mes_pieces, x_mid, y_mid);
        }

        if (is_dame(new_x, new_y, *game))
        {
            GtkWidget* btn3 = gtk_grid_get_child_at(gridd->grid, posy, posx);
            gtk_button_set_image(GTK_BUTTON(btn3), create_resized_image(NULL, 50, 50));

            game->Players[c]->Mes_pieces = promote_to_king(new_x, new_y, game->Players[c]->Mes_pieces);
        }
    }


    for (i = 0; i < 2; i++)
    {

        if (taille_liste(game->Players[i]->Mes_pieces) == 0)
        {
            game->fini = 1;
            game->vainqueur = 1 - i;
            dialogue_ajouter(window.window);
            //show_game_over_dialog(1 - i);
        }

    }


}

void make_move2(int posx, int posy, int new_x, int new_y, Board_Jeu* game)
{

    int adversaire = 1 - game->tour_Joueur;
    Liste_piece* tmp = NULL;
    int tst_dame = 0;

    Liste_piece* liste_tmp = game->Players[game->tour_Joueur]->Mes_pieces;
    while (liste_tmp)
    {
        if (liste_tmp->x == posx && liste_tmp->y == posy)
        {
            tst_dame = liste_tmp->is_dame;
            break;
        }
        liste_tmp = liste_tmp->svt;
    }


    piece_t tmp_cel = game->board[posx][posy];
    game->board[posx][posy] = EMPTY;
    game->board[new_x][new_y] = tmp_cel;

    if (tst_dame == 1)
    {
        int deltaY = (new_x > posx) ? 1 : -1;
        int deltaX = (new_y > posy) ? 1 : -1;
        //mise  a jour

        game->Players[game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[game->tour_Joueur]->Mes_pieces, posx, posy);
        int tmp_x = posx + deltaY;
        int tmp_y = posy + deltaX;

        while (tmp_x != new_x && tmp_y != new_y) {
            // Comparer la couleur du pion sur la case intermédiaire avec celle du joueur en cours
            game->board[tmp_x][tmp_y] = EMPTY;
            game->Players[1 - game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[1 - game->tour_Joueur]->Mes_pieces, tmp_x, tmp_y);
            tmp_x += deltaY;
            tmp_y += deltaX;
        }

        Liste_piece* pion = cree_piece(new_x, new_y);
        pion->is_dame = tst_dame;
        pion->svt = game->Players[game->tour_Joueur]->Mes_pieces;
        game->Players[game->tour_Joueur]->Mes_pieces = pion;
    }
    else
    {

        // Si c'est une prise, retirer la pièce prise
        if (abs(new_x - posx) == 2)
        {

            int x_mid = (posx + new_x) / 2;
            int y_mid = (posy + new_y) / 2;
            game->board[x_mid][y_mid] = EMPTY;

            //diminue la piece de joueur
            game->Players[1 - game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[1 - game->tour_Joueur]->Mes_pieces, x_mid, y_mid);
            game->Players[game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[game->tour_Joueur]->Mes_pieces, posx, posy);

            Liste_piece* pion = cree_piece(new_x, new_y);
            pion->svt = game->Players[game->tour_Joueur]->Mes_pieces;
            game->Players[game->tour_Joueur]->Mes_pieces = pion;
        }
        else
        {
            game->Players[game->tour_Joueur]->Mes_pieces = supprimer_val(game->Players[game->tour_Joueur]->Mes_pieces, posx, posy);
            Liste_piece* pion = cree_piece(new_x, new_y);
            pion->svt = game->Players[game->tour_Joueur]->Mes_pieces;
            game->Players[game->tour_Joueur]->Mes_pieces = pion;
        }

    }
}

void afficher_tab(tableau* tab)
{
    int i;
    printf("\n couples : \n");
    for (i = 0; i < tab->nbreElem + 1; i++)
    {
        printf(" (%d,%d)\n ", tab->tableau[i].x, tab->tableau[i].y);
    }
}

void copy(piece_t Board[Colonne][ligne], piece_t New_Board[Colonne][ligne])
{
    int i, j;
    for (i = 0; i < ligne; i++)
    {
        for (j = 0; j < Colonne; j++)
        {
            New_Board[i][j] = Board[i][j];
        }
    }
}
List_Pions* CreeCel(int x, int y, int new_x, int new_y)
{
    List_Pions* NE = (List_Pions*)malloc(sizeof(List_Pions));
    NE->svt = NULL;
    NE->from.x = x;
    NE->from.y = y;

    NE->to.x = new_x;
    NE->to.y = new_y;
    NE->Score = 0;
    return ((List_Pions*)NE);
}
List_Pions* ajouter_liste(int x, int y, int new_x, int new_y, List_Pions* Maliste)
{
    List_Pions* NE = CreeCel(x, y, new_x, new_y);
    if (!Maliste) return ((List_Pions*)NE);
    List_Pions* tmp = Maliste;
    while (tmp->svt)
    {
        tmp = tmp->svt;
    }
    tmp->svt = NE;
    return ((List_Pions*)Maliste);
}

void affichage_pions(List_Pions* Maliste)
{
    if (Maliste == NULL) return;

    List_Pions* tmp = Maliste;
    while (tmp->svt)
    {
        printf(" (%d,%d) ->", tmp->from.x, tmp->from.y);
        tmp = tmp->svt;
    }
    printf("(%d,%d) ->(%d,%d) ", tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y);

}


Liste_piece* copierListePiece(Liste_piece* liste, Liste_piece* nouvelleListe)
{
    if (liste == NULL) {
        return NULL;
    }

    Liste_piece* dernier = NULL;

    // Parcourir la liste originale
    Liste_piece* courant = liste;
    while (courant != NULL)
    {
        // Créer une nouvelle pièce
        Liste_piece* cel = (Liste_piece*)malloc(sizeof(Liste_piece));
        cel->x = courant->x;
        cel->y = courant->y;
        cel->is_dame = courant->is_dame;
        cel->svt = NULL;

        // Insérer la nouvelle pièce dans la nouvelle liste
        if (nouvelleListe == NULL) nouvelleListe = cel;
        else {
            cel->svt = nouvelleListe;
            nouvelleListe = cel;
        }
        // Passer à la pièce suivante dans la liste originale
        courant = courant->svt;
    }

    return (Liste_piece*)nouvelleListe;
}

void copy_Jeu(Board_Jeu* game, Board_Jeu* Newgame)
{
    Newgame->fini = game->fini;
    Newgame->Score = game->Score;
    Newgame->tour_Joueur = game->tour_Joueur;

    int i = 0, j;
    for (i = 0; i < 2; i++)
    {
        Joueur* NE = (Joueur*)malloc(sizeof(Joueur));
        if (NE)
        {
            NE->type = i;
            NE->nom = "salma";
            //strcpy(NE->nom, "test");
            NE->Mes_pieces = NULL;
            NE->score = 120;
            Newgame->Players[i] = NE;
        }


    }
    copy(game->board, Newgame->board);
    for (i = 0; i < 2; i++)
    {
        Newgame->Players[i]->Mes_pieces = copierListePiece(game->Players[i]->Mes_pieces, Newgame->Players[i]->Mes_pieces);
    }
}

/******************************************************************************************************************/
List_Pions* supprimer_dernier_elem(List_Pions* liste);
void insere_tabG(List_Pions* Mouvements);
List_Pions* checkCapture(int row, int col, piece_t board[8][8], tableau* tab, int tour, List_Pions* Mouvements, int* valeur);

List_Pions* checkCapturesInDirection2(int row, int col, int dRow, int dCol, piece_t board[8][8], tableau* tab, int tour, List_Pions* Mouvements, int* valeur)
{
    int i, j, f;
    piece_t newB[8][8], tmp;
    int compteur = 0;
    int foundCapture = 0;
    for (i = row + dRow, j = col + dCol; i >= 0 && i < 8 && j >= 0 && j < 8; i += dRow, j += dCol) {
        if (is_opposing_piece(board[i][j], tour) == 0) {
            break;
        }
        else if (is_opposing_piece(board[i][j], tour) == 1)
        {
            // On a trouvé une pièce ennemie à capturer
            foundCapture = 1;
            for (i += dRow, j += dCol; i >= 0 && i < 8 && j >= 0 && j < 8; i += dRow, j += dCol) {

                break;
            }
            // Si on a trouvé une pièce à capturer, on ajoute le mouvement à la liste des mouvements de capture
            if (i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == EMPTY)
            {
                copy(board, newB);
                tmp = newB[row][col];
                newB[row][col] = EMPTY;
                newB[i][j] = tmp;
                newB[i - dRow][j - dCol] = EMPTY;
                insere_tab(tab, row, col);  insere_tab(tab, i, j);
                Mouvements = ajouter_liste(row, col, i, j, Mouvements);
                List_Pions* Moves = NULL;
                List_Pions* tmp = Mouvements;
                while (tmp)
                {
                    Moves = ajouter_liste(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, Moves);
                    tmp = tmp->svt;
                }
                n++;
                *valeur = n;
                Mouvements = checkCapture(i, j, newB, tab, tour, Moves, valeur);
                if (Mouvements != NULL)
                {
                    if (n == *valeur) insere_tabG(Mouvements);
                    Mouvements = supprimer_dernier_elem(Mouvements);
                    n--;
                }
            }
            // On ne peut pas capturer d'autres pièces dans cette direction après avoir capturé une pièce
            break;
        }
    }
    return ((List_Pions*)Mouvements);
}

List_Pions* checkCapture(int row, int col, piece_t board[8][8], tableau* tab, int tour, List_Pions* Mouvements, int* valeur)
{
    int i;
    int final_x, final_y, matrice[2][2];

    if (tour == 1)
    {

        matrice[0][0] = -1;
        matrice[0][1] = -1;
        matrice[1][0] = -1;
        matrice[1][1] = 1;

    }
    else
    {

        matrice[0][0] = 1;
        matrice[0][1] = -1;
        matrice[1][0] = 1;
        matrice[1][1] = 1;

    }

    for (i = 0; i < 2; i++)
    {
        Mouvements = checkCapturesInDirection2(row, col, matrice[i][0], matrice[i][1], board, tab, tour, Mouvements, valeur);
    }
    return ((List_Pions*)Mouvements);

}
List_Pions* supprimer_dernier_elem(List_Pions* liste) {
    if (liste == NULL) {
        // La liste est vide, il n'y a rien à supprimer
        return NULL;
    }

    if (liste->svt == NULL) {
        // Il n'y a qu'un seul élément dans la liste, on le supprime
        free(liste);
        return NULL;
    }
    // Parcourir la liste jusqu'au deuxième dernier élément
    List_Pions* current = liste;
    while (current->svt->svt != NULL) {
        current = current->svt;
    }
    // Supprimer le dernier élément
    free(current->svt);
    current->svt = NULL;

    return liste;
}
int comparer_couple(List_Pions* cp1, List_Pions* cp2)
{
    if ((cp1->from.x == cp2->from.x) && (cp1->from.y == cp2->from.y) && (cp1->to.x == cp2->to.x) && (cp1->to.y == cp2->to.y)) return 1;
    return 0;
}

int comparer_listes(List_Pions* liste1, List_Pions* liste2)
{
    int index = 0;
    int trouver = 0;

    while (liste1 != NULL && liste2 != NULL)
    {
        if (liste1->from.x == liste2->from.x && liste1->from.y == liste2->from.y &&
            liste1->to.x == liste2->to.x && liste1->to.y == liste2->to.y) {
            trouver = 1;
        }
        else
        {
            return 0;
        }

        liste1 = liste1->svt;
        liste2 = liste2->svt;
    }

    if (liste1 != NULL || liste2 != NULL) {
        return 0;
    }
    if (trouver == 1) return 1;

}

int chercherChemin(List_Pions* chercher, int k)
{
    int index;
    int compteur = 0;
    int trouver = 0;
    int s = 0;
    for (index = 0; index < indice; index++)
    {
        List_Pions* tmp2 = chercher;
        List_Pions* tmp3 = NULL;
        int compteur = 0;
        while (compteur < k && tmp2)
        {
            tmp3 = ajouter_liste(tmp2->from.x, tmp2->from.y, tmp2->to.x, tmp2->to.y, tmp3);
            compteur++;
            tmp2 = tmp2->svt;
        }
        if (comparer_listes(tmp3, tabG[index]) == 1)
        {
            return 1;
        }
    }
    return 0;
}
void insere_tabG(List_Pions* Mouvements)
{
    int k = taille1(Mouvements);
    while (k > 0)
    {
        tabG[indice] = NULL;
        List_Pions* tmp2 = Mouvements;
        int r = 0;
        int trouver = chercherChemin(Mouvements, k);
        if (trouver == 0)
        {
            while (tmp2 && r < k)
            {
                tabG[indice] = ajouter_liste(tmp2->from.x, tmp2->from.y, tmp2->to.x, tmp2->to.y, tabG[indice]);
                r++;
                tmp2 = tmp2->svt;
            }
            indice++;
        }
        k--;
    }
}

List_Pions* checkCaptureDame(int row, int col, piece_t board[8][8], tableau* tab, List_Pions* Mouvements, int* valeur);

List_Pions* checkCapturesInDirection(int row, int col, int dRow, int dCol, piece_t board[8][8], tableau* tab, List_Pions* Mouvements, int* valeur)
{
    int i, j;
    piece_t newB[8][8], tmp;
    int found = 0;
    for (i = row + dRow, j = col + dCol; i >= 0 && i < 8 && j >= 0 && j < 8; i += dRow, j += dCol) {

        if (is_opposing_piece(board[i][j], New_game.tour_Joueur) == 0) { // ops ou empty
            // Si on tombe sur une dame ou un roi, on s'arrête
            if (board[i][j] == EMPTY)
            {
                // Si la case est vide, on continue dans cette direction
                continue;
            }
            break;
        }
        else {
            found = 1;
            // On a trouvé une pièce ennemie à capturer
            for (i += dRow, j += dCol; i >= 0 && i < 8 && j >= 0 && j < 8; i += dRow, j += dCol) break;
            // Si on a trouvé une pièce à capturer, on ajoute le mouvement à la liste des mouvements de capture
            if (i >= 0 && i < 8 && j >= 0 && j < 8 && is_opposing_piece(board[i][j], New_game.tour_Joueur) == 0 && board[i][j] == EMPTY)
            {

                copy(board, newB);
                tmp = newB[row][col];
                newB[row][col] = EMPTY;
                newB[i][j] = tmp;
                newB[i - dRow][j - dCol] = EMPTY;
                insere_tab(tab, row, col);
                insere_tab(tab, i, j);
                Mouvements = ajouter_liste(row, col, i, j, Mouvements);
                List_Pions* Moves = NULL;
                List_Pions* tmp = Mouvements;
                while (tmp)
                {
                    Moves = ajouter_liste(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, Moves);
                    tmp = tmp->svt;
                }
                n++;
                *valeur = n;
                Mouvements = checkCaptureDame(i, j, newB, tab, Moves, valeur);
                if (Mouvements != NULL)
                {
                    if (n == *valeur) insere_tabG(Mouvements);
                    Mouvements = supprimer_dernier_elem(Mouvements);
                    n--;
                }
            }
            // On ne peut pas capturer d'autres pièces dans cette direction après avoir capturé une pièce
            break;
        }
    }

    return ((List_Pions*)Mouvements);
}

List_Pions* checkCaptureDame(int row, int col, piece_t board[8][8], tableau* tab, List_Pions* Mouvements, int* valeur)
{
    int i;
    int final_x, final_y;
    int matrice[4][2] = {
            {1,-1},
            {1,1},
            {-1,-1},
            {-1,1},
    };
    for (i = 0; i < 4; i++)
    {
        Mouvements = checkCapturesInDirection(row, col, matrice[i][0], matrice[i][1], board, tab, Mouvements, valeur);

    }
    return ((List_Pions*)Mouvements);
}

int get_indice(tableau tab, int x, int y)
{
    int i;
    for (i = 0; i <= tab.nbreElem; i++)
    {
        if (tab.tableau[i].x == x && tab.tableau[i].y == y) return ((int)i);
    }
    return ((int)-1);
}

void matrice_Adj(List_Pions* liste, tableau tab, int matrice[Max][Max])
{
    int i, j, k;


    for (i = 0; i <= tab.nbreElem; i++)
    {
        for (j = 0; j <= tab.nbreElem; j++)
        {
            matrice[i][j] = 0;
        }
    }

    for (i = 0; i <= tab.nbreElem; i++)
    {
        List_Pions* tmp = liste;
        while (tmp)
        {
            if (tmp->from.x == tab.tableau[i].x && tmp->from.y == tab.tableau[i].y)
            {
                k = get_indice(tab, tmp->to.x, tmp->to.y);
                if (k != -1 && matrice[i][k] == 0) matrice[i][k] = 1;
            }
            tmp = tmp->svt;
        }
    }
}

int taille1(List_Pions* Maliste)
{
    List_Pions* tmp = Maliste;
    int compteur = 0;
    while (tmp)
    {
        compteur++;
        tmp = tmp->svt;
    }
    return ((int)compteur);
}

int taille_piece(Liste_piece* Maliste)
{
    Liste_piece* tmp = Maliste;
    int compteur = 0;
    while (tmp)
    {
        compteur++;
        tmp = tmp->svt;
    }
    return ((int)compteur);
}

void get_val(int i, tableau tab, int* x, int* y)
{
    *x = tab.tableau[i].x;
    *y = tab.tableau[i].y;

}


List_Pions* copierListe(List_Pions* des, List_Pions* source)
{
    if (source == NULL) return NULL;
    List_Pions* tmp = source;
    while (tmp)
    {
        des = ajouter_liste(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, des);
        tmp = tmp->svt;
    }
    return des;
}

Noeud* Noeud_cell(List_Pions* liste)
{
    Noeud* New = (Noeud*)malloc(sizeof(Noeud));
    New->fils = NULL;
    New->frere = NULL;
    New->contenu = NULL;
    New->Score = -200;
    New->contenu = copierListe(New->contenu, liste);
    return New;
}

Noeud* remplir_arbre(List_Pions* liste, Noeud* Arbre)
{
    Noeud* NE = Noeud_cell(liste);

    if (!Arbre) return ((Noeud*)NE);
    if (Arbre->fils == NULL)
    {
        Arbre->fils = NE;
        return ((Noeud*)Arbre);
    }
    Noeud* tmp = Arbre->fils;
    while (tmp->frere)
    {
        tmp = tmp->frere;
    }
    tmp->frere = NE;

    return ((Noeud*)Arbre);
}

void check_non_capture(Liste_piece* pion, Board_Jeu game)
{
    int matrice[4][4];
    int dame = 2;


    if (pion->is_dame == 1)
    {
        matrice[0][0] = -1;
        matrice[0][1] = -1;
        matrice[1][0] = -1;
        matrice[1][1] = 1;

        matrice[2][0] = 1;
        matrice[2][1] = -1;
        matrice[3][0] = 1;
        matrice[3][1] = 1;

        dame = 4;

    }
    else

    {
        if (game.tour_Joueur == 1)
        {
            matrice[0][0] = -1;
            matrice[0][1] = -1;
            matrice[1][0] = -1;
            matrice[1][1] = 1;

        }
        else
        {

            matrice[0][0] = 1;
            matrice[0][1] = -1;
            matrice[1][0] = 1;
            matrice[1][1] = 1;

        }


    }


    int i;
    int newx, newy;
    for (i = 0; i < dame; i++)
    {
        newx = pion->x + matrice[i][0];
        newy = pion->y + matrice[i][1];

        if (game.board[newx][newy] == EMPTY && newx <= 7 && newx >= 0 && newy <= 7 && newy >= 0)
        {
            List_Pions* NE = CreeCel(pion->x, pion->y, newx, newy);
            insere_tabG(NE);
        }
    }
}


Noeud* genererMov2(Board_Jeu game, Noeud* arbre)
{
    List_Pions* Moves = NULL;
    tableau tab;
    Liste_piece* tmp = game.Players[game.tour_Joueur]->Mes_pieces;
    int valeur = 0, compteur = 0, k;
    init_tab(&tab);
    while (tmp)
    {
        if (tmp->is_dame == 0) Moves = checkCapture(tmp->x, tmp->y, game.board, &tab, game.tour_Joueur, Moves, &valeur, 1);
        else  Moves = checkCaptureDame(tmp->x, tmp->y, game.board, &tab, Moves, &valeur, 1);

        check_non_capture(tmp, game);
        tmp = tmp->svt;
    }

    for (k = 0; k < indice; k++)
        arbre = remplir_arbre(tabG[k], arbre);

    indice = 0;

    return ((Noeud*)arbre);
}


Noeud* genererMov(Board_Jeu game, Noeud* arbre)
{
    List_Pions* Moves = NULL;
    tableau tab;
    Liste_piece* tmp = game.Players[game.tour_Joueur]->Mes_pieces;
    int valeur = 0, compteur = 0, k;
    init_tab(&tab);
    while (tmp)
    {
        if (tmp->is_dame == 0) Moves = checkCapture(tmp->x, tmp->y, game.board, &tab, game.tour_Joueur, Moves, &valeur, 0);
        else  Moves = checkCaptureDame(tmp->x, tmp->y, game.board, &tab, Moves, &valeur, 1);
        check_non_capture(tmp, game);
        tmp = tmp->svt;
    }
    for (k = 0; k < indice; k++)    arbre = remplir_arbre(tabG[k], arbre);
    return ((Noeud*)arbre);
}


void printTree(Noeud* root, int niveau)
{
    int espace;
    if (root == NULL)
        return;
    printTree(root->frere, niveau);
    printf("\n");
    for (espace = 0; espace < niveau; espace++) printf("\t");

    affichage_pions(root->contenu); printf("le score : %d ", root->Score);
    printTree(root->fils, niveau + 1);
}

void appliquer_Mouvement(Board_Jeu* JeuSimule, List_Pions* liste)
{
    List_Pions* tmp = liste;


    while (tmp)
    {
        GtkWidget* button = gtk_grid_get_child_at(gridd->grid, tmp->from.y, tmp->from.x);
        GtkWidget* button2 = gtk_grid_get_child_at(gridd->grid, tmp->to.y, tmp->to.x);

        pion1 = malloc(sizeof(Cell));
        pion1->row = tmp->from.x;
        pion1->col = tmp->from.y;
        pion1->button = button;


        pion2 = malloc(sizeof(Cell));
        pion2->row = tmp->to.x;
        pion2->col = tmp->to.y;
        pion2->button = button2;
        make_move(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, JeuSimule);
        tmp = tmp->svt;

        pion1 = NULL;
        pion2 = NULL;
    }
}

void appliquer_Mouvement2(Board_Jeu* JeuSimule, List_Pions* liste)
{
    List_Pions* tmp = liste;
    while (tmp)
    {
        make_move2(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, JeuSimule);
        tmp = tmp->svt;
    }
    printf("\n -----------------------------------\n");
    printf("\n le mouvement appliquer est : \n");
    affichage_pions(liste);
    printf("\n");
    print_board(JeuSimule);
    printf("\n -----------------------------------\n");
}


Noeud* simulation_finale(Noeud* Arbre, Board_Jeu New_game)
{
    Noeud* tmp = Arbre->fils;
    int compteur = 0, i, j;
    int index = 0;
    while (tmp)
    {
        compteur++;
        Board_Jeu JeuSimule;
        copy_Jeu(&New_game, &JeuSimule);
        appliquer_Mouvement2(&JeuSimule, tmp->contenu);
        switch_tour(&JeuSimule);
        indice = 0;
        int taille1 = taille_piece(JeuSimule.Players[1]->Mes_pieces);
        int taille2 = taille_piece(JeuSimule.Players[0]->Mes_pieces);
        int score = taille1 - taille2;
        tmp->Score = score;
        tmp = tmp->frere;
    }
    return ((Noeud*)Arbre);
}

Noeud* simulation(Noeud* Arbre, Board_Jeu New_game)
{
    Noeud* tmp = Arbre->fils;
    int compteur = 0, i, j;
    int index = 0;
    printTree(Arbre, 0);
    while (tmp)
    {
        compteur++;
        //copie le board
        Board_Jeu JeuSimule;
        copy_Jeu(&New_game, &JeuSimule);
        appliquer_Mouvement2(&JeuSimule, tmp->contenu);
        switch_tour(&JeuSimule);
        indice = 0;
        int taille1 = taille_piece(JeuSimule.Players[1]->Mes_pieces);
        int taille2 = taille_piece(JeuSimule.Players[0]->Mes_pieces);
        int score = taille1 - taille2;
        tmp->Score = score;
        tmp = genererMov(JeuSimule, tmp);
        if (depth != 0)
        {
            depth--;
            tmp = simulation(tmp, JeuSimule);
        }
        else tmp = simulation_finale(tmp, JeuSimule);
        tmp = tmp->frere;
    }
    return ((Noeud*)Arbre);
}

int evaluer(Noeud* Arbre)
{
    Noeud* tmp = Arbre;
    int score = tmp->Score;
    while (tmp)
    {
        if (tmp->Score < score)
            score = tmp->Score;

        tmp = tmp->frere;
    }
    return score;
}

void evaluer_arbre(Noeud* Arbre)
{
    evaluer_arbre(Arbre->fils);
    int score = evaluer(Arbre);
}

int minimax(Noeud* node, int maximizingPlayer) 
{
    if (node->fils == NULL) 
    {
        return node->Score;
    }

    if (maximizingPlayer) 
    {
        int maxScore = INT_MIN;
        Noeud* child = node->fils;
        while (child != NULL) 
        {
            int score = minimax(child, 0);
            maxScore = (score > maxScore) ? score : maxScore;
            child = child->frere;
        }
        return maxScore;
    }
    else 
    {
        /*if (New_game.mode == 3)
        {
            int maxScore = INT_MIN;
            Noeud* child = node->fils;
            while (child != NULL)
            {
                int score = minimax(child, 1);
                maxScore = (score > maxScore) ? score : maxScore;
                child = child->frere;
            }
            return maxScore;
        }
        else
        {*/
            int minScore = INT_MAX;
            Noeud* child = node->fils;
            while (child != NULL)
            {
                int score = minimax(child, 1);
                minScore = (score < minScore) ? score : minScore;
                child = child->frere;
            }
            return minScore;
        //}
    }
}

Noeud* findBestMove(Noeud* root, int* bestScore) {
    Noeud* bestMove = NULL;
    *bestScore = INT_MIN;


    Noeud* child = root->fils;
    while (child != NULL) {
        int score = minimax(child, 0);  // Appel à l'algorithme Minimax pour évaluer les scores
        if (score > *bestScore) {
            *bestScore = score;
            bestMove = child;
        }
        child = child->frere;
    }
    return bestMove;
}

void initialiserValeurs(Noeud* node, int niveau) {
    if (node == NULL) {
        return;
    }

    if (node->fils == NULL) {

    }
    else if (niveau % 2 == 0) {
        node->Score = 1000;
    }
    else {
        node->Score = -1000;
    }

    initialiserValeurs(node->fils, niveau + 1);
    initialiserValeurs(node->frere, niveau);
}


GtkWidget* create_resized_image(const gchar* filename, gint width, gint height)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf* resized_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    GtkWidget* image = gtk_image_new_from_pixbuf(resized_pixbuf);
    g_object_unref(pixbuf);
    g_object_unref(resized_pixbuf);
    return image;
}
void colorer_button(GtkWidget* button, char* couleur)
{
    GtkStyleContext* context = gtk_widget_get_style_context(button);
    gtk_style_context_add_class(context, "hello");
    GtkCssProvider* provider = gtk_css_provider_new();
    char* css_data = g_strdup_printf(" .%s { background: %s; }", "hello", couleur);
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void swap_button_labels()
{
    GtkWidget* btn1 = GTK_BUTTON(pion1->button);
    GtkWidget* btn2 = GTK_BUTTON(pion2->button);
    printf("%d swaaap %d\n", New_game.board[pion1->row][pion1->col], New_game.board[pion2->row][pion2->col]);
    printf("%d %d to %d %d\n", pion1->row, pion1->col, pion2->row, pion2->col);
    //char* imagePaths[5] = { NULL, "piece2.png", "piece1.png", "piece4.png", "piece3.png" };
    char* imagePaths[5] = { NULL, "pionjaune.jfif", "pionblue.jfif", "piece4.png", "piece3.png" };
    gtk_button_set_image(GTK_BUTTON(btn2), create_resized_image(imagePaths[New_game.board[pion1->row][pion1->col]], 50, 50));

    if (abs(pion2->row - pion1->row) == 2)
    {
        int x_mid = (pion1->row + pion2->row) / 2;
        int y_mid = (pion1->col + pion2->col) / 2;
        GtkWidget* btn3 = gtk_grid_get_child_at(gridd->grid, y_mid, x_mid);
        gtk_button_set_image(GTK_BUTTON(btn3), create_resized_image(NULL, 50, 50));
    }

    gtk_button_set_image(GTK_BUTTON(btn1), create_resized_image(imagePaths[New_game.board[pion2->row][pion2->col]], 50, 50));
    piece_t c;
    gtk_sleep(5);
    c = New_game.board[pion2->row][pion2->col];
    New_game.board[pion2->row][pion2->col] = New_game.board[pion1->row][pion1->col];
    New_game.board[pion1->row][pion1->col] = c;
}

laliste* Creer_cellule(Cell* pion)
{
    laliste* cl;
    cl = (laliste*)malloc(sizeof(laliste));
    if (!cl)
    {
        printf("Erreur d'allocation !");
        exit(0);
    }
    cl->pion = pion;
    cl->svt = NULL;
    return (laliste*)cl;
}


laliste* inserer_fin_pliste(laliste* liste, Cell* pion)
{
    laliste* crt, * ne;
    ne = Creer_cellule(pion);
    if (!liste)    return (laliste*)ne;
    crt = liste;
    while (crt->svt)     crt = crt->svt;
    crt->svt = ne;
    return ((laliste*)liste);
}

int taille_pliste1(laliste* l1)
{
    laliste* crt;
    int cmpt = 0;
    if (!l1) return((int)0);
    crt = l1;
    while (crt)
    {
        cmpt++;
        crt = crt->svt;
    }
    return ((int)cmpt);
}


void afficher_pliste(laliste* liste)
{
    laliste* tmp;
    if (!liste)
    {
        printf("la liste est vide");
        return;
    }
    tmp = liste;
    while (tmp->svt)
    {
        printf("%d %d->", tmp->pion->row, tmp->pion->col);
        tmp = tmp->svt;
    }
    printf("%d %d\n", tmp->pion->row, tmp->pion->col);
}

laliste* Vider_Pliste(laliste* Maliste);

void chemin_piece(GtkWidget* widget, gpointer data)
{

    Cell* cell = (Cell*)data;
    int x = 0, y = 0, new_x = 0, new_y = 0;
    int is_capturing = 0;
    int compteur = 0;
    laliste* tmp;

    Noeud* Abre = NULL;
    Abre = genererMov2(New_game, Abre);

    if (tst == 0)
    {
        if (pion1 == NULL)
        {
            pion1 = create_new_cell(cell);

            if (is_opposing_piece(New_game.board[pion1->row][pion1->col], New_game.tour_Joueur) == 0 && New_game.board[pion1->row][pion1->col] != EMPTY)
            {
                if (chemin)
                    chemin = Vider_Pliste(chemin);
            }

            chemin = inserer_fin_pliste(chemin, pion1);
            pion1 = NULL;
        }
        else
            pion1 = NULL;

    }
    else tst = 0;
}

laliste* Vider_Pliste(laliste* Maliste)
{
    laliste* ptcourant;//pointeur pour parcourir la liste
    if (!Maliste) exit(0);//si la liste est déjà vide on sort
    while (Maliste) //tant que il y a des éléments dans la liste
    {
        ptcourant = Maliste;//prendre le tète de la liste
        Maliste = Maliste->svt;//passer à la cellule suivante
        free(ptcourant);//vider l'emplacement mémoire de la cellule
    }
    return ((laliste*)Maliste);//retourner la liste vide
}


int tester_chemin(laliste* Maliste, Board_Jeu New_game)
{
    laliste* tmp = Maliste;
    int capture = 0, is_capturing, longeur = 0;

    Board_Jeu JeuSimule;
    copy_Jeu(&New_game, &JeuSimule);
    while (tmp->svt)
    {
        is_capturing = is_capturing_move(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, JeuSimule);
        if (is_valid_input(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, is_capturing, JeuSimule) == 1)
        {
            if (is_capturing == 0) return ((int)0);
            make_move2(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, &JeuSimule);
        }
        tmp = tmp->svt;
    }
    return ((int)1);
}
void updateH(laliste* leChemin);


void joueur()
{
    int compteur = 0;
    int is_capturing = 0;
    int resultat;
    laliste* tmp;
    tmp = chemin;
    if (!tmp) return;//compteur = 1;
    else if (!tmp->svt) return;// compteur = 1;  // un seul elm
        else if (tmp->svt->svt)
        {
            resultat = tester_chemin(tmp, New_game);
            if (resultat == 0)
            {
                tst = 1;
                chemin = Vider_Pliste(chemin);
                if (chemin == NULL) printf("\n liste bien vider j \n");
                return;
            }
        }

    Board_Jeu JeuSimule;
    copy_Jeu(&New_game, &JeuSimule);

    while (tmp->svt)
    {
        is_capturing = is_capturing_move(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, JeuSimule);
        if (is_valid_input(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, is_capturing, JeuSimule) == 0)
        {
            compteur = 1;
            break;
        }
        make_move2(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, &JeuSimule);
        tmp = tmp->svt;
    }

   // updateH(chemin);

    if (compteur == 0)
    {
        updateH(chemin);
        tmp = chemin;
        while (tmp->svt)
        {
            print_board(&New_game);
            pion1 = tmp->pion;
            pion2 = tmp->svt->pion;
            Liste_piece* nf = tester_gain(New_game);
            make_move(tmp->pion->row, tmp->pion->col, tmp->svt->pion->row, tmp->svt->pion->col, &New_game);
            /*if (is_capturing == 0)
            {
                if (nf!=NULL)
                {
                    printf(" \n you did not move the right one %d %d \n",nf->x,nf->y);


                    if( nf->x == tmp->pion->row && nf->y == tmp->pion->col)
                    {
                        New_game.Players[New_game.tour_Joueur]->Mes_pieces = supprimer_val(New_game.Players[New_game.tour_Joueur]->Mes_pieces, tmp->svt->pion->row, tmp->svt->pion->col);
                        afficher_liste(New_game.Players[New_game.tour_Joueur]->Mes_pieces);
                        New_game.board[tmp->svt->pion->row][tmp->svt->pion->col]=EMPTY;
                        GtkWidget* btn3 = gtk_grid_get_child_at(gridd->grid, tmp->svt->pion->col, tmp->svt->pion->row);
                        gtk_button_set_image(GTK_BUTTON(btn3), create_resized_image(NULL, 50, 50));
                    }
                    else
                    {
                        New_game.Players[New_game.tour_Joueur]->Mes_pieces = supprimer_val(New_game.Players[New_game.tour_Joueur]->Mes_pieces, nf->x, nf->y);
                        afficher_liste(New_game.Players[New_game.tour_Joueur]->Mes_pieces);
                        New_game.board[nf->x][nf->y]=EMPTY;
                        GtkWidget* btn3 = gtk_grid_get_child_at(gridd->grid, nf->y, nf->x);
                        gtk_button_set_image(GTK_BUTTON(btn3), create_resized_image(NULL, 50, 50));
                    }


                }
            }*/
            tmp = tmp->svt;
        }

        New_game.Players[New_game.tour_Joueur]->score = taille_liste(New_game.Players[New_game.tour_Joueur]->Mes_pieces);
        New_game.Players[1 - New_game.tour_Joueur]->score = taille_liste(New_game.Players[1 - New_game.tour_Joueur]->Mes_pieces);

        switch_tour(&New_game);
        update();
        tst = 1;
        chemin = Vider_Pliste(chemin);
        afficher_pliste(chemin);
        tmp = NULL;
    }
    else
    {
        chemin = Vider_Pliste(chemin);
        pion1 = NULL;
        tst = 1;
    }
    pion1 = NULL;
    pion2 = NULL;
}


/*
    Description : Ajoute un historique de mouvements à l'interface 
                    graphique.
    Entrée : liste - Pointeur vers une liste de mouvements à afficher
                dans l'historique.
    Sortie : aucun (affiche l'historique dans l'interface graphique)
*/
void ajouter_historique(List_Pions* liste)
{
    // Chaîne de caractères pour stocker le contenu de l'historique
    char content[1024] = ""; 
    // Pointeur temporaire pour parcourir la liste de mouvements
    List_Pions* tmp = liste; 

    // Construction de l'en-tête du mouvement avec le nom du joueur
    char move1[256];
    snprintf(move1, sizeof(move1), "Joueur : %s  : ", 
        New_game.Players[New_game.tour_Joueur]->nom);
    strcat(content, move1);

    /*Parcours de la liste de mouvements pour ajouter 
            chaque mouvement à l'historique*/
    while (tmp)
    {
        char move_text[256];
        //Afficher la source et la destination de chaque mouvement valide
        //snprintf(move_text, sizeof(move_text), "(%d, %d) - (%d, %d) ",
          //  tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y);
        int is_capturing;
        is_capturing = is_capturing_move(tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y, New_game);
        if (is_capturing == 1)
        {
            tmp->Score = 2;
        }
        else if (is_capturing == 0)
        {
            tmp->Score = 1;
        }
        else tmp->Score = 0;
       // if(abs(tmp->to.x - tmp->from.x) != abs(tmp->to.y - tmp->from.y))
        snprintf(move_text, sizeof(move_text), "(%d, %d) - (%d, %d) : (%d)",
            tmp->from.x, tmp->from.y, tmp->to.x, tmp->to.y,tmp->Score);
        /* jouter progressivement chaque mouvement à la chaîne content afin
             de former l'historique complet des mouvements */
        strcat(content, move_text);
        //Passer au mouvement suivant
        tmp = tmp->svt;
    }

    // Création d'une étiquette pour afficher le contenu de l'historique
    GtkWidget* move_label = gtk_label_new(content);

    // Ajout de l'étiquette à la boîte d'affichage
    gtk_box_pack_start(GTK_BOX(inner_vbox), move_label, FALSE, FALSE, 0);

    ButtonItem* button_pause = init_button();
    button_pause->label = "Pause";
    button_pause->couleur = "red";
    button_pause = cree_Button(button_pause);
    gtk_box_pack_end(GTK_BOX(inner_vbox), button_pause, TRUE, TRUE, 1);
    affecter_signal_button(button_pause, NULL, pauser_jeu);
   
    
    //ajouter_content(&window1, button_start->widget, 300, 550);
    // Affichage de tous les widgets contenus dans la boîte d'affichage
    gtk_widget_show_all(inner_vbox);
}


void updateH(laliste* leChemin)
{
    char content2[1024] = "";
    laliste* tmp = leChemin;
    int test;
    char move22[256];
    snprintf(move22, sizeof(move22), "Joueur : %s  :  ",
        New_game.Players[New_game.tour_Joueur]->nom);
    strcat(content2, move22);

    while (tmp)
    {
        char move_text[256];
        snprintf(move_text, sizeof(move_text), "(%d, %d) - ",
            tmp->pion->row, tmp->pion->col);
        strcat(content2, move_text);
        tmp = tmp->svt;
    }
 
   
    int is_capturing;
    is_capturing = is_capturing_move(leChemin->pion->row, leChemin->pion->col, leChemin->svt->pion->row, leChemin->svt->pion->col, New_game);
    if (is_capturing == 1)
    {
        test = 2;
    }
    else if (is_capturing == 0)
    {
        test = 1;
    }
    else test = 0;
    char move_text[256];
    snprintf(move_text, sizeof(move_text), ": (%d)", test);


    GtkWidget* move_label = gtk_label_new(content2);

    gtk_box_pack_start(GTK_BOX(inner_vbox), move_label, FALSE, FALSE, 0);

    gtk_widget_show_all(inner_vbox);
}

void ordi()
{
    gtk_sleep(20);
    Noeud* racine = NULL, * Arbre = NULL;
    List_Pions* Mouvements = NULL, * liste1 = NULL;
    int valeur = 0;
    liste1 = ajouter_liste(0, 0, 0, 0, liste1);
    Arbre = remplir_arbre(liste1, Arbre);
    Arbre = genererMov(New_game, Arbre);
    /*if (Arbre == NULL)
    {
        New_game.Players[New_game.tour_Joueur]->fini = 1;
        fini_Jeu();
    }*/
    printf("\n indice : %d \n", indice);
    if (indice == 0)
    {
        printf("arbre fin");
        is_capture = 0;
        New_game.Players[New_game.tour_Joueur]->fini = 1;
        fini_Jeu();
        return;
    }
    Arbre = simulation(Arbre, New_game);

    //int valeur5 = minimax(Arbre,1);
    initialiserValeurs(Arbre, 0);

    int score_best;
    Noeud* Best = findBestMove(Arbre, &score_best);
    if (Best != NULL) {
        appliquer_Mouvement(&New_game, Best->contenu);
        ajouter_historique(Best->contenu);
        Best = NULL;
    }
    print_board(&New_game);
    New_game.Players[New_game.tour_Joueur]->score = taille_liste(New_game.Players[New_game.tour_Joueur]->Mes_pieces);
    New_game.Players[1 - New_game.tour_Joueur]->score = taille_liste(New_game.Players[1 - New_game.tour_Joueur]->Mes_pieces);
    switch_tour(&New_game);
    update();
}

gboolean ordin(GtkWidget* widget, GdkEventKey* event)
{
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
    {
        ordi();
    }
}


gboolean joueur_j(GtkWidget* widget, GdkEventKey* event)
{
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
    {
        joueur();
    }

    return FALSE;
}

gboolean joueur_ordi(GtkWidget* widget, GdkEventKey* event)
{

    Board_Jeu JeuSimule;
    copy_Jeu(&New_game, &JeuSimule);

    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
    {
        /*if (New_game.tour_Joueur == 0)
        {
            joueur();
        }
        if (New_game.tour_Joueur == 1)
        {
            ordi();
        }*/
        if (tour == 0)
        {
            if (New_game.tour_Joueur == 0)
            {
                joueur();

            }
            if (New_game.tour_Joueur == 1)
            {
                ordi();
            }
        }
        else if(tour == 1)
        {
            if (New_game.tour_Joueur == 0)
            {
                joueur();
            }
            if (New_game.tour_Joueur == 1)
            {
                ordi();
            }
        }


    }
    return FALSE;
}

void init_Matrice()
{
    int i, j;

    for (i = 0; i < Colonne; i++)
    {
        for (j = 0; j < ligne; j++) {

            if ((i + j) % 2 == 1) {
                if (i < 3) {
                    New_game.board[i][j] = PLAYER1;
                }
                else if (i > 4) {
                    New_game.board[i][j] = PLAYER2;
                }
                else {
                    New_game.board[i][j] = EMPTY;
                }
            }
            else {
                New_game.board[i][j] = EMPTY;
            }
        }
    }
}

void init_Jeu2(GtkWidget* grid, Board_Jeu Jeu)
{
    char* Labels[5] = { "Vide", "Blanc", "Noir", "King1", "King2" };
    //char* imagePaths[5] = { NULL, "piece2.png", "piece1.png", "piece4.png", "piece3.png" };
    char* imagePaths[5] = { NULL, "pionjaune.jfif", "pionblue.jfif", "piece4.png", "piece3.png" };
    
        
    char* couleurs[5] = { "white","black" };
    int i, j, valeur;
    GtkWidget* button;
    //init_Matrice();

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            int indice = Jeu.board[i][j];
            Cell* cell = malloc(sizeof(Cell));
            button = gtk_button_new();
            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
            cell->row = i;
            cell->col = j;
            cell->button = button;
            g_signal_connect(button, "clicked", G_CALLBACK(chemin_piece), cell);
            gtk_button_set_image(GTK_BUTTON(button), create_resized_image(imagePaths[indice], 50, 50));
            GtkStyleContext* context = gtk_widget_get_style_context(button);
            GtkCssProvider* provider = gtk_css_provider_new();
            char* css_data;
            if ((i + j) % 2 == 0)
            {
                gtk_style_context_add_class(context, "red");
                css_data = g_strdup_printf(" .%s {background: %s; }", "red", "red");
            }
            else
            {
                gtk_style_context_add_class(context, "green");
                css_data = g_strdup_printf(" .%s { background: %s; }", "green", "green");
            }
            gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
            gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

        }
        printf("\n");
    }
}
