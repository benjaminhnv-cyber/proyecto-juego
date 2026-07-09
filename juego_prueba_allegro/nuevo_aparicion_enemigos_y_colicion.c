#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define KEY_SEEN     1
#define KEY_DOWN     2

#define MAX_ENEMY 100
#define MAX_SPAWNS 20

#define TAMANO_X_MAP 50
#define TAMANO_Y_MAP 35

#define TAMAMANO_CASILLA 32
#define TAMAMANO_ENEMIGO 96


int wave = 1, enemy_alive = 0, spawn_timer = 0, spanw_count=0, enemy_to_spawn = 0;
int total_spawns = 0, timer_move_enemy=0;

typedef enum
{
    ENEMY_NR=0,
    ENEMY_TANK
}TYPE_ENEMY;

typedef struct ENEMY
{
    TYPE_ENEMY type;
    int life;
    bool used;

    int colison; // esto a futuro 
   
   
    //UBICACIONDEL ENEMIGO
    int nx,ny; 

    // esto lo que hace es la ubicacion del mapa donde esta ubicado
    int psmapx; 
    int psmapy;

    //lo que hace es para evitar que vuelva a la posicion anterior  
    int anteriorx;
    int anteriory;

    // textura del enemigo inprovisional
    int frame_actual;
    int total_frames;
    double tiemo_frame;
    double temporiazador;

}ENEMY;
ENEMY enemy[MAX_ENEMY];

void cero() // textura reinicio
{
    for(int i=0; i<MAX_ENEMY; i++)
    {
        enemy[i].frame_actual = 0;
        enemy[i].total_frames = 16;
        enemy[i].tiemo_frame = 8;
        enemy[i].temporiazador = 0;
    }
}

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

typedef struct 
{
    /*int live;posible*/
    bool used;
    int atack;
    int bullet;
}ESTRUCTURA;

/* posible aliado para que ataquen a los enemy
typedef struct 
{
    int life;
    int dx, dy;
    bool used;
    int attack_enemy;
}DEFENSOR;
*/

//
void spwan_enemy()
{
    int i, random;
    for(i=0; i < MAX_ENEMY/* aqui va cantidad maxima que se puede generar los enemigos*/;i++)
    {
        if(!enemy[i].used)
        {
            enemy[i].used = true;
            enemy[i].type = ENEMY_NR;

            random = rand()% total_spawns;
            
            enemy[i].psmapx = spawns[random].posx;
            enemy[i].psmapy = spawns[random].posy;
            
            enemy[i].nx = enemy[i].psmapx * TAMAMANO_CASILLA;
            enemy[i].ny = enemy[i].psmapy * TAMAMANO_CASILLA;

            enemy_alive++;

            return;
        }
    }
}

bool casilla_ocupada(int x, int y, int id) // lo que hace es evitar que los enemigos esten en el mismo punto
{
    int i;
    for(i = 0; i < MAX_ENEMY; i++)
    {
        if(i == id)
        continue;

        if(enemy[i].used && enemy[i].psmapx == x && enemy[i].psmapy == y)
        {
            return true;
        }
    }
    return false;
}

