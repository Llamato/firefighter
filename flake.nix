{
  description = "Figherfighter c64 llvm-mos development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-26.05";
    llamato-dotfiles.url = "github:llamato/dotfiles";
  };

  outputs = { self, nixpkgs, llamato-dotfiles, ... } @ inputs: let
    systems = [ "x86_64-linux" ];
  in {
    devShells = nixpkgs.lib.genAttrs systems (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        llvm-mos = pkgs.callPackage (inputs.llamato-dotfiles + "/nixos/packages/llvm-mos/package.nix") { };
      in {
        default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
          buildInputs = with pkgs; [
            bun
            acme
            llvm-mos
            vice
          ];
        };
      }
    );
  };
}