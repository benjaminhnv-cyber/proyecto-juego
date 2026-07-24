#ifndef COMONS_H
#define COMONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h> // esto lo que hace es para tener mas precicion en la posicion de los aldeanos 


#define KEY_SEEN     1
#define KEY_DOWN     2

#define TAMANO_X_MAP 60
#define TAMANO_Y_MAP 52

#define TAMAÑO_X_PT 1942
#define TAMAÑO_Y_PT 1100

#define TAMANO_CASILLA 32
#define DPI 0.5
#define TIEMPO_DE_APRICION 300 // 60 por segundo estos son 5 segundos por generacion 
#define TIEMPO_CONTRUYENDO 300
#define TIEMPO_DISPARO 60 // lo mismo por frame

#define MAX_ALDEANO 10
#define MAX_RECURSOS 70
#define MAX_ESTRUCTURA 10
#define MAX_BULLET 200
#define MAX_ARBOLES 200
#define MURO 10

#define TAMAMANO_ENEMIGO 120
#define MAX_ENEMY 100
#define MAX_SPAWNS 20
#define MAX_PUNTOS 6000
#define MAX_BORDES 300
#define ALCANCE_TOWER 150

bool modo_contrucccion = false; //variable global para que se active el modo contruccion 
int tiempo_spawn = TIEMPO_DE_APRICION;
int tiempo_contruyendo = TIEMPO_CONTRUYENDO; 

// estoe s el sistema de los aldeanos 
int wave = 1, enemy_alive = 0, spawn_timer = 0,enemy_to_spawn = 0;
int total_spawns = 0, timer_move_enemy=0, sax, say;
int costo_aldeanos = 5;

// tema del pos del mause
float x = 0, y = 0, dx = 0, dy = 0;

//hacerlo una variable global para las imagenes
ALLEGRO_BITMAP *texture_suelo;
ALLEGRO_BITMAP *enemy1;
ALLEGRO_BITMAP *sp;
ALLEGRO_BITMAP *ez;
ALLEGRO_BITMAP *ez2;
ALLEGRO_BITMAP *ez3;
ALLEGRO_BITMAP *ez4;
ALLEGRO_BITMAP *bor4;
ALLEGRO_BITMAP *bor3;
ALLEGRO_BITMAP *bor2;
ALLEGRO_BITMAP *bor;
ALLEGRO_BITMAP *torre;
ALLEGRO_BITMAP *menu;
ALLEGRO_BITMAP *ruta_suelo;

typedef struct
{
    int textura_x;
    int textura_y;
    int x;
    int y;
}RUTA;

typedef struct
{
    RUTA list[MAX_PUNTOS];
    int numero;
}RUTAS;
RUTAS rutas;

typedef enum 
{
    BORDE_ES_SUP_DER,
    BORDE_ES_INF_DER,
    BORDE_ES_INF_IZQ,
    BORDE_ES_SUP_IZQ,
    BORDE_VR_IZQ,
    BORDE_VR_DER,
    BORDE_HR_SUP,
    BORDE_HR_INF,
    BORDE_HORIZONTAL,
    BORDE_VERTICAL
}TIPO_BORDE;

typedef struct 
{
    int x;
    int y;
    int textura_x;
    int textura_Y;
    TIPO_BORDE tipo;
}BORDE_MAPA;

typedef struct
{
    BORDE_MAPA list[MAX_BORDES];
    int numero;
}BORDES;
BORDES bordes;

typedef struct 
{
    int x;
    int y;
    bool used;
    
    int textura_x;
    int textura_y;
}PUNTO;

typedef struct 
{
    PUNTO lista[MAX_PUNTOS];
    int numero;
}PUNTOS;
PUNTOS puntos;

typedef struct 
{
    int x;
    int y;
    bool used;
}ARBOL;

typedef struct 
{
    ARBOL lista[MAX_ARBOLES];
    int numero;
}ARBOLES;
ARBOLES arboles;


//esto es la estructura de las torres
typedef enum
{
    estado_simiento,
    contruyendo,
    listo
}ESTADO_ESTRUCTURA;

typedef struct 
{
    float x;
    float y;

    float dx;
    float dy;

    int objetivo;
    float velocidad;
    int dano;
    bool used;

}BULLET;
BULLET bullet[MAX_BULLET];

