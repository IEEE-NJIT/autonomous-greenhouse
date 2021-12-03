#include "ui.h"

void createText(Text *text, const char *label, vec3 position, vec4 textColor, float characterSpacing, float scale) {
    text->count = strlen(label);
    text->scale = scale;
    text->text = calloc(text->count + 1, sizeof(char));
    strcpy(text->text, label);
    glm_vec3_copy(position, text->position);
    glm_vec4_copy(textColor, text->color);
    text->spacing = characterSpacing;

    text->texturedQuads = calloc(text->count, sizeof(Rectangle));
    text->texturedQuadsAllocated = text->count;
    vec2 textureCoords[4];
    stbtt_aligned_quad q;

    position[0] = position[0]/scale;
    position[1] = position[1]/scale;
    
    for (uint32_t i = 0; i < text->count; ++i) {
        if (label[i] >= 32 /*&& label[i] < 128*/) {
            stbtt_GetBakedQuad(cdata, 512, 512, label[i]-32, &position[0], &position[1], &q, 1); // 1=opengl & d3d10+,0=d3d9

            textureCoords[0][0] = q.s0;
            textureCoords[0][1] = q.t0;

            textureCoords[1][0] = q.s1;
            textureCoords[1][1] = q.t0;

            textureCoords[2][0] = q.s0;
            textureCoords[2][1] = q.t1;

            textureCoords[3][0] = q.s1;
            textureCoords[3][1] = q.t1;

            //createTexturedRectangle( &(text->texturedQuads[i]), fontShader, (vec3){q.x0, q.y0, position[2]}, q.y1-q.y0, q.x1-q.x0, textureCoords, textColor );

            createTexturedRectangle( &(text->texturedQuads[i]), fontShader, (vec3){q.x0 * scale, q.y0 * scale, position[2]}, (q.y1-q.y0)*scale, (q.x1-q.x0)*scale, textureCoords, textColor );
            //position[0] -= (q.y1-q.y0) - ((q.y1-q.y0)*0.8f);
            position[0] += characterSpacing;
        }
    }

}

void drawText(Text text, mat4 viewMatrix, mat4 projMatrix) {
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    for( uint32_t i = 0; i < text.count; ++i )
        drawRectangle( text.texturedQuads[i], viewMatrix, projMatrix );
    //glBindTexture(GL_TEXTURE_2D, 0);
}

void changeText( Text *text, const char *label, float characterSpacing, float scale ) {
    text->spacing = characterSpacing;
    text->scale = scale;
    uint32_t newCount = strlen( label );
    if( text->count < newCount ) {
        text->text = realloc( text->text, (newCount + 1) * sizeof(char) );
    }
    text->count = newCount;
    memset( text->text, 0, text->count+1 );
    strcpy( text->text, label );

    vec2 textureCoords[4];
    stbtt_aligned_quad q;

    vec3 position = {text->position[0]/scale, text->position[1]/scale, text->position[2]};

    uint32_t characterIndex;
    
    for( characterIndex = 0; characterIndex < text->count && characterIndex < text->texturedQuadsAllocated; ++characterIndex ) {
        if (label[characterIndex] >= 32 /*&& label[i] < 128*/) {
            stbtt_GetBakedQuad(cdata, 512, 512, label[characterIndex]-32, &position[0], &position[1], &q, 1); // 1=opengl & d3d10+,0=d3d9

            textureCoords[0][0] = q.s0;
            textureCoords[0][1] = q.t0;

            textureCoords[1][0] = q.s1;
            textureCoords[1][1] = q.t0;

            textureCoords[2][0] = q.s0;
            textureCoords[2][1] = q.t1;

            textureCoords[3][0] = q.s1;
            textureCoords[3][1] = q.t1;

            changeRectangleProperties( &(text->texturedQuads[characterIndex]), (vec3){q.x0 * scale, q.y0 * scale, position[2]}, (q.y1-q.y0)*scale, (q.x1-q.x0)*scale, textureCoords, text->color );

            //createTexturedRectangle( &(text->texturedQuads[i]), fontShader, (vec3){q.x0 * scale, q.y0 * scale, position[2]}, (q.y1-q.y0)*scale, (q.x1-q.x0)*scale, textureCoords, textColor );
            //position[0] -= (q.y1-q.y0) - ((q.y1-q.y0)*0.8f);
            position[0] += characterSpacing;
        }
    }

    if( text->count > text->texturedQuadsAllocated ) {
        text->texturedQuads = realloc( text->texturedQuads, text->count * sizeof(Rectangle) );
        text->texturedQuadsAllocated = text->count;

        for( ; characterIndex < text->count; ++characterIndex ) {
            if (label[characterIndex] >= 32 /*&& label[i] < 128*/) {
                stbtt_GetBakedQuad(cdata, 512, 512, label[characterIndex]-32, &position[0], &position[1], &q, 1); // 1=opengl & d3d10+,0=d3d9

                textureCoords[0][0] = q.s0;
                textureCoords[0][1] = q.t0;

                textureCoords[1][0] = q.s1;
                textureCoords[1][1] = q.t0;

                textureCoords[2][0] = q.s0;
                textureCoords[2][1] = q.t1;

                textureCoords[3][0] = q.s1;
                textureCoords[3][1] = q.t1;

                //createTexturedRectangle( &(text->texturedQuads[i]), fontShader, (vec3){q.x0, q.y0, position[2]}, q.y1-q.y0, q.x1-q.x0, textureCoords, textColor );

                createTexturedRectangle( &(text->texturedQuads[characterIndex]), fontShader, (vec3){q.x0 * scale, q.y0 * scale, position[2]}, (q.y1-q.y0)*scale, (q.x1-q.x0)*scale, textureCoords, text->color );
                //position[0] -= (q.y1-q.y0) - ((q.y1-q.y0)*0.8f);
                position[0] += characterSpacing;
            }
        }
    }
}

