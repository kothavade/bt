with import <nixpkgs> { };

mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.clang_19
    pkgs.python3
    pkgs.git
  ];
}
