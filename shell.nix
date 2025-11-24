{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell
{
  name = "openGL";

  nativeBuildInputs = with pkgs; [
    # clang
    # clang-tools
    cmake
    glfw
    lua
    glm
    gcc
    gdb
    glibc
  ];

}
