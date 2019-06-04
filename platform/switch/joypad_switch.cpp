// Very simple implementation of a joypad input.
// It currently supports whatever is connected as joypad 1

#include "joypad_switch.h"

#include <unistd.h>


JoypadSwitch::Joypad::Joypad() {
  type = nxlib::TYPE_HANDHELD;
  joyID = nxlib::CONTROLLER_UNKNOWN;
}

JoypadSwitch::Joypad::~Joypad() {
}

void JoypadSwitch::Joypad::reset() {
  type = nxlib::TYPE_HANDHELD;
  joyID = nxlib::CONTROLLER_UNKNOWN;
  
}

JoypadSwitch::JoypadSwitch(InputDefault *in) {
	input = in;
	joy_mutex = Mutex::create();
	joy_thread = Thread::create(joy_thread_func, this);
}

JoypadSwitch::~JoypadSwitch() {
	Thread::wait_to_finish(joy_thread);
	memdelete(joy_thread);
	memdelete(joy_mutex);
	close_joypad();
}

void JoypadSwitch::joy_thread_func(void *p_user) {

	if (p_user) {
		JoypadSwitch *joy = (JoypadSwitch *)p_user;
		joy->run_joypad_thread();
	}
	return;
}

void JoypadSwitch::run_joypad_thread() {
	monitor_joypads();
}

// checked
void JoypadSwitch::monitor_joypads() {

  joy_mutex->lock();

  nxlib::hidScanInput();
  if (nxlib::hidGetControllerType(nxlib::CONTROLLER_P1_AUTO) > 0) {
    if (attached_devices.find(nxlib::CONTROLLER_P1_AUTO) == -1) {
      open_joypad(nxlib::CONTROLLER_P1_AUTO);
    }
  } else {
    close_joypad(get_joy_from_id(nxlib::CONTROLLER_P1_AUTO));
  }

  joy_mutex->unlock();
  usleep(1000000); // 1s
}

void JoypadSwitch::close_joypad(int joy_num) {
  if (joy_num == -1) {
    for (int i = 0; i < JOYPADS_MAX; i++) {
      
      close_joypad(i);
    };
    return;
  } else if (joy_num < 0)
    return;
  
  Joypad &joy = joypads[joy_num];

  if (joy.joyID != nxlib::CONTROLLER_UNKNOWN) {

    joy.joyID = nxlib::CONTROLLER_UNKNOWN;
    joy.type = nxlib::TYPE_HANDHELD;
    attached_devices.remove(attached_devices.find(joy.joyID));
    input->joy_connection_changed(joy_num, false, "");
  };
}

void JoypadSwitch::open_joypad(nxlib::HidControllerID joyID) {

  int joy_num = input->get_unused_joy_id();
  if (joy_num != -1) {

    // add to attached devices so we don't try to open it again
    attached_devices.push_back(joyID);

    joypads[joy_num].reset();

    Joypad &joy = joypads[joy_num];
    joy.type = nxlib::hidGetControllerType(joyID);
    joy.joyID = joyID;

    String uid ="__SWITCH_PRO__";
    String name = "Nintendo Switch Pro Controller";

    input->joy_connection_changed(joy_num, true, name, uid);

  }
}

int JoypadSwitch::get_joy_from_id(nxlib::HidControllerID joyID) {

  for (int i = 0; i < JOYPADS_MAX; i++) {

    if (joypads[i].joyID == joyID) {
      return i;
    }
  }
  return -2;
}

void JoypadSwitch::process_joypads() {

  if (joy_mutex->try_lock() != OK) {
    return;
  }
  for (int i = 0; i < JOYPADS_MAX; i++) {
    
    if (joypads[i].joyID == nxlib::CONTROLLER_UNKNOWN) continue;

    Joypad *joy = &joypads[i];

    nxlib::hidScanInput();

    // Buttons
    nxlib::u64 kDown = nxlib::hidKeysDown(joy->joyID);
    nxlib::u64 kHeld = nxlib::hidKeysHeld(joy->joyID);
    nxlib::u64 kUp = nxlib::hidKeysUp(joy->joyID);

    int button_mask = 1;
    for (int j = 0; j < 29; j++) {
      input->joy_button(i, j, (kDown & button_mask) or (kHeld & button_mask));
      button_mask *= 2;
    }

  }
  joy_mutex->unlock();
}
