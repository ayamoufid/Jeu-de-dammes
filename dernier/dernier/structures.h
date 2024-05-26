#include<stdio.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include<limits.h>
#define Colonne 8
#define ligne 8
#define BOARD_SIZE 8
#define nbreCapture
#define Max 100
#define TOKEN_BUFFER_SIZE 100
static int n = 0;
static int m = 0;
static int indice = 0;
int v = 0;
int tst = 0;
int tour;
int is_capture = 0;//ordi

char token_buffer[TOKEN_BUFFER_SIZE];
static int i = 0;  // variable statique pour conserver l'index du tableau entre les appels


int id = 0;
//piece_t MonJeu[Colonne][ligne];
// Tableau 8x8 pour représenter le plateau

typedef enum
{
    EMPTY,
    PLAYER1,
    PLAYER2,
    PLAYER1_KING,
    PLAYER2_KING
}piece_t;

typedef struct
{
    int x;
    int y;
}couple;

typedef struct Move
{
    couple from;
    couple to;
    int capture;
}Move;

typedef struct piece
{
    int x;
    int y;
    int is_dame;
    struct piece* svt;
}Liste_piece;

typedef struct joueur
{
    Liste_piece* Mes_pieces;
    int type; //1 : joueur normale 0: ordinateur
    char* nom;
    int fini;
    int score;
}Joueur;

typedef struct Jeu
{
    piece_t board[Colonne][ligne];
    Joueur* Players[2];
    //char* imagePaths[5];
    int tour_Joueur;
    int fini;
    int vainqueur;
    int Score;
    int mode;
}Board_Jeu;

typedef struct List
{
    couple from;
    couple to;
    int Score;
    struct List* svt;
}List_Pions;

typedef struct Cel
{
    List_Pions* contenu;
    int Score;
    struct Cel* fils;
    struct Cel* frere;
}Noeud;

typedef struct tab
{
    couple tableau[20];
    int nbreElem;
}tableau;

typedef struct
{
    List_Pions* chemins;
    int nbreElem;
}Chemins;


typedef struct
{
    GtkWidget* container; //conteneur d'une entrée.
    int posx; //position selon axe x
    int posy; //position selon axe y
}commune;

typedef struct
{
    int width; //longueur
    int height; //largeur
}taille;

// Structure pour représenter une case du plateau
typedef struct {
    int row;
    int col;
    char* couleur;
    GtkWidget* button;
} Cell;


typedef struct
{
    char nom[9];
    GtkWidget* info;
}cell;

typedef struct Cele
{
    cell* cell;
    struct Cele* suivant;
}Cellule;


typedef struct cellule_propriete
{
    char attribut[40];
    char valeur[40];
    struct cellule_propriete* suivant;
}cellule_propriete;

typedef struct
{
    GtkWidget* container;
    GtkWidget* grid; // la grille
    int nbr_ligne; // nombre des lignes
    int nbr_colonne; // nombre des colonnes
    gboolean homogene; // true : pour une grille homogène, sinon false ;
    commune commune;
} Grid_object;

//structure de fenetre
typedef struct fenetre
{
    int window_type;
    GtkWidget* fixed;
    gboolean decorated; //TRUE or =FALSE, the window decorations will be removed,
    gboolean resizable; //TRUE the user will be able to resize the window by dragging its edges or corners
    gboolean deletable; //the window will have a close button that the user can click to close the window
    taille taille;
    int border_taille;
    char name[20];
    GtkWindowPosition position;  //position de window
    float x; // en cas de position = GTK_WIN_POS_NONE (x,y)
    float y;
    char icon[20];  //link de image
    char bg_color[20];
    float opacity; // Set the opacity of the window
    int screen; //object to fullscreen mode
    //0 nothing 1 fullscrean 2 unfullscrean
    //3 maxmise 4 unmaximize 5 minimise
    gboolean present; //to bring a GTK window to the front and give it focus.
    GtkWidget* window;   //the window
    // GtkWidget* parent;
    int scroll;
    char image_background[20];
    int id;
}fenetre;



//structure de app
typedef struct app
{
    fenetre fenetre;
    struct app* svt;
}application;



Board_Jeu New_game;
Grid_object* gridd = NULL;
List_Pions* tabG[30];
Cell* pion1 = NULL;
Cell* pion2 = NULL;


typedef struct l
{
    Cell* pion;
    struct l* svt;
}laliste;

typedef struct
{
    char style[30];
    char family[30]; //sans-serif arial calibri verdana timesnewroman..
    int size; //longueur
    char weight[30]; //taille des caracteres
    char color[30];
    char bgcolor[30];
    char underline[30];//single low double
    char underline_color[30];
    char barrer_txt[6];//true false
    char color_bar[30];
    int select;//true false
    int alignement;//left,right,center,fill
} Format;

int depth = 0;



typedef struct
{
    GtkWidget* label;
    commune position;
    taille size;
    char* text;
    Format lab;
} label_object;

laliste* chemin;
fenetre  window;
//label_object* lb = NULL,*lb2= NULL, *lb3=NULL,*sc1=NULL,*sc2=NULL;

label_object* tab[2];

typedef struct
{
    GtkWidget* header;
    GtkWidget* fixed_header;
}header;

typedef struct
{
    GtkWidget* entry; // entrée.
    commune position;
    int height; //la longueur d'entrée.
    int width; //largeur.
    gboolean visible; //le texte est visible ou pas.
    char invisible_car; //le caractère d'un texte invisible.
    gboolean editable;//on peut saisir par clavier.
    char text_to_set[30];//le texte à saisir.
    int max_car; //max des caracteres
    char placeholder_text[50];
    char icon_name[30];
    char* completion_strings[30];
    int num_completion_strings;
}zonetxt_object;

typedef struct 
{
    GtkWidget* dialog;
    GtkWidget* content_area;
    GtkWidget* fixed;
    int modal;
    int height;
    int width;
} DialogData;

DialogData* dialog_Message;


label_object* mode_label;
zonetxt_object* player1_name_entry;
zonetxt_object* player2_name_entry;

fenetre window1;

int tour;


typedef struct {
    gchar* value1;
    gchar* value2;
} EntryValues;

GtkWidget* inner_vbox;

int joueur_index = 0;
EntryValues entryValues = { NULL, NULL };