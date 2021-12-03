#include "renderer.h"
#include "ogl.h"
#include "display.h"

void createMesh(Mesh *mesh, uint32_t shaderProg, Vertex *vertices, uint32_t vertexCount, uint32_t *indices, uint32_t indexCount) {
    
    uint32_t vbo = oglVBOCreate(vertices, vertexCount, GL_DYNAMIC_DRAW);
    uint32_t ibo = oglIBOCreate(indices, indexCount, GL_DYNAMIC_DRAW);

    if( !vbo || !ibo ) {
        fprintf( stderr, "ERROR: Mesh VBO/IBO failed\n" );
        return;
    }

    mesh->matrixLoc = -1;
    mesh->matrixLoc = glGetUniformLocation( shaderProg, "mvpMatrix" );
    if( mesh->matrixLoc < 0 ) {
        fprintf( stderr, "ERROR: Couldn't get mvpMatrix's location\n" );
        return;
    }

    mesh->shaderProg = shaderProg;
    mesh->vertexCount = vertexCount;
    mesh->vertices = vertices;
    mesh->indexCount = indexCount;
    mesh->indices = indices;
    mesh->VBO = vbo;
    mesh->IBO = ibo;
    glUseProgram( shaderProg );

    // TODO: Move shader location code outside mesh
    mesh->positionLoc = glGetAttribLocation( shaderProg, "position" );
    mesh->colorLoc = glGetAttribLocation( shaderProg, "color" );
    mesh->textureLoc = glGetAttribLocation( shaderProg, "texture" );

    #ifdef DEBUG
    if( mesh->positionLoc < 0 ) {
        fprintf( stderr, "OpenGL Error: Could not find position attribute\n" );
    }
    if( mesh->colorLoc < 0 ) {
        fprintf( stderr, "OpenGL Error: Could not find color attribute\n" );
    }
    if( mesh->textureLoc < 0 ) {
        fprintf( stderr, "OpenGL Error: Could not find texture attribute\n" );
    }
    #endif
}

void drawMesh( Mesh mesh, mat4 modelMatrix, mat4 viewMatrix, mat4 projMatrix ) {

    glUseProgram( mesh.shaderProg );
    glBindBuffer( GL_ARRAY_BUFFER, mesh.VBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.IBO );

    if( mesh.positionLoc >= 0 ) {
    	glVertexAttribPointer( mesh.positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof( Vertex, position) );
    	glEnableVertexAttribArray( mesh.positionLoc );
    }

    if( mesh.colorLoc >= 0 ) {
    	glVertexAttribPointer( mesh.colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color) );
    	glEnableVertexAttribArray( mesh.colorLoc );
    }

    if( mesh.textureLoc >= 0 ) {
    	glVertexAttribPointer( mesh.textureLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texCoords) );
    	glEnableVertexAttribArray( mesh.textureLoc );
    }

    mat4 mvpMatrix;
    glm_mat4_mul( projMatrix, viewMatrix, mvpMatrix );
    glm_mat4_mul( mvpMatrix, modelMatrix, mvpMatrix );
    glUniformMatrix4fv( mesh.matrixLoc, 1, GL_FALSE, (GLfloat*) mvpMatrix );

    glDrawElements( GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)0 );
}

void deleteMesh( Mesh mesh ) {
    oglVBODestroy( mesh.VBO );
    oglIBODestroy( mesh.IBO );
}

void createColoredRectangle(Rectangle *rect, vec3 position, float height, float width, vec4 color) {
    glm_vec3_copy(position, rect->position);
    glm_vec4_copy(color, rect->color);

    Vertex *vertices = calloc(4, sizeof(Vertex));
        vertices[0].position[0] = 0.0f;
        vertices[0].position[1] = 0.0f;
        vertices[0].position[2] = 0.0f;
        glm_vec4_copy(color, vertices[0].color);

        vertices[1].position[0] = 1.0f;
        vertices[1].position[1] = 0.0f;
        vertices[1].position[2] = 0.0f;
        glm_vec4_copy(color, vertices[1].color);

        vertices[2].position[0] = 0.0f;
        vertices[2].position[1] = 1.0f;
        vertices[2].position[2] = 0.0f;
        glm_vec4_copy(color, vertices[2].color);

        vertices[3].position[0] = 1.0f;
        vertices[3].position[1] = 1.0f;
        vertices[3].position[2] = 0.0f;
        glm_vec4_copy(color, vertices[3].color);

    uint32_t *indices = malloc(6 * sizeof(uint32_t));
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 2;
        indices[4] = 1;
        indices[5] = 3;

    createMesh(&(rect->mesh), basicRGBShader, vertices, 4, indices, 6);

    glm_translate_make( rect->modelMatrix, (vec3){position[0], position[1], -position[2]} );
    glm_scale( rect->modelMatrix, (vec3){width, height, 1} );

    rect->height = height;
    rect->width = width;
}

