#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define KEY_SEEN 1
#define KEY_DOWN 2

// este son los tipos de enemigos
#define ENEMY_TYPE_COUNT 6 //cantidad de enemigos
#define MAX_ENEMIES 2000

int wave=1;
int enemy_to_spawn = 0;
int enemy_alive = 0;
int spawn_timer = 0;
int spawn_count = 0;
char mapa[35][50];


typedef enum
{
    ENEMY_NR = 0,
    ENEMY_FAST,
    ENEMY_TANK,
    ENEMY_MGTNAK,
    ENEMY_MG_FAST,
    ENEMY_BFG,
    ENEMY_TYPE_MAX

}TYPE_ENEMY;

typedef struct 
{
    int nx,ny; // ubicacion del enemigo
    TYPE_ENEMY type;
    int life;
    bool used;

    int objetivo_map;

}ENEMY;

ENEMY enemy[MAX_ENEMIES];

typedef struct {
    int x, y;
} SPAWN_POINT;

SPAWN_POINT spawns[100];

// creacion de ruta
typedef struct
{
    int x;
    int y;
} PUNTO;

PUNTO ruta[1000];
int ruta_len = 0;

void generar_ruta()
{
    int x,y;
    int px=-1, py=-1;

    // buscar S
    for(y=0;y<35;y++)
    {
        for(x=0;x<50;x++)
        {
            if(mapa[y][x]=='S')
            {
                break;
            }
        }

        if(x<50)
            break;
    }

    while(1)
    {
        ruta[ruta_len].x = x*32;
        ruta[ruta_len].y = y*32;
        ruta_len++;

        if(mapa[y][x]=='E')
            break;

        if(x+1!=px && (mapa[y][x+1]=='r' || mapa[y][x+1]=='E'))
        {
            px=x;
            py=y;
            x++;
        }
        else if(y+1!=py && (mapa[y+1][x]=='r' || mapa[y+1][x]=='E'))
        {
            px=x;
            py=y;
            y++;
        }
        else if(x-1!=px && (mapa[y][x-1]=='r' || mapa[y][x-1]=='E'))
        {
            px=x;
            py=y;
            x--;
        }
        else if(y-1!=py && (mapa[y-1][x]=='r' || mapa[y-1][x]=='E'))
        {
            px=x;
            py=y;
            y--;
        }
        else
        {
            break;
        }
    }

    printf("Ruta generada: %d puntos\n", ruta_len);
}
// 

void enemy_init() // esto lo que hace es para desactivar los enemigos antes de empear
{
    int i;
    for(i = 0; i < MAX_ENEMIES; i++)
    {
        enemy[i].used = false;
    }
}

void start_wave() // el spawn cantidad de enemigos de enemigos 
{
    enemy_to_spawn = 20 + wave * 2;   // cada ronda que pase, 0 mas aparescan enemigos aparesen cad
    enemy_alive = 0;
    spawn_timer = 0; // tiempo de spawn
}

void spawn_enemy() //el spawn de los enemigos
{
    int i,s;
    for(i=0; i < MAX_ENEMIES; i++)
    {
        if(!enemy[i].used)
        {
            enemy[i].used = true;
            enemy[i].life = 15;
            enemy[i].type = ENEMY_NR;
            
            s = rand()% spawn_count;
            
            enemy[i].nx = ruta[0].x;
            enemy[i].ny = ruta[0].y;

            enemy[i].objetivo_map = 1;

            enemy_alive++;
            break;
        }
    }
}

void update_wave() // esto lo que hace es basicamente cada 60 FPS pase spawnea un enemigo 
{
    if(enemy_to_spawn > 0)
    {
        spawn_timer++;

        if(spawn_timer >= 60) // cada 1 segundo cada 60 FPS
        {
            spawn_enemy();   // este es el spawn de los enemigos
            enemy_to_spawn--; // resta el valor de enmy_spawn para que ya no aparescan mas enemigos despues
            spawn_timer = 0;
        }
    }
}

void check_wave() // ronda que cada vez paza 
{
    if(enemy_to_spawn == 0 &&  enemy_alive == 0)
    {
        wave++;
        start_wave();
    }
}

void update_enemy() // esto es es el movimiento del enemigo
{
    int i,tx,ty;
    for(i=0;i<MAX_ENEMIES;i++)
    {
        if(enemy[i].used)
        {
            if(enemy[i].objetivo_map >= ruta_len)
            {
                enemy[i].used = false;
                enemy_alive--;
                continue;
            }

            tx = ruta[enemy[i].objetivo_map].x;
            ty = ruta[enemy[i].objetivo_map].y;

            if(enemy[i].nx < tx)
            {
                enemy[i].nx++;
            }
            if(enemy[i].nx > tx)
            { 
                enemy[i].nx--;
            }
            if(enemy[i].ny < ty)
            { 
                enemy[i].ny++;
            }
            if(enemy[i].ny > ty)
            {
                enemy[i].ny--;
            }

            if(enemy[i].nx == tx && enemy[i].ny == ty)
            {
                enemy[i].objetivo_map++;
            }
        }
    }
}

