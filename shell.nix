{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = with pkgs; [
    gcc
    gnumake
    libGL
    libGLU
    glfw
    glew

    libx11
    libxext
    libxrandr
    libxinerama
    libxcursor
    libxi
  ];
}