void createTexturedRectangle( Rectangle *rect, uint32_t shader, vec3 position, float height, float width, vec2 textureCoords[4], vec4 color ) {
    glm_vec3_copy(position, rect->position);

    Vertex *vertices = calloc(4, sizeof(Vertex));
        vertices[0].position[0] = 0.0f;
        vertices[0].position[1] = 0.0f;
        vertices[0].position[2] = 0.0f;
        glm_vec2_copy(textureCoords[0], vertices[0].texCoords);
        glm_vec4_copy(color, vertices[0].color);

        vertices[1].position[0] = 1.0f;
        vertices[1].position[1] = 0.0f;
        vertices[1].position[2] = 0.0f;
        glm_vec2_copy(textureCoords[1], vertices[1].texCoords);
        glm_vec4_copy(color, vertices[1].color);

        vertices[2].position[0] = 0.0f;
        vertices[2].position[1] = 1.0f;
        vertices[2].position[2] = 0.0f;
        glm_vec2_copy(textureCoords[2], vertices[2].texCoords);
        glm_vec4_copy(color, vertices[2].color);

        vertices[3].position[0] = 1.0f;
        vertices[3].position[1] = 1.0f;
        vertices[3].position[2] = 0.0f;
        glm_vec2_copy(textureCoords[3], vertices[3].texCoords);
        glm_vec4_copy(color, vertices[3].color);

    uint32_t *indices = malloc(6 * sizeof(uint32_t));
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 2;
        indices[4] = 1;
        indices[5] = 3;

    createMesh(&(rect->mesh), shader, vertices, 4, indices, 6);

    glm_translate_make(rect->modelMatrix, (vec3){position[0], position[1], -position[2]});
    glm_scale(rect->modelMatrix, (vec3){width, height, 1});

    rect->height = height;
    rect->width = width;
}

void drawRectangle( Rectangle rect, mat4 viewMatrix, mat4 projMatrix ) {
    drawMesh( rect.mesh, rect.modelMatrix, viewMatrix, projMatrix );
}

void deleteRectangle( Rectangle rect ) {
    deleteMesh( rect.mesh );
    //free( rect.mesh.vertices );
    //free( rect.mesh.indices );
}

// Checks if x,y in world space is inside rectangle
int checkCoordInsideRectangle( float x, float y, Rectangle rect ) {
    vec3 topLeft, bottomRight;  

    glm_mat4_mulv3( rect.modelMatrix, rect.mesh.vertices[0].position, 1.0f, topLeft );
    glm_mat4_mulv3( rect.modelMatrix, rect.mesh.vertices[3].position, 1.0f, bottomRight );

    if( topLeft[0] < x && topLeft[1] < y && bottomRight[0] > x && bottomRight[1] > y ) {
        return 1;
    }

    return 0;
}

void changeRectangleColor( Rectangle *rect, vec4 color ) {
    glm_vec4_copy( color, rect->color );
    glBindBuffer( GL_ARRAY_BUFFER, rect->mesh.VBO );
    for( uint32_t i = 0; i < rect->mesh.vertexCount; ++i ) {
        glm_vec4_copy( color, rect->mesh.vertices[i].color );
        glBufferSubData( GL_ARRAY_BUFFER, (i * (sizeof(Vertex))) + offsetof(Vertex, color), 4 * sizeof(float), color );
    }
}

