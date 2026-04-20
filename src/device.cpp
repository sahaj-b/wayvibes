#include "device.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <string>
#include <unistd.h>
#include <vector>

#define deviceDir "/dev/input/"
// ANSI color codes for terminal styling
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

std::string findKeyboardDevices() {
  DIR *dir = opendir(deviceDir);
  if (!dir) {
    std::cerr << RED << "Failed to open /dev/input directory" << RESET << std::endl;
    return "";
  }

  std::vector<std::string> devices;
  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strncmp(entry->d_name, "event", 5) == 0) {
      devices.push_back(entry->d_name);
    }
  }

  closedir(dir);

  if (devices.empty()) {
    std::cerr << RED << "No input devices found!" << RESET << std::endl;
    return "";
  }

  std::vector<std::string> filteredDevices;

  std::cout << CYAN << "Available Keyboard devices:" << RESET << std::endl;

  for (size_t i = 0, displayIndex = 1; i < devices.size(); ++i) {
    std::string devicePath = deviceDir + devices[i];
    struct libevdev *dev = nullptr;
    int fd = open(devicePath.c_str(), O_RDONLY);
    if (fd < 0) {
      std::cerr << RED << "Error opening " << devicePath << RESET << std::endl;
      continue;
    }

    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
      std::cerr << RED << "Failed to create evdev device for " << devicePath << RESET
                << std::endl;
      close(fd);
      continue;
    }

    if (libevdev_has_event_code(dev, EV_KEY, KEY_A)) {
      std::cout << CYAN << BOLD << displayIndex << ". " << RESET << YELLOW
                << libevdev_get_name(dev) << RESET << " (" << devices[i] << ")"
                << std::endl;
      filteredDevices.push_back(devices[i]);
      displayIndex++;
    }

    libevdev_free(dev);
    close(fd);
  }

  if (filteredDevices.empty()) {
    std::cerr << RED << "No suitable keyboard input devices found!" << RESET << std::endl;
    return "";
  }

  if (filteredDevices.size() == 1) {
    std::cout << CYAN << "Selecting this keyboard device." << RESET << std::endl;
    return filteredDevices[0];
  }

  std::string selectedDevice;
  bool validChoice = false;

  while (!validChoice) {
    std::cout << CYAN << "Select a keyboard input device (1-" << filteredDevices.size()
              << "): " << RESET;
    int choice;
    std::cin >> choice;

    if (choice >= 1 && choice <= (int)filteredDevices.size()) {
      selectedDevice = filteredDevices[choice - 1];
      validChoice = true;
    } else {
      std::cerr << RED << "Invalid choice. Please try again." << RESET << std::endl;
    }
  }

  return selectedDevice;
}

// Finds a keyboard device path by its exact evdev name.
// Used by the --device-name CLI flag to select a device without interactive prompting.
std::string getDevicePathByName(const std::string &name) {
  DIR *dir = opendir(deviceDir);
  if (!dir) {
    std::cerr << RED << "Failed to open /dev/input directory" << RESET << std::endl;
    return "";
  }

  struct dirent *entry;
  std::string foundPath = "";

  while ((entry = readdir(dir)) != NULL) {
    if (strncmp(entry->d_name, "event", 5) == 0) {
      std::string devicePath = std::string(deviceDir) + entry->d_name;
      struct libevdev *dev = nullptr;
      int fd = open(devicePath.c_str(), O_RDONLY);
      if (fd < 0) continue;

      int rc = libevdev_new_from_fd(fd, &dev);
      if (rc < 0) {
        close(fd);
        continue;
      }

      if (libevdev_has_event_code(dev, EV_KEY, KEY_A)) {
        if (name == libevdev_get_name(dev)) {
          foundPath = devicePath;
          libevdev_free(dev);
          close(fd);
          break;
        }
      }

      libevdev_free(dev);
      close(fd);
    }
  }

  closedir(dir);
  return foundPath;
}

std::string getInputDevicePath(std::string &configDir) {
  std::string inputFilePath = configDir + "/input_device_name";
  std::ifstream inputFile(inputFilePath);

  if (inputFile.is_open()) {
    std::string savedPath;
    std::getline(inputFile, savedPath);
    inputFile.close();

    if (!savedPath.empty() && std::filesystem::exists(savedPath)) {
      return savedPath;
    }
  }

  return "";
}

void saveInputDevice(std::string &configDir) {
  std::string selectedDevice = findKeyboardDevices();
  if (!selectedDevice.empty()) {
    // Default to the direct /dev/input/eventX path
    std::string finalPath = std::string(deviceDir) + selectedDevice;

    // Prefer a stable symlink from /dev/input/by-id/ if one points to this device,
    // so the correct device is still found after a reboot or re-plug.
    for (const auto &entry : std::filesystem::directory_iterator("/dev/input/by-id/")) {
      if (std::filesystem::is_symlink(entry)) {
        if (std::filesystem::read_symlink(entry).filename() == selectedDevice) {
          finalPath = entry.path().string();
          break;
        }
      }
    }

    std::ofstream outputFile(configDir + "/input_device_name");
    outputFile << finalPath;
    outputFile.close();
    std::cout << GREEN << "Device path saved: " << finalPath << RESET << std::endl;
  } else {
    std::cerr << RED << "No device selected. Exiting." << RESET << std::endl;
    exit(1);
  }
}

