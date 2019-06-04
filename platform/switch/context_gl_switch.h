#ifndef CONTEXT_GL_SWITCH_H
#define CONTEXT_GL_SWITCH_H

#include <stdio.h>

namespace nxlib {
#include <switch.h>
}

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions

// GLM headers
#define GLM_FORCE_PURE
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "drivers/gl_context/context_gl.h"

class ContextGL_Switch : public ContextGL {
private:
  EGLDisplay s_display;
  EGLContext s_context;
  EGLSurface s_surface;
  nxlib::NWindow *window;
  
  EGLint width;
  EGLint height;
  
  bool use_vsync;

public:
  virtual Error initialize();
  virtual void release_current();
  virtual void make_current();
  virtual void swap_buffers();
  virtual int get_window_width();
  virtual int get_window_height();

  virtual void set_use_vsync(bool p_use);
  virtual bool is_using_vsync() const;

  ContextGL_Switch(nxlib::NWindow *win);
  virtual ~ContextGL_Switch();
};

#endif
