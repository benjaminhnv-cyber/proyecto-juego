#include "comons.h"

int main()
{
    int s;
    cero();

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_mouse(), "mouse");
    
    // esssto lo que hace es poner 0 en todo 
    memset(aldeanos,0,sizeof(aldeanos));
    memset(recursos,0,sizeof(recursos));
    memset(estructura,0,sizeof(estructura));
    memset(bullet,0,sizeof(bullet));
    memset(enemy,0,sizeof(enemy));

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

    ALLEGRO_FONT *font = al_load_ttf_font("/usr/share/fonts/opentype/mathjax/MathJax_Caligraphic-Regular.otf", 24, 0);
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
    int botonx = 53 * TAMANO_CASILLA;
    int bontony = 10 * TAMANO_CASILLA;

    int boton_2x = 53 * TAMANO_CASILLA;
    int boton_2y = 20 * TAMANO_CASILLA;

    stock.madera = 20;
    stock.piedra = 0;
    stock.oro = 0;
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
            imprimir_mapa_y_enemigos(mapa,fila);
            imprimir_enemigo();
            dibujo_aldeano();
            al_draw_filled_rectangle(x, y, x + 10, y + 10, al_map_rgb_f(0,1,1));
            imprimir_recursos(mapa,fila);
            imprimir_texto(font);
            dibujo_proyectil();
            imprimir_boton(botonx,bontony);
            modo_cont(x,y);


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
    int i = 0;
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
                
                if(fila >= 0 && fila < TAMANO_Y_MAP && columna >= 0 && columna < TAMANO_X_MAP && mapa[fila][columna] != '#' && mapa[fila][columna] != '\0')
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
                int fila = (aldeanos[i].ay + TAMANO_CASILLA / 2) / TAMANO_CASILLA;
                int colum = (aldeanos[i].ax + TAMANO_CASILLA / 2) / TAMANO_CASILLA;

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
                estructura[s].daño = 2 * estructura[s].tier;
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
        if(bullet[p].objetivo >=0 && bullet[p].objetivo < MAX_ENEMY && enemy[bullet[p].objetivo].used)
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
                printf("Aldeano %d estado:%d recurso:%c\n", i, aldeanos[i].estado, aldeanos[i].recurso_obj);
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
    eleccion = rand()%3 + 1;
    if(eleccion == 2)
    {   
        while(intento < arboles.numero)
        {
            int randoms = rand()% arboles.numero;
            if(!arboles.lista[randoms].used && mapa[arboles.lista[randoms].y][arboles.lista[randoms].x] == 'A')
            {
                int rx = arboles.lista[randoms].x;
                int ry = arboles.lista[randoms].y;

                recursos[i].terreno_original = 'A';

                mapa[ry][rx] = 'M';
                recursos[i].tipo = 'M';

                recursos[i].durabilidad = 10;

                recursos[i].x = rx;
                recursos[i].y = ry;

                recursos[i].estado = true;
                arboles.lista[randoms].used = true;

                recursos[i].i_arbol = randoms;

                return;
            }
            intento++;
        }        
    }
    else
    {
        while(intento < 100)
        {
            int posicion = rand()% puntos->numero;
            if(!puntos->lista[posicion].used)
            {
                int rx = puntos->lista[posicion].x;
                int ry = puntos->lista[posicion].y;

                if(eleccion == 1)
                {
                    recursos[i].terreno_original = '.';
                    mapa[ry][rx] = 'O';
                    recursos[i].tipo = 'O';
                    recursos[i].durabilidad = 15;
                }
                else
                {
                    recursos[i].terreno_original = '.';
                    mapa[ry][rx] = 'P';
                    recursos[i].tipo = 'P';
                    recursos[i].durabilidad = 20;
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
}

void aparicion_material(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP]) // bueno encontre una solucion para esto antes funcionaba y tube que preguntar a la ia es mejor colocar respawn global
{   
    for(int i = 0; i < MAX_RECURSOS; i++)
    {
        if(!recursos[i].estado && recursos[i].respawn)
        {
            recursos[i].tiempo_aparicion--;

            if(recursos[i].tiempo_aparicion <= 0)
            {
                mapa[recursos[i].y][recursos[i].x] = recursos[i].tipo;

                recursos[i].estado = true;
                recursos[i].respawn = false;

                if(recursos[i].tipo == 'M')
                {
                    recursos[i].durabilidad = 10;
                    arboles.lista[recursos[i].i_arbol].used = true;
                }
                else if(recursos[i].tipo == 'O')
                {
                    recursos[i].durabilidad = 15;
                }
                else if(recursos[i].tipo == 'P')
                {
                    recursos[i].durabilidad = 20;
                }
            }
        }
    }
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
                recursos[m].respawn = true;
                recursos[m].tiempo_aparicion = 300;

                mapa[fila][colum] = recursos[m].terreno_original;
                if(recursos[m].tipo == 'M')
                {
                    arboles.lista[recursos[m].i_arbol].used = false;
                }
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
                recursos[n].respawn = false;
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
            if(mapa[fila][columna] == '.' ||mapa[fila][columna] == 'r')
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
                    if(stock.madera < 5)
                    {
                        printf("No hay suficiente madera.\n");
                        modo_contrucccion = false;
                        return;
                    }   
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
                            stock.madera -= 5;
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
            enemy[i].max_life = 3 + 2 * wave;
            enemy[i].life = enemy[i].max_life;
            enemy[i].damage = wave;

            random = rand()% total_spawns;
            
            enemy[i].psmapx = spawns[random].posx;
            enemy[i].psmapy = spawns[random].posy;
            
            enemy[i].nx = enemy[i].psmapx * TAMANO_CASILLA + TAMANO_CASILLA / 2;
            enemy[i].ny = enemy[i].psmapy * TAMANO_CASILLA + TAMANO_CASILLA / 2;

            enemy[i].anteriorx = -1;
            enemy[i].anteriory = -1;
            enemy[i].siguiente_casilla = false;
            enemy[i].retardo_movimiento = rand()% 60;

            enemy_alive++;

            return;
        }
    }
}

