# Wayvibes

Wayvibes is a Wayland-native CLI made in C++ that plays mechanical keyboard sounds (or custom sounds) globally on keypresses. It utilizes `libevdev` to capture keypress events and [miniaudio](https://miniaud.io) to play sounds.

## Installing
### From AUR
Install [wayvibes-git](https://aur.archlinux.org/packages/wayvibes-git) from AUR (maintained by  [@justanoobcoder](https://www.github.com/justanoobcoder)).

```bash
#using yay
yay -S wayvibes-git
```

### From Source
#### Prerequisites:
Ensure the following dependencies are installed:

Ubuntu-based distros:
- `libevdev-dev`
- `nlohmann-json*-dev`

You can install them easily by running
`sudo apt install libevdev-dev nlohmann-json*-dev`

Arch:
- `libevdev`
- `nlohmann-json`

To install wayvibes, use the following commands: 

```bash
git clone https://github.com/sahaj-b/wayvibes
cd wayvibes
make
sudo make install
```

## Uninstalling
```bash
sudo make uninstall
```

## Usage

### Initial Setup
1. Add user to the `input` group by the following command:

```bash
sudo usermod -a -G input <your_username>
```

2. **Reboot** or **Logout and Login** for the changes to take effect.

3. Run the application:

```bash
wayvibes <soundpack_path> -v <volume(0.0-10.0)> -s
```

**Example:** 

```bash
wayvibes ~/wayvibes/akko_lavender_purples/ -v 3 -s
```

#### Note:
- Default **Soundpack Path:** `./`
- Default **Volume:** `1`

### Keyboard Device Configuration
Upon the first run, Wayvibes will prompt you to select your keyboard device if there are multiple available. This selection will be stored in:

`$XDG_CONFIG_HOME/wayvibes/input_device_path`

Typically, the input device will be something like `AT Translated Set 2 keyboard` or `USB Keyboard`. If you use a key remapper like `keyd`, select its virtual device (e.g., `keyd virtual keyboard`).

To reset and prompt for input device selection again, use:

```bash 
wayvibes --prompt
```

**Note that input device will change after reboot if you plug in/out any other device, so use --prompt when this happens**


### **Important**:
**Do not run the program with root privileges as it will monopolize the audio device until reboot.**

## Manual Installation
```bash
git clone https://github.com/sahaj-b/wayvibes
cd wayvibes
g++ -o wayvibes main.cpp -levdev
sudo cp wayvibes /usr/local/bin # to add the executable to PATH
```

## Get Soundpacks

Wayvibes is compatible with the Mechvibes soundpack format. So, You can find soundpacks from the following sources:

- [Mechvibes Soundpacks](https://docs.google.com/spreadsheets/d/1PimUN_Qn3CWqfn-93YdVW8OWy8nzpz3w3me41S8S494)
- [Discord Community](https://discord.com/invite/MMVrhWxa4w) (got akko_lavender_purples soundpack from here)

### Note:
Some soundpacks with single audio file configuration won't work, use [this tool](https://github.com/KunalBagaria/packfixer-rustyvibes) to convert them into a compatible format

### Ogg files incompatiblity
Wayvibes uses miniaudio to play sounds, which doesn't support all ogg files by default. So, you need to convert ogg files to wav/mp3 files using `ffmpeg` or `sox`, and change the extensions in the `config.json` file. Use this command for this:

Converting ogg files to wav using `ffmpeg` and change extensions in `config.json`:

```bash
cd <soundpack_path>
for f in *.ogg; do ffmpeg -i "$f" "${f%.ogg}.wav"; done && sed -i 's/\.ogg/\.wav/g' config.json
rm *.ogg # remove ogg files
```

## Why Wayvibes?

Unlike [mechvibes](https://mechvibes.com) and [rustyvibes](https://github.com/KunalBagaria/rustyvibes), which encounter [issues](https://github.com/KunalBagaria/rustyvibes/issues/23) on Wayland, Wayvibes aims to provide a seamless integration with wayland.
