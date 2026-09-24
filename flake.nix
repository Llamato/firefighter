{
  description = "Figherfighter c64 game development environment";

  inputs = {
    nixpkgs.url = "github:NixOs/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
    dotfiles-llamato = {
      url = "github:llamato/dotfiles";
    };
  };

  outputs = { self, nixpkgs, ... } @ inputs: let
    supportedSystems = [
      "x86_64-linux"
      "aarch64-linux"
      "riscv64-linux"
    ];
  in 
  inputs.flake-utils.lib.eachSystem supportedSystems (system:
    let
        pkgs = import nixpkgs { inherit system; };
        lib = pkgs.lib;
        llvm-mos-sdk = inputs.dotfiles-llamato.packages.${system}.llvm-mos-sdk;
        src = ./.;
        treverse = dir: builtins.concatMap 
          (filesystemNode: if filesystemNode.value == "regular" then ["${dir}/${filesystemNode.name}"] else treverse "${dir}/${filesystemNode.name}") 
          (lib.attrsToList (builtins.readDir dir));
        allFiles = treverse src;
        cFiles = builtins.filter (file: lib.hasSuffix ".c" file) allFiles;
        mosCincludes = lib.concatStringsSep " " cFiles;
        cFlags = [
          "-Os"
          "-Wl,--section-start=.housedata=0xC000"
        ];
        mosCflags = lib.concatStringsSep " " cFlags;
    in
    {
      packages = rec {
        firefighter = pkgs.stdenv.mkDerivation rec {
            inherit src;
            name = "firefighter";
            version = "0.0.1";
            buildPhase = ''
              runHook preBuild
              mkdir -p $out
              ${pkgs.acme}/bin/acme --cpu 6510 --format cbm -o assets/mysprites.prg assets/mysprites.asm
              ${llvm-mos-sdk}/bin/mos-c64-clang ${mosCincludes} ${mosCflags} -o ${name}.prg
              runHook postBuild
            '';

            installPhase = ''
              mkdir -p $out
              cp ${name}.prg $out
            '';
          };
          default = firefighter;
      };

      apps.default = let 
        package = self.packages.${system}.default; 
      in {
      type = "app";
      program = "${pkgs.writeShellScript "run-spacebirds64" ''
        exec ${pkgs.vice}/bin/x64sc ${package}/${package.name}.prg "$@"
      ''}";
    };

      devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            acme
            llvm-mos-sdk
            vice
          ];

          shellHook = ''
            echo "Includes are:"
            find . -name "*.c" -type f | tr '\n' ' '
          '';
        };
    }
  );
}