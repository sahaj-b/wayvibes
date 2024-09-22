# Wayvibes
A wayland native C++ CLI which plays mechanical keyboard sounds (or custom sounds) on keypresses.
It uses libevdev to get keypresses and [miniaudio](https://miniaud.io) to play sounds.

## Currently Work In Progress

# Compiling

## Prerequisites:
- libevdev

`g++ -o main main.cpp -levdev`

# Usage
`./main <soundpack_path> -v <volume(1.0-10.0)>`

Example: `./main ./akko_lavender_purples/ -v 3`

Default soundpack path: `./`

Default volume: `2`

For specifying your input device again, run `./main --prompt`

It will be stored in $XDG_CONFIG_HOME/wayvibes/input_device_path

# Get Soundpacks
Wayvibes is compatible with mechvibes soundpacks format. So you can get them from [Mechvibes Soundpacks](https://docs.google.com/spreadsheets/d/1PimUN_Qn3CWqfn-93YdVW8OWy8nzpz3w3me41S8S494) and there [discord](https://discord.com/invite/MMVrhWxa4w) for more soundpacks

# Why wayvibes?
mechvibes and rustyvibes has [ issues ](https://github.com/KunalBagaria/rustyvibes/issues/23) working on wayland.