// movimineto en teclado

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    must_init(al_init(), "allegro");
    srand(time(NULL));

    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");

    // carga o sirve del archivo 


    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);  // este papu coso o el bitmap es para cargar los imagnes si es draw dibuja imagen

    ALLEGRO_DISPLAY* disp = al_create_display(1750, 1000);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    float x,y;
    x=100;
    y=100;
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;




    // IMAGE
    if(!al_init_image_addon())
	{
	    printf("couldn't initialize image addon\n");
	    return 1;
	}
    
    ALLEGRO_BITMAP* mysha = al_load_bitmap("mysha.png");
    must_init(mysha, "mysha.png");



    // mapa
    FILE *archivo_map = fopen("map.txt", "r");

    // comprobacion si cargo el archivo
    if(archivo_map == NULL)
    {
        printf("No se pudo abrir map.txt\n");
        return 1;
    }   

    
    int fila = 0;
    int i,j;
    while(fgets(mapa[fila], sizeof(mapa[fila]), archivo_map))
    {
        for(j = 0; mapa[fila][j] != '\0'; j++)
        {
            if(mapa[fila][j] == 'S')
            {
                spawns[spawn_count].x = j * 32;
                spawns[spawn_count].y = fila * 32;
                spawn_count++;
            }
        }
        fila++;
    }    
     fclose(archivo_map);

     //aqui esta el generador de mapa
     generar_ruta();


    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));


    // agregado 
    enemy_init();
    start_wave();
    //

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

            switch(event.type)
            {
        

                case ALLEGRO_EVENT_TIMER:

                    // enemigo
                    update_wave();
                    update_enemy();
                    check_wave();
                    update_enemy();
                    //

                    if(key[ALLEGRO_KEY_UP])
                    y--;
                    if(key[ALLEGRO_KEY_DOWN])
                    y++;
                    if(key[ALLEGRO_KEY_LEFT])
                    x--;
                    if(key[ALLEGRO_KEY_RIGHT])
                    x++;

                    if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                    for( i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= ~KEY_SEEN;
                    
                    redraw = true;
                    break;

                    case ALLEGRO_EVENT_KEY_DOWN:
                    key[event.keyboard.keycode] = KEY_SEEN | KEY_DOWN;
                    break;
                    case ALLEGRO_EVENT_KEY_UP:
                    key[event.keyboard.keycode] &= ~KEY_DOWN;
                    break;

                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        done = true;
                    break;
            }


            if(done)
                break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

        
            
            //s spawn
	        al_draw_filled_rectangle(x, y, x+10, y+10, al_map_rgba_f(1, 1, 1,0));

            //muestra mapa
            for(i=0;i<fila;i++)
            {
                for(j=0; mapa[i][j] != '\0'; j++)
                {
                    switch (mapa[i][j])
                    {
                    case '#':
                        al_draw_filled_rectangle(j*32, i*32 , j*32+32 , i*32+32, al_map_rgba_f(1, 0, 0, 1));
                        break;
                    case '.':
                        al_draw_filled_rectangle(j*32, i*32 , j*32+32 , i*32+32, al_map_rgba_f(1, 0, 1, 1));
                        break;
                    
                        case 'R': // este coso sirve para a futuro para que renderize el suelo de la ruta del enemigo
                        al_draw_filled_rectangle(j*32, i*32 , j*32+32 , i*32+32, al_map_rgba_f(1, 0, 0, 1));
                        break;

                    case 'S':
                        al_draw_filled_rectangle(j*32, i*32 , j*32+32 , i*32+32, al_map_rgba_f(0, 1, 1, 1));
                        break;
                    case 'E':
                        al_draw_filled_rectangle(j*32, i*32 , j*32+32 , i*32+32, al_map_rgba_f(1, 1, 0, 1));
                        break;
                    }
                }
            }

            // muestra el enemigo
            for(i = 0; i < MAX_ENEMIES; i++)
            {
                if(enemy[i].used)
                {
                                       
                    al_draw_filled_rectangle(enemy[i].nx, enemy[i].ny, enemy[i].nx + 20, enemy[i].ny + 20, al_map_rgb(0,255,255));
                }
            }
            
            al_flip_display();

           redraw = false;
    
        }
    }

   
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
