
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Fonction_computer.h"


GtkWidget* createHistoryBar()
{
    // Création d'une boîte de rangement verticale
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    gtk_widget_set_size_request(vbox, 500, 500);

    // Ajout d'un titre pour l'historique
    GtkWidget* label = gtk_label_new("Historique de jeu");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    // Ajout d'un scrollable pour l'historique
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Ajout d'une boîte de rangement verticale à l'intérieur du scrollable
    inner_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    gtk_widget_set_size_request(inner_vbox, 500, 500);

    gtk_container_add(GTK_CONTAINER(scrolled_window), inner_vbox);

    return vbox;
}


void toggle_entry_visibility(GtkWidget* widget, gpointer data)
{
    GSList* radio_buttons = gtk_radio_button_get_group(GTK_RADIO_BUTTON(widget));
    while (radio_buttons != NULL)
    {
        GtkWidget* radio_button = GTK_WIDGET(radio_buttons->data);
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button)))
        {
            const gchar* label = gtk_button_get_label(GTK_BUTTON(radio_button));
            if (g_strcmp0(label, "Joueur contre joueur") == 0)
            {
                gtk_widget_set_visible(player1_name_entry->entry, TRUE);
                gtk_widget_set_visible(player2_name_entry->entry, TRUE);
                radioButton_Visibility(GTK_BUTTON(data), FALSE);
            }
            else if (g_strcmp0(label, "Joueur contre ordinateur") == 0)
            {
                gtk_widget_set_visible(player1_name_entry->entry, TRUE);
                gtk_widget_set_visible(player2_name_entry->entry, FALSE);
                radioButton_Visibility(GTK_BUTTON(data), TRUE);
            }
            else
            {
                gtk_widget_set_visible(player1_name_entry->entry, FALSE);
                gtk_widget_set_visible(player2_name_entry->entry, FALSE);
                radioButton_Visibility(GTK_BUTTON(data), TRUE);
            }

        }
        radio_buttons = g_slist_next(radio_buttons);
    }

}

// Définir la fonction de rappel pour le clic sur le bouton "Commencer"
void on_start_button_clicked(GtkWidget* button, gpointer data)
{
    init_Jeu(&New_game);
    gchar* text1 = NULL, * text2 = NULL;
    ToogleButton* toggle_button = (ToogleButton*)data;

    GSList* radio_buttons = gtk_radio_button_get_group(GTK_RADIO_BUTTON(toggle_button->widget));

    while (radio_buttons != NULL)
    {
        GtkWidget* radio_button = GTK_WIDGET(radio_buttons->data);
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button)))
        {
            const gchar* label = gtk_button_get_label(GTK_BUTTON(radio_button));
            if (g_strcmp0(label, "Joueur contre joueur") == 0)
            {
                g_free(text1);
                g_free(text2);
                text1 = g_strdup(gtk_entry_get_text(GTK_ENTRY(player1_name_entry->entry)));
                text2 = g_strdup(gtk_entry_get_text(GTK_ENTRY(player2_name_entry->entry)));

                g_free(entryValues.value1);
                g_free(entryValues.value2);
                entryValues.value1 = text1;
                entryValues.value2 = text2;

                New_game.mode = 1;
            }
            else if (g_strcmp0(label, "Joueur contre ordinateur") == 0)
            {

                g_free(text1);
                g_free(text2);
                text1 = g_strdup(gtk_entry_get_text(GTK_ENTRY(player1_name_entry->entry)));

                g_free(entryValues.value1);
                g_free(entryValues.value2);
                entryValues.value1 = text1;
                entryValues.value2 = "ordinateur";

                New_game.mode = 2;
            }
            else
            {
                //g_print("Vous allez jouer contre l'ordinateur en tant que ordinateur.\n");
                g_free(text1);
                g_free(text2);
                g_free(entryValues.value1);
                g_free(entryValues.value2);
                entryValues.value1 = "ordinateur 1";
                entryValues.value2 = "ordinateur 2";

                New_game.mode = 3;

            }

        }
        radio_buttons = g_slist_next(radio_buttons);
    }


    // Masquer la fenêtre d'accueil
    GtkWidget* window2 = window1.window;
    gtk_widget_hide(window2);

    window = initialiser_valeurs_fenetre();
    window.taille.width = 1;
    window.taille.height = 1000;
    ajouter_fenetre(&window, 1);

    g_signal_connect(window.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    if (New_game.mode == 1)g_signal_connect(window.window, "key-press-event", G_CALLBACK(joueur_j), NULL);
    else if (New_game.mode == 2)g_signal_connect(window.window, "key-press-event", G_CALLBACK(joueur_ordi), NULL);
    else g_signal_connect(window.window, "key-press-event", G_CALLBACK(ordin), NULL);
    gridd = init_grid_default();
    gridd = create_grid(gridd);
    GtkWidget* box = createHistoryBar();
    gtk_widget_set_size_request(gridd->grid, 800, 800);
    ajouter_content(&window, gridd->grid, 0, 0);
    ajouter_content(&window, box, 800, 0);


    New_game.Players[0]->nom = entryValues.value1; New_game.Players[1]->nom = entryValues.value2;

    printf("\n players : %s , %s \n", New_game.Players[0]->nom, New_game.Players[1]->nom);

    print_board(&New_game);
    printf("\n tour : %d \n", New_game.tour_Joueur);
    printf("\n liste player 0 \n ");  afficher_liste(New_game.Players[0]->Mes_pieces);
    printf("\n liste player 1 \n ");  afficher_liste(New_game.Players[1]->Mes_pieces);
    vider_grid(gridd->grid);
    init_Jeu2(gridd->grid, New_game);

    header* h = create_header(&window);
    Format frm = init_format();
    Format frm2 = init_format();

    strcpy(frm.color, "green");
    strcpy(frm2.color, "red");


    //label 1
    tab[0] = initLabelObject();
    tab[0]->lab = frm;
    tab[0]->position.posx = tab[0]->position.posy = 0;
    tab[0]->size.width = 200;
    tab[0]->text = "salma";
    tab[0]->position.container = h->fixed_header;

    tab[0] = createLabelObject(tab[0]);

    tab[1] = initLabelObject();
    tab[1]->lab = frm2;
    tab[1]->position.posx = 0;
    tab[1]->position.posy = 50;
    tab[1]->size.width = 200;
    tab[1]->text = "aya";
    tab[1]->position.container = h->fixed_header;
    tab[1] = createLabelObject(tab[1]);


    update();

    Noeud* bestMove = NULL;

    gtk_widget_show_all(window.window);


}


