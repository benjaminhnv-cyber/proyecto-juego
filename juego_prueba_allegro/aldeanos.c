#include "comons.h"

int main()
{
    int s;
    cero();

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_mouse(), "mouse");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");


    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    
    ALLEGRO_DISPLAY* disp = al_create_display(TAMAÑO_X_PT, TAMAÑO_Y_PT);
    must_init(disp, "display");
    al_hide_mouse_cursor(disp);
    
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_FONT *font = al_load_ttf_font("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24, 0);
    must_init(font, "font");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    char mapa[TAMANO_Y_MAP][TAMANO_X_MAP];
    //el nivel es un int
    //carga mapa (mapa, nivel, ....)
    int fila = carga_mapa(mapa);
    
    al_grab_mouse(disp);
    bool done = false;
    bool redraw = true;
    
    // lectura del mapa para el spwan de los enemigos 
   
    enemy_init();
    start_wave();
    
    must_init(al_init_image_addon(), "image addon");    
    carga_imagen();
    
    for(int i = 0; i < MAX_ENEMY; i++)
    {
        enemy[i].used = false;
    }

    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    //ejecucion 
    int recursos_cargado;
    spwan_aldeano();
    recursos_cargado = leectura_material_mapa(mapa);
    vaciar_recursos_res(recursos_cargado);
   
    //aqui esta la base donde se colocara el boton aqui puede ir futuro cambios para cuando haya un menu
    int botonx = 55 * TAMANO_CASILLA;
    int bontony = 18 * TAMANO_CASILLA;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);
        switch (event.type)
        {
            case ALLEGRO_EVENT_TIMER:
            {
                // movimiento de aldeano
                update_wave();
                movimiento_aldeano(mapa);
                recoleccion(mapa);
                update_torreta();
                
                update_enemy(mapa, sax, say);
                
                update_bullet();
                check_wave();//revisa si se tiene que pasar la ronda

                aparicion_material(mapa);
                //
                movimiento_mouse(&x, &y, &dx, &dy, key, &done, &redraw);
                break;
            }

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            click(x, y, event.mouse.button, botonx, bontony, mapa);
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
            imprimir_boton(botonx,bontony);
            imprimir_mapa_y_enemigos(mapa,fila);
            imprimir_enemigo();
            dibujo_aldeano();
            modo_cont(x,y);
            al_draw_filled_rectangle(x, y, x + 10, y + 10, al_map_rgb_f(0,1,1));
            imprimir_texto(font);
            dibujo_proyectil();

            al_flip_display();
            

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(texture_suelo);
    al_destroy_bitmap(enemy1);
    al_destroy_bitmap(sp);
}

// aldeanos 
void spwan_aldeano()
{
    int i = 1;
    aldeanos[i].used = true;
    aldeanos[i].ax = 100;
    aldeanos[i].ay = 100;
            
    aldeanos[i].estado = id_aldeano;
    aldeanos[i].selec = false;
    aldeanos[i].dest_x = aldeanos[i].ax;
    aldeanos[i].dest_y = aldeanos[i].ay;
    aldeanos[i].contador_recollecion = 0;
}

