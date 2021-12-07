#include "application.h"

#include "display.h"
#include "renderer.h"

#define WHITE   (vec4) {1.0f, 1.0f, 1.0f, 1.0f}
#define BLACK   (vec4) {0.0f, 0.0f, 0.0f, 1.0f}
#define RED     (vec4) {1.0f, 0.0f, 0.0f, 1.0f}
#define GREEN   (vec4) {0.0f, 1.0f, 0.0f, 1.0f}
#define BLUE    (vec4) {0.0f, 0.0f, 1.0f, 1.0f}
#define GRAY    (vec4) {0.5f, 0.5f, 0.5f, 1.0f}

Text plantText;
Text daysSinceText;
Text currentPlantTime;

Rectangle plantTextRect;
Rectangle daysSinceTextRect;
Rectangle currentPlantTimeRect;

Circle testCircle;

Graph soilMoistureGraph;
Graph solarIntensityGraph;

Text fpsCounter;
Rectangle background;
Rectangle textRect;

Button testButton;
uint32_t textureId;

Rectangle graphBackground1, graphBackground2;

void testButtonCallback( Button *btn, enum ButtonEvent event ) {
    if( btn->stateChanged ) {
        if( event == BTN_HOVER ) {
            printf("Btn hovered\n");
        } else if( event == BTN_RELEASED ) {
            printf("Btn released\n");
        } else if( event == BTN_CLICKED ) {
            printf("Btn clicked\n");
        }
    }
    btn->stateChanged = 0;
}

typedef struct {
    int second;
    int minute;
    int hour;
    int mday;
    int mon;
    int year;
    int wday;
    int yday;
    int isdst;
} MyTime;

