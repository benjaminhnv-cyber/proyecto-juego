#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


// esto lo que hace es revisar si funciona si no lo que hace es avisar lo que falla al jugar
void must_init(bool test, const char *description)
{
    if(test) 
    return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_mouse(), "mouse");

    // es lo que actualiza la pantalla va a 60 fps
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    // esto lo que hace es guardado de informacion de cada e vento lo que transcurre 
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    // esto lo que hace es el tamaño de la pantalla 
    ALLEGRO_DISPLAY* disp = al_create_display(750, 750);
    must_init(disp, "display");

    // esto lo que crea son las estructura como un cubo u otras cosas circulos , etc ;esto lo que pienso puede servir como hitbox del juego 
    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    //esto lo que hace es el registro de lo eventos 
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());  
 
    // ubicacion  del cubo 
    float x,y;
    x=0;
    y=0;

    float dx,dy;
    dx=0;
    dy=0;


    al_grab_mouse(disp);
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    ALLEGRO_KEYBOARD_STATE ks;

    #define KEY_SEEN     1
    #define KEY_DOWN     2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_start_timer(timer);
    while(1)
    {
        // esto lo que hace es espera un evento y lo guarda el evento
        al_wait_for_event(queue, &event);

       switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // esto lo que hace es si recibe un ESC del teclado termina cierra la ventanañ
                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                x += dx;
                y += dy;
                
                //esto lo que hace es basicamente si el numero de la posicion x es menor a cero le pone un limite en el cubo y rebota y multiplica por el numero negativo y pasa positivo
                if(x < 0)
                {
                    x *= -1;
                    dx *= -1;
                }

                // lo mismo pero rebota en la pared derecha
                if(x > 510)
                {
                    x -= (x - 510) * 2;
                    dx *= -1;
                }

                //esto pasa lo mismo que rebota hacia abajo
                if(y < 0)
                {
                    y *= -1;
                    dy *= -1;
                }

                // paso lo  mismo rebota hacia arriba
                if(y > 480)
                {
                    y -= (y - 480) * 2;
                    dy *= -1;
                }   

                // esto frena el movimiento del cubo o puedes decir que es para dps o el que sea mas pesado el cubo
                dx *= 0.5;
                dy *= 0.5;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= ~KEY_SEEN;

                redraw = true;
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

        //corta o termina el juego
        if(done)
            break;
        

        // esto lo que hace es dibuje la pantalla 
        if(redraw && al_is_event_queue_empty(queue))
        {
            // es el fondo de la ventana que se genera 
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // esto lo que dibuja por pantalla el cuadrado x , y es la ubicacion donde empiesa a dibujar y el x+? y el y+? es el tamaño del cubo    
            // esto lo que al_map_rgba_f es el color del cubo  tambien o hacerlo invisible 
	        al_draw_filled_rectangle(x, y, x+250, y+250, al_map_rgba_f(0, 1, 1,0));

            // esto es lo que imprime por pantalla del computador 
            al_flip_display();

            //esto lo que indica que ya no va a dibugar  cuando de nuevo pase por el whyle
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

