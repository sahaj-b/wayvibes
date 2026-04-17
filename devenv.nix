{
  pkgs,
  lib,
  ...
}: {
  packages = with pkgs; [
    nlohmann_json
    libevdev
    gnumake
    gcc
    alsa-lib
    libpulseaudio
  ];

  env.LD_LIBRARY_PATH = lib.makeLibraryPath [pkgs.alsa-lib pkgs.libpulseaudio];

  scripts.build.exec = ''
    make "$@"
  '';

  scripts.clean.exec = "make clean";
  enterShell = ''
    echo "wayvibes dev environment"
  '';
}