bool casilla_ocupada(int x, int y, int id)
{
    for(int i = 0; i < MAX_ENEMY; i++)
    {
        if(i == id)
            continue;

        if(enemy[i].used)
        {
            int enemigo_x = enemy[i].nx / TAMANO_CASILLA;
            int enemigo_y = enemy[i].ny / TAMANO_CASILLA;

            if(enemigo_x == x && enemigo_y == y)
            {
                return true;
            }
            if(enemy[i].psmapx == x && enemy[i].psmapy == y)
            {
                return true;
            }
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
            if(enemy[i].retardo_movimiento > 0)
            {
                enemy[i].retardo_movimiento--;
                continue;
            }
            if(enemy[i].life <= 0)
            {
                enemy[i].used = false;
                enemy_alive--;
                continue;
            }

            int destino_x = enemy[i].psmapx * TAMANO_CASILLA + TAMANO_CASILLA / 2;
            int destino_y = enemy[i].psmapy * TAMANO_CASILLA + TAMANO_CASILLA / 2;

            
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
            if(abs(enemy[i].nx - destino_x) <= 2 && abs(enemy[i].ny - destino_y) <= 2)
            {
                enemy[i].nx = destino_x;
                enemy[i].ny = destino_y;
                eleccion_de_la_siguiente_casilla(i, mapa, salx, saly);
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
    
    for(int b = 0; b < bordes.numero; b++)
    {
        int x = bordes.list[b].x;
        int y = bordes.list[b].y;
        switch(bordes.list[b].tipo)
        {
            case BORDE_ES_SUP_DER:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(ez3, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_ES_INF_DER:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(ez2, 0, 10, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_ES_INF_IZQ:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(ez, 0, 10, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_ES_SUP_IZQ:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(ez4, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_VR_IZQ:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(bor4, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_VR_DER:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(bor2, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_HR_SUP:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(bor3, 0, 0, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;

            case BORDE_HR_INF:
            al_draw_bitmap_region(texture_suelo, bordes.list[b].textura_x, bordes.list[b].textura_Y, TAMANO_CASILLA, TAMANO_CASILLA, bordes.list[b].x * TAMANO_CASILLA, bordes.list[b].y * TAMANO_CASILLA, 0);
            al_draw_bitmap_region(bor, 0, 9, TAMANO_CASILLA, TAMANO_CASILLA, x * TAMANO_CASILLA, y * TAMANO_CASILLA, 0);
            break;
        }
    }
    for(int q = 0; q < puntos.numero; q++)
    {
        al_draw_bitmap_region(texture_suelo, puntos.lista[q].textura_x, puntos.lista[q].textura_y, TAMANO_CASILLA, TAMANO_CASILLA, puntos.lista[q].x * TAMANO_CASILLA, puntos.lista[q].y * TAMANO_CASILLA, 0);
    }
    for(int q = 0; q < rutas.numero; q++)
    {
        al_draw_bitmap_region(texture_suelo, rutas.list[q].textura_x, rutas.list[q].textura_y, TAMANO_CASILLA, TAMANO_CASILLA, rutas.list[q].x * TAMANO_CASILLA, rutas.list[q].y * TAMANO_CASILLA, 0);
    }
    al_draw_scaled_bitmap(menu, 0, 0, al_get_bitmap_width(menu), al_get_bitmap_height(menu), 1568, 0, 352, 1080, 0);
}

void imprimir_recursos(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP], int fila)
{
    for(int i = 0; i < fila; i++)
    {
        for(int j=0; mapa[i][j] != '\0'; j++)
        {
            switch (mapa[i][j])
            {    
    
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
    int x = (52 * TAMANO_CASILLA);
    int y = (5 * TAMANO_CASILLA);
    sprintf(texto, "MATERIALES");
    al_draw_text(font, al_map_rgb(255, 0, 255), 5 + x, y, 0, texto);

    sprintf(texto, "MADERA: %d", stock.madera);
    al_draw_text(font, al_map_rgb(255, 0, 255), 15 + x, 25 + y, 0, texto);
    
    sprintf(texto,"PIEDRA: %d", stock.piedra);
    al_draw_text(font, al_map_rgb(255, 0, 255), 30 + x, 50 + y, 0, texto);

    sprintf(texto,"   ORO: %d", stock.oro);
    al_draw_text(font, al_map_rgb(255, 0, 255), 35 + x, 77 + y, 0, texto);

    sprintf(texto,"WAVE: %d", wave);
    al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 110, 0, texto);

    sprintf(texto, "COSTO:");
    al_draw_text(font, al_map_rgb(255, 0 ,255), 1700, 355, 0, texto);

    sprintf(texto, "5 MADERA");
    al_draw_text(font, al_map_rgb(255, 0, 255), 1670, 380, 0, texto);

    /* 
    if( ESTA COSA ES cuando a seleccionado ya una yorreta para meorar)
    {
        
        sprintf(texto, "COSTO MEJORA");
        al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 450, 0, texto);

        sprintf(texto, "MADERA: %d", stock.madera);
        al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 475, 0, texto);

        sprintf(texto, "PIEDRA: %d", stock.piedra);
        al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 495, 0, texto);

        sprintf(texto, "ORO: %d", stock.oro);
        al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 515, 0, texto);

        sprintf(texto, "MEJORAR");
        al_draw_text(font, al_map_rgb(255, 0, 255), 1700, 540, 0, texto);
        aparece el boton de mejora
        
    }
    */

    for(int i = 0; i < MAX_ENEMY; i++)
    {
        sprintf(texto,"vida: %d",enemy[i].life);
        if(enemy[i].used == true)
        {
            float porcentaje = (float)enemy[i].life / enemy[i].max_life;
            int ancho_barra = 40;
            int altura_barra = 5;

            // fondo de la barra
            al_draw_filled_rectangle(enemy[i].nx - 5, enemy[i].ny - 25, enemy[i].nx - 5 + ancho_barra, enemy[i].ny - 25 + altura_barra, al_map_rgb(100, 100, 100));

            // vida actual
            al_draw_filled_rectangle(enemy[i].nx - 5, enemy[i].ny - 25, enemy[i].nx - 5 + (ancho_barra * porcentaje), enemy[i].ny - 25 + altura_barra, al_map_rgb(0, 255, 0));
        }
    }
}

void carga_imagen()
{
    texture_suelo = al_load_bitmap("assets/grass.png");
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
    ruta_suelo = al_load_bitmap("assets/suelo_ruta.png");
}

int carga_mapa(char mapa[TAMANO_Y_MAP][TAMANO_X_MAP])
{
    FILE *archivo_map = fopen("map.txt", "r");
    if(archivo_map == NULL)
    {
        printf("no se pudo abrir el archivo\n");
        return 1;
    }

    int fila = 0;
    arboles.numero = 0;
    bordes.numero = 0;
    puntos.numero = 0;
    total_spawns = 0;
    rutas.numero = 0;
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
        int ancho = strlen(mapa[i]);
        for(int j = 0; j < ancho; j++)
        {
            if(mapa[i][j] == 'S' && total_spawns < MAX_SPAWNS)
            {
                spawns[total_spawns].posx = j;
                spawns[total_spawns].posy = i;
                total_spawns++;
            }
            if(mapa[i][j] == 'r')
            {
                rutas.list[rutas.numero].textura_x = (rand()% 8) * 32;
                rutas.list[rutas.numero].textura_y = ((rand()% 4) + 4) * 32;

                rutas.list[rutas.numero].x = j;
                rutas.list[rutas.numero].y = i;

                int random = rand()% 18;
                puntos.lista[puntos.numero].textura_x = (random % 8) * 32;
                puntos.lista[puntos.numero].textura_y = ((rand()% 3) + 1) * 32;
                puntos.lista[puntos.numero].x = j;
                puntos.lista[puntos.numero].y = i;
                puntos.lista[puntos.numero].used = true;
                puntos.numero++;

                rutas.numero++;
            }
            if(mapa[i][j] == 'E')
            {
                sax = j;
                say = i;
            }
            if(mapa[i][j] == '.')
            {
                int random = rand()% 18;
                puntos.lista[puntos.numero].textura_x = (random % 8) * 32;
                puntos.lista[puntos.numero].textura_y = ((rand()% 3) + 1) * 32;

                puntos.lista[puntos.numero].x = j;
                puntos.lista[puntos.numero].y = i;
                puntos.lista[puntos.numero].used = false;
                puntos.numero++;
            }
            if(mapa[i][j] == 'A')
            {
                int random = rand()% 18;
                printf("Agregando arbol (%d,%d)\n", j, i);
            
                arboles.lista[arboles.numero].x = j;
                arboles.lista[arboles.numero].y = i;
                arboles.lista[arboles.numero].used = false;

                int randoms = rand()% 18;
                puntos.lista[puntos.numero].textura_x = (randoms % 8) * 32;
                puntos.lista[puntos.numero].textura_y = ((rand()% 3) + 1) * 32;
                puntos.lista[puntos.numero].x = j;
                puntos.lista[puntos.numero].y = i;
                puntos.lista[puntos.numero].used = false;
                puntos.numero++;
                
                arboles.numero++;
            }
            if(mapa[i][j] == '#')
            {
                int b = bordes.numero;

                int random = rand()% 18;
                bordes.list[bordes.numero].textura_x = (random % 8) * 32;
                bordes.list[bordes.numero].textura_Y = (rand()% 3) + 1;

                bordes.list[b].x = j;
                bordes.list[b].y = i;
                // esquinas
                if(i == 0 && j == 0)
                {
                    bordes.list[b].tipo = BORDE_ES_SUP_IZQ;
                }
                else if(i == 0 && j == ancho-1)
                {
                    bordes.list[b].tipo = BORDE_ES_SUP_DER;
                }
                else if(i == fila-1 && j == 0)
                {
                    bordes.list[b].tipo = BORDE_ES_INF_IZQ;
                }
                else if(i == fila-1 && j == ancho-1)
                {
                    bordes.list[b].tipo = BORDE_ES_INF_DER;
                }
                // lados
                else if(i == 0)
                {
                    bordes.list[b].tipo = BORDE_HR_SUP;
                }
                else if(i == fila-1)
                {
                    bordes.list[b].tipo = BORDE_HR_INF;
                }
                else if(j == 0)
                {
                    bordes.list[b].tipo = BORDE_VR_IZQ;
                }
                else if(j == ancho-1)
                {
                    bordes.list[b].tipo = BORDE_VR_DER;
                }
                bordes.numero++;
            }
            
        }
    }
    for(int i = 0; i < arboles.numero; i++)
{
    printf("Arbol %d: (%d,%d) mapa=%c\n",
           i,
           arboles.lista[i].x,
           arboles.lista[i].y,
           mapa[arboles.lista[i].y][arboles.lista[i].x]);
}

return fila;
    return fila;
}