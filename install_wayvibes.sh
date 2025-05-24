#!/bin/bash
# filepath: ./install_wayvibes.sh

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Try to find a soundpack directory (first subdir with config.json)
SOUNDPACK_PATH=""
for d in "$SCRIPT_DIR"/*/ ; do
    if [ -f "$d/config.json" ]; then
        SOUNDPACK_PATH="$d"
        break
    fi
done

if [ -z "$SOUNDPACK_PATH" ]; then
    echo "Could not find a soundpack directory (with config.json) in $SCRIPT_DIR."
    read -p "Please enter the full path to your soundpack: " SOUNDPACK_PATH
fi

echo "Building with make..."
make
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

read -p "Do you want to install wayvibes system-wide (sudo make install)? (y/n): " INSTALL_CHOICE
if [[ "$INSTALL_CHOICE" =~ ^[Yy]$ ]]; then
    sudo make install
    echo "wayvibes installed to /usr/local/bin/"
else
    echo "You can run it from $SCRIPT_DIR/build/wayvibes or as built by make."
fi

read -p "Do you want to add auto-start to ~/.profile? (y/n): " PROFILE_CHOICE
if [[ "$PROFILE_CHOICE" =~ ^[Yy]$ ]]; then
    CMD="wayvibes \"$SOUNDPACK_PATH\" -v 3 -s"
    if ! grep -Fxq "$CMD" ~/.profile; then
        echo "$CMD" >> ~/.profile
        echo "Added to ~/.profile:"
        echo "$CMD"
    else
        echo "Command already present in ~/.profile"
    fi
else
    echo "Auto-start not added."
fi

echo "Done."