void movimiento_aldeano(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    // podria agregar que el aldeano solo se pueda mover dentro de la matriz del mapa 
    for(int i = 0; i < MAX_ALDEANO; i++)
    {
        if(aldeanos[i].used)
        {

            // lo que hace es que el aldeano se mueva hacia el objetivo indicado por el jugador
            //actualizacion de est parte se modificara para que no se lsalga del cuadrado o del map impreso
            if(aldeanos[i].estado == aldeano_caminando)
            {
                float nv_x = aldeanos[i].ax;
                float nv_y = aldeanos[i].ay;
                if(nv_x < aldeanos[i].dest_x)
                {
                    nv_x++;
                }
                if(nv_x > aldeanos[i].dest_x)
                {
                    nv_x--;
                }
                if(nv_y < aldeanos[i].dest_y)
                {
                    nv_y++;
                }
                if(nv_y > aldeanos[i].dest_y)
                {
                    nv_y--;
                }

                int fila = nv_y / TAMANO_CASILLA;
                int columna = nv_x / TAMANO_CASILLA;
                
                if(fila >= 0 && fila < TAMANO_Y_MAP && columna >= 0 && columna < TAMANO_X_MAP && mapa[fila][columna] != '#')
                {                            
                    aldeanos[i].ax = nv_x;
                    aldeanos[i].ay = nv_y;
                }
                else
                {
                    aldeanos[i].estado = id_aldeano;
                }
            }  
                // 
            if(fabs((aldeanos[i].ax - aldeanos[i].dest_x)) <= 1 && fabs((aldeanos[i].ay - aldeanos[i].dest_y)) <= 1)// esto lo que hace es cuando si esta en la posicion del mapa da la forma de recolectar recursoso
            {

                int fila = aldeanos[i].ay / TAMANO_CASILLA;
                int colum = aldeanos[i].ax / TAMANO_CASILLA;
                if(fila >= 0 && fila < TAMANO_Y_MAP && colum >= 0 && colum < TAMANO_X_MAP)// aqui ubo cambio 
                {                  
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
                    else if(mapa[fila][colum] == 'T')
                    {
                        aldeanos[i].estado = aldeano_contruyendo;
                    }
                    else// si no esto esta
                    {
                        for(int t = 0; t < MAX_ESTRUCTURA; t++)
                        {
                            if(estructura[t].used && estructura[t].x == colum && estructura[t].y == fila)
                            {
                                aldeanos[i].estado = id_aldeano;
                                estructura[t].estado = contruyendo;
                            }
                        }
                    }
                }
            }
        }
    }
}

void dibujo_aldeano()
{
    for(int i = 0; i < MAX_ALDEANO; i++)
    {
        if(aldeanos[i].used)
        {
            al_draw_filled_rectangle(aldeanos[i].ax , aldeanos[i].ay, aldeanos[i].ax + TAMANO_CASILLA, aldeanos[i].ay + TAMANO_CASILLA, al_map_rgb(0, 255, 1));
            if(aldeanos[i].selec) // esto lo que hace es mostra el aldeano seleccionado 
            {
               al_draw_rectangle(aldeanos[i].ax - 2, aldeanos[i].ay - 2, aldeanos[i].ax + TAMANO_CASILLA + 2, aldeanos[i].ay + TAMANO_CASILLA + 2, al_map_rgb(255,0,0), 2);
            }
        }
    }
}

// torretas
void contruccion(int i)
{
    for(int s = 0; s < MAX_ESTRUCTURA; s++)
    {
        int fila = aldeanos[i].ay / TAMANO_CASILLA;
        int colum = aldeanos[i].ax / TAMANO_CASILLA;

        if(estructura[s].estado == estado_simiento && estructura[s].x == colum && estructura[s].y == fila)
        {   
            estructura[s].tiempo_de_contruccion++;
            printf("contruccion tiempo: %d \n",estructura[s].tiempo_de_contruccion);
            if(estructura[s].tiempo_de_contruccion >= tiempo_contruyendo)
            {
                estructura[s].tiempo_de_contruccion = 0;
                aldeanos[i].estado = id_aldeano;
                estructura[s].estado = listo;
                estructura[s].tier++;
                estructura[s].daño = 2;
            }
        }
    }
}

