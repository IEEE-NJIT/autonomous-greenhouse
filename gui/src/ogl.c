#include "ogl.h"

#include "display.h"
#include "application.h"

#ifdef HEADLESS_PI
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#endif

#if 0
void sdlInit( SDLContext *sdl, const char *windowName, int width, int height ) {
    /*#ifdef OSX
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    //#endif 

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    uint32_t windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    sdl->hWnd = SDL_CreateWindow( windowName, 0, 0, width, height, windowFlags );
    ASSERT( sdl->hWnd );

    sdl->context = SDL_GL_CreateContext( sdl->hWnd );
    ASSERT( sdl->context );
    SDL_GL_SetSwapInterval( 1 ); // 1: enables vsync 
}
#endif

#ifdef HEADLESS_PI
EGLBoolean CreateEGLContext( EGLNativeWindowType hWnd, EGLDisplay *eglDisplay, EGLContext *eglContext, EGLSurface *eglSurface, EGLint attribList[] ) {
    EGLint numConfigs;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    #ifndef HEADLESS_PI
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    #else 
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    #endif

    // Get Display
   #ifndef HEADLESS_PI
   display = eglGetDisplay((EGLNativeDisplayType)x_display);
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }
   #else
   display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }
   #endif

   // Initialize EGL
   if ( !eglInitialize(display, &majorVersion, &minorVersion) )
   {
      return EGL_FALSE;
   }

   // Get configs
   if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Choose config
   if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Create a surface
   surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      return EGL_FALSE;
   }

   // Create a GL context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return EGL_FALSE;
   }   
   
   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      return EGL_FALSE;
   }
   
   *eglDisplay = display;
   *eglSurface = surface;
   *eglContext = context;
   return EGL_TRUE;
}

EGLBoolean WinCreate( OGLContext *esContext, const char *title ) {
    static EGL_DISPMANX_WINDOW_T nativeWindow;

    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T  dispmanUpdate;
    VC_RECT_T dstRect, srcRect;

    int displayWidth, displayHeight;
    // create an EGL window surface, passing context width/height
    if( graphics_get_display_size( 0 /* LCD */, &displayWidth, &displayHeight ) < 0 ) {
        return EGL_FALSE;
    }

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.widht = displayWidth;
    dstRect.height = displayHeight;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.width = displayWidth << 16;
    srcRect.height = displayHeight << 16;

    dispmanDisplay = vc_dispmanx_display_open( 0 /* LCD */ );
    dispmanUpdate  = vc_dispmanx_update_start( 0 );
    dispmanElement = vc_dispmanx_element_add( dispmanUpdate, dispmanDisplay, 0/*layer*/, &dstRect, 0/*src*/, &srcRect, DISPMANX_PROTECTION_NONE, 0/*alpha*/, 0/*clamp*/, 0/*transform*/ );

    nativeWindow.element = dispmanElement;
    nativeWindow.width   = displayWidth;
    nativeWindow.height  = displayHeight;
    vc_dispmanx_update_submit_sync( dispmanUpdate );

    esContext->hWnd = &nativeWindow;

    return EGL_TRUE;
}

void esInitContext( OGLContext *esContext ) {
    #ifdef HEADLESS_PI
    bcm_host_init();
    #endif
    if( esContext ) {
        memset( esContext, 0, sizeof( ESContext ) );
    }
}

EGLBoolean esCreateWindow( OGLContext *esContext, const char *title, GLuint flags ) {
    EGLint attribList[] = {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
        EGL_NONE
    };

    if( !esContext ) {
        return GL_FALSE;
    }

    esContext->width  = width;
    esContext->height = height;

    if( !WinCreate( esContext, title ) ) {
        return GL_FALSE;
    }

    if( !CreateEGLContext( esContext->hWnd, &esContext->eglDisplay, &esContext->eglContext, &esContext->eglSurface, attribList) ) {
        return GL_FALSE;
    }

    return GL_TRUE;
}
#endif

bool32_t oglLoadShader( GLuint *shaderId, const char *shaderFilename, GLenum type ) {
    FileBuffer shaderFile;

    if( !bufferFile( &shaderFile, shaderFilename ) ) {
        fprintf( stderr, "Could not open shader file [%s]\n", shaderFilename );
        return 0;
    }

    // Create the shader
    GLuint shader = glCreateShader( type );
    glShaderSource(shader, 1, (const GLchar**)&shaderFile.data, NULL);

    // Compile it
    glCompileShader(shader);
    GLint compileSucceeded = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
    if( !compileSucceeded ) {
        // Compilation failed
        fprintf( stderr, "compilation of shader %s failed\n", shaderFilename );
        printf( "\n\n%s\n\n",(char*) shaderFile.data );
        GLint logLength = 0;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
        GLchar *errLog = (GLchar*)malloc( logLength );
        if( errLog ) {
            glGetShaderInfoLog( shader, logLength, &logLength, errLog );
            fprintf( stderr, "%s\n", errLog );
            free( errLog );
            return 0;
        } else {
            fprintf( stderr, "Couldn't get shader log; out of memory\n" );
            return 0;
        }

        glDeleteShader(shader);
        free( errLog );
        return 0;
    }

    *shaderId = shader;
    freeFileBuffer( &shaderFile );
    return 1;
}

