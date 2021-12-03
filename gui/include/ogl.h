#ifndef _OGL_H
#define _OGL_H

#include "display.h"
#include "renderer.h"

// Raspberry EGL initialization code based on https://github.com/matusnovak/rpi-opengl-without-x
// &
// https://benosteen.wordpress.com/2012/04/27 using-opengl-es-2-0-on-the-raspberry-pi-without-x-windows/

#ifdef HEADLESS_PI
typedef struct {
    GLint width;
    GLint height;
    EGLNativeWindowType hWnd;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
} OGLContext;
#else
/*typedef struct {
    SDL_Window *hWnd;
    SDL_GLContext context;
} SDLContext;*/
#endif

#ifdef HEADLESS_PI
OGLContext oglContext;
#else 
//SDLContext sdlContext;
#endif

#ifdef HEADLESS_PI
// EGL functions for a headless raspberry pi 
void esInitContext( OGLContext *esContext ); // Must be called before using esContext
GLboolean esCreateWindow( OGLContext *esContext, const char *title, GLuint flags );
#else
// SDL functions for other machines
//void sdlInit( SDLContext *sdlContext, const char *windowName, int width, int height );
#endif

// oglLoadShader: Returns 1 if shader was compiled successfully and 0 if not. Returns the shader id through parameter shaderId
bool32_t oglLoadShader( GLuint *shaderId, const char *shaderFilename, GLenum type );
GLuint oglLoadProgram( const char *vertFilename, const char *fragFilename );

void oglDestroyShader( GLuint shaderId );
void oglDestroyProgram( GLuint progId );

void oglSwapBuffers();

// Buffers
GLuint oglVBOCreate( const Vertex *vertices, GLuint numVertices, GLenum usage );
void oglVBODestroy( GLuint vbo );

GLuint oglIBOCreate( GLuint *indices, GLuint numIndices, GLenum usage );
void oglIBODestroy( GLuint ibo );

// Textures
GLuint oglTextureLoad( const char *filename );
void oglTextureDestroy( GLuint texName );

#endif /* _OGL_H */
