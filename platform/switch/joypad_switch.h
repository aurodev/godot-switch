#ifndef JOYPAD_SWITCH_H
#define JOYPAD_SWITCH_H

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "main/input_default.h"

namespace nxlib {
#include <switch.h>
}

class JoypadSwitch {
public:
  JoypadSwitch(InputDefault *in);
  ~JoypadSwitch();
  void process_joypads();

private:
  enum {
    JOYPADS_MAX = 8,
  };

  struct Joypad {
    nxlib::HidControllerType type;
    nxlib::HidControllerID joyID;

    Joypad();
    ~Joypad();
    void reset();
  };

  Mutex *joy_mutex;
  Thread *joy_thread;
  InputDefault *input;
  Joypad joypads[JOYPADS_MAX];
  Vector<nxlib::HidControllerID> attached_devices;

  static void joy_thread_func(void *p_user);

  int get_joy_from_path(String p_path) const;

  void close_joypad(int p_id = -1);

  void monitor_joypads();
  void run_joypad_thread();
  void open_joypad(nxlib::HidControllerID joyID);
  int get_joy_from_id(nxlib::HidControllerID joyID);
};

#endif // JOYPAD_SWITCH_H
