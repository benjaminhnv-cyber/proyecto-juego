#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define KEY_SEEN     1
#define KEY_DOWN     2

#define TAMANO_X_MAP 1200
#define TAMANO_Y_MAP 1250
#define TAMANO_CASILLA 32
#define DPI 0.5

#define MAX_ALDEANO 2
#define MAX_RECURSOS 100

typedef enum
{
    id_aldeano,
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
    int recurso_obj;

    ESTADO_ALDEANO estado;

    bool selec;
    bool used;

}ALDEANOS;
ALDEANOS aldeanos[MAX_ALDEANO];

typedef struct 
{
    int x,y;
    char tipo;
    bool estado;
    int durabilidad;
}RECURSOS;
RECURSOS recursos[MAX_RECURSOS];

typedef struct 
{
    int oro;
    int madera;
    int piedra;
}MATERIALES_RECOLECTADOS;
MATERIALES_RECOLECTADOS stock;

void spwan_aldeano()
{
    int i;
    for(i = 0; i < MAX_ALDEANO; i++)
    {
        if(!aldeanos[i].used)
        {
            aldeanos[i].used = true;
            aldeanos[i].ax = 100;
            aldeanos[i].ay = 100;
            
            aldeanos[i].estado = id_aldeano;
            aldeanos[i].selec = false;
            aldeanos[i].dest_x = aldeanos[i].ax;
            aldeanos[i].dest_y = aldeanos[i].ay;
            aldeanos[i].contador_recollecion = 0;
            

        }
    }
}

void movimiento_aldeano(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    for(int i = 0; i < MAX_ALDEANO; i++)
    {


        if(aldeanos[i].estado == aldeano_caminando) // lo que hace es que el aldeano se mueva hacia el objetivo indicado por el jugador
        {
            if(aldeanos[i].ax < aldeanos[i].dest_x)
            {
                aldeanos[i].ax++;
            }
            if(aldeanos[i].ax > aldeanos[i].dest_x)
            {
                aldeanos[i].ax--;
            }

            if(aldeanos[i].ay < aldeanos[i].dest_y)
            {
                aldeanos[i].ay++;
            }
            if(aldeanos[i].ay > aldeanos[i].dest_y)
            {
                aldeanos[i].ay--;
            }
        }
        if(abs((int)(aldeanos[i].ax - aldeanos[i].dest_x)) <= 1 && abs((int)(aldeanos[i].ay - aldeanos[i].dest_y)) <= 1)// esto lo que hace es cuando si esta en la posicion del mapa da la forma de recolectar recursoso
        {

            int fila = aldeanos[i].ay / TAMANO_CASILLA;
            int colum = aldeanos[i].ax / TAMANO_CASILLA;
          
            if(mapa[fila][colum] == 'M')
            {
                aldeanos[i].estado = aldeano_recolectando;
                aldeanos[i].recurso_obj = 'M';
                                printf("m");

            }
            else if(mapa[fila][colum] == 'O')
            {
                aldeanos[i].estado = aldeano_recolectando;
                aldeanos[i].recurso_obj = 'O';
                            printf("o");

               
            }
            else if(mapa[fila][colum] == 'P')
            {
                aldeanos[i].estado = aldeano_recolectando;
                aldeanos[i].recurso_obj = 'P';
                /**/
                printf("p");
                
            }
            else// si no esto esta
            {
                aldeanos[i].estado = id_aldeano;
            }
        
        }
    }
}

