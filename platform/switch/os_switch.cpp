#include <thread>
#include <chrono>

#include "os_switch.h"

#include "drivers/gles3/rasterizer_gles3.h"

#include "main/main.h"
#include "servers/visual/visual_server_raster.h"
#include "servers/visual/visual_server_wrap_mt.h"


void OS_Switch::delete_main_loop() {
  if (main_loop)
    memdelete(main_loop);
  main_loop = NULL;
}

int OS_Switch::get_current_video_driver() const {
  return video_driver_index;
}
Error OS_Switch::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver) {
  current_videomode = p_desired;
  context_gl = memnew(ContextGL_Switch(nxlib::nwindowGetDefault()));
  context_gl->initialize();
  context_gl->make_current();

  if (RasterizerGLES3::is_viable() == OK) {
    RasterizerGLES3::register_config();
    RasterizerGLES3::make_current();
  } else return FAILED;

  visual_server = memnew(VisualServerRaster);
  if (get_render_thread_mode() != RENDER_THREAD_UNSAFE) {
    visual_server = memnew(VisualServerWrapMT(visual_server, false));
  }

  video_driver_index = p_video_driver;

  
  // hid
  input = memnew(InputDefault);

  // joypad
  joypad = memnew(JoypadSwitch(input));

  // touchscreen

  last_touch_pos = Point2(0,0);
  last_button_state = 0;

  visual_server->init();

  AudioDriverManager::initialize(p_audio_driver);

  return OK;
}

void OS_Switch::finalize() {
  if (main_loop)
    memdelete(main_loop);
  main_loop = NULL;

  visual_server->finish();
  memdelete(visual_server);

  memdelete(joypad);

  memdelete(input);

  memdelete(context_gl);

  // TODO Shutdown everything

}

void OS_Switch::set_main_loop(MainLoop *p_main_loop) {
  main_loop = p_main_loop;
  input->set_main_loop(p_main_loop);
}

void OS_Switch::set_cursor_shape(CursorShape p_shape){
  // TODO, maybe
}

void OS_Switch::set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot) {
  // TODO, maybe
}

bool OS_Switch::_check_internal_feature_support(const String &p_feature) {
  // TODO: What is this used for?
  return p_feature == "mobile";
}

Point2 OS_Switch::get_mouse_position() const {
  return last_touch_pos;
}

int OS_Switch::get_mouse_button_state() const {
  return last_button_state;
}

void OS_Switch::set_window_title(const String &p_title) {
  // empty
}

MainLoop *OS_Switch::get_main_loop() const {
  return main_loop;
}

bool OS_Switch::can_draw() const {
  return true;
}

void OS_Switch::set_video_mode(const VideoMode &p_video_mode, int p_screen) {
}

OS::VideoMode OS_Switch::get_video_mode(int p_screen) const {
  return current_videomode;
}

void OS_Switch::get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen) const {
  
}

Size2 OS_Switch::get_window_size() const {
  // TODO: what about when docked?
  return Vector2(1280, 720);
}

void OS_Switch::run() {

  if (!main_loop)
    return;

  main_loop->init();

  while (nxlib::appletMainLoop()) {

    // Scan inputs (only joycons for the moment)
    joypad->process_joypads();

    if (Main::iteration())
      break;
  };

  main_loop->finish();
}

bool OS_Switch::is_joy_known(int p_device) {
  return input->is_joy_mapped(p_device);
}

String OS_Switch::get_joy_guid(int p_device) const {
  return input->get_joy_guid_remapped(p_device);
}

void OS_Switch::release_rendering_thread() {

  context_gl->release_current();
}

void OS_Switch::make_rendering_thread() {

  context_gl->make_current();
}

void OS_Switch::swap_buffers() {
  context_gl->swap_buffers();
}

OS_Switch::OS_Switch() {
  AudioDriverManager::add_driver(&driver_switchaudio);
}
