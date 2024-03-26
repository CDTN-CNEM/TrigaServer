{
  description = "Get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    plc.url = "github:SEURT-CDTN/libOpcTrigaPLC";
    plc.inputs.nixpkgs.follows = "nixpkgs";
    plc.inputs.flake-utils.follows = "flake-utils";

    spu.url = "github:SEURT-CDTN/libModbusSystematomSPU";
    spu.inputs.nixpkgs.follows = "nixpkgs";
    spu.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { nixpkgs, flake-utils, spu, plc, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        plc-pkg = plc.packages.${system}.default;
        spu-pkg = spu.packages.${system}.default;
        buildInputs = with pkgs; [
          boost
          cxxopts
          jsoncpp
          libmodbus
          libserialport
          plc-pkg
          spu-pkg
        ];
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
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
            "-DCMAKE_MODULE_PATH=${plc-pkg}/lib/cmake/LibOpcTrigaPLC;${spu-pkg}/lib/cmake/LibModbusSystematomSPU"
          ];
          inherit nativeBuildInputs;
          inherit buildInputs;
        };
      }
    );
}
