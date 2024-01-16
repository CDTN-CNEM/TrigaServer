{
  description = "TrigaServer";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    spu.url = github:CDTNuclear/libModbusSystematomSPU;
    spu.flake = false;

    plc.url = github:CDTNuclear/libModbusMaestecPLC;
    plc.flake = false;
  };

  outputs = { self, nixpkgs, flake-utils, spu, plc, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            boost
            cmake
            cxxopts
            jsoncpp
            libmodbus
            libserialport
            pkg-config
            stdenv
          ];
        };

        packages.default = pkgs.stdenv.mkDerivation {
          name = "TrigaServer";
          src = ./.;
          cmakeFlags = [
            "-DDONT_INSTALL_CONF=1"
            "-DFETCHCONTENT_SOURCE_DIR_LIBMODBUSSYSTEMATOMSPU=${spu}"
            "-DFETCHCONTENT_SOURCE_DIR_LIBMODBUSMAESTECPLC=${plc}"
          ];
          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
          ];
          buildInputs = with pkgs;[
            boost
            cxxopts
            jsoncpp
            libmodbus
            libserialport
          ];
        };
      }
    );
}
