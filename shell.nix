with import <nixpkgs> { };

mkShell.override
  {
    stdenv = pkgs.llvmPackages.stdenv;
  }
  {
    buildInputs = [
      pkgs.clang-tools
      pkgs.python3
      pkgs.git
      pkgs.cmake
      pkgs.ninja
    ];
    shellHook = ''
      export PATH="${pkgs.clang-tools}/bin:$PATH";
    '';
  }
