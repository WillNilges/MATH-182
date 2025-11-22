{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell
{
  name = "openGL";

  nativeBuildInputs = with pkgs; [
    cmake
    glfw
    lua
    glm
    gcc
  ];

}