void desgaste_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int i)
{
    for(int m = 0; m < MAX_RECURSOS; m++)
    {
        int fila = aldeanos[i].ay / TAMANO_CASILLA;
        int colum = aldeanos[i].ax / TAMANO_CASILLA;
        if(recursos[m].x == colum && recursos[m].y == fila)
        {
            recursos[m].durabilidad--;
            printf("vida del recurso: %d",recursos[m].durabilidad);
            if(recursos[m].durabilidad <=0)
            {
                mapa[fila][colum] = '.';
            }
        }
    }
}
/*
void desgaste_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int ax/TAM, ay/TAM int i)
void desgaste_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    for(int q = 0; q < MAX_RECURSOS; q++)
    {
        if(recursos[q].durabilidad <= 0 )
        {
            continue;
        }
        for(int i = 0; i < MAX_ALDEANO; i++)
        {
            int fila = aldeanos[i].ay / TAMANO_CASILLA;
            int colum = aldeanos[i].ax / TAMANO_CASILLA;
            printf("[%d,%d][%c,%d]\n", recursos[q].x,recursos[q].y,recursos[q].tipo,recursos[q].durabilidad);
            if(recursos[q].x == colum && recursos[q].y == fila)
            {
                recursos[q].durabilidad--;
                
                if(recursos[q].durabilidad == 0)
                {
                    mapa[fila][colum] = '.';
                    printf("vida del recurso: %d",recursos[q].durabilidad);
                }
            }
            
        }
    }
}crear funcion que pase parametros x y  del recurso xy
*/
void dibujo_aldeano()
{
    for(int i = 0; i < MAX_ALDEANO; i++)
    {
        if(aldeanos[i].used)
        {
            al_draw_filled_rectangle(aldeanos[i].ax , aldeanos[i].ay, aldeanos[i].ax + TAMANO_CASILLA, aldeanos[i].ay + TAMANO_CASILLA, al_map_rgb(0, 255, 1));
        }
        if(aldeanos[i].selec) // esto lo que hace es mostra el aldeano seleccionado 
        {
            al_draw_rectangle(aldeanos[i].ax - 2, aldeanos[i].ay - 2, aldeanos[i].ax + TAMANO_CASILLA + 2, aldeanos[i].ay + TAMANO_CASILLA + 2, al_map_rgb(255,0,0), 2);
        }
    }
}

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initializa %s\n", description);
    exit(1);
}

void imprimir_mapa(int fila, char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    for(int i = 0; i < fila; i++)
    {
        for(int j = 0;mapa[i][j] != '\0' ; j++)
        {
            switch(mapa[i][j])
            {
                case 'M':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(0, 0, 255, 255) );
                break;
                case 'O':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(1, 255, 0, 1) );
                break;
                case 'P':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(255, 1, 255, 1) );
                break;
            }
        }
    }
}