void update_torreta()
{
    for(int t = 0; t < MAX_ESTRUCTURA; t++)
    {
        int objetivo = -1;
        float distancia_enemy = 10000; // esto va hacer un ejemplo de la distancia maxima de la torreta el alcance y esto sirvira para que ataquen siempre el enemigo mas cercano
        if(estructura[t].used)
        {
            if(estructura[t].estado == listo)
            {
                estructura[t].tiempo_disparo++;
                if(estructura[t].tiempo_disparo >= TIEMPO_DISPARO) // tiempo que tarda en dispara la torreta
                {
                    estructura[t].tiempo_disparo = 0;
                    for(int e = 0; e < MAX_ENEMY; e++)
                    {
                        if(enemy[e].used)
                        {
                            float dx = enemy[e].nx - (estructura[t].x * TAMANO_CASILLA + TAMANO_CASILLA / 2);
                            float dy = enemy[e].ny - (estructura[t].y * TAMANO_CASILLA + TAMANO_CASILLA / 2);

                            printf("enemigo detectado [%d]\n", e);
                            float distancia_corta = sqrt(dx * dx + dy * dy); // lo que hace es trigonometria para detectar uie es el que esta mas seca de la ubicacion  
                            if(distancia_corta < ALCANCE_TOWER && distancia_corta < distancia_enemy)
                            {
                                distancia_enemy = distancia_corta;
                                objetivo = e;
                            }
                        }
                    }
                    if(objetivo != -1)
                    {
                        printf("bang a [%d]\n",objetivo);
                        disparo(estructura[t].x * TAMANO_CASILLA + TAMANO_CASILLA / 2, estructura[t].y * TAMANO_CASILLA + TAMANO_CASILLA / 2,objetivo, t);
                        // aqui deberia estar una funcion que dispare 
                    }
                }
            }
        }
    }
}

void disparo(float x, float y, int objetivo, int t)
{
    for(int p = 0; p < MAX_BULLET; p++)
    {
        if(!bullet[p].used)
        {
            bullet[p].used = true;
            bullet[p].objetivo = objetivo;
            bullet[p].x = x;
            bullet[p].y = y;
            bullet[p].velocidad = 5;
            bullet[p].dano = estructura[t].daño;
            break;
        }
        
    }
}

void update_bullet()
{
    for(int p = 0; p < MAX_BULLET; p++)
    {    
        if(bullet[p].used)
        {
            if(enemy[bullet[p].objetivo].used)
            {
                float dx = enemy[bullet[p].objetivo].nx - bullet[p].x;
                float dy = enemy[bullet[p].objetivo].ny - bullet[p].y;

                float distancia = sqrt(dx*dx + dy*dy);

                if(distancia < 8)
                {
                    enemy[bullet[p].objetivo].life -= bullet[p].dano;
                    bullet[p].used = false;
                }
                else
                {
                    bullet[p].x += dx / distancia * bullet[p].velocidad;
                    bullet[p].y += dy / distancia * bullet[p].velocidad;
                }
            }
            if(!enemy[bullet[p].objetivo].used)
            {
                bullet[p].used = false;
            }
            
        }
    }
}

void dibujo_proyectil()
{
    for(int i=0;i<MAX_BULLET;i++)
    {
        if(bullet[i].used)
        {
            al_draw_filled_circle(bullet[i].x, bullet[i].y, 4, al_map_rgb(255,255,0));
        }
    }
}

// tema de recursos 
void recoleccion(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    for(int i = 0; i < MAX_ALDEANO; i++)
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
                    break;
                            
                    case 'O':
                    stock.oro++;
                    printf("(%f,%f)\n", aldeanos[i].ax, aldeanos[i].ay);
                    break;
                        
                    case 'P':
                    stock.piedra++;
                    printf("(%f,%f)\n", aldeanos[i].ax, aldeanos[i].ay);
                            /*i, ax/TAM_CASILLA y ay/TAM_CASILLA*/
                    break;

                }
                desgaste_material(mapa,i);
                printf("Madera:%d Oro:%d Piedra:%d\n", stock.madera, stock.oro, stock.piedra);
            }
        }
        if(aldeanos[i].estado == aldeano_contruyendo)
        {
            contruccion(i);
        }

    }
}

void generar_recurso(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], PUNTOS *puntos, int i)
{
    int eleccion, intento = 0;

    if(puntos->numero == 0)
    {    
        return;
    }
    while(intento < 100)
    {
        int posicion = rand() % puntos->numero;
        if(!puntos->lista[posicion].used)
        {
            int rx = puntos->lista[posicion].x;
            int ry = puntos->lista[posicion].y;
            eleccion = rand()%3 + 1;
            switch(eleccion)
            {
                case 1:
                mapa[ry][rx] = 'O';
                recursos[i].tipo = 'O';
                recursos[i].durabilidad = 15;
                break;


                case 2:
                mapa[ry][rx] = 'M';
                recursos[i].tipo = 'M';
                recursos[i].durabilidad = 10;
                break;


                case 3:
                mapa[ry][rx] = 'P';
                recursos[i].tipo = 'P';
                recursos[i].durabilidad = 20;
                break;
            }

            recursos[i].x = rx;
            recursos[i].y = ry;
            recursos[i].estado = true;
            puntos->lista[posicion].used = true;
            return;
        }
        intento++;
    }
    
}