void deleteText(Text text) {
    free( text.text );
    for( uint32_t i = 0; i < text.texturedQuadsAllocated; ++i ) 
        deleteRectangle( text.texturedQuads[i] );
}

// Button

void createButton(Button *btn, const char *label, vec4 labelColor, vec3 position, float height, float width, vec4 color, vec4 highlightColor, ButtonCallback callbackFunction) {

    glm_vec4_copy(labelColor, btn->label.color);

    createColoredRectangle(&(btn->rect), position, height, width, color);
    createText(&(btn->label), label, position, labelColor, 0.0f, 0.8f);

    glm_vec4_copy(color, btn->color);
    glm_vec4_copy(highlightColor, btn->highlightColor);
    btn->callback = callbackFunction;
    btn->currentState = BTN_UNKNOWN;
    btn->stateChanged = 0;
}

void deleteButton(Button btn) {
    deleteText(btn.label);
    deleteRectangle(btn.rect);
}

void drawButton(Button btn, mat4 viewMatrix, mat4 projMatrix) {
    drawRectangle(btn.rect, viewMatrix, projMatrix);
    drawText(btn.label, viewMatrix, projMatrix);
}

void updateButtonColor(Button *btn, vec4 color) {
    btn->color[0] = color[0];
    btn->color[1] = color[1];
    btn->color[2] = color[2];
    btn->color[3] = color[3];
}

void buttonHighlighting(Button *btn, enum ButtonEvent event) {
    if (event == BTN_HOVER) {
        changeRectangleColor(&(btn->rect), btn->highlightColor);
    } else if (event == BTN_CLICKED) {
        changeRectangleColor(&(btn->rect), (vec4){btn->highlightColor[0] - 0.1f, btn->highlightColor[1] - 0.1f, btn->highlightColor[2] - 0.1f, btn->highlightColor[3]});
    } else if (event == BTN_RELEASED) {
        changeRectangleColor(&(btn->rect), btn->color);
    } else if (event == BTN_EXIT) {
        changeRectangleColor(&(btn->rect), btn->color);
    }
}

/*
    typedef struct {
    Text label;
    Rectangle background;
    Circle *plots;
    size_t plotCount;
    } Graph;
*/

void createGraph( Graph *graph, const char *label, vec4 labelColor, vec3 position, float height, float width, vec4 backgroundColor ) {
    createColoredRectangle( &graph->background, (vec3){position[0], position[1]+10.0f, position[2]+1.0f}, height, width, backgroundColor );
    createText( &graph->label, label, position, labelColor, 0.0f, 0.5f );
}

void deleteGraph( Graph graph ) {
    deleteText( graph.label );
    deleteRectangle( graph.background );
}

void drawGraph( Graph graph, mat4 viewMatrix, mat4 projMatrix ) {
    drawRectangle( graph.background, viewMatrix, projMatrix );
    drawText( graph.label, viewMatrix, projMatrix );
}