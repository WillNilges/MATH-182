{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell
{
  name = "openGL";

  buildInputs = [
    pkgs.clang-tools
    pkgs.clang
  ];

  nativeBuildInputs = with pkgs; [
    # Compiler + Build System
    cmake
    gcc
    glibc

    # Dependencies for MATH-182
    glfw
    glm
    lua
    assimp

    # Dependencies for Neovim LSP setup
    python312 # clang-format

    # Tools for debugging
    gdb
    valgrind
  ];
}
