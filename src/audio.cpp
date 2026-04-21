#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"
#include "miniaudio.h"
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

ma_engine engine;

ma_result initializeAudioEngine() { return ma_engine_init(NULL, &engine); }

void uninitializeAudioEngine() { ma_engine_uninit(&engine); }

void playSound(const std::string &soundFile) {
  if (ma_engine_play_sound(&engine, soundFile.c_str(), NULL) != MA_SUCCESS) {
    std::cerr << "Error playing sound: " << soundFile << std::endl;
  }
}

void setVolume(float volume) { ma_engine_set_volume(&engine, volume); }

void runMainLoop(const std::string &devicePath,
                 const std::unordered_map<int, std::string> &keySoundMap, float volume,
                 const std::string &soundpackPath) {
  int fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    std::cerr << "Failed to open input device: " << devicePath << std::endl;
    return;
  }

  char name[256] = "Unknown";
  ioctl(fd, EVIOCGNAME(sizeof(name)), name);

  setVolume(volume);

  std::cout << "Listening for key events on: " << name << " (" << devicePath << ")" << std::endl;

  struct input_event ev;
  while (true) {
    ssize_t n = read(fd, &ev, sizeof(ev));
    if (n == sizeof(ev)) {
      if (ev.type == EV_KEY && ev.value == 1) { // key press
        auto it = keySoundMap.find(ev.code);
        if (it != keySoundMap.end()) {
          std::string soundFile = soundpackPath + "/" + it->second;
          playSound(soundFile);
        }
      }
    } else {
      usleep(1000); // sleep 1ms to avoid busy loop
    }
  }

  close(fd);
}
