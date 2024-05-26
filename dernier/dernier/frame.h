
typedef struct
{
    // GtkWidget* container;//L'objet p?re
    GtkWidget* frame;
    char label[40];//nom de la frame
    taille taille;
    //int x; // la position horizontale
    //int y; // la position verticale
    commune commune;
    GtkWidget* fixed;
    GtkShadowType type; //type style du cadre
}Frame_object;

GtkWidget* create_frame(Frame_object* frame)
{
    frame->frame = gtk_frame_new(frame->label);
    gtk_widget_set_size_request(frame->frame, frame->taille.width, frame->taille.height);
    frame->fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(frame->frame), frame->fixed);
    //gtk_frame_set_shadow_type(GTK_FRAME(frame->frame), type);
    //gtk_frame_set_shadow_visible(GTK_FRAME(frame->frame), TRUE);
    return frame;
}
Frame_object* init_frame_default()
{
    Frame_object* frame = g_new(Frame_object, 1);

    strcpy(frame->label, "");
    frame->commune.posx = 0;
    frame->commune.posy = 0;
    frame->taille.width = 50;
    frame->taille.height = 50;
    frame->fixed = NULL;
    frame->type = GTK_SHADOW_NONE;
    if (!frame)
    {
        printf("\nErreur d'allocation memoire");
        exit(1);
    }
    return frame;
}
Frame_object* set_propriety_frame(Frame_object* frame, cellule_propriete* liste)
{
    while (liste != NULL)
    {
        if (strcmp(liste->attribut, "x") == 0)
            frame->commune.posx = atoi(liste->valeur);
        if (strcmp(liste->attribut, "y") == 0)
            frame->commune.posy = atoi(liste->valeur);
        if (strcmp(liste->attribut, "label") == 0)
            strcpy(frame->label, liste->valeur);
        if (strcmp(liste->attribut, "width") == 0)
            frame->taille.width = atoi(liste->valeur);
        if (strcmp(liste->attribut, "height") == 0)
            frame->taille.height = atoi(liste->valeur);
        liste = liste->suivant;
    }
    return frame;
}


void ajouter_frame_container(GtkWidget* container, Frame_object* frame)
{
    gtk_container_add(GTK_CONTAINER(container), frame->frame);
}

void ajouter_frame(Frame_object* frame, GtkWidget* contenu, int x, int y)
{
    gtk_fixed_put(GTK_FIXED(frame->fixed), contenu, x, y);
}