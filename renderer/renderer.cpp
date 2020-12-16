#include "renderer.hpp"

Renderer::Renderer() {
    // Raspberry Pi specific
    bcm_host_init();
}

void Renderer::initialize() {
    EGLBoolean egl_error;
    static const EGLint attribute_list[] {
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_DEPTH_SIZE,     16,
        EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_NONE
    };
    static const EGLint context_attributes[] {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLConfig config;
    EGLint num_config;
    static EGL_DISPMANX_WINDOW_T nativewindow;
    
    // Get display connection
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        std::cerr << "Unable to get display\n";
        std::exit(1);
    }
    
    // Initialize display
    egl_error = eglInitialize(display_, &major_, &minor_);
    if (egl_error == EGL_FALSE) {
        std::cerr << "Unable to initialize display\n";
        std::exit(1);
    }
    
    // Set EGL config
    egl_error = eglChooseConfig(
        display_,
        attribute_list,
        &config,
        1,
        &num_config);
    if (egl_error == EGL_FALSE) {
        std::cerr << "Unable to set EGL config\n";
        std::exit(1);
    }
    
    egl_error = eglBindAPI(EGL_OPENGL_ES_API);
    if (egl_error == EGL_FALSE) {
        std::cerr << "Unable to bind EGL API\n";
        std::exit(1);
    }
    
    context_ = eglCreateContext(display_, config, nullptr, context_attributes);
    if (context_ == EGL_NO_CONTEXT) {
        std::cerr << "Unable to create context\n";
        std::exit(1);
    }
    
    std::cerr << eglQueryString(display_, EGL_VENDOR);
    std::cerr << " EGL " << eglQueryString(display_, EGL_VERSION) << std::endl;
    
    VC_RECT_T dst_rect, src_rect;
    int32_t vc_error;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    
    vc_error = graphics_get_display_size(
        0,  // LCD
        &screen_wid_,
        &screen_hei_);
    if (vc_error) {
        std::cerr << "Unable to get screen size\n";
        std::exit(1);
    }
    
    dst_rect.x = dst_rect.y = 0;
    dst_rect.width = screen_wid_;
    dst_rect.height = screen_hei_;
    
    src_rect.x = src_rect.y = 0;
    src_rect.width = screen_wid_ << 16;
    src_rect.height = screen_hei_ << 16;
    
    dispman_display_ = vc_dispmanx_display_open(0);
    dispman_update = vc_dispmanx_update_start(0);
    
    dispman_element_ = vc_dispmanx_element_add(
        dispman_update,
        dispman_display_,
        0,                  // Layer
        &dst_rect,
        0,                  // Src
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        0,                  // Alpha
        0,                  // Clamp
        (DISPMANX_TRANSFORM_T)0); // Transform
        
    nativewindow.element = dispman_element_;
    nativewindow.width = screen_wid_;
    nativewindow.height = screen_hei_;
    vc_dispmanx_update_submit_sync(dispman_update);
    
    surface_ = eglCreateWindowSurface(
        display_,
        config,
        &nativewindow,
        nullptr);
    if (surface_ == EGL_NO_SURFACE) {
        std::cerr << "Unable to create EGL surface\n";
        std::exit(1);
    }
    
    egl_error = eglMakeCurrent(display_, surface_, surface_, context_);
    if (egl_error == EGL_FALSE) {
        std::cerr << "Unable to make EGL current\n";
        std::exit(1);
    }
    
    std::cerr << glGetString(GL_VENDOR) << " ";
    std::cerr << glGetString(GL_VERSION) << std::endl;
    
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, screen_wid_, screen_hei_);
}

void Renderer::updateScreen() {
    eglSwapBuffers(display_, surface_);
}

void Renderer::setViewPort() {
    glViewport(0, 0, screen_wid_, screen_hei_);
}

void Renderer::cleanUp() {
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    int s;
    
    eglDestroySurface(display_, surface_);
    
    dispman_update = vc_dispmanx_update_start(0);
    s = vc_dispmanx_element_remove(dispman_update, dispman_element_);
    assert(s == 0);
    vc_dispmanx_update_submit_sync(dispman_update);
    s = vc_dispmanx_display_close(dispman_display_);
    assert(s == 0);
    
    // Release EGL
    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display_, context_);
    eglTerminate(display_);
}
