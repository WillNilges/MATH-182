{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell
{
  name = "openGL";

  buildInputs = [
    pkgs.clang-tools
    pkgs.clang
  ];

  nativeBuildInputs = with pkgs; [
    #clang-tools
    #clang
    cmake
    glfw
    lua
    glm
    gcc
    gdb
    glibc
    python312
  ];

}
