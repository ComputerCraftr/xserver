#!/bin/bash

set -e

. .github/scripts/util.sh

mkdir -p $X11_BUILD_DIR
cd $X11_BUILD_DIR

# Provide X11-style GL headers using Apple's OpenGL framework
# This prevents Meson from linking against a Homebrew mesa package while
# still allowing the GLX server code to compile.
mkdir -p $X11_PREFIX/include/GL
SDK_PATH=$(xcrun --show-sdk-path)
ln -sf "$SDK_PATH/System/Library/Frameworks/OpenGL.framework/Headers/gl.h" $X11_PREFIX/include/GL/gl.h
ln -sf "$SDK_PATH/System/Library/Frameworks/OpenGL.framework/Headers/glext.h" $X11_PREFIX/include/GL/glext.h

build_meson   rendercheck       $(fdo_mirror rendercheck)                  $PKG_RENDERCHECK_REF
build_meson   libxcvt           $(fdo_mirror libxcvt)                      $PKG_LIBXCVT_REF
build_meson   xorgproto         $(fdo_mirror xorgproto)                    $PKG_XORGPROTO_REF
build_ac      xset              $(fdo_mirror xset)                         xset-1.2.5
