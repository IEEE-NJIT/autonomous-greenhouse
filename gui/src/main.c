#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "thirdparty/stb_truetype.h"

#include "thirdparty/cglm/cglm.h"

#include "util.h"

bool32_t running = 1;
#define DISPLAY_FPS 1
#define PLANT_DATA_FILE	"plant_data_example.txt"

#include "ogl.c"
#include "renderer.c"
#include "ui.c"
#include "application.c"
#include "display.c"

int main( int argc, char *argv[] ) {
   
    Input input;
    Memory memory;
    State state;
    initApplication( &memory, &state );

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
