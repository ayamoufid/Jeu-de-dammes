#pragma once
#include <gtk\gtk.h>



DialogData* cree_Boite(char* nom, int modal, int height, int width, GtkWidget* window1)
{
    DialogData* dialog_data = malloc(sizeof(DialogData));

    GtkDialogFlags dialogTypes[3] = { GTK_DIALOG_MODAL , GTK_DIALOG_DESTROY_WITH_PARENT ,GTK_DIALOG_USE_HEADER_BAR };

    if (dialog_data)
    {

        dialog_data->dialog = gtk_dialog_new_with_buttons(nom, GTK_WINDOW(window1), dialogTypes[modal], NULL);
        dialog_data->content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog_data->dialog));
        dialog_data->height = height;
        dialog_data->width = width;
        gtk_window_set_default_size(GTK_WINDOW(dialog_data->dialog), dialog_data->height, dialog_data->width);
        gtk_window_set_transient_for(GTK_WINDOW(dialog_data->dialog), GTK_WINDOW(window1));
        dialog_data->fixed = gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(dialog_data->content_area), dialog_data->fixed);
        return ((DialogData*)dialog_data);
    }
    else
    {
        printf("\n error d'allocation m?moire \n");
        exit(1);
    }
}

void ajouter_content(fenetre* Boite, GtkWidget* contenu, int x, int y)
{
    gtk_fixed_put(GTK_FIXED(Boite->fixed), contenu, x, y);
}
