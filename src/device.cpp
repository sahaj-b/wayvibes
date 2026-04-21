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

std::vector<KeyboardDevice> getAvailableKeyboards() {
  std::vector<KeyboardDevice> keyboards;
  DIR *dir = opendir(deviceDir);
  if (!dir) {
    std::cerr << RED << "Failed to open /dev/input directory" << RESET << std::endl;
    return keyboards;
  }

  struct dirent *entry;
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
        keyboards.push_back({devicePath, libevdev_get_name(dev)});
      }

      libevdev_free(dev);
      close(fd);
    }
  }

  closedir(dir);
  return keyboards;
}

std::string promptForDevice(const std::vector<KeyboardDevice> &keyboards) {
  if (keyboards.empty()) {
    std::cerr << RED << "No suitable keyboard input devices found!" << RESET << std::endl;
    return "";
  }

  std::cout << CYAN << "Available Keyboard devices:" << RESET << std::endl;

  for (size_t i = 0; i < keyboards.size(); ++i) {
    std::cout << CYAN << BOLD << (i + 1) << ". " << RESET << YELLOW << keyboards[i].name
              << RESET << " (" << keyboards[i].eventPath << ")" << std::endl;
  }

  if (keyboards.size() == 1) {
    std::cout << CYAN << "Selecting this keyboard device." << RESET << std::endl;
    return keyboards[0].eventPath.substr(std::string(deviceDir).length());
  }

  std::string selectedDevice;
  bool validChoice = false;

  while (!validChoice) {
    std::cout << CYAN << "Select a keyboard input device (1-" << keyboards.size()
              << "): " << RESET;
    int choice;
    std::cin >> choice;

    if (choice >= 1 && choice <= keyboards.size()) {
      selectedDevice =
          keyboards[choice - 1].eventPath.substr(std::string(deviceDir).length());
      validChoice = true;
    } else {
      std::cerr << RED << "Invalid choice. Please try again." << RESET << std::endl;
    }
  }

  return selectedDevice;
}

std::string resolveToByIdPath(const std::string &eventDevice) {
  namespace fs = std::filesystem;
  std::string byIdDir = "/dev/input/by-id/";

  try {
    if (!fs::exists(byIdDir)) {
      return ""; // No by-id directory, fallback to event path
    }

    std::string targetPath = fs::canonical(deviceDir + eventDevice);

    for (const auto &entry : fs::directory_iterator(byIdDir)) {
      if (fs::is_symlink(entry)) {
        std::string linkTarget = fs::canonical(entry);

        if (linkTarget == targetPath) {
          return entry.path().string();
        }
      }
    }
  } catch (const std::exception &e) {
    std::cerr << RED << "Error resolving symlink: " << e.what() << RESET << std::endl;
  }

  return ""; // No matching symlink found
}

std::string getDevicePathByName(const std::string &name) {
  auto keyboards = getAvailableKeyboards();
  for (const auto &kb : keyboards) {
    if (kb.name == name) {
      return kb.eventPath;
    }
  }
  return "";
}

std::string getInputDevicePath(std::string &configDir) {
  namespace fs = std::filesystem;
  std::string newFilePath = configDir + "/input_device";
  std::string oldNamePath = configDir + "/input_device_name";
  std::string oldPathPath = configDir + "/input_device_path";

  // Migration logic
  if (!fs::exists(newFilePath)) {
    if (fs::exists(oldNamePath)) {
      std::ifstream oldFile(oldNamePath);
      if (oldFile.is_open()) {
        std::string content;
        std::getline(oldFile, content);
        oldFile.close();
        if (!content.empty()) {
          std::ofstream newFile(newFilePath);
          newFile << "NAME:" << content;
          newFile.close();
        }
      }
      fs::remove(oldNamePath);
    } else if (fs::exists(oldPathPath)) {
      std::ifstream oldFile(oldPathPath);
      if (oldFile.is_open()) {
        std::string content;
        std::getline(oldFile, content);
        oldFile.close();
        if (!content.empty()) {
          std::ofstream newFile(newFilePath);
          newFile << "PATH:" << content;
          newFile.close();
        }
      }
      fs::remove(oldPathPath);
    }
  }

  std::ifstream inputFile(newFilePath);
  if (inputFile.is_open()) {
    std::string content;
    std::getline(inputFile, content);
    inputFile.close();

    if (!content.empty()) {
      if (content.substr(0, 5) == "NAME:") {
        return getDevicePathByName(content.substr(5));
      } else if (content.substr(0, 5) == "PATH:") {
        return content.substr(5);
      }
    }
  }

  return "";
}

void saveByPath(std::ofstream &outputFile, const std::string &selectedDeviceEvent,
                const std::string &devicePath, const std::string &reason) {
  std::string byIdPath = resolveToByIdPath(selectedDeviceEvent);
  std::string deviceToSave;

  if (!byIdPath.empty()) {
    deviceToSave = byIdPath;
  } else {
    std::cout << YELLOW << BOLD
              << "\nWarning: No persistent by-id symlink found for this device." << RESET
              << std::endl
              << YELLOW
              << "Saving the raw event path. If you plug/unplug devices, this path might "
                 "change. In that case, run `wavibes --device` to select the correct "
                 "device again."
              << RESET << std::endl;
    deviceToSave = devicePath;
  }

  outputFile << "PATH:" << deviceToSave;
  std::cout << GREEN << "Device PATH saved " << reason << ": " << deviceToSave << RESET
            << std::endl;
}

void saveInputDevice(std::string &configDir) {
  auto keyboards = getAvailableKeyboards();
  std::string selectedDeviceEvent = promptForDevice(keyboards);

  if (!selectedDeviceEvent.empty()) {
    std::string devicePath = std::string(deviceDir) + selectedDeviceEvent;
    std::string deviceName = "";
    int duplicateCount = 0;

    for (const auto &kb : keyboards) {
      if (kb.eventPath == devicePath) {
        deviceName = kb.name;
      }
    }

    std::ofstream outputFile(configDir + "/input_device");

    if (deviceName.empty()) {
      std::cerr << YELLOW
                << "Warning: Could not determine device name. Saving by PATH instead"
                << RESET << std::endl;
      saveByPath(outputFile, selectedDeviceEvent, devicePath, "");
    } else {
      for (const auto &kb : keyboards) {
        if (kb.name == deviceName) {
          duplicateCount++;
        }
      }

      if (duplicateCount > 1) {
        saveByPath(outputFile, selectedDeviceEvent, devicePath,
                   "(due to duplicate names)");
      } else {
        outputFile << "NAME:" << deviceName;
        std::cout << GREEN << "Device name saved: " << deviceName << RESET << std::endl;
      }
    }

    outputFile.close();
  } else {
    std::cerr << RED << "No device selected. Exiting." << RESET << std::endl;
    exit(1);
  }
}
