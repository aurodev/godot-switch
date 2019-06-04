#ifndef OS_SWITCH_H
#define OS_SWITCH_H

#include "drivers/unix/os_unix.h"
#include "main/input_default.h"
#include "servers/visual_server.h"
#include "context_gl_switch.h"
#include "joypad_switch.h"
#include "audio_driver_switch.h"

class OS_Switch: public OS_Unix {
  MainLoop *main_loop;
  bool force_quit;
  int video_driver_index;
  VideoMode current_videomode;
  VisualServer *visual_server;

  ContextGL_Switch *context_gl;

  // Controls
  InputDefault *input;
  JoypadSwitch *joypad;

  AudioDriverSwitch driver_switchaudio;

  // Buttons
  nxlib::u64 kDown = 0;
  nxlib::u64 kHeld = 0;
  nxlib::u64 kUp = 0;
  nxlib::u32 kDownOld = 0;
  nxlib::u32 kHeldOld = 0;
  nxlib::u32 kUpOld = 0;

  // Touchscreen
  Point2 all_pos [25];
  Point2 last_touch_pos;
  int last_button_state;

  virtual void delete_main_loop();

  void process_hid();

 protected:
  virtual int get_current_video_driver() const;
  //  virtual int get_audio_driver_count() const;
  //  virtual const char *get_audio_driver_name(int p_driver) const;
  virtual Error initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver);
  virtual void finalize();
  virtual void set_main_loop(MainLoop *p_main_loop);

 public:
  virtual void set_cursor_shape(CursorShape p_shape);
  virtual void set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot);
  virtual bool _check_internal_feature_support(const String &p_feature);
  virtual Point2 get_mouse_position() const;
  virtual int get_mouse_button_state() const;
  virtual void set_window_title(const String &p_title);
  virtual MainLoop *get_main_loop() const;
  virtual bool can_draw() const;
  virtual void set_video_mode(const VideoMode &p_video_mode, int p_screen = 0);
  virtual VideoMode get_video_mode(int p_screen = 0) const;
  virtual void get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen = 0) const;
  virtual Size2 get_window_size() const;

  virtual bool is_joy_known(int p_device);
  virtual String get_joy_guid(int p_device) const;

  virtual void release_rendering_thread();
  virtual void make_rendering_thread();
  virtual void swap_buffers();

  void run();

  OS_Switch();
};

#endif
