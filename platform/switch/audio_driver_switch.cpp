// Audio driver implementation - NOT WORKING -

#include <malloc.h>

#include "audio_driver_switch.h"

#include "core/project_settings.h"

void AudioDriverSwitch::thread_func(void *p_udata){
  AudioDriverSwitch *ad = (AudioDriverSwitch *)p_udata;
  while (!ad->exit_thread) {

    ad->lock();
    ad->start_counting_ticks();

    if (!ad->active) {
      for (unsigned int i = 0; i < ad->period_size * ad->channels; i++) {
	ad->out_buf_data[i] = 0;
      }

    } else {
      ad->audio_server_process(ad->period_size, ad->samples_in.ptrw());

      for (unsigned int i = 0; i < ad->period_size * ad->channels; i++) {
	ad->out_buf_data[i] = ad->samples_in[i] >> 16;
      }
    }
    int todo = ad->period_size;
    int total = 0;

    while (todo && !ad->exit_thread) {

      nxlib::AudioOutBuffer buffer;
      buffer.next = NULL;
      buffer.buffer = ad->out_buf_data; // (void *)(src + (total * ad->channels));
      buffer.buffer_size = ad->buffer_size; //(todo + 0xfff) & ~0xfff;
      buffer.data_size = ad->data_size; // todo;
      buffer.data_offset = 0;
      nxlib::Result rc = nxlib::audoutAppendAudioOutBuffer(&buffer);
      
      if (rc == 0) {
	total = todo;
	todo = 0;
      } else {
	ad->active = false;
	ad->exit_thread = true;
      }
    }

    // User selected a new device, finish the current one so we'll init the new device
    if (ad->device_name != ad->new_device) {
      ad->device_name = ad->new_device;
      ad->finish_device();

      Error err = ad->init_device();
      if (err != OK) {
	ERR_PRINT("ALSA: init_device error");
	ad->device_name = "Default";
	ad->new_device = "Default";

	err = ad->init_device();
	if (err != OK) {
	  ad->active = false;
	  ad->exit_thread = true;
	}
      }
    }

    ad->stop_counting_ticks();
    ad->unlock();
  }

  ad->thread_exited = true;
}

Error AudioDriverSwitch::init_device() {
  speaker_mode = SPEAKER_MODE_STEREO;
  channels = nxlib::audoutGetChannelCount();

  unsigned int periods = 2;
  latency = GLOBAL_DEF("audio/output_latency", DEFAULT_OUTPUT_LATENCY);

  buffer_frames = closest_power_of_2(latency * nxlib::audoutGetSampleRate() / 1000);
  buffer_size = buffer_frames * periods;
  period_size = buffer_frames;

  // Allocate the buffer.
  out_buf_data = (nxlib::u8*)memalign(0x1000, buffer_size);
  memset(out_buf_data, 0, buffer_size);



  samples_in.resize(period_size * channels);

  nxlib::audoutInitialize();
  nxlib::audoutStartAudioOut();

  return OK;
}

Error AudioDriverSwitch::init(){
  active = false;
  thread_exited = false;
  exit_thread = false;

  Error err = init_device();
  if (err == OK) {
    mutex = Mutex::create();
    // TODO (fhidalgo): enable this once driver is working
    //thread = Thread::create(AudioDriverSwitch::thread_func, this);
  }

  return err;
}

void AudioDriverSwitch::start(){
  active = true;
}

int AudioDriverSwitch::get_mix_rate() const {
  return nxlib::audoutGetSampleRate();
}

AudioDriver::SpeakerMode AudioDriverSwitch::get_speaker_mode() const {
  return get_speaker_mode_by_total_channels(channels);
}

void AudioDriverSwitch::lock(){
  if (!thread || !mutex)
    return;
  mutex->lock();
}

void AudioDriverSwitch::unlock(){
  if (!thread || !mutex)
    return;
  mutex->unlock();
}

void AudioDriverSwitch::finish(){
  if (thread) {
    exit_thread = true;
    Thread::wait_to_finish(thread);

    memdelete(thread);
    thread = NULL;

    if (mutex) {
      memdelete(mutex);
      mutex = NULL;
    }
  }

  finish_device();
}

void AudioDriverSwitch::finish_device() {
  nxlib::Result rc = nxlib::audoutStopAudioOut();

  // Terminate the default audio output device.
  nxlib::audoutExit();
}

float AudioDriverSwitch::get_latency(){
  return latency;
}

AudioDriverSwitch::AudioDriverSwitch() {
}

AudioDriverSwitch::~AudioDriverSwitch(){
}