void changeRectangleProperties( Rectangle *rect, vec3 position, float height, float width, vec2 textureCoords[4], vec4 color ) {
    glm_vec4_copy( color, rect->color );
    glm_vec3_copy( position, rect->position );

    glBindBuffer( GL_ARRAY_BUFFER, rect->mesh.VBO );

    for( uint32_t i = 0; i < 4; ++i ) {
        glm_vec2_copy( textureCoords[i], rect->mesh.vertices[i].texCoords );
        glm_vec4_copy( color, rect->mesh.vertices[i].color );

        glBufferSubData( GL_ARRAY_BUFFER, (i * (sizeof(Vertex))) + offsetof(Vertex, texCoords), 2 * sizeof(float), textureCoords[i] );
        glBufferSubData( GL_ARRAY_BUFFER, (i * (sizeof(Vertex))) + offsetof(Vertex, color), 4 * sizeof(float), color );
    }

    glm_translate_make(rect->modelMatrix, (vec3){position[0], position[1], -position[2]});
    glm_scale(rect->modelMatrix, (vec3){width, height, 1});

    rect->height = height;
    rect->width = width;
}


// Fonts
uint32_t fontTexture = 0;
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

void initFont(const char *filePath) {
    //unsigned char ttfBuffer = calloc( 1<<20, sizeof( unsigned char) );
    unsigned char *tempBitmap = calloc( 512*512, sizeof(unsigned char) );
    if( !tempBitmap ) {
        fprintf( stderr, "Allocation for font failed\n" );
        return;
    }

    FileBuffer fontFile;
    if( !bufferFile( &fontFile, filePath ) ) {
        fprintf( stderr, "Allocation for font failed\n" );
        return;
    }
    //FILE *fontFile = fopen(filePath, "rb");
    //fread(ttfBuffer, 1, 1<<20, fontFile);
    //fclose(fontFile);

    if( stbtt_BakeFontBitmap(fontFile.data, 0, 64.0, tempBitmap, 512, 512, 32, 96, cdata) == 0 ) {
        //free( ttfBuffer );
        free( tempBitmap );
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tempBitmap);
    glBindTexture(GL_TEXTURE_2D, 0);

    //free( ttfBuffer );
    free( tempBitmap );
    freeFileBuffer( &fontFile );
}

void destroyFont() {
    glDeleteTextures( 1, &fontTexture );
}


void createColoredCircle( Circle *circle, vec3 position, float radius, vec4 color ) {
    glm_vec4_copy( color, circle->color );
    glm_vec3_copy( position, circle->position );
    circle->radius = radius;


    unsigned numberOfTriangles = radius/2;

    if( numberOfTriangles < 30 ) {
        numberOfTriangles = 30;
    }

    if( radius < 2.0f ) {
        numberOfTriangles = 10;
    }

    Vertex *vertices = calloc( numberOfTriangles + 1, sizeof( Vertex ) );
    uint32_t *indices = calloc( numberOfTriangles * 3, sizeof(uint32_t) );
    uint32_t *indiceData = indices;

    float currentAngle = 0.0f;
    float angleIncrementor = glm_rad( 360.0f / numberOfTriangles );

    glm_vec3_copy( (vec3){0.0f, 0.0f, 0.0f}, vertices[0].position );
    glm_vec4_copy( color, vertices[0].color );

    for( unsigned i = 1; i < numberOfTriangles+1; ++i ) {
        float x = cosf( currentAngle );
        float y = sinf( currentAngle );

        glm_vec3_copy( (vec3){x, y, position[2]}, vertices[i].position );
        glm_vec4_copy( color, vertices[i].color );

        *indiceData++ = 0;
        *indiceData++ = i;
        *indiceData++ = i + 1;
        currentAngle += angleIncrementor;
    }

    *(indiceData-1) = 1; // Last triangle wraps index around

    createMesh(&(circle->mesh), basicRGBShader, vertices, numberOfTriangles + 1, indices, numberOfTriangles * 3);

    glm_translate_make( circle->modelMatrix, (vec3){position[0], position[1], -position[2]} );
    glm_scale( circle->modelMatrix, (vec3){radius, radius, 1} );
}

void drawCircle( Circle circle, mat4 viewMatrix, mat4 projMatrix ) {
    drawMesh( circle.mesh, circle.modelMatrix, viewMatrix, projMatrix );
}

void deleteCircle( Circle circle ) {
    deleteMesh( circle.mesh );
}
