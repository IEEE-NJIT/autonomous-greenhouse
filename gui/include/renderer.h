#ifndef _RENDERER_H
#define _RENDERER_H


typedef struct {
    vec3 position;
    vec4 color;
    vec2 texCoords;
} Vertex;

#include "ogl.h"

enum TextureType {
    diffuse,
    specular,
};

typedef struct {
    uint32_t id;
    enum TextureType type;
} Texture;

typedef struct {
    uint32_t shaderProg, VBO, IBO;
    int32_t positionLoc, colorLoc, textureLoc, matrixLoc;

    uint32_t vertexCount;
    Vertex *vertices;
    uint32_t indexCount;
    uint32_t *indices;
} Mesh;

// Mesh
void createMesh( Mesh *mesh, uint32_t shaderProg, Vertex *vertices, uint32_t vertexCount, uint32_t *indices, uint32_t indexCount );
void drawMesh( Mesh mesh, mat4 modelMatrix, mat4 viewMatrix, mat4 projMatrix );
void deleteMesh( Mesh mesh );

// Rectangle
uint32_t basicRGBShader;
uint32_t textureShader;
uint32_t fontShader;

typedef struct {
    vec3 position;
    vec4 color;
    Mesh mesh;
    mat4 modelMatrix;
    float height;
    float width;
} Rectangle;

void createColoredRectangle( Rectangle *rect, vec3 position, float height, float width, vec4 color );
void createTexturedRectangle( Rectangle *rect, uint32_t shader, vec3 position, float height, float width, vec2 textureCoord[4], vec4 color );
void drawRectangle( Rectangle rect, mat4 viewMatrix, mat4 projMatrix );
void changeRectangleProperties( Rectangle *rect, vec3 position, float height, float width, vec2 textureCoords[4], vec4 color );
void deleteRectangle( Rectangle rect );
int checkCoordInsideRectangle( float x, float y, Rectangle rect );
void changeRectangleColor( Rectangle *rect, vec4 color );

// Circle

typedef struct {
    vec3 position;
    vec4 color;
    Mesh mesh;
    mat4 modelMatrix;
    float radius;
} Circle;

void createColoredCircle( Circle *circle, vec3 position, float radius, vec4 color );
void drawCircle( Circle circle, mat4 viewMatrix, mat4 projMatrix );
void deleteCircle( Circle circle );


// Font
void initFont(const char *filePath);
void destroyFont();



#endif /* _RENDERER_H */