#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <unordered_map>

// find available keyboard devices
std::string findKeyboardDevices();

// get the path of a device by its exact name
std::string getDevicePathByName(const std::string &name);

void runMainLoop(const std::string &devicePath,
                 const std::unordered_map<int, std::string> &keySoundMap, float volume,
                 const std::string &soundpackPath);

// get the input device path from the configuration directory
std::string getInputDevicePath(std::string &configDir);

// save the selected input device path to the configuration directory
void saveInputDevice(std::string &configDir);

#endif // DEVICE_H
