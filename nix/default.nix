{pkgs ? import <nixpkgs> {}}:
pkgs.stdenv.mkDerivation {
  pname = "wayvibes";
  version = "main";

  src = ../.;

  nativeBuildInputs = [
    pkgs.gnumake
    pkgs.makeWrapper
  ];

  buildInputs = [
    pkgs.nlohmann_json
    pkgs.libevdev
    pkgs.gcc
  ];

  installPhase = ''
    mkdir -p $out/bin
    cp wayvibes $out/bin/
    wrapProgram $out/bin/wayvibes \
      --prefix LD_LIBRARY_PATH : ${pkgs.lib.makeLibraryPath [pkgs.alsa-lib pkgs.libpulseaudio]}
  '';
}
