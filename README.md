# Wayvibes

Wayvibes is a Wayland-native CLI that plays mechanical keyboard sounds (or custom sounds) globally on keypresses. It utilizes `libevdev` to capture keypress events and [miniaudio](https://miniaud.io) to play sounds.

### Compiling

#### Prerequisites:
Ensure the following dependencies are installed:

- `libevdev` (`libevdev-devel` on debian/fedora)
- `nlohmann-json` (`nlohmann-json-devel` on debian/fedora)

To compile wayvibes, use the following command: 

```bash
g++ -o wayvibes main.cpp -levdev
```

### Usage

## Initial Setup
1. Add user to the `input` group by the following command:

```bash
sudo usermod -a -G input <your_username>
```

2. **Reboot** or **Logut and Login** for the changes to take effect.

3. Run the application:

```bash
./main <soundpack_path> -v <volume(0.0-10.0)>
```

**Example:** 

```bash
./main ./akko_lavender_purples/ -v 3
```
### Note:
- Default **Soundpack Path:** `./`
- Default **Volume:** `1`

## **Important**:
**Do not run the program with root privileges as it will monopolize the audio device until reboot.**

## Input Device Configuration
Upon the first run, Wayvibes will prompt you to select your input device if there are multiple available. This selection will be stored in:

`$XDG_CONFIG_HOME/wayvibes/input_device_path`

Typically, the input device will be something like `AT Translated Set 2 keyboard` or `USB Keyboard`. If you use a key remapper like `keyd`, select its virtual device (e.g., `keyd virtual keyboard`).

To reset and prompt for input device selection again, use:

```bash 

./wayvibes --prompt```


### Get Soundpacks

Wayvibes is compatible with the Mechvibes soundpack format. So, You can find soundpacks from the following sources:

- [Mechvibes Soundpacks](https://docs.google.com/spreadsheets/d/1PimUN_Qn3CWqfn-93YdVW8OWy8nzpz3w3me41S8S494)
- [Discord Community](https://discord.com/invite/MMVrhWxa4w)

### Why Wayvibes?

Unlike [mechvibes](https://mechvibes.com) and [rustyvibes](https://github.com/KunalBagaria/rustyvibes), which encounter [issues](https://github.com/KunalBagaria/rustyvibes/issues/23) on Wayland, Wayvibes aims to provide a seamless integration with wayland.
