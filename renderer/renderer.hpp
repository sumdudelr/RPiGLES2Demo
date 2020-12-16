#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <cassert>

#include <bcm_host.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

class Renderer {
private:
    EGLDisplay display_;
    EGLint major_, minor_;
    EGLContext context_;
    EGLSurface surface_;
    
    DISPMANX_ELEMENT_HANDLE_T dispman_element_;
    DISPMANX_DISPLAY_HANDLE_T dispman_display_;
    
public:
    Renderer();
    
    ~Renderer() {};
    
    uint32_t screen_wid_, screen_hei_;
    
    void initialize();
    
    void updateScreen();
    
    void setViewPort();
    
    void cleanUp();
};

#endif
