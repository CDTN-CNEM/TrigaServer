{
  description = "TrigaServer";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    spu.url = "github:SEURT-CDTN/libModbusSystematomSPU";
    spu.flake = false;

    plc.url = "github:SEURT-CDTN/libOpcTrigaPLC";
    plc.flake = false;

    open26541pp.url = "https://github.com/open62541pp/open62541pp?submodules=true";
    open26541pp.flake = false;
    open26541pp.type = "git";
    open26541pp.submodules = true;
  };

  outputs = { nixpkgs, flake-utils, spu, plc, open26541pp, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        buildInputs = with pkgs; [
          boost
          cxxopts
          jsoncpp
          libmodbus
          libserialport
        ];
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          python3
          stdenv
        ];
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs;
            [
              busybox
              git
            ]
            ++ buildInputs
            ++ nativeBuildInputs;
        };

        packages.default = pkgs.stdenv.mkDerivation {
          name = "TrigaServer";
          src = ./.;
          cmakeFlags = [
            "-DDONT_INSTALL_CONF=1"
            "-DFETCHCONTENT_SOURCE_DIR_LIBMODBUSSYSTEMATOMSPU=${spu}"
            "-DFETCHCONTENT_SOURCE_DIR_LIBOPCTRIGAPLC=${plc}"
            "-DFETCHCONTENT_SOURCE_DIR_OPEN62541PP=${open26541pp}"
          ];
          inherit nativeBuildInputs;
          inherit buildInputs;
        };
      }
    );
}
