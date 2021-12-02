// Code sourced from https://github.com/rst-/raspberry-compote.git
// The raspberry-compote repository provides code for interfacing and rendering to the linux framebuffer.
// Code sourced from https://github.com/matusnovak/rpi-opengl-without-x.git
// The rpi-opengl-without-x repository provides code for initializing egl on the raspberry pi.

/*
 * This code is for opening a framebuffer on linux
 * to allow the opengl framebuffer to be copied directly 
 * to the screen without using X11 or window manager software.
 * This is required when running on raspberry os lite without window manager(WM).
 * A WM is not used because the operating system will be modified to not write to
 * the filesystem to prevent sd card corruption and a window manager may need to 
 * log extensively and use much of the avaiable ram.
 * The general flow is render scene to opengl framebuffer and then copy that data
 * to the framebuffer opened from linux, performing any formating required for pixel
 * depth of the framebuffer.
 */

#ifndef _DISPLAY_H
#define _DISPLAY_H

#define EGL_ERROR_CODE		-100
#define FRAMEBUFFER_ERROR_CODE  -101
#define DRM_ERROR_CODE		-102

// Linux Framebuffer
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>

// Struct to store data pertaining to the framebuffer managed by linux
typedef struct {    
    uint32_t height; // Display height
    uint32_t width;  // Display width
    uint32_t bpp;    // Display bits per pixel
    void *buffer;    // Memory mapped framebuffer
    int bufferSize;  // Size of the buffer in bytes
    int fbfd;        // Framebuffer file descriptor
} Framebuffer;


Framebuffer openFramebuffer( uint32_t framebufferIndex ); // Main framebuffer will always be index zero.
void closeFramebuffer( Framebuffer *framebuffer );
void updateFramebufferProp( Framebuffer *framebuffer );   // Updates framebuffer's height, width, and bpp.

#ifdef RPI4
// The Raspberry Pi 4 needs to access the linux DRM to get an EGL surface for rendering since there is no full KMS driver for the GPU
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#endif /* RPI4 */

#include <sys/ioctl.h>

// OpenglES
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#ifdef RPI4
const EGLint eglConfigAttribs[] = {
    EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
    EGL_NONE
};
#else
const EGLint eglConfigAttribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 8,
    // Uncomment the following to enable MSAA
    // EGL_SAMPLE_BUFFERS, 1, // Must be set to 1 to enable multisampling
    // EGL_SAMPLES, 4, // Number of samples
    
    // Uncomment the following to enable stencil buffer
    // EGL_STENCIL_SIZE, 1,
 
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE
};
#endif

typedef struct {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int major, minor;
    EGLConfig *configs;
    EGLint configCount;
} EGLWindow;

extern EGLWindow eglWindow;

static const EGLint eglContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

void initializeEGL( unsigned winHeight, unsigned winWidth );
void closeEGL();
void renderGLToFramebuffer( Framebuffer framebuffer );

#ifdef RPI4 // Raspberry Pi 4 drm functions and variables
typedef struct {
    int device;
    drmModeModeInfo mode;
    struct gbm_device *gbmDevice;
    struct gbm_surface *gbmSurface;
    drmModeCrtc *crtc;
    uint32_t connectorId;
    struct gbm_bo *previousBo;
    uint32_t previousFb;
} DRMData;

extern DRMData drmData;

drmModeConnector *getConnector( drmModeRes *resources );
drmModeEncoder *findEncoder( drmModeConnector *connector );
int getDisplay( EGLDisplay *display );
int matchConfigToVisual( EGLDisplay display, EGLint visualId, EGLConfig *configs, int count );
void gbmSwapBuffers( EGLDisplay *display, EGLSurface *surface );
void gbmClean();
#endif /* RPI4 */

static const char* eglGetErrorStr() {
    switch( eglGetError() ) {
	case EGL_SUCCESS:
	    return "The last function succeeded without error";
	case EGL_NOT_INITIALIZED:
	    return "EGL is not initialized, or could not be initialized, for the specified EGL display connection";
	case EGL_BAD_ACCESS:
	    return "EGL cannot access a requested resource";
	case EGL_BAD_ALLOC:
	    return "EGL failed to allocate resources for the requested operation";
	case EGL_BAD_ATTRIBUTE:
	    return "An unrecognized attribute or attribute value was passed in the attribute list";
	case EGL_BAD_CONTEXT:
	    return "An EGLContext argument does not name a valid EGL rendering context";
	case EGL_BAD_CONFIG:
	    return "An EGLConfig argument does not name a valid EGL frame buffer configuration";
	case EGL_BAD_CURRENT_SURFACE:
	    return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid";
	case EGL_BAD_DISPLAY:
	    return "An EGLDisplay argument does not name a valid EGL display connection";
	case EGL_BAD_SURFACE:
	    return "An EGLSurface argument does not name a valid surface (window, pixel buffer, or pixmap) configure for GL rendering";
	case EGL_BAD_MATCH:
	    return "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface";
	case EGL_BAD_PARAMETER:
	    return "One or more argument values are invalid";
	case EGL_BAD_NATIVE_PIXMAP:
	    return "A NativePixmapType argument does not refer to a valid native pixmap";
	case EGL_BAD_NATIVE_WINDOW:
	    return "A NativeWindowType argument does not refer to a valid nativ window";
	case EGL_CONTEXT_LOST:
	    return "A power management event has occurred. The application must destroy all contexts and reinitialize OpenGL ES state and objects to continue rendering";
	default:
	    break;
    }
    return "Unknown error";
}

#endif /* _DISPLAY_H */
