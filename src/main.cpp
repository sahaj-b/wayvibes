#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "audio.h"
#include "device.h"
#include "config.h"
#include <algorithm>
#include <unistd.h>

int main(int argc, char *argv[]) {
    std::string soundpackPath = "./";
    float volume = 1.0f;
    std::string configDir;
    bool silent = false;
    const char *xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
    configDir = (xdgConfigHome ? xdgConfigHome : std::string(getenv("HOME")) + "/.config") + "/wayvibes";

    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--prompt") {
            saveInputDevice(configDir);
            return 0;
        } else if (std::string(argv[i]) == "-v" && (i + 1) < argc) {
            try {
                volume = std::stof(argv[i + 1]);
                i++;
            } catch (...) {
                std::cerr << "Invalid volume argument. Using default volume." << std::endl;
            }
        } else if (std::string(argv[i]) == "--silent" || std::string(argv[i]) == "-s") {
            silent = true;
        } else if (argv[i][0] != '-') {
            soundpackPath = argv[i];
        }
    }

    if (silent) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "Failed to fork for silent/background mode." << std::endl;
            return 1;
        }
        if (pid > 0) {
            // Parent process exits, child continues in background
            return 0;
        }
        // Child process becomes session leader, detaches from terminal
        setsid();
        // Optionally redirect stdio to /dev/null
        freopen("/dev/null", "r", stdin);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
    }

    volume = std::clamp(volume, 0.0f, 10.0f);

    if (initializeAudioEngine() != MA_SUCCESS) {
        if (!silent) std::cerr << "Failed to initialize audio engine" << std::endl;
        return 1;
    }

    if (!silent) std::cout << "Soundpack: " << soundpackPath << std::endl;
    std::unordered_map<int, std::string> keySoundMap = loadKeySoundMappings(soundpackPath + "/config.json");

    std::string devicePath = getInputDevicePath(configDir);

    if (devicePath.empty()) {
        if (!silent) std::cout << "No device found. Prompting user." << std::endl;
        saveInputDevice(configDir);
        devicePath = getInputDevicePath(configDir);
    }

    runMainLoop(devicePath, keySoundMap, volume, soundpackPath);

    ma_engine_uninit(&engine);
    return 0;
}