void aparicion_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]) // bueno encontre una solucion para esto antes funcionaba y tube que preguntar a la ia es mejor colocar respawn global
{   
    tiempo_spawn--;

    if(tiempo_spawn <= 0)
    {
    for(int i = 0; i < MAX_RECURSOS; i++)
    {
        if(!recursos[i].estado)
        {
            generar_recurso(mapa, &puntos, i);
            tiempo_spawn = TIEMPO_DE_APRICION;
            break;
        }
    }
}
}

void vaciar_recursos_res(int recursos_cargados)
{
    for(int i = recursos_cargados; i < MAX_RECURSOS; i++)
    {
        recursos[i].estado = false;
        recursos[i].tiempo_aparicion = TIEMPO_DE_APRICION;
    }
}

void desgaste_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int i)
{
    for(int m = 0; m < MAX_RECURSOS; m++)
    {
        int fila = aldeanos[i].ay / TAMANO_CASILLA;
        int colum = aldeanos[i].ax / TAMANO_CASILLA;
        if(recursos[m].estado && recursos[m].x == colum && recursos[m].y == fila)
        {
            recursos[m].durabilidad--;
            printf("vida del recurso: %d\n",recursos[m].durabilidad);
            if(recursos[m].durabilidad <=0)
            {
                recursos[m].estado = false;
                recursos[m].tiempo_aparicion = 300; // esto tarda 60 frames por seg estonces son
                mapa[fila][colum] = '.';
                aldeanos[i].estado = id_aldeano;
                aldeanos[i].recurso_obj = '.';
                break;
            }
        }
    }
}

// interaccion de los jugadores 
void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initializa %s\n", description);
    exit(1);
}

int leectura_material_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    int n = 0;
    for(int i = 0; i < TAMANO_Y_MAP; i++)
    {
        for(int j = 0; j < TAMANO_X_MAP; j++)
        {
            if(mapa[i][j] == 'M' || mapa[i][j] == 'P' || mapa[i][j] == 'O')
            {
                recursos[n].x = j;
                recursos[n].y = i;
                recursos[n].tipo = mapa[i][j];
                recursos[n].estado = true;
                recursos[n].tiempo_aparicion = 0;
                switch(mapa[i][j])
                {
                case 'M':
                    recursos[n].durabilidad = 10;
                    break;

                case 'P':
                    recursos[n].durabilidad = 20;
                    break;

                case 'O':
                    recursos[n].durabilidad = 15;
                    break;
                }
            n++;
            }
        }
    }
    return n;
}

void imprimir_boton(int botonx, int botony)
{
    al_draw_filled_rectangle(botonx, botony, botonx + TAMANO_CASILLA + 64, botony + TAMANO_CASILLA, al_map_rgba_f(255,255,0,1));

    // esto deja el simiento 
    for(int i = 0; i < MAX_ESTRUCTURA; i++)
    {
        if(estructura[i].used)
        {
            if(estructura[i].estado == estado_simiento)
            {
                al_draw_filled_rectangle(estructura[i].x * TAMANO_CASILLA, estructura[i].y * TAMANO_CASILLA, estructura[i].x * TAMANO_CASILLA + TAMANO_CASILLA, estructura[i].y * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgb(150,150,150));
            }
            if(estructura[i].estado == listo)
            {
                al_draw_filled_rectangle(estructura[i].x * TAMANO_CASILLA, estructura[i].y * TAMANO_CASILLA, estructura[i].x * TAMANO_CASILLA + TAMANO_CASILLA, estructura[i].y * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgb(150,150,0));
            }
        }
    }
}

