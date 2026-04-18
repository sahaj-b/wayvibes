{
  config,
  lib,
  ...
}: let
  cfg = config.services.wayvibes;
in {
  options.services.wayvibes = {
    enable = lib.mkEnableOption "Wayvibes keyboard sound service";

    soundpack = lib.mkOption {
      type = lib.types.str;
      description = "Path to the soundpack directory";
      example = "/home/youruser/wayvibes/soundpacks/cherrymx-red-abs";
    };

    volume = lib.mkOption {
      type = lib.types.int;
      description = "Volume level";
      default = 3;
    };

    package = lib.mkOption {
      type = lib.types.package;
      description = "The wayvibes package to use";
    };
  };

  config = lib.mkIf cfg.enable {
    home.packages = [cfg.package];

    systemd.user.services.wayvibes = {
      Unit = {
        Description = "Wayvibes keyboard sound";
        After = ["pipewire.service" "graphical-session.target"];
        Wants = ["pipewire.service"];
        PartOf = ["graphical-session.target"];
      };
      Service = {
        Type = "simple";
        ExecStart = "${cfg.package}/bin/wayvibes ${cfg.soundpack} -v ${toString cfg.volume}";
        Restart = "on-failure";
        RestartSec = 3;
      };
      Install = {
        WantedBy = ["graphical-session.target"];
      };
    };
  };
}
