#ifndef AUDIO_DRIVER_SWITCH_H
#define AUDIO_DRIVER_SWITCH_H

#include "servers/audio_server.h"

#ifdef SWITCH_ENABLED

#include "core/os/mutex.h"
#include "core/os/thread.h"

namespace nxlib {
#include <switch.h>
}

class AudioDriverSwitch : public AudioDriver {
  Thread *thread;
  Mutex *mutex;

  String device_name;
  String new_device;

  Vector<int32_t> samples_in;

  SpeakerMode speaker_mode;
  int latency;

  nxlib::u32 data_size;
  nxlib::u8* out_buf_data;

  int buffer_frames;
  nxlib::u32 buffer_size;
  unsigned int period_size;
  int channels;

  bool active;
  bool thread_exited;
  mutable bool exit_thread;

  Error init_device();
  static void thread_func(void *p_udata);
  void finish_device();

public:
  const char *get_name() const {
    return "SwitchAudio";
  };

  virtual Error init();
  virtual void start();
  virtual int get_mix_rate() const;
  virtual SpeakerMode get_speaker_mode() const;
  virtual void lock();
  virtual void unlock();
  virtual void finish();

  virtual float get_latency();

  AudioDriverSwitch();
  ~AudioDriverSwitch();
};

#endif
#endif