void modo_cont(int x ,int y)
{
    if(modo_contrucccion)
    {
        int fila = y / TAMANO_CASILLA;
        int columna = x / TAMANO_CASILLA;

        al_draw_rectangle(columna * TAMANO_CASILLA, fila * TAMANO_CASILLA, columna * TAMANO_CASILLA + TAMANO_CASILLA, fila * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgb(255,0,255), 2);
    }

}

void seleccion_estructura(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP],int x, int y)
{
    int fila = y / TAMANO_CASILLA;
    int columna = x / TAMANO_CASILLA;
    bool ocupado = false;
    if(modo_contrucccion)
    {
        if(fila >= 0 && fila < TAMANO_Y_MAP && columna >= 0 && columna < TAMANO_X_MAP)
        {
            if(mapa[fila][columna] == '.' ||mapa[fila][columna] == 'R')
            {
                for(int i = 0; i < MAX_ESTRUCTURA; i++)
                {
                    if(estructura[i].used && estructura[i].x == columna && estructura[i].y == fila) //  esto lo que hace es evitar que no se sobre ponga la estructura
                    {
                        ocupado = true;
                        break;
                    }               
                }
                if(!ocupado)
                {
                    for(int i = 0; i < MAX_ESTRUCTURA; i++)
                    {
                        if(!estructura[i].used)
                        {
                            estructura[i].x = columna;
                            estructura[i].y = fila;
                            estructura[i].used = true;
                            estructura[i].estado = estado_simiento;
                            modo_contrucccion = false;
                            mapa[fila][columna] = 'T'; 
                            estructura[i].tiempo_de_contruccion = 0;
                            break;
                        }
                    }
                } 
            }
        }
    }
}

void click(int x, int y,int buton, int botonx, int bontony, char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    if(buton== 1) // este es el boton izquierdo 
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
        if(x >= botonx && x <= botonx + TAMANO_CASILLA + 64 && y >= bontony && y <= bontony + TAMANO_CASILLA)
        {
            modo_contrucccion = true;
            printf("modo construccion se ha activado\n");
        }
        if(modo_contrucccion == true)
        {
            seleccion_estructura(mapa,x,y);
        }
    }       
    if(buton == 2) // este boton derecho
    {        
        int fila = y / TAMANO_CASILLA;
        int columna = x / TAMANO_CASILLA;
        if(fila >= 0 && fila < TAMANO_Y_MAP && columna >= 0 && columna < TAMANO_X_MAP && mapa[fila][columna] != '#')
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
    }
}

void movimiento_mouse(float *x, float *y, float *dx, float *dy, unsigned char key[], bool *done, bool *redraw)
{
    if(key[ALLEGRO_KEY_ESCAPE])
    {
        *done = true; 
    }      
    *x += *dx;
    *y += *dy;
    
    if(*x < 0) // esto lo que hace es limitar el espeacio para evitar que salga el cubo o mause personalisado
    {
        *x = 1;
        *dx = 1;
    }
    if(*x > TAMAÑO_X_PT - TAMANO_CASILLA) // para que rebote a la izquierda
    {
        *x -= (*x - (TAMAÑO_X_PT- TAMANO_CASILLA));
        *dx *= -1;   
    }
            
    if(*y < 0)
    {
        *y = 1;
        *dy = 1;
    }

    if(*y > (TAMAÑO_Y_PT - TAMANO_CASILLA))
    {
        *y -= (*y - (TAMAÑO_Y_PT - TAMANO_CASILLA));
        *dy *= -1;
    }
            
    // esto es para dpi para el mause 
    *dx *= DPI;
    *dy *= DPI; 
            
    for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
    {
        key[i] &= ~KEY_SEEN;
    }
    *redraw = true;
}

// esta parte es la generacion de enemigos y interaccion del enemigo

