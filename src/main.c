#include "commons.h"

/* ================================================================
   MAIN.C — Punto de entrada y game loop
   Este archivo arranca el programa y mantiene el loop corriendo.
   No contiene lógica de juego. Su único trabajo es inicializar
   todo, llamar a los módulos en el orden correcto, y cerrar limpio.
   ================================================================ */

int main(void) {

    /* ------------------------------------------------------------
       1. INICIALIZACIÓN DE ALLEGRO
       ------------------------------------------------------------ */
      al_init();
      al_install_keyboard();
      al_install_mouse();
      al_init_font_addon();
    /* ------------------------------------------------------------
       2. CREAR LA VENTANA Y EL DISPLAY
       ------------------------------------------------------------ */
      ALLEGRO_DISPLAY* disp = al_create_display(320, 200);
    /* ------------------------------------------------------------
       3. CREAR EL EVENT QUEUE Y EL TIMER
       ------------------------------------------------------------ */
      ALLEGRO_TIMER* timer = al_create_timer (1.0 / TARGET_FPS);// Genera TARGET_FPS eventos por segundo para que se actualise el juego.
      ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); // esto lo que hace es recibir las intrucciones del mause o teclado o el tiempo, etc
      ALLEGRO_FONT* font = al_create_builtin_font(); // esto no es tan inportante y hay otras maneras de utilizar texto para imprimir en la pantalla pero estobasicamente sirve para imprimir textos
      al_register_event_source(queue, al_get_keyboard_event_source());// esto sirve para conectar el teclado y detectar las teclas 
      al_register_event_source(queue, al_get_mouse_event_source());// esto es lo mismo que el anterior pero en vez del teclado con el mause
      al_register_event_source(queue, al_get_display_event_source(disp));// esto permite conecta la venta de evento y pueda cerrar la venntana
      al_register_event_source(queue, al_get_timer_event_source(timer));// esto sirve para actualizar los eventos del juego y conectar el temporaisador a la cola de juego
    /* ------------------------------------------------------------
       4. INICIALIZAR EL ESTADO DEL JUEGO Y LOS ASSETS
       ------------------------------------------------------------ */
      GameState game_state;
      InputState input_state;
      al_start_timer(timer);
      game_init(&game_state);
      assets_load();

    /* ------------------------------------------------------------
       5. GAME LOOP
       ------------------------------------------------------------ */
      while (game_state.running)    
      {


        /* -- INPUT -------------------------------------------- */
        input_update(&input_state);

        /* -- UPDATE ------------------------------------------- */
        update(&game_state, &input_state);

        /* -- RENDER ------------------------------------------- */
        render_gameview(&game_state);
        render_ui(&game_state);
      }


    /* ------------------------------------------------------------
       6. CIERRE Y LIBERACIÓN DE RECURSOS
       ------------------------------------------------------------ */
      al_destroy_font(font);
      al_destroy_display(disp);
      al_destroy_timer(timer);
      al_destroy_event_queue(queue);
    return 0;
}