void initApplication( Memory *memory, State *state ) {
    state->width = 800; // Resolution of touch screen
    state->height = 480;

    state->framebuffer = openFramebuffer( 0 );
    initializeEGL( state->framebuffer.height, state->framebuffer.width );

    //initializeEGL( state->height, state->width );

    /*
    state->plantDataFile = fopen( PLANT_DATA_FILE, "w+" );
    if( !state->plantDataFile ) {
        fprintf( stderr, "Could not open file %s\n", PLANT_DATA_FILE );
        ASSERT( NULL );
    }
    
    time_t rawTime;
    struct tm *timeInfo;
    time( &rawTime );
    timeInfo = localtime( &rawTime );
    printf( "Current local time and data: %s\n", asctime(timeInfo) );
    printf( "Raw Time: %ld\n", rawTime );
    MyTime currentTime;
    memcpy( &currentTime, timeInfo, sizeof(MyTime) );

    time_t startTime = 1637348313;
    struct tm *oldTime = localtime( &startTime );
    printf( "Current local time and data: %s\n", asctime(oldTime) );
    
    printf( "Delta time: %dh %dm %ds\n", currentTime.hour - oldTime->tm_hour, currentTime.minute - oldTime->tm_min, currentTime.second - oldTime->tm_sec );
    */

    glClearColor( 0.3f, 0.3f, 0.3f, 0.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    glEnable( GL_STENCIL_TEST );
    glStencilMask( 0x00 );

    glClearDepthf(1.0f);

    basicRGBShader = oglLoadProgram( "shaders/basic.vert", "shaders/basic.frag" );
    textureShader = oglLoadProgram( "shaders/texture.vert", "shaders/texture.frag" );
    fontShader = oglLoadProgram( "shaders/font.vert", "shaders/font.frag" );

    initFont( "assets/fonts/bitstream_vera_mono/VeraMono.ttf" );
    //initFont( "assets/fonts/montserrat/Montserrat-Bold.ttf" );

    //createColoredRectangle( &background, (vec3){250.0f, 10.0f, -1.0f}, 200.0f, 200.0f, (vec4){0.5f, 0.0f, 0.8f, 1.0f} );
    createText( &plantText, "Pea Shoots", (vec3){20.0f, 80.0f, -1.0f}, WHITE, 0.0f, 1.0f );
    createText( &daysSinceText, "Days Since Planted", (vec3){20.0f, 160.0f, -1.0f}, WHITE, 0.0f, 0.6f );
    createText( &currentPlantTime, "00y 00m 00w 00d 00h 00m", (vec3){20.0f, 200.0f, -1.0f}, WHITE, 0.0f, 0.5f );

    //createColoredCircle( &testCircle, (vec3){20.0f, 240.0f, 10.0f}, 5.0f, BLACK );

    
    #if DISPLAY_FPS
    createText( &fpsCounter, "00", (vec3){740.0f, 40.0f, -10.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.8f );
    #endif

    /*vec2 textureCoords[4] = {
        {0.0f, 0.0f}, 
        {1.0f, 0.0f}, 
        {0.0f, 1.0f}, 
        {1.0f, 1.0f}
    };*/
    vec2 textureCoords[4] = {
        {1.0f, 1.0f}, 
        {0.0f, 1.0f}, 
        {1.0f, 0.0f}, 
        {0.0f, 0.0f}
    };

    textureId = oglTextureLoad( "assets/textures/sapling_sprouting.jpeg" );
    createTexturedRectangle( &background, textureShader, (vec3){0.0f, 0.0f, 20.0f}, state->height, state->width, textureCoords, (vec4){1.0f, 1.0f, 1.0f, 1.0f} );
    vec4 backgroundColor = {29.0f/255.0f, 61.0f/255.0f, 26.0f/255.0f, 1.0f};
    //createColoredRectangle( &background, (vec3){0.0f, 0.0f, 20.0f}, state->height, state->width, backgroundColor );

    //createButton( &testButton, "Button", (vec4){1.0f, 1.0f, 1.0f, 1.0f}/*label color*/, (vec3){300.0f, 400.f, -2.0f}/*position*/, 100.0f /*height*/, 300.0f /*width*/, (vec4){0.4f, 0.9f, 0.2f, 1.0f}/*color*/ , (vec4){0.5f, 0.9f, 0.3f, 1.0f} /*highlightColor*/, &testButtonCallback );

    float graphHeight = 280;
    float graphWidth = 370;

    createGraph( &soilMoistureGraph, "Soil Moisture", WHITE, (vec3){20.0f, state->height-(graphHeight+20.0f), -10.0f}, graphHeight, graphWidth, (vec4) {0.6f, 0.7f, 0.6f, 0.5f} );
    createGraph( &solarIntensityGraph, "Solar Intensity", WHITE, (vec3){state->width-(graphWidth+20.0f), state->height-(graphHeight+20.0f), -10.0f}, graphHeight, graphWidth, (vec4) {0.6f, 0.7f, 0.6f, 0.5f} );

    //createColoredRectangle( &graphBackground1, (vec3){20.0f, state->height-(graphHeight+20.0f), -10.0f}, graphHeight, graphWidth, (vec4) {0.6f, 0.7f, 0.6f, 0.5f} );
    //createColoredRectangle( &graphBackground2, (vec3){state->width-(graphWidth+20.0f), state->height-(graphHeight+20.0f), -10.0f}, graphHeight, graphWidth, (vec4) {0.6f, 0.7f, 0.6f, 0.5f} );
}

void destroyApplication( Memory *memory, State *state ) {
    //oglDestroyContext();
    //fclose( state->plantDataFile );

    closeEGL();
    closeFramebuffer( &state->framebuffer );

}

void processInput( Input *input ) {
    clearStaleInput( input );
    
}

mat4 viewMatrix;
mat4 projMatrix;

// Ortho default cam position
float camPosX = 0.0f;
float camPosY = 0.0f;
float camPosZ = 30.0f;

void updateRender( Memory *memory, State *state, Input *input ) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm_translate_make(viewMatrix, (vec3) {camPosX, -camPosY, -camPosZ});
    //glm_ortho(0.0f, state->width, 0.0f, state->height, 0.1f, 1000.0f, projMatrix);
    glm_ortho(0.0f, state->width, state->height, 0.0f, 0.1f, 1000.0f, projMatrix);

    glBindTexture( GL_TEXTURE_2D, textureId );
    //drawRectangle( textRect, viewMatrix, projMatrix );

    //drawCircle( testCircle, viewMatrix, projMatrix );

    drawRectangle( background, viewMatrix, projMatrix );

    drawGraph( soilMoistureGraph, viewMatrix, projMatrix );
    drawGraph( solarIntensityGraph, viewMatrix, projMatrix );


    // Hacky button
    // Check for button event
    /*{
        mat4 viewMatInv;
        glm_mat4_inv_fast(viewMatrix, viewMatInv);
        vec3 mouseWorldSpace;
        glm_mat4_mulv3(viewMatInv, (vec3){input->mouseEvent.cursorPosition[0], input->mouseEvent.cursorPosition[1], 1.0f}, 1.0f, mouseWorldSpace);
        int mouseInsideButton = checkCoordInsideRectangle(mouseWorldSpace[0], mouseWorldSpace[1], testButton.rect);
        if (mouseInsideButton) {
            if (testButton.currentState != BTN_CLICKED && input->mouseEvent.leftBtn == EVENT_PRESSED) {
                testButton.currentState = BTN_CLICKED;
                testButton.stateChanged = 1;
                buttonHighlighting(&(testButton), BTN_CLICKED);
                testButton.callback(&(testButton), BTN_CLICKED);
            } else if (testButton.currentState != BTN_RELEASED && input->mouseEvent.leftBtn == EVENT_RELEASED) {
                testButton.currentState = BTN_RELEASED;
                testButton.stateChanged = 1;
                buttonHighlighting(&(testButton), BTN_RELEASED);
                testButton.callback(&(testButton), BTN_RELEASED);
            } else if (testButton.currentState != BTN_HOVER && testButton.currentState != BTN_CLICKED) {
                testButton.currentState = BTN_HOVER;
                testButton.stateChanged = 1;
                buttonHighlighting(&(testButton), BTN_HOVER);
                testButton.callback(&(testButton), BTN_HOVER);
            }
        } else if (testButton.currentState != BTN_EXIT && !mouseInsideButton) {
            testButton.currentState = BTN_EXIT;
            testButton.stateChanged = 1;
            buttonHighlighting(&(testButton), BTN_EXIT);
            testButton.callback(&(testButton), BTN_EXIT);
        }
        drawButton(testButton, viewMatrix, projMatrix);
    }*/
    
    drawText( plantText, viewMatrix, projMatrix );
    drawText( daysSinceText, viewMatrix, projMatrix );
    drawText( currentPlantTime, viewMatrix, projMatrix );

    #if DISPLAY_FPS
    drawText( fpsCounter, viewMatrix, projMatrix );
    #endif

    //renderGLToFramebuffer( state->framebuffer );
 
    #if RPI4
    gbmSwapBuffers( &eglWindow.display, &eglWindow.surface );
    #endif /* RPI4 */

    //oglSwapBuffers();
}

void drawFPS( char fps[25] ) {
    #if DISPLAY_FPS
    changeText( &fpsCounter, fps, 0.0f, 0.8f );
    #endif
}

#define EVENT_DEBUG_MESSAGES 0

/* void updateKeyboardEvent(KeyboardEvent *keyboardEvent, SDL_KeyboardEvent sdlKeyboardEvent, enum EventType type) {
    keyboardEvent->state = type;
    if (sdlKeyboardEvent.repeat) {
        keyboardEvent->state = EVENT_REPEAT;
    }
    keyboardEvent->key = sdlKeyboardEvent.keysym.sym;
    keyboardEvent->mod = sdlKeyboardEvent.keysym.mod;
    if (type == EVENT_PRESSED)
        printf("Keyboard Event: [%d, %c]\n", keyboardEvent->mod, keyboardEvent->key);
}

void updateTextEvent(TextEvent *textEvent, SDL_TextInputEvent sdlTextInputEvent) {
    strcpy(textEvent->text, sdlTextInputEvent.text);
    textEvent->start = 0;
    textEvent->length = 0;
    #if EVENT_DEBUG_MESSAGES
    printf("Text Input: %s\n", textEvent->text);
    #endif
}

void updateTextEditEvent(TextEvent *textEvent, SDL_TextEditingEvent sdlTextEditingEvent) {
    strcpy(textEvent->text, sdlTextEditingEvent.text);
    textEvent->start = sdlTextEditingEvent.start;
    textEvent->length = sdlTextEditingEvent.length;
    #if EVENT_DEBUG_MESSAGES
    printf("Text Edit: %d %d %s\n", textEvent->start, textEvent->length, textEvent->text);
    #endif
}

void updateMouseButtonEvent(MouseEvent *mouseEvent, SDL_MouseButtonEvent sdlMouseButtonEvent, enum EventType type) {
    mouseEvent->cursorPosition[0] = sdlMouseButtonEvent.x;
    mouseEvent->cursorPosition[1] = sdlMouseButtonEvent.y;
    switch (sdlMouseButtonEvent.button) {
        case SDL_BUTTON_LEFT:
            mouseEvent->leftBtn = type;
            break;
        case SDL_BUTTON_RIGHT:
            mouseEvent->rightBtn = type;
            break;
        case SDL_BUTTON_MIDDLE:
            mouseEvent->middleBtn = type;
            break;
        default:
            break;
    }

    #if EVENT_DEBUG_MESSAGES
    printf("Mouse Button [%d, %d, %d]\n", mouseEvent->leftBtn, mouseEvent->middleBtn, mouseEvent->rightBtn);
    #endif
}

void updateMouseMotionEvent(MouseEvent *mouseEvent, SDL_MouseMotionEvent sdlMouseMotionEvent) {
    mouseEvent->cursorPosition[0] = sdlMouseMotionEvent.x;
    mouseEvent->cursorPosition[1] = sdlMouseMotionEvent.y;
    mouseEvent->cursorDelta[0] = sdlMouseMotionEvent.xrel;
    mouseEvent->cursorDelta[1] = sdlMouseMotionEvent.yrel;
    #if EVENT_DEBUG_MESSAGES
    printf("Mouse motion position:[%f, %f] rel:[%f, %f]\n", mouseEvent->cursorPosition[0], mouseEvent->cursorPosition[1], mouseEvent->cursorDelta[0], mouseEvent->cursorDelta[1]);
    #endif
}

void updateMouseWheelEvent(MouseEvent *mouseEvent, SDL_MouseWheelEvent sdlMouseWheelEvent) {
    mouseEvent->wheel[0] = sdlMouseWheelEvent.x;
    mouseEvent->wheel[1] = sdlMouseWheelEvent.y;
    #if EVENT_DEBUG_MESSAGES
    printf("Mouse wheel [%f, %f]\n", mouseEvent->wheel[0], mouseEvent->wheel[1]);
    #endif
}

void updateTouchEvent(TouchEvent *touchEvent, SDL_TouchFingerEvent sdlTouchFingerEvent, enum EventType type) {
    touchEvent->state = type;
    touchEvent->position[0] = sdlTouchFingerEvent.x;
    touchEvent->position[1] = sdlTouchFingerEvent.y;
    touchEvent->delta[0] = sdlTouchFingerEvent.dx;
    touchEvent->delta[1] = sdlTouchFingerEvent.dy;
} */

void clearStaleInput( Input *input ) {
    input->keyboardEvent.state  = EVENT_STALE;
    input->mouseEvent.leftBtn   = EVENT_STALE;
    input->mouseEvent.rightBtn  = EVENT_STALE;
    input->mouseEvent.middleBtn = EVENT_STALE;
    glm_vec2_copy( input->mouseEvent.cursorPosition, input->mouseEvent.lastCursorPosition );
    glm_vec2_copy( (vec2){0.0f, 0.0f}, input->mouseEvent.cursorDelta );
    input->mouseEvent.numLeftClicks   = 0;
    input->mouseEvent.numRightClicks  = 0;
    input->mouseEvent.numMiddleClicks = 0;
    input->touchEvent.state = EVENT_STALE;
}