void spwan_enemy()
{
    if (total_spawns <= 0)
    {
        return;
    }
    int i, random;
    for(i=0; i < MAX_ENEMY/* aqui va cantidad maxima que se puede generar los enemigos*/;i++)
    {
        if(!enemy[i].used)
        {
            enemy[i].used = true;
            enemy[i].type = ENEMY_NR;
            enemy[i].life = 3 + 2 * wave;
            enemy[i].damage = wave;

            random = rand()% total_spawns;
            
            enemy[i].psmapx = spawns[random].posx;
            enemy[i].psmapy = spawns[random].posy;
            
            enemy[i].nx = enemy[i].psmapx * TAMANO_CASILLA;
            enemy[i].ny = enemy[i].psmapy * TAMANO_CASILLA;

            enemy[i].anteriorx = -1;
            enemy[i].anteriory = -1;
            enemy[i].siguiente_casilla = false;

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

void eleccion_de_la_siguiente_casilla(int id, char mapa[TAMANO_Y_MAP][TAMANO_X_MAP],int salx, int saly)
{
    int direcciones[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    int opciones_x[4], opciones_y[4];
    int cant = 0, nx, ny, i, j;
    char celda;
    int random;
    int distancia;
    
    // busca la salida o mejor dicho la base del player donde tiene que ir 
   
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
            if((celda == 'r' || celda == 'E') && !casilla_ocupada(nx, ny, id) && !(nx == enemy[id].anteriorx && ny == enemy[id].anteriory))
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
            int mejor_distancia = 10000;
            int mejor_opcion = 0;
            
            if(rand()%5 == 0)
            {
                mejor_opcion = rand()%cant;
            }
            else 
            {
                for(i = 0; i < cant; i++)
                {
                    distancia = abs(opciones_x[i] - salx) + abs(opciones_y[i] - saly);

                    if(distancia < mejor_distancia)
                    {
                        mejor_distancia = distancia;
                        mejor_opcion = i;
                    }
                    else if(distancia <= mejor_distancia + 1)
                    {
                        if(rand()%100 < 30)
                        {
                            mejor_opcion = i;
                        }
                    }
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
            enemy[id].anteriorx = enemy[id].psmapx;
            enemy[id].anteriory = enemy[id].psmapy;
            enemy[id].psmapx = opciones_x[0];
            enemy[id].psmapy = opciones_y[0];
        }
    }
}

void update_enemy(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int salx, int saly) //esto es es el movimiento del enemigo
{
    for(int i = 0; i < MAX_ENEMY; i++)
    {
        
        if(enemy[i].used)
        {
            int destino_x = enemy[i].psmapx * TAMANO_CASILLA;
            int destino_y = enemy[i].psmapy * TAMANO_CASILLA;

            if(enemy[i].nx < destino_x)
            {
                enemy[i].nx++;
                if(enemy[i].nx > destino_x)
                {
                    enemy[i].nx = destino_x;
                }
            }
            else if(enemy[i].nx > destino_x)
            {
                enemy[i].nx--;
                if(enemy[i].nx < destino_x)
                {
                    enemy[i].nx = destino_x;
                }
            }
            if(enemy[i].ny < destino_y)
            {
                enemy[i].ny++;
                if(enemy[i].ny > destino_y)
                {
                    enemy[i].ny = destino_y;
                }
            }
            else if(enemy[i].ny > destino_y)
            {
                enemy[i].ny--;
                if(enemy[i].ny < destino_y)
                {
                    enemy[i].ny = destino_y;
                }
            }
            if(abs(enemy[i].nx - destino_x) <= 8 && abs(enemy[i].ny - destino_y) <= 8 && !enemy[i].siguiente_casilla)
            {
                eleccion_de_la_siguiente_casilla(i, mapa, salx, saly);
                enemy[i].siguiente_casilla = true;
            }
            if(enemy[i].nx == destino_x && enemy[i].ny == destino_y)
            {
                enemy[i].siguiente_casilla = false;
            }
        }
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

// oleadas o wave
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


//mapa
void imprimir_mapa_y_enemigos(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int fila)
{
    for(int i = 0; i < fila; i++)
    {
        for(int j=0; mapa[i][j] != '\0'; j++)
        {
            switch (mapa[i][j])
            {
                case '#':
                // aqui hay que colocar un if  
                bool arriba  = (i > 0 && mapa[i-1][j] == '#');
                bool abajo   = (i < fila-1 && mapa[i+1][j] == '#');
                bool izquierda = (j > 0 && mapa[i][j-1] == '#');
                bool derecha = (mapa[i][j+1] != '\0' && mapa[i][j+1] == '#');         
                if (arriba && derecha)
                {
                    al_draw_bitmap_region(ez, 0, 10, TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA * j, TAMANO_CASILLA * i, 0);
                }
                else if (abajo && derecha)
                {
                    al_draw_bitmap_region(ez4, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA * j, TAMANO_CASILLA * i, 0);
                }
                else if (abajo && izquierda)
                {
                    al_draw_bitmap_region(ez3, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA * j, TAMANO_CASILLA * i, 0);
                }
                else if (arriba && izquierda)
                {
                    al_draw_bitmap_region(ez2, 0, 10, TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA * j, TAMANO_CASILLA * i, 0);
                }
                // lados del mapa
                if(mapa[i][j + 1] == '\n' && i > 0 && i < fila - 1)
                {
                    al_draw_bitmap_region(bor2, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j * TAMANO_CASILLA, i * TAMANO_CASILLA, 0);
                }
                if(j == 0 && i > 0 && i < fila - 1)
                {
                    al_draw_bitmap_region(bor4, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j * TAMANO_CASILLA, i * TAMANO_CASILLA, 0);
                }
                if(i == 0 && j > 0 && mapa[i][j + 2] != '\0')
                {
                    al_draw_bitmap_region(bor3, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j * TAMANO_CASILLA, i * TAMANO_CASILLA, 0);
                }
                if(i == fila - 1 && j > 0 && mapa[i][j + 1] != '\0')
                {
                    al_draw_bitmap_region(bor, 0, 9, TAMANO_CASILLA, TAMANO_CASILLA, j * TAMANO_CASILLA, i * TAMANO_CASILLA, 0);
                }

                break;
                    
                case '.':
                al_draw_bitmap_region(texture_suelo, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j* TAMANO_CASILLA, i* TAMANO_CASILLA, 0);
                break;
                
                case 'r':
                al_draw_bitmap_region(texture_suelo, 165, 10, TAMANO_CASILLA, TAMANO_CASILLA, j* TAMANO_CASILLA, i* TAMANO_CASILLA, 0);
                break;

                case 'S':
                al_draw_bitmap_region(sp, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j* TAMANO_CASILLA, i* TAMANO_CASILLA, 0);
                break;
                
                case 'E':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA,al_map_rgba_f(1, 1, 0, 1));
                break;

                case 'M':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(0, 0, 255, 255) );
                break;

                case 'O':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(1, 255, 0, 1) );
                break;

                case 'P':
                al_draw_filled_rectangle(j * TAMANO_CASILLA, i * TAMANO_CASILLA, j * TAMANO_CASILLA + TAMANO_CASILLA, i * TAMANO_CASILLA + TAMANO_CASILLA, al_map_rgba_f(255, 1, 255, 1) );
                break;
                
                case 'T':
                al_draw_bitmap_region(texture_suelo, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, j * TAMANO_CASILLA, i * TAMANO_CASILLA, 0);
                for(int t = 0; t < MAX_ESTRUCTURA; t++)
                {
                    if(estructura[t].used)
                    {    
                        if (estructura[t].x == j && estructura[t].y == i)
                        {
                            if (estructura[t].tier == 0)
                            {
                                int etapa = (estructura[t].tiempo_de_contruccion * 4) / TIEMPO_CONTRUYENDO;
                                if(etapa > 3)
                                {   
                                    etapa = 3;
                                }
                                switch (etapa)
                                {
                                    case 0:
                                    al_draw_scaled_bitmap(torre, 168, 248, 310, 320, j * TAMANO_CASILLA, i * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA, 0);
                                    break;

                                    case 1:
                                    al_draw_scaled_bitmap(torre, 550, 248, 310, 320, j * TAMANO_CASILLA, i * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA, 0);
                                    break;

                                    case 2:
                                    al_draw_scaled_bitmap(torre, 985, 248, 310, 320., j * TAMANO_CASILLA, i * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA, 0);
                                    break;

                                    case 3:
                                    al_draw_scaled_bitmap(torre, 1350, 248, 310, 320, j * TAMANO_CASILLA, i * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA, 0);
                                    break;
                                }
                            }
                            else if(estructura[t].tier == 1)
                            {
                                al_draw_scaled_bitmap(torre, 1375, 700, 230, 320, j * TAMANO_CASILLA, i * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA, 0);
                            }
                        }
                    }

                }
            }
        }
    }
    al_draw_bitmap(menu, 1568, 0, 0);

}

void imprimir_enemigo()
{
    for(int i=0; i < MAX_ENEMY; i++)
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
            al_draw_scaled_bitmap(enemy1, sx, 0, 96, 96, (enemy[i].nx - 62), (enemy[i].ny - 75), TAMAMANO_ENEMIGO + 13, TAMAMANO_ENEMIGO + 13, 0);
            sx = 70 + sx;
        }
    }
}

void imprimir_texto(ALLEGRO_FONT *font)
{
    char texto[100];
    int x = 55 * TAMANO_CASILLA;
    int y = 20 * TAMANO_CASILLA;
    sprintf(texto, "Madera: %d", stock.madera);
    al_draw_text(font, al_map_rgb(255, 255, 255), x, y, 0, texto);
    
    sprintf(texto,"Piedra: %d", stock.piedra);
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 20+ y, 0, texto);

    sprintf(texto,"   Oro: %d", stock.oro);
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 45 + y, 0, texto);

    sprintf(texto,"wave: %d", wave);
    al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 35, 0, texto);
    for(int i = 0; i < MAX_ENEMY; i++)
    {
        sprintf(texto,"vida: %d",enemy[i].life);
        if(enemy[i].used == true)
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), enemy[i].nx - 5, enemy[i].ny - 25, 0, texto); 
        }
    }
}

