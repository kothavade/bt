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
      pkgs.vcpkg
      pkgs.vcpkg-tool
    ];
    shellHook = ''
      export PATH="${pkgs.clang-tools}/bin:$PATH";
      export VCPKG_ROOT="/Users/ved/.vcpkg-clion/vcpkg/";
    '';
  }