void leectura_material_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    int n = 0;
    for(int i = 0; i < TAMANO_Y_MAP; i++)
    {
        for(int j = 0; j < TAMANO_X_MAP; j++)
        {
            if(mapa[i][j] == 'M')
            {
                recursos[n].x = j;
                recursos[n].y = i;
                recursos[n].tipo = 'M';
                recursos[n].estado = true;
                recursos[n].durabilidad = 10;
                n++;
            }
            if(mapa[i][j] == 'P')
            {
                recursos[n].x = j;
                recursos[n].y = i;
                recursos[n].tipo = 'P';
                recursos[n].estado = true;
                recursos[n].estado = 20;
                n++;
            }
            if(mapa[i][j] == 'O')
            {
                recursos[n].x = j;
                recursos[n].y = i;
                recursos[n].tipo = 'O';
                recursos[n].estado = true;
                recursos[n].durabilidad = 15;
                n++;
            }
        }
    }
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_mouse(), "mouse");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY* disp = al_create_display(TAMANO_X_MAP, TAMANO_Y_MAP);
    must_init(disp, "display");
    al_hide_mouse_cursor(disp);

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    FILE *archivo_map = fopen("map_materiales.txt", "r");
    if(archivo_map == NULL)
    {
        printf("no se pudo abrir el archivo");
        return 1;
    }

    char mapa[TAMANO_Y_MAP][TAMANO_X_MAP];
    int fila = 0;
    int i,j;
    while(fila < TAMANO_Y_MAP && fgets(mapa[fila], sizeof(mapa[fila]),archivo_map))
    {
        fila++;
    }
    fclose(archivo_map);
    

    float x = 0, y = 0, dx = 0, dy = 0;

    al_grab_mouse(disp);
    bool done = false;
    bool redraw = true;

    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    //ejecucion 
    spwan_aldeano();
    leectura_material_mapa(mapa);
    
    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);
        switch (event.type)
        {
            case ALLEGRO_EVENT_TIMER:
            {
                // movimiento de aldeano
                movimiento_aldeano(mapa);
                for(i = 0; i < MAX_ALDEANO; i++)
                {
                    if(aldeanos[i].estado == aldeano_recolectando)
                    {
                        aldeanos[i].contador_recollecion++;
                        if(aldeanos[i].contador_recollecion >= 60)
                        {
                            aldeanos[i].contador_recollecion = 0;
                            switch(aldeanos[i].recurso_obj)
                            {
                                case 'M':
                                stock.madera++;
                                printf("(%f,%f)\n", aldeanos[i].ax, aldeanos[i].ay);
                                desgaste_material(mapa,i);
                                break;
                            
                                 case 'O':
                                stock.oro++;
                                printf("(%f,%f)\n", aldeanos[i].ax, aldeanos[i].ay);
                                desgaste_material(mapa,i);
                                break;
                            
                                case 'P':
                                stock.piedra++;
                                printf("(%f,%f)\n", aldeanos[i].ax, aldeanos[i].ay);
                                /*i, ax/TAM_CASILLA y ay/TAM_CASILLA*/
                                desgaste_material(mapa,i);
                                break;

                            }
                            printf("Madera:%d Oro:%d Piedra:%d\n", stock.madera, stock.oro, stock.piedra);
                        }
                    }
                }
                //

                if(key[ALLEGRO_KEY_ESCAPE])
                {
                    done = true; 
                }
            
                x += dx;
                y += dy;

                if(x < 0) // esto lo que hace es limitar el espeacio para evitar que salga el cubo o mause personalisado
                {
                    x = 1;
                    dx = 1;
                }

                if(x > TAMANO_X_MAP - TAMANO_CASILLA) // para que rebote a la izquierda
                {
                    x -= (x - (TAMANO_X_MAP- TAMANO_CASILLA));
                    dx *= -1;   
                }
            
                if(y < 0)
                {
                    y = 1;
                    dy = 1;
                }

                if(y > (TAMANO_Y_MAP - TAMANO_CASILLA))
                {
                    y -= (y - (TAMANO_Y_MAP - TAMANO_CASILLA));
                    dy *= -1;
                }
            
                // esto es para dpi para el mause 
                dx *= DPI;
                dy *= DPI; 
            
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                {
                    key[i] &= ~KEY_SEEN;
                }
                redraw = true;
                break;
            }

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if(event.mouse.button == 1) // este es el boton izquierdo 
            {
                for(int i = 0; i < MAX_ALDEANO; i++)
                {
                    if(x >= aldeanos[i].ax && x <= aldeanos[i].ax + TAMANO_CASILLA && y >= aldeanos[i].ay && y <= aldeanos[i].ay + TAMANO_CASILLA)
                    {
                        aldeanos[i].selec = true;
                    }
                    else 
                    {
                        aldeanos[i].selec = false;
                    }
                }
            }
            
            if(event.mouse.button == 2) // este boton derecho
            {
                for(int i = 0; i < MAX_ALDEANO; i++)
                {
                    if(aldeanos[i].selec)
                    {
                        aldeanos[i].dest_x = x;
                        aldeanos[i].dest_y = y;
                        aldeanos[i].estado = aldeano_caminando;
                    }
                }
            }
            break;
            
            case ALLEGRO_EVENT_MOUSE_AXES:
            dx += event.mouse.dx * 0.1;
            dy += event.mouse.dy * 0.1;
            al_set_mouse_xy(disp, 320, 240);
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
        {
            break;
        }
        
        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb_f(0, 0, 0));
            imprimir_mapa(fila,mapa);
            al_draw_filled_rectangle(x, y, x + 10, y + 10, al_map_rgb_f(0,1,1));
             dibujo_aldeano();
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}