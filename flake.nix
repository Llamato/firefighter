{
  description = "Figherfighter c64 game development environment";

  inputs = {
    nixpkgs.url = "github:NixOs/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
    dotfiles-llamato = {
      url = "github:llamato/dotfiles";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, ... } @ inputs: let
    supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "riscv64-linux"
        "aarch64-darwin"
      ];
  in 
  inputs.flake-utils.lib.eachSystem supportedSystems (system:
    let
        pkgs = import nixpkgs { inherit system; };
        llvm-mos-sdk = pkgs.callPackage (
          inputs.dotfiles-llamato + "/nixos/packages/llvm-mos-sdk/package.nix"
        ) { };
    in
    {
      packages = rec {
        firefighter = pkgs.stdenv.mkDerivation rec {
            name = "firefighter";
            version = "0.0.1";
            src = ./.;
            #includes = 

            buildPhase = ''
              runHook preBuild
              mkdir -p $out
              ${pkgs.acme}/bin/acme --cpu 6510 --format cbm -o assets/mysprites.prg assets/mysprites.asm
              ${llvm-mos-sdk}/bin/mos-c64-clang -Os main.c glibs64c/common.c glibs64c/graphics.c glibs64c/gllm/gllm.c glibs64c/hardware/cia.c glibs64c/hardware/disk.c glibs64c/hardware/sid.c glibs64c/hardware/vic.c -o ${name}.prg
              runHook postBuild
            '';

            installPhase = ''
              mkdir -p $out
              cp ${name}.prg $out
            '';
          };
          default = firefighter;
      };
      devShells = {
        packages = with pkgs; [
          acme
          llvm-mos-sdk
          vice
        ];
      };
    }
  );
}