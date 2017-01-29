# -----------------------------------------------------------------------------
# Distributed under the GPL-3.0 License.
# This CMake file is integrated with the whole game project.
# Just intend to find SDL2_image module installed by user.
# --------
#
# This .cmake file is written with the style of official `FindSDL_image.cmake`.
# See official documentation:
#   https://cmake.org/cmake/help/v3.7/module/FindSDL_image.html
#
# You can also find this file named `FindSDL_image.cmake` in the default CMake
# module directory (for example, /usr/local/cmake/share/cmake-x.x/Modules/).
# But notice that this official .cmake file can only locate SDL_image library
# released as an old version.
#
# --------
# Locate SDL2_image library
#
# This cmake module defines:
#
# ::
#
#   SDL2_IMAGE_LIBRARIES,       the name of the library to link against
#   SDL2_IMAGE_INCLUDE_DIRS,    where to find the headers
#   SDL2_IMAGE_FOUND,           if false, do not try to link against
#   SDL2_IMAGE_VERSION_STRING   human-readable string containing the version
#                               of SDL_image
#
# For backward compatibility the following variables are also set:
#
# ::
#
#   SDL2_IMAGE_LIBRARY (same value as SDL_IMAGE_LIBRARIES)
#   SDL2_IMAGE_INCLUDE_DIR (same value as SDL_IMAGE_INCLUDE_DIRS)
#
# $SDLDIR is an environment variable that would correspond to the command
#   ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Perry Leo. (Improvements welcomed!)
# -----------------------------------------------------------------------------

find_package(PkgConfig QUIET)
pkg_check_modules(PC_SDL2_IMAGE QUIET SDL2_image)

find_path(SDL2_IMAGE_INCLUDE_DIR
    NAMES SDL_image.h
    HINTS ${PC_SDL2_IMAGE_INCLUDEDIR}
          ${PC_SDL2_IMAGE_INCLUDE_DIRS}
    # path suffixes to search inside include directories
    PATH_SUFFIXES SDL2
)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS ${PC_SDL2_IMAGE_LIBDIR}
          ${PC_SDL2_IMAGE_LIBRARY_DIRS}
    PATH_SUFFIXES x64 x86
)

if(SDL2_IMAGE_INCLUDE_DIR AND EXISTS "${SDL2_IMAGE_INCLUDE_DIR}/SDL_image.h")
    file(STRINGS "${SDL2_IMAGE_INCLUDE_DIR}/SDL_image.h" SDL2_IMAGE_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_IMAGE_MAJOR_VERSION[ \t]+[0-9]+$")
    file(STRINGS "${SDL2_IMAGE_INCLUDE_DIR}/SDL_image.h" SDL2_IMAGE_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_IMAGE_MINOR_VERSION[ \t]+[0-9]+$")
    file(STRINGS "${SDL2_IMAGE_INCLUDE_DIR}/SDL_image.h" SDL2_IMAGE_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_IMAGE_PATCHLEVEL[ \t]+[0-9]+$")
    string(REGEX REPLACE "^#define[ \t]+SDL_IMAGE_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_IMAGE_VERSION_MAJOR "${SDL2_IMAGE_VERSION_MAJOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+SDL_IMAGE_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_IMAGE_VERSION_MINOR "${SDL2_IMAGE_VERSION_MINOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+SDL_IMAGE_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_IMAGE_VERSION_PATCH "${SDL2_IMAGE_VERSION_PATCH_LINE}")
    set(SDL2_IMAGE_VERSION_STRING ${SDL2_IMAGE_VERSION_MAJOR}.${SDL2_IMAGE_VERSION_MINOR}.${SDL2_IMAGE_VERSION_PATCH})
    unset(SDL2_IMAGE_VERSION_MAJOR_LINE)
    unset(SDL2_IMAGE_VERSION_MINOR_LINE)
    unset(SDL2_IMAGE_VERSION_PATCH_LINE)
    unset(SDL2_IMAGE_VERSION_MAJOR)
    unset(SDL2_IMAGE_VERSION_MINOR)
    unset(SDL2_IMAGE_VERSION_PATCH)
endif()

# for backward compatibility
set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})
set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2_image
                                  REQUIRED_VARS SDL2_IMAGE_INCLUDE_DIRS SDL2_IMAGE_LIBRARIES
                                  VERSION_VAR SDL2_IMAGE_VERSION_STRING)

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
