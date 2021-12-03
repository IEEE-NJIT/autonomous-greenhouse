#ifndef _UI_H
#define _UI_H

typedef struct {
    vec3 position;
    vec2 texture;
    vec4 color;
    float spacing;
    float scale;
    uint32_t count;
    char *text;
    Rectangle *texturedQuads;
    uint32_t texturedQuadsAllocated;
} Text;

void createText(Text *text, const char *label, vec3 position, vec4 textColor, float characterSpacing, float scale);
void drawText(Text text, mat4 viewMatrix, mat4 projMatrix);
void changeText( Text *text, const char *label, float characterSpacing, float scale );
void deleteText(Text text);

// Button 

struct Button;
enum ButtonEvent {
    BTN_UNKNOWN,
    BTN_CLICKED,
    BTN_RELEASED,
    BTN_HOVER,
    BTN_EXIT,
};

typedef void (*ButtonCallback)(struct Button*, enum ButtonEvent);

typedef struct Button {
    Text label;
    Rectangle rect;
    vec4 color;
    vec4 highlightColor;
    ButtonCallback callback;
    enum ButtonEvent currentState;
    bool32_t stateChanged;
} Button;

void createButton(Button *btn, const char *label, vec4 labelColor, vec3 position, float height, float width, vec4 color, vec4 highlightColor, ButtonCallback callbackFunction);
void deleteButton(Button btn);
void drawButton(Button btn, mat4 viewMatrix, mat4 projMatrix);
void updateButtonColor(Button *btn, vec4 color);
void changeButtonDefaultColor(Button *btn, vec4 color);

void basicButtonCallback(Button *btn, enum ButtonEvent event);
void buttonHighlighting(Button *btn, enum ButtonEvent event);

// Graph

typedef struct {
    Text label;
    Rectangle background;
    Circle *plots;
    size_t plotCount;
    mat4 modelMatrix;
} Graph;

void createGraph( Graph *graph, const char *label, vec4 labelColor, vec3 position, float height, float width, vec4 backgroundColor );
void deleteGraph( Graph graph );
void drawGraph( Graph graph, mat4 viewMatrix, mat4 projMatrix );

#endif /* _UI_H */