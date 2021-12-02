#include "display.h"

Framebuffer openFramebuffer( uint32_t framebufferIndex ) {
    Framebuffer newFramebuffer;

    if( framebufferIndex > 99 ) {
        printf( "Error: framebuffer index %d is too large.\n", framebufferIndex );
        exit( FRAMEBUFFER_ERROR_CODE );
    }

    char framebufferDirectory[10];
    memset( framebufferDirectory, 0, sizeof(framebufferDirectory) );
    sprintf( framebufferDirectory, "/dev/fb%d", framebufferIndex );
    int fbfd = open( framebufferDirectory, O_RDWR );
    if( fbfd == -1 ) {
        printf( "Error: cannot open framebuffer device [%s]\n", framebufferDirectory );
	exit( FRAMEBUFFER_ERROR_CODE );
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    // Get fixed screen information
    if( ioctl( fbfd, FBIOGET_FSCREENINFO, &finfo ) ) {
	printf( "Error reading fixed information.\n" );
	exit( FRAMEBUFFER_ERROR_CODE );
    }
    
    // Get variable screen information
    if( ioctl( fbfd, FBIOGET_VSCREENINFO, &vinfo ) ) {
	printf( "Error reading variable information.\n" );
	exit( FRAMEBUFFER_ERROR_CODE );
    }

    newFramebuffer.bufferSize = finfo.smem_len;

    newFramebuffer.height = vinfo.yres;
    newFramebuffer.width  = vinfo.xres;
    newFramebuffer.bpp    = vinfo.bits_per_pixel;
    newFramebuffer.buffer = mmap( 0, newFramebuffer.bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0 );
    newFramebuffer.fbfd   = fbfd;

    if( (int)newFramebuffer.buffer == -1 ) {
        printf( "Failed to mmap framebuffer.\n" );
	exit( FRAMEBUFFER_ERROR_CODE );
    }

    return newFramebuffer;
}

void closeFramebuffer( Framebuffer *framebuffer ) {
    framebuffer->height = 0;
    framebuffer->width  = 0;
    framebuffer->bpp    = 0;
    munmap( framebuffer->buffer, framebuffer->bufferSize );
    close( framebuffer->fbfd );
    framebuffer->buffer = NULL;
    framebuffer->bufferSize = 0;
    framebuffer->fbfd = -1;
}

void updateFramebufferProp( Framebuffer *framebuffer ) {

    struct fb_var_screeninfo vinfo;

    // Get variable screen information
    if( ioctl( framebuffer->fbfd, FBIOGET_VSCREENINFO, &vinfo ) ) {
	printf( "Error reading variable information.\n" );
	exit( FRAMEBUFFER_ERROR_CODE );
    }

    framebuffer->height = vinfo.yres;
    framebuffer->width  = vinfo.xres;
    framebuffer->bpp    = vinfo.bits_per_pixel;
}

void renderGLToFramebuffer( Framebuffer framebuffer ) {
    switch( framebuffer.bpp ) {
        case 16:    
	    glReadPixels( 0, 0, framebuffer.width, framebuffer.height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, framebuffer.buffer );
        break;
        case 32:
	    glReadPixels( 0, 0, framebuffer.width, framebuffer.height, GL_RGB, GL_UNSIGNED_BYTE, framebuffer.buffer );
	break;
	default:
	    printf( "Unsupported screen depth %d.\n", framebuffer.bpp );	
    }
}

EGLWindow eglWindow = {0};

#ifdef RPI4 
DRMData drmData = {0};

drmModeConnector *getConnector( drmModeRes *resources ) {
    for( int i = 0; i < resources->count_connectors; ++i ) {
	drmModeConnector *connector = drmModeGetConnector( drmData.device, resources->connectors[i] );
	if( connector->connection == DRM_MODE_CONNECTED ) {
	    return connector;
	}
	drmModeFreeConnector( connector );
    }

    return NULL;
}

drmModeEncoder *findEncoder( drmModeConnector *connector ) {
    if( connector->encoder_id ) {
	return drmModeGetEncoder( drmData.device, connector->encoder_id );
    }
    return NULL;
}

int getDisplay( EGLDisplay *display ) {
    drmModeRes *resources = drmModeGetResources( drmData.device );
    if( resources == NULL ) {
	fprintf( stderr, "Unable to get DRM resources.\n" );
	return -1;
    }
 
    drmModeConnector *connector = getConnector( resources );
    if( connector == NULL ) {
	fprintf( stderr, "Unable to get connector.\n" );
	drmModeFreeResources( resources );
	return -1;
    }

    drmData.connectorId = connector->connector_id;
    drmData.mode = connector->modes[0];

    drmModeEncoder *encoder = findEncoder( connector );
    if( encoder == NULL ) {
	fprintf( stderr, "Unable to get encoder.\n" );
	drmModeFreeConnector( connector );
 	drmModeFreeResources( resources );
	return -1;
    }

    drmData.crtc = drmModeGetCrtc( drmData.device, encoder->crtc_id );
    drmModeFreeEncoder( encoder );
    drmModeFreeConnector( connector );
    drmModeFreeResources( resources );
    
    drmData.gbmDevice  = gbm_create_device( drmData.device );
    drmData.gbmSurface = gbm_surface_create( drmData.gbmDevice, drmData.mode.hdisplay, drmData.mode.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING );
    *display = eglGetDisplay( drmData.gbmDevice );
    return 0; 
}

int matchConfigToVisual( EGLDisplay display, EGLint visualId, EGLConfig *configs, int count ) {
    EGLint id;
    for( int i = 0; i < count; ++i ) {
	if( !eglGetConfigAttrib( display, configs[i], EGL_NATIVE_VISUAL_ID, &id ) ) {
	    continue;
	}
	if( id == visualId ) {
	    return i;
	}
    }
    return -1;
}

void gbmSwapBuffers( EGLDisplay *display, EGLSurface *surface ) {
    eglSwapBuffers( *display, *surface );
    struct gbm_bo *bo = gbm_surface_lock_front_buffer( drmData.gbmSurface );
    uint32_t handle = gbm_bo_get_handle( bo ).u32;
    uint32_t pitch  = gbm_bo_get_stride( bo );
    uint32_t fb;
    drmModeAddFB( drmData.device, drmData.mode.hdisplay, drmData.mode.vdisplay, 24, 32, pitch, handle, &fb );
    drmModeSetCrtc( drmData.device, drmData.crtc->crtc_id, fb, 0, 0, &drmData.connectorId, 1, &drmData.mode );

    if( drmData.previousBo ) {
	drmModeRmFB( drmData.device, drmData.previousFb );
	gbm_surface_release_buffer( drmData.gbmSurface, drmData.previousBo );
    }

    drmData.previousBo = bo;
    drmData.previousFb = fb;
}

void gbmClean() {
    drmModeSetCrtc( drmData.device, drmData.crtc->crtc_id, drmData.crtc->buffer_id, drmData.crtc->x, drmData.crtc->y, &drmData.connectorId, 1, &drmData.crtc->mode );
    drmModeFreeCrtc( drmData.crtc );

    if( drmData.previousBo ) {
	drmModeRmFB( drmData.device, drmData.previousFb );
	gbm_surface_release_buffer( drmData.gbmSurface, drmData.previousBo );
    }

    gbm_surface_destroy( drmData.gbmSurface );
    gbm_device_destroy( drmData.gbmDevice );
}

#endif /* RPI4 */

void initializeEGL( unsigned winHeight, unsigned winWidth ) {
    EGLint pbufferAttribs[] = {
	EGL_WIDTH, winWidth, EGL_HEIGHT, winHeight, EGL_NONE
    };

    #ifdef RPI4
    drmData.device = open( "/dev/dri/card0", O_RDWR | 0 );
    if( getDisplay( &eglWindow.display ) != 0 ) {
	drmData.device = open( "/dev/dri/card1", O_RDWR | 0 );
	if( getDisplay( &eglWindow.display ) != 0 ) {
	    fprintf( stderr, "Unable to get EGL display.\n" );
	    close( drmData.device );
	    exit( EGL_ERROR_CODE );
	}
    }

    if( eglInitialize( eglWindow.display, &eglWindow.major, &eglWindow.minor ) == EGL_FALSE ) {
	fprintf( stderr, "Failed to get EGL version! Error: %s.\n", eglGetErrorStr() );
	eglTerminate( eglWindow.display );
	gbmClean();
	exit( EGL_ERROR_CODE );
    }

    eglBindAPI( EGL_OPENGL_API );

    EGLint configCount;
    eglGetConfigs( eglWindow.display, NULL, 0, &configCount ); 
    eglWindow.configs = malloc( configCount * sizeof( eglWindow.configs ) );

    if( !eglChooseConfig( eglWindow.display, eglConfigAttribs, eglWindow.configs, configCount, &configCount ) ) {
	fprintf( stderr, "Failed to get EGL configs! Error: %s.\n", eglGetErrorStr() );
 	eglTerminate( eglWindow.display );
	gbmClean();
	exit( EGL_ERROR_CODE );
    }  

    // The EGL config must match the GBM format.
    int configIndex = matchConfigToVisual( eglWindow.display, GBM_FORMAT_XRGB8888, eglWindow.configs, configCount );
    if( configIndex < 0 ) {
	fprintf( stderr, "Failed to find matching EGL config! Error: %s.\n", eglGetErrorStr() );
  	eglTerminate( eglWindow.display );
 	gbm_surface_destroy( drmData.gbmSurface );
	gbm_device_destroy( drmData.gbmDevice );
	exit( EGL_ERROR_CODE );
    }

    eglWindow.context = eglCreateContext( eglWindow.display, eglWindow.configs[configIndex], EGL_NO_CONTEXT, eglContextAttribs );
    if( eglWindow.context == EGL_NO_CONTEXT ) {
	fprintf( stderr, "Failed to create EGL context! Error: %s.\n", eglGetErrorStr() );
 	eglTerminate( eglWindow.display );
    	gbmClean();
	exit( EGL_ERROR_CODE );
    }

    eglWindow.surface = eglCreateWindowSurface( eglWindow.display, eglWindow.configs[configIndex], drmData.gbmSurface, NULL );
    if( eglWindow.surface == EGL_NO_SURFACE ) {
	fprintf( stderr, "Failed to create EGL surface! Error: %s.\n", eglGetErrorStr() );
	eglDestroyContext( eglWindow.display, eglWindow.context );
	eglTerminate( eglWindow.display );
	gbmClean();
	exit( EGL_ERROR_CODE );
    }

    #else // Code for all other RPIs
    if( (eglWindow.display = eglGetDisplay( EGL_DEFAULT_DISPLAY ) ) == EGL_NO_DISPLAY ) {
	fprintf( stderr, "Failed to get EGL display. Error %s,\n", eglGetErrorStr() );
	exit( EGL_ERROR_CODE );
    }
    
    if( eglInitialize( eglWindow.display, &eglWindow.major, &eglWindow.minor ) == EGL_FALSE ) {
	fprintf( stderr, "Failed to get EGL version! Error: %s.\n", eglGetErrorStr() );
	eglTerminate( eglWindow.display );
	exit( EGL_ERROR_CODE );
    }

    EGLint configCount;
    eglWindow.configs = calloc( 1, sizeof( EGLConfig ) );
    if( !eglChooseConfig( eglWindow.display, eglConfigAttribs, eglWindow.configs, 1, &configCount ) ) {
	fprintf( stderr, "Failed to get EGL config! Error: %s.\n", eglGetErrorStr() );
 	eglTerminate( eglWindow.display );
	exit( EGL_ERROR_CODE );
    }

    eglWindow.surface = eglCreatePbufferSurface( eglWindow.display, eglWindow.configs, pbufferAttribs );
    if( eglWindow.surface == EGL_NO_SURFACE ) {
	fprintf( stderr, "Failed to create EGL surface: Error: %s.\n", eglGetErrorStr() );
	eglTerminate( eglWindow.display );
	exit( EGL_ERROR_CODE );
    }

    eglBindAPI( EGL_OPENGL_API );

    eglWindow.context = eglCreateContext( eglWindow.display, eglWindow.configs, EGL_NO_CONTEXT, eglContextAttribs );
    if( eglWindow.context == EGL_NO_CONTEXT ) {
	fprintf( stderr, "Failed to create EGL context! Error: %s.\n", eglGetErrorStr() );
	eglDestroySurface( eglWindow.display, eglWindow.surface );
	eglTerminate( eglWindow.display );
	exit( EGL_ERROR_CODE );
    }

    #endif

    eglMakeCurrent( eglWindow.display, eglWindow.surface, eglWindow.surface, eglWindow.context );

    glViewport( 0, 0, winWidth, winHeight );

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport ); // Check if viewport size is correct.

    if( winWidth != viewport[2] || winHeight != viewport[3] ) {
	fprintf( stderr, "Error! The glViewport/glGetIntegerv are not working! EGL might be faulty!\n" );
	closeEGL();
 	exit( EGL_ERROR_CODE );
    }
    
}

void closeEGL() {
    free( eglWindow.configs );
    eglDestroyContext( eglWindow.display, eglWindow.context );
    eglDestroySurface( eglWindow.display, eglWindow.surface );
    eglTerminate( eglWindow.display );
    #ifdef RPI4
    gbmClean();
    close( drmData.device );
    #endif /* RPI4 */
}

