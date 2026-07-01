{
  description = "Figherfighter c64 llvm-mos development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-26.05";
    llamato-dotfiles.url = "github:llamato/dotfiles";
    rust-overlay.url = "github:oxalica/rust-overlay";
  };

  outputs = { self, nixpkgs, llamato-dotfiles, rust-overlay, ... } @ inputs: let
    systems = [ "x86_64-linux" ];
    overlays = [(import rust-overlay)];
    forAllSystems = nixpkgs.lib. genAttrs systems;
    pkgsFor = system: import nixpkgs {
      inherit system overlays;
    };
  in {
    devShells = forAllSystems (system:
      let
        pkgs = pkgsFor system;
        llvm-mos = pkgs.callPackage (inputs.llamato-dotfiles + "/nixos/packages/llvm-mos/package.nix") { };
        rustToolchain = pkgs.rust-bin.stable."1.92.0".default.override {
          extensions = [ "rust-src" "rust-std" "rust-analyzer" "clippy" ];
          targets = [ "x86_64-unknown-linux-gnu" ];
        };
      in {
        default = pkgs.mkShell {
          buildInputs = with pkgs; [
            rustToolchain
            acme
            llvm-mos
            vice
          ];
        };
      }
    );
  };
}