void changerTour(Board_Jeu* game, int tour)
{
    game->tour_Joueur = tour;
}
//NE TRAVAILLE PAS (PRINCESS)
void onRadioButtonToggled(GtkToggleButton* toggleButton, gpointer data)
{

    gboolean isActive = gtk_toggle_button_get_active(toggleButton);
    printf("---------------------------------------%d\n", New_game.tour_Joueur);
    if (isActive)
    {
        const gchar* label = gtk_button_get_label(GTK_BUTTON(toggleButton));
        if (g_strcmp0(label, "Blanc") == 0)
        {
            New_game.tour_Joueur = 0;
            
        }
        else
        {
            New_game.tour_Joueur = 1;
        }
        printf("---------------------------------------%d\n", New_game.tour_Joueur);
    }
}

int main(int argc, char* argv[])
{
    // Initialiser GTK
    gtk_init(&argc, &argv);

    cellule_window* windows = NULL;

    ToogleButton* radio_player_vs_computer = init_radio_button();
    ToogleButton* radio_computer_vs_computer = init_radio_button();
    ToogleButton* radio_player_vs_player = init_radio_button();

    New_game.tour_Joueur = 1;

    window1 = initialiser_valeurs_fenetre();
    window1.taille.width = 1000;
    window1.taille.height = 1000;
    ajouter_fenetre(&window1, 1);
    windows = ajouter_window(windows, window1);

    //GtkWidget* welcome_label = gtk_label_new("Bienvenue dans le jeu de dames !");
    //gtk_label_set_justify(GTK_LABEL(welcome_label), GTK_JUSTIFY_CENTER);
    ////gtk_box_pack_start(GTK_BOX(vbox), welcome_label, TRUE, TRUE, 0);
    //ajouter_content(&window1, welcome_label, 300, 10);
    label_object* welcome_label = initLabelObject();
    welcome_label->lab = init_format();
    strcpy(welcome_label->lab.color, "red");

    //welcome_label->lab.alignement = 2;
    welcome_label->position.container = window1.fixed;
    welcome_label->position.posx = 200;
    welcome_label->position.posy = 0;
    welcome_label->text = "Bienvenue dans le jeu de dames !";
    welcome_label = createLabelObject(welcome_label);

    ButtonItem* button_start = init_button();
    button_start->label = "Demarrer la partie";
    button_start->couleur = "beige";
    button_start = cree_Button(button_start);
    ajouter_content(&window1, button_start->widget, 300, 550);


    g_signal_connect(button_start->widget, "clicked", G_CALLBACK(on_start_button_clicked), radio_player_vs_computer);

    affecter_signal_Quit(window1, gtk_main_quit);

    Frame_object* frame = init_frame_default();
    strcpy(frame->label, "Choisir Le mode ");
    frame->commune.posx = 200;
    frame->commune.posy = 30;
    frame->taille.width = 400;
    frame->taille.height = 100;
    frame = create_frame(frame);

    Frame_object* frameN = init_frame_default();
    strcpy(frameN->label, "Choisir Le niveau ");
    frameN->commune.posx = 200;
    frameN->commune.posy = 180;
    frameN->taille.width = 400;
    frameN->taille.height = 50;
    frameN = create_frame(frameN);

    Frame_object* frameM = init_frame_default();
    strcpy(frameM->label, "Choisir Les Noms  ");
    frameM->commune.posx = 200;
    frameM->commune.posy = 350;
    frameM->taille.width = 400;
    frameM->taille.height = 100;
    frameM = create_frame(frameM);

    

    ajouter_content(&window1, frame->frame, frame->commune.posx, frame->commune.posy);
    ajouter_content(&window1, frameN->frame, frameN->commune.posx, frameN->commune.posy);
    ajouter_content(&window1, frameM->frame, frameM->commune.posx, frameM->commune.posy);


    radio_player_vs_computer->label = "Joueur contre ordinateur";

    radio_player_vs_computer->ButtonPere = NULL;
    radio_player_vs_computer = cree_radio_button(radio_player_vs_computer);
    ajouter_frame(frame, radio_player_vs_computer->widget, 0, 0);

    radio_computer_vs_computer->label = "Ordinateur contre ordinateur";
    radio_computer_vs_computer->ButtonPere = radio_player_vs_computer->widget;
    radio_computer_vs_computer = cree_radio_button(radio_computer_vs_computer);
    ajouter_frame(frame, radio_computer_vs_computer->widget, 0, 20);

    radio_player_vs_player->label = "Joueur contre joueur";
    radio_player_vs_player->ButtonPere = radio_player_vs_computer->widget;
    radio_player_vs_player->active = TRUE;
    radio_player_vs_player = cree_radio_button(radio_player_vs_player);
    ajouter_frame(frame, radio_player_vs_player->widget, 0, 40);

    ToogleButton* facile = init_radio_button();
    ToogleButton* moyenne = init_radio_button();
    ToogleButton* difficile = init_radio_button();

    facile->label = "facile";
    facile->ButtonPere = NULL;
    facile = cree_radio_button(facile);

    ajouter_frame(frameN, facile->widget, 0, 0);


    moyenne->label = "moyenne";
    moyenne->ButtonPere = facile->widget;
    moyenne = cree_radio_button(moyenne);
    ajouter_frame(frameN, moyenne->widget, 80, 0);

    difficile->label = "difficile";
    difficile->ButtonPere = facile->widget;
    difficile = cree_radio_button(difficile);
    ajouter_frame(frameN, difficile->widget, 180, 0);

    

    player1_name_entry = zonetxt_init();
    player2_name_entry = zonetxt_init();


    strcpy(player1_name_entry->placeholder_text, "Nom du joueur 1");
    player1_name_entry->width = 300;

    strcpy(player2_name_entry->placeholder_text, "Nom du joueur 2");
    player2_name_entry->width = 300;

    player1_name_entry = zonetxt_create(player1_name_entry);
    player2_name_entry = zonetxt_create(player2_name_entry);

    if (player1_name_entry != NULL) ajouter_frame(frameM, player1_name_entry->entry, 0, 0);
    if (player2_name_entry != NULL) ajouter_frame(frameM, player2_name_entry->entry, 0, 40);

    affecter_signal_toggle(radio_player_vs_computer, facile, toggle_entry_visibility);

    affecter_signal_toggle(facile, NULL, toggle_depth);
    affecter_signal_toggle(moyenne, NULL, toggle_depth);
    affecter_signal_toggle(difficile, NULL, toggle_depth);

    Frame_object* frameA = init_frame_default();
    strcpy(frameA->label, "choisir la couleur");
    frameA->commune.posx = 200;
    frameA->commune.posy = 450;
    frameA->taille.width = 400;
    frameA->taille.height = 100;
    frameA = create_frame(frameA);
    ajouter_content(&window1, frameA->frame, frameA->commune.posx, frameA->commune.posy);

    ToogleButton* blanc = init_radio_button();
    ToogleButton* noir = init_radio_button();

    blanc->label = "Second";
    blanc->ButtonPere = NULL;
    blanc = cree_radio_button(blanc);
    ajouter_frame(frameA, blanc->widget, 0, 0);

    noir->label = "Premier";
    noir->ButtonPere = blanc->widget;
    noir = cree_radio_button(noir);
    ajouter_frame(frameA, noir->widget, 80, 0);

    affecter_signal_toggle(blanc, NULL, toggle_depth2);

    affecter_signal_toggle(noir, NULL, toggle_depth2);


    gtk_widget_show_all(window1.window);
    gtk_main();

    return 0;
}