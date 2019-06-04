#include "core/os/os.h"
#include "context_gl_switch.h"

ContextGL_Switch::ContextGL_Switch(nxlib::NWindow *win) {
  window = win;

}

ContextGL_Switch::~ContextGL_Switch() {
  if (s_display)
    {
      eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
      if (s_context)
	{
	  eglDestroyContext(s_display, s_context);
	  s_context = nullptr;
	}
      if (s_surface)
	{
	  eglDestroySurface(s_display, s_surface);
	  s_surface = nullptr;
	}
      eglTerminate(s_display);
      s_display = nullptr;
    }
}

Error ContextGL_Switch::initialize() {
  // Connect to the EGL default display
  s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!s_display)
    {
      goto _fail0;
    }
  // Initialize the EGL display connection
  eglInitialize(s_display, nullptr, nullptr);

  // Select OpenGL (Core) as the desired graphics API
  if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
    {
      goto _fail1;
    }
  // Get an appropriate EGL framebuffer configuration
  EGLConfig config;
  EGLint numConfigs;
    static const EGLint framebufferAttributeList[] =
      {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE,     8,
        EGL_GREEN_SIZE,   8,
        EGL_BLUE_SIZE,    8,
        EGL_ALPHA_SIZE,   8,
        EGL_DEPTH_SIZE,   24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
      };

    eglChooseConfig(s_display, framebufferAttributeList, &config, 1, &numConfigs);
    if (numConfigs == 0)
      {
        goto _fail1;
      }

    // Create an EGL window surface
    s_surface = eglCreateWindowSurface(s_display, config, window, nullptr);
    if (!s_surface)
      {
        goto _fail1;
      }

    // Create an EGL rendering context
    static const EGLint contextAttributeList[] =
      {
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
        EGL_CONTEXT_MINOR_VERSION_KHR, 3,
        EGL_NONE
      };
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttributeList);
    if (!s_context)
      {
        goto _fail2;
      }

    eglQuerySurface(s_display, s_surface, EGL_WIDTH, &width);
    eglQuerySurface(s_display, s_surface, EGL_HEIGHT, &height);
    
    return OK;

 _fail2:
    eglDestroySurface(s_display, s_surface);
    s_surface = nullptr;
 _fail1:
    eglTerminate(s_display);
    s_display = nullptr;
 _fail0:
    return FAILED;

}

void ContextGL_Switch::release_current() {
  eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void ContextGL_Switch::make_current() {
  // Connect the context to the surface
  eglMakeCurrent(s_display, s_surface, s_surface, s_context);
}

void ContextGL_Switch::swap_buffers() {
  eglSwapBuffers(s_display, s_surface);
}

int ContextGL_Switch::get_window_width() {
  return window->width;
}

int ContextGL_Switch::get_window_height() {
  return window->height;
}

void ContextGL_Switch::set_use_vsync(bool p_use) {
  // TODO: nothing is being done with this
  use_vsync = p_use;
}

bool ContextGL_Switch::is_using_vsync() const {
  return false;
}