typedef struct 
{
    bool used; 
    bool selec;

    int x; // ubicacion 
    int y;
    int live;

    ESTADO_ESTRUCTURA estado; // este es el estado que va a estar

    int tier; // esto va a hacer las mejoras de las estructuras 

    int costo_madera; // costo de las diferentes estructura
    int costo_piedra;
    int costo_oro;

    // esto es apartado de la torreta
    int tiempo_disparo;
    int rango_torreta;
    int daño;
    //estado vida

    int tiempo_de_contruccion;// como indica es el tiempo de contruccion 
}ESTRUCTURA;
ESTRUCTURA estructura[MAX_ESTRUCTURA];

typedef struct
{
    int life;

    float x;
    float y;

}MUROS;
MUROS muro[MURO];

// esto es la estructura de los aldeanos y sus funciones 
typedef enum
{
    id_aldeano, // estado quieto
    aldeano_caminando,
    aldeano_recolectando,
    aldeano_contruyendo    
}ESTADO_ALDEANO;

typedef struct 
{
    int live;
    // posicion
    float ax;
    float ay;

    // destino donde tiene que ir
    float dest_x;
    float dest_y;

    int contador_recollecion;
    char recurso_obj;

    ESTADO_ALDEANO estado;

    bool selec;
    bool used;

}ALDEANOS;
ALDEANOS aldeanos[MAX_ALDEANO];

// esto son los materiales
typedef struct 
{
    int x,y;
    char tipo;
    bool estado;
    int durabilidad;
    int tiempo_aparicion;
    int i_arbol;
    bool respawn;
    char terreno_original;
}RECURSOS;
RECURSOS recursos[MAX_RECURSOS];

typedef struct aldeanos
{
    int oro;
    int madera;
    int piedra;
}MATERIALES_RECOLECTADOS;
MATERIALES_RECOLECTADOS stock;

// esto son las estructura de los enemigos
typedef enum
{
    ENEMY_NR=0,
    ENEMY_TANK
}TYPE_ENEMY;

typedef struct ENEMY
{
    TYPE_ENEMY type;
    int life;
    int max_life;
    bool used;
    int retardo_movimiento;
    int camino;

    int colison; // esto a futuro 
   
   
    //UBICACIONDEL ENEMIGO
    int nx,ny; 

    // esto lo que hace es la ubicacion del mapa donde esta ubicado
    int psmapx; 
    int psmapy;

    //lo que hace es para evitar que vuelva a la posicion anterior  
    int anteriorx;
    int anteriory;
    int siguiente_casilla;
    int damage;

    // textura del enemigo inprovisional
    int frame_actual;
    int total_frames;
    double tiemo_frame;
    double temporiazador;

}ENEMY;
ENEMY enemy[MAX_ENEMY];

void cero();

typedef struct // esto lo que hace es guardar el punto de aparicion del enemigo 
{
    int posx;
    int posy;

}SPAWNS;
SPAWNS spawns[MAX_SPAWNS];

typedef struct 
{
    bool used; // si se esta haciendo utilisado
    int mx,my; // ubicacion actual
    
    int indi_x; // ubicacion donde tiene que ir
    int indi_y; 

    int recoleccion;// indicadores de que esta haciendo 
    int contruccion;
}ALDEANO;

void must_init(bool test, const char *description);

void spwan_aldeano();
void movimiento_aldeano(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void recoleccion(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void desgaste_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int i);
void aparicion_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void generar_recurso(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], PUNTOS *puntos, int i);
void vaciar_recursos_res(int recursos_cargados);
int leectura_material_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void imprimir_enemigo();

//sistema de torretas
void update_torreta();
void disparo(float x, float y, int objetivo, int t);
void update_bullet();
void dibujo_proyectil();
int carga_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);

void movimiento_mouse(float *x, float *y, float *dx, float *dy,unsigned char key[], bool *done, bool *redraw);
void click(int x, int y, int buton, int botonx, int bontony,char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void carga_imagen();

void imprimir_boton(int botonx, int botony);
void imprimir_mapa(int fila, char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]);
void imprimir_mapa_y_enemigos(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int fila);
void imprimir_recursos(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int fila);
void imprimir_texto();
void dibujo_aldeano();
void modo_cont(int x, int y);

void enemy_init();
void start_wave();
void update_wave();
void check_wave();
void update_enemy(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int salx, int saly);

#endif