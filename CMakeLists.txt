cmake_minimum_required(VERSION 3.8)

project (pacman)

set(32BLIT_PATH "../32blit-beta" CACHE PATH "Path to 32blit.cmake")

include (${32BLIT_PATH}/32blit.cmake)

blit_executable (pacman pacman.cpp pacman.hpp)
blit_assets_yaml(pacman assets.yml)
blit_metadata(pacman metadata.yml)