void eleccion_de_la_siguiente_casilla(int id, char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    int direcciones[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    int opciones_x[4], opciones_y[4];
    int cant = 0, nx, ny, i, j;
    char celda;
    int random, salx = -1 ,saly = -1;
    int distancia;
    
    // busca la salida o mejor dicho la base del player donde tiene que ir 
    for(i = 0; i < TAMANO_Y_MAP; i++)
    {
        for(j = 0; j < TAMANO_X_MAP; j++)
        {
            if(mapa[i][j] == 'E')
            {
                salx = j;
                saly = i;
                break;
            }
        }
        if(salx != -1) break;
    }

    if(mapa[enemy[id].psmapy][enemy[id].psmapx] == 'E')
    {
        enemy[id].used = false;
        enemy_alive--;
        return;
    }
    
    // Recopilacion de las opciones válidas para moverse 
    for(i = 0; i < 4; i++)
    {
        nx = enemy[id].psmapx + direcciones[i][0];
        ny = enemy[id].psmapy + direcciones[i][1];
        
        if(nx >= 0 && nx < TAMANO_X_MAP && ny >= 0 && ny < TAMANO_Y_MAP)
        {
            celda = mapa[ny][nx];
            if((celda == 'r' || celda == 'E') && !casilla_ocupada(nx, ny, id))
            {
                opciones_x[cant] = nx;
                opciones_y[cant] = ny;
                cant++;
            }
        }
    }
    
    if(cant > 0)
    {
        // Si hay más de una opción puede elegir la que más se acerca a la salida o base del player
        if(cant > 1 && salx != -1 && saly != -1)
        {
            int mejor_distancia = 9999;
            int mejor_opcion = 0;
            
            for(i = 0; i < cant; i++)
            {
                distancia = abs(opciones_x[i] - salx) + abs(opciones_y[i] - saly);
                
                if(rand() % 5 == 0) // esto es un 20% aleatorio hacia donde va 
                {
                    mejor_opcion = rand() % cant;
                    break;
                }
                
                if(distancia < mejor_distancia)
                {
                    mejor_distancia = distancia;
                    mejor_opcion = i;
                }
            }
            
            // esto lo que hace es que se mueva hacia la mejor opcion
            enemy[id].anteriorx = enemy[id].psmapx;
            enemy[id].anteriory = enemy[id].psmapy;
            enemy[id].psmapx = opciones_x[mejor_opcion];
            enemy[id].psmapy = opciones_y[mejor_opcion];
        }
        else
        {
            random = rand() % cant;
            enemy[id].anteriorx = enemy[id].psmapx;
            enemy[id].anteriory = enemy[id].psmapy;
            enemy[id].psmapx = opciones_x[random];
            enemy[id].psmapy = opciones_y[random];
        }
        
        enemy[id].nx = enemy[id].psmapx * TAMAMANO_CASILLA;
        enemy[id].ny = enemy[id].psmapy * TAMAMANO_CASILLA;
    }
}

void enemy_init() // esto lo que hace es para desactivar los enemigos antes de empear
{
    int i;
    for(i = 0; i < MAX_ENEMY; i++)
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

void update_wave() // esto lo que hace es basicamente cada 60 FPS pase spawnea un enemigo 
{
    if(enemy_to_spawn > 0)
    {
        spawn_timer++;

        if(spawn_timer >= 60) // cada 1 segundo cada 60 FPS
        {
            spwan_enemy();   // este es el spawn de los enemigos
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

void update_enemy(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]) //esto es es el movimiento del enemigo
{
    for(int i = 0; i < MAX_ENEMY; i++)
    {
        if(enemy[i].used)
        {
            eleccion_de_la_siguiente_casilla(i, mapa);
        }
    }
}

void generacion_material()
{
    //este momo lo que va hacer es generar diferentes recursos con diferentes proabilidad  oro, madera ,o piedras arboles etc
}

void generacion_de_insumos(int mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    int i,j;
    for(i = 0; i < TAMANO_Y_MAP; i++)
    {
        for(i = 0; i < TAMANO_X_MAP; j++)
        {
            if(enemy_to_spawn > 0)
            {
                spawn_timer++;

                if(spawn_timer >= 60) // cada 1 segundo cada 60 FPS
                {
                   // este es el spawn de los recursoso y que los enemigos nopueda pasar
                enemy_to_spawn--; // resta el valor de enmy_spawn para que ya no aparescan mas recursos despues
                spawn_timer = 0;
                }
                else if(1<1)
                {
                    // aqui los materiales no se generarn porque ya llego el limite
                    break; 
                }
            }
        }
    }
}

void aldeano_aparicion()
{
    // este momo va a hacer los secuases del jugador para hacer mandados 
}

void almacen()
{
    //
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) // esto es a futuro 
{
    if(ax2 < bx1) return false;
    if(ax1 > bx2) return false;
    if(ay2 < by1) return false;
    if(ay1 > by2) return false;

    return true;
}

void colisiones() // esto es a futuro 
{
    for(int i=0; i < MAX_ENEMY; i++)
    {
        for(int j=i+1; j< MAX_ENEMY;j++)
        {
            if(collide(enemy[i].nx, enemy[i].ny, enemy[i].nx + 20, enemy[i].ny + 20, enemy[j].nx, enemy[j].ny, enemy[j].nx + 20, enemy[j].ny + 20))
            {
            }
        }
    }
}

//

void must_init(bool test, const char *description) // lo que hace es revisar si se a ejecutado correctamente 
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
        
    cero();
    int s;
    srand(time(NULL));
    must_init(al_init(), "allegro");
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

    ALLEGRO_DISPLAY* disp = al_create_display(1150, 750);
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

    ALLEGRO_KEYBOARD_STATE ks;

    // mapa
    FILE *archivo_map = fopen("map.txt", "r");

    // comprobacion si cargo el archivo
    if(archivo_map == NULL)
    {
        printf("No se pudo abrir map.txt\n");
        return 1;
    }   

    char mapa[TAMANO_Y_MAP][TAMANO_X_MAP];
    int fila = 0;
    int i,j;


    while(fila < TAMANO_Y_MAP && fgets(mapa[fila], sizeof(mapa[fila]), archivo_map))
    {
        fila++;
    }
    fclose(archivo_map);
    for(i=0; i<fila ; i++)
    {
        for(j=0; mapa[i][j] != '\0' && mapa[i][j] != '\n'; j++)
        {
            if(mapa[i][j]=='S' && total_spawns< MAX_SPAWNS)
            {
                spawns[total_spawns].posx = j;
                spawns[total_spawns].posy = i;
                total_spawns++;
            }
        }
    }

    printf("Spawns encontrados: %d\n", total_spawns);
    enemy_init();
    
    start_wave();

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));


    must_init(al_init_image_addon(), "image addon");
    ALLEGRO_BITMAP* texture_suelo = al_load_bitmap("assets/texture_suelo.png");
    ALLEGRO_BITMAP* enemy1 = al_load_bitmap("assets/RUN.png");
    ALLEGRO_BITMAP* sp = al_load_bitmap("assets/texture.png");
    if (!texture_suelo || !enemy1 || !sp) 
    {
        if(!enemy1)
        {
            printf("run\n");
        }
        if(!sp)
        {
            printf("sp\n");
        }
        printf("Error cargando bitmaps\n");
    }

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);
        
        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
            // ronda

            update_wave();//generacion de enemigos

            timer_move_enemy++;// esto lo que hace es que se mueva mas lento los cuboss
            if(timer_move_enemy >= 20)
            {
                update_enemy(mapa);
                timer_move_enemy = 0;
            }
            check_wave();//revisa si se tiene que pasar la ronda
            colisiones();

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

            for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
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
                for(j=0;mapa[i][j] != '\0';j++)
                {
                    switch (mapa[i][j])
                    {
                    case '#':
                        al_draw_bitmap_region(texture_suelo, 400, 160, TAMAMANO_CASILLA, TAMAMANO_CASILLA, j* TAMAMANO_CASILLA, i* TAMAMANO_CASILLA, 0);            
                        break;
                    case '.':
                        al_draw_bitmap_region(texture_suelo, 0, 0, TAMAMANO_CASILLA, TAMAMANO_CASILLA, j* TAMAMANO_CASILLA, i* TAMAMANO_CASILLA, 0);
                        break;
                    case 'r':
                        al_draw_bitmap_region(texture_suelo, 165, 10, TAMAMANO_CASILLA, TAMAMANO_CASILLA, j* TAMAMANO_CASILLA, i* TAMAMANO_CASILLA, 0);
                        break;
                    case 'S':
                        al_draw_bitmap_region(sp, 0, 0, TAMAMANO_CASILLA, TAMAMANO_CASILLA, j* TAMAMANO_CASILLA, i* TAMAMANO_CASILLA, 0);
                        break;
                    case 'E':
                        al_draw_filled_rectangle(j*TAMAMANO_CASILLA,i*TAMAMANO_CASILLA,j*TAMAMANO_CASILLA+TAMAMANO_CASILLA,i*TAMAMANO_CASILLA+TAMAMANO_CASILLA,al_map_rgba_f(1, 1, 0, 1));
                        break;
                    }
                }
            }
            // enemigos dibujo
            for(i=0; i < MAX_ENEMY; i++)
            {    
                if (enemy[i].used)
                {
                   
                    enemy[i].temporiazador++;
                    if(enemy[i].temporiazador >= enemy[i].tiemo_frame)
                    {
                        enemy[i].temporiazador = 0;
                        enemy[i].frame_actual++; 
                        if(enemy[i].frame_actual >= enemy[i].total_frames)
                        {
                            enemy[i].frame_actual = 0;
                        }
                    }
                    int sx = enemy[i].frame_actual * 96;
                    al_draw_scaled_bitmap(enemy1, sx, 0, 96, 96, (enemy[i].nx - 30), (enemy[i].ny - 45), TAMAMANO_ENEMIGO, TAMAMANO_ENEMIGO, 0);
                    sx = 70 + sx;
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