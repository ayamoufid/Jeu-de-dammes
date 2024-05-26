#pragma once
#include "fenetre.h"

Grid_object* init_grid_default()
{
    Grid_object* grid = g_new(Grid_object, 1);
    grid->nbr_ligne = 3;
    grid->nbr_colonne = 3;
    grid->homogene = TRUE;
    grid->commune.posx = 0;
    grid->commune.posy = 0;
    if (!grid)
    {
        printf("\nErreur d'allocation memoire");
        exit(1);
    }
    return grid;
}

Grid_object* create_grid(Grid_object* grid)
{
    grid->grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid->grid), grid->homogene);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid->grid), grid->homogene);
    return grid;
}

void add_widget_to_grid(Grid_object* grid, GtkWidget* widget, gint left, gint right, gint top, gint bottom)
{
    gtk_grid_attach(GTK_GRID(grid->grid), widget, left, top, right - left + 1, bottom - top + 1);
}

void ajouter_conteneur(GtkWidget* c1, GtkWidget* c2)
{
    gtk_container_add(GTK_CONTAINER(c1), c2);
}


void vider_grid(GtkWidget* grid)
{
    GList* children, * iter;

    children = gtk_container_get_children(GTK_CONTAINER(grid));
    iter = children;

    while (iter != NULL)
    {
        GtkWidget* child = GTK_WIDGET(iter->data);
        gtk_widget_destroy(child);
        iter = g_list_next(iter);
    }

    g_list_free(children);
}



typedef struct cellule_window
{
    fenetre var;
    struct cellule_window* suivant;
}cellule_window;

cellule_window* ajouter_window(cellule_window* liste, fenetre wn)
{
    cellule_window* NE = g_new(cellule_window, 1);
    NE->var = wn;
    NE->suivant = NULL;


    if (!liste) return (cellule_window*)NE;
    cellule_window* tmp = liste;
    while (tmp->suivant)
        tmp = tmp->suivant;
    //ajout
    tmp->suivant = NE;

    return (cellule_window*)liste;
}

gboolean on_key_press(GtkWidget* widget, GdkEventKey* event);
fenetre initialiser_valeurs_fenetre();
void ajouter_fenetre(fenetre* var, int t);
void print_board(Board_Jeu* game);
void init_Jeu(Board_Jeu* game);
void init_Jeu2(GtkWidget* grid, Board_Jeu Jeu);
void clearBox(GtkWidget* box);
void on_dialog_response(GtkDialog* dialog, gint response_id, gpointer user_data)
{
    GtkWidget* window_anc = GTK_WIDGET(user_data);


    fenetre* tmp = &window;
    //gtk_widget_destroy(tmp->window);
    if (response_id == GTK_RESPONSE_YES) {

        /*char* name1 = NULL;
        strcpy(name1, New_game.Players[0]->nom);

        char* name2 = NULL;
        strcpy(name2,New_game.Players[1]->nom);*/

        int mode = New_game.mode;
        clearBox(inner_vbox);
        init_Jeu(&New_game);
        print_board(&New_game);
        vider_grid(gridd->grid);
        tst = 0;
        New_game.mode = mode;


        New_game.Players[0]->nom = entryValues.value1; New_game.Players[1]->nom = entryValues.value2;
        /*if (name1 && name2)
        {
            strcpy(New_game.Players[0]->nom, name1);
            strcpy(New_game.Players[1]->nom, name2);
        }*/
        update();
        pion1 = 0;
        init_Jeu2(gridd->grid, New_game);
        afficher_fenetre(&window);
    }

    // Fermer la boîte de dialogue
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

//--------------------------------------------
void dialogue_ajouter(GtkWidget* window)
{

    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* image;
    GtkWidget* hbox; // Nouveau widget GtkBox

    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        0,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_NONE,
        "");


    gtk_window_move(GTK_WINDOW(dialog), 50, 50);

    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Recommencer", GTK_RESPONSE_YES);
    // gtk_dialog_add_button(GTK_DIALOG(dialog), "Non", GTK_RESPONSE_NO);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Annuler", GTK_RESPONSE_CANCEL);


    g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_response), window);

    if (New_game.fini == 2)
    {
        GtkWidget* label = gtk_label_new(g_strdup_printf("Le jeu est fini avec egalite Joueur 1 : %s Score %d - Joueur 2 : %s Score %d", New_game.Players[0]->nom, New_game.Players[0]->score, New_game.Players[1]->nom, New_game.Players[1]->score));
    }
    GtkWidget* label = gtk_label_new(g_strdup_printf("Le joueur %s a gagne avec un score de %d", New_game.Players[New_game.vainqueur]->nom, New_game.Players[New_game.vainqueur]->score));
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    // gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
    // Ajout de la boîte horizontale à la boîte de dialogue
    content_area = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(dialog));
    gtk_box_pack_start(GTK_BOX(content_area), hbox, FALSE, FALSE, 0);
    gtk_widget_show_all(dialog);
}





