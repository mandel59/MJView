/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ft_cairo.h
 * Copyright (C) Ryusei Yamaguchi 2011 <mandel59@gmail.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#ifndef FT_CAIRO_H
#define FT_CAIRO_H

#include <ft2build.h>
#include FT_FREETYPE_H

cairo_font_face_t* ft_init(char *fontpath);
cairo_font_face_t* ft_get_cairo_font_face(void);
void ft_done(void);
FT_UInt ft_get_name_index(char *glyph_name);

#endif