GLuint oglLoadProgram( const char *vertFilename, const char *fragFilename ) {
    GLuint vertShader;
    bool32_t vertResult = oglLoadShader( &vertShader, vertFilename, GL_VERTEX_SHADER );
    GLuint fragShader;
    bool32_t fragResult = oglLoadShader( &fragShader, fragFilename, GL_FRAGMENT_SHADER );
    
    if( !vertResult || !fragResult ) {
        fprintf( stderr, "Couldn't load shaders: [%s] [%s]\n", vertFilename, fragFilename );
        oglDestroyShader( vertShader );
        oglDestroyShader( fragShader );
        return 0;
    }

    GLuint shaderProg = glCreateProgram();
    if( shaderProg ) {
        glAttachShader( shaderProg, vertShader );
        glAttachShader( shaderProg, fragShader );

        glLinkProgram( shaderProg );

        GLint linkingSucceeded = GL_FALSE;
        glGetProgramiv( shaderProg, GL_LINK_STATUS, &linkingSucceeded );
        if( !linkingSucceeded ) {
            fprintf( stderr, "Linking shader failed (vert. shader: %s, frag. shader: %s)\n", vertFilename, fragFilename );
            GLint logLength = 0;
            glGetProgramiv( shaderProg, GL_INFO_LOG_LENGTH, &logLength );
            GLchar *errLog = (GLchar*)malloc( logLength );
            if( errLog ) {
                glGetProgramInfoLog( shaderProg, logLength, &logLength, errLog );
                fprintf( stderr, "%s\n", errLog );
                free( errLog );
            } else {
                fprintf( stderr, "Couldn't get shader link log; out of memory\n" );
            }
            free( errLog );
            glDeleteProgram( shaderProg );
            shaderProg = 0;
        }
    } else {
        fprintf( stderr, "Couldn't create shader program\n" );
    }

    oglDestroyShader( vertShader );
    oglDestroyShader( fragShader );

    return shaderProg;
}

void oglDestroyShader( GLuint shaderId ) {
    glDeleteShader( shaderId );
}

void oglDestroyProgram( GLuint progId ) {
    glDeleteProgram( progId );
}

#ifdef HEADLESS_PI
void oglSwapBuffers() {
    eglSwapBuffers( oglContext->eglDisplay, oglContext->eglSurface);
}
#else
void oglSwapBuffers() {
    //renderGLToFramebuffer( state->framebuffer );
}
#endif

GLuint oglVBOCreate( const Vertex *vertices, GLuint numVertices, GLenum usage ) {
    // Create the Vertex Buffer Object
    GLuint vbo;
    int nBuffers = 1;
    glGenBuffers( nBuffers, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    // Copy the vertex data in, and deactivate
    glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, usage );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Check for problems
    GLenum err = glGetError();
    if( err != GL_NO_ERROR ) {
        // Failed
        glDeleteBuffers( nBuffers, &vbo );
        fprintf( stderr, "Creating VBO failed, code %u\n", err );
        vbo = 0;
    }

    return vbo;
}

void oglVBODestroy( GLuint vbo ) {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glDeleteBuffers( 1, &vbo );
}

GLuint oglIBOCreate( GLuint *indices, GLuint numIndices, GLenum usage ) {
    // Create the Index Buffer Object
    GLuint ibo;
    int nBuffers = 1;
    glGenBuffers( nBuffers, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ); 

    // Copy the index data in, and deactivate
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numIndices, indices, usage );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // Check for problems
    GLenum err = glGetError();
    if( err != GL_NO_ERROR ) {
        // Failed
        glDeleteBuffers( nBuffers, &ibo );
        fprintf( stderr, "Creating IBO failed, code %u\n", err );
        ibo = 0;
    }

    return ibo;
}

void oglIBODestroy( GLuint ibo ) {
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 
    glDeleteBuffers( 1, &ibo );
}

GLuint oglTextureLoad( const char *filename ) {
    GLuint texture = 0;
    int height = 0, width = 0, bpp = 0;

    stbi_set_flip_vertically_on_load( 1 );
    unsigned char *localBuffer = stbi_load( filename, &width, &height, &bpp, 4 );
    if( !localBuffer ) {
        fprintf( stderr, "Failed to load texture file [%s]\n", filename );
        return 0;
    }

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    unsigned int format = bpp == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if( localBuffer )
        stbi_image_free( localBuffer );

    return texture;
}

void oglTextureDestroy( GLuint texName ) {
    glDeleteTextures( 1, &texName );
}
