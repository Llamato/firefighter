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
        src = ./.;
        treverse = with builtins // lib; dir: concatMap 
          (filesystemNode: if filesystemNode.value == "regular" then ["${dir}/${filesystemNode.name}"] else treverse "${dir}/${filesystemNode.name}") 
          (attrsToList (readDir dir));
        allFiles = treverse src;
        cFiles = with builtins // lib; filter (file: hasSuffix ".c" file) allFiles;
        llvm-mos-sdk = inputs.dotfiles-llamato.packages.${system}.llvm-mos-sdk;
    in
    {
      packages = rec {
        firefighter = pkgs.stdenv.mkDerivation rec {
            inherit src;
            name = "firefighter";
            version = "0.0.1";
            includes = with builtins // lib; concatStringsSep " " cFiles;
            buildPhase = ''
              runHook preBuild
              mkdir -p $out
              ${pkgs.acme}/bin/acme --cpu 6510 --format cbm -o assets/mysprites.prg assets/mysprites.asm
              ${llvm-mos-sdk}/bin/mos-c64-clang -Os ${includes} -o ${name}.prg
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