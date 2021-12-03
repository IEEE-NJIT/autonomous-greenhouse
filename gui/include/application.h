#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "display.h"

typedef struct {
    int width, height;
    FILE *plantDataFile;
    Framebuffer framebuffer;
} State;

typedef struct {
    void*  permanentStorage;
    size_t permanentStorageSize;
    void*  transientStorage;
    size_t transientStorageSize;
} Memory;

enum EventType {
    EVENT_UNKNOWN,
    EVENT_STALE,
    // Keyboard/Mouse Input Types
    EVENT_PRESSED,
    EVENT_RELEASED,
    EVENT_REPEAT,

    // Touch Input Types
    EVENT_FINGERDOWN,
    EVENT_FINGERMOTION,
    EVENT_FINGERUP,
};

typedef struct {
    enum EventType state;
    uint32_t key;
    uint16_t mod;
} KeyboardEvent;

typedef struct {
    char text[32];
    int32_t start, length;
} TextEvent;

typedef struct {
    vec2 cursorPosition;
    vec2 lastCursorPosition;
    vec2 cursorDelta;
    enum EventType leftBtn, rightBtn, middleBtn;
    uint8_t numLeftClicks;
    uint8_t numRightClicks;
    uint8_t numMiddleClicks;
    vec2 wheel;
} MouseEvent;

typedef struct {
    enum EventType state;
    vec2 position;
    vec2 delta;
} TouchEvent;

typedef struct {
    KeyboardEvent keyboardEvent;
    TextEvent textEvent;
    MouseEvent mouseEvent;
    TouchEvent touchEvent;
} Input;

void initApplication( Memory *memory, State *state );
void destroyApplication( Memory *memory, State *state );
void processInput( Input *input );
void updateRender( Memory *memory, State *state, Input *input );

void drawFPS( char fps[25] );

void clearStaleInput( Input *input );
/*void updateKeyboardEvent(KeyboardEvent *keyboardEvent, SDL_KeyboardEvent sdlKeyboardEvent, enum EventType type);
void updateTextEvent(TextEvent *textEvent, SDL_TextInputEvent sdlTextInputEvent);
void updateTextEditEvent(TextEvent *textEvent, SDL_TextEditingEvent sdlTextEditingEvent);
void updateMouseButtonEvent(MouseEvent *mouseEvent, SDL_MouseButtonEvent sdlMouseButtonEvent, enum EventType type);
void updateMouseMotionEvent(MouseEvent *mouseEvent, SDL_MouseMotionEvent sdlMouseMotionEvent);
void updateMouseWheelEvent(MouseEvent *mouseEvent, SDL_MouseWheelEvent sdlMouseWheelEvent);
void updateTouchEvent(TouchEvent *touchEvent, SDL_TouchFingerEvent sdlTouchFingerEvent, enum EventType type);
*/
#endif /* _APPLICATION_H */
