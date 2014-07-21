#ifndef LUACXX_egl_INCLUDED
#define LUACXX_egl_INCLUDED

#include "stack.hpp"
#include "algorithm.hpp"
#include "convert/numeric.hpp"
#include "convert/callable.hpp"
#include "convert/string.hpp"

#include <EGL/egl.h>

/*

=head1 NAME

EGL 1.4 - https://www.khronos.org/registry/egl/

=head1 SYNOPSIS

    require "egl";

=head1 DESCRIPTION

This binding allows direct use of EGL as described in:

    http://cgit.freedesktop.org/mesa/mesa/tree/include/EGL/egl.h

=head1 LICENSE

Mesa's EGL/egl.h is licensed under the following terms:

    Copyright (c) 2007-2009 The Khronos Group Inc.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and/or associated documentation files (the
    "Materials"), to deal in the Materials without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Materials, and to
    permit persons to whom the Materials are furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Materials.

    THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

*/

extern "C" int luaopen_egl(lua_State* const);

#endif // LUACXX_egl_INCLUDED