void carga_imagen()
{
    texture_suelo = al_load_bitmap("assets/texture_suelo.png");
    enemy1 = al_load_bitmap("assets/RUN.png");
    sp = al_load_bitmap("assets/texture.png");
    ez = al_load_bitmap("assets/ez.png");
    ez2 = al_load_bitmap("assets/ez2.png");
    ez3 = al_load_bitmap("assets/ez3.png");
    ez4 = al_load_bitmap("assets/ez4.png");
    bor4 = al_load_bitmap("assets/bord4.png");
    bor3 = al_load_bitmap("assets/bord3.png");
    bor2 = al_load_bitmap("assets/bord2.png");
    bor = al_load_bitmap("assets/bord.png");
    torre = al_load_bitmap("assets/torre.png");
    menu = al_load_bitmap("assets/gamemenu1.png");
}

int carga_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    puntos.numero = 0;
    FILE *archivo_map = fopen("map.txt", "r");
    if(archivo_map == NULL)
    {
        printf("no se pudo abrir el archivo");
        return 1;
    }
    int fila = 0;

    total_spawns = 0;
    sax = -1;
    say = -1;

    while(fila < TAMANO_Y_MAP && fgets(mapa[fila], TAMANO_X_MAP, archivo_map))
    {
        mapa[fila][strcspn(mapa[fila], "\n")] = '\0';
        fila++;
    }
    fclose(archivo_map);
    for(int i = 0; i < fila; i++)
    {
        for(int j = 0; mapa[i][j] != '\0'; j++)
        {
            if(mapa[i][j] == 'S' && total_spawns < MAX_SPAWNS)
            {
                spawns[total_spawns].posx = j;
                spawns[total_spawns].posy = i;
                total_spawns++;
            }

            if(mapa[i][j] == 'E')
            {
                sax = j;
                say = i;
            }
            if(mapa[i][j] == '.')
            {
                puntos.lista[puntos.numero].x = j;
                puntos.lista[puntos.numero].y = i;
                puntos.lista[puntos.numero].used = false;
                puntos.numero++;
            }
            if(mapa[i][j] == 'A')
            {
                arboles.lista[arboles.numero].x = j;
                arboles.lista[arboles.numero].y = i;
                arboles.lista[arboles.numero].used = false;
                arboles.numero++;
            }
        }
    }
    return fila;
}