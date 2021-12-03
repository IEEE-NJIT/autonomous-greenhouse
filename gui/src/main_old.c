#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <bcm_host.h>

#define CNFGCONTEXTONLY
#define CNFG_IMPLEMENTATION
//#include "thirdparty/rawdraw_sf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "cglm/cglm.h"

#define PI  3.14159265359

#include "util.h"

#include <SDL2/SDL.h>

bool32_t running = 1;
#define DISPLAY_FPS 0
#define PLANT_DATA_FILE     "plant_data_example.txt"

#include "ogl.c"
#include "renderer.c"
#include "ui.c"
#include "application.c"

#ifdef HEADLESS_PI
//#include "raspi.c"
#endif

int main( int argc, char *argv[] ) {

    Input input;
    Memory memory;
    State state;
    initApplication( &memory, &state );

    SDL_Event Event;

    TimeInterval timer;
    char fps[25];
    memset( fps, 0, 25 );
    float delta = 0.0f;

    uint32_t frameCounter = 0;

    while( running ) {
        startTimeInterval( &timer );
        
        processInput( &input );
        updateRender( &memory, &state, &input );

        endTimeInterval( &timer );

        #if DISPLAY_FPS
        if( frameCounter % 5 == 0 ) {
            memset( fps, 0, 25 );
            delta = calcTimeDelta( timer );
            int frameRate = (int)(1.0f/delta);

            sprintf(fps, "%d", frameRate);
            fps[24] = 0;
            //printf("FPS: %s\n", fps);
            //fflush(stdout);
            //printf("\e[2J");
        }
        drawFPS( fps );
        #endif

        frameCounter++;
    }

    destroyApplication( &memory, &state );

    return 0;
} 
