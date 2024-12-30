with import <nixpkgs> { };

mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.python3
    pkgs.git
    pkgs.cmake
    pkgs.ninja
  ];
}
