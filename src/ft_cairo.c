/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ft_cairo.c
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
#include <config.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cairo-ft.h>

FT_Library ft = NULL;
FT_Face ft_face = NULL;
cairo_font_face_t *font_face = NULL;

cairo_font_face_t* ft_init(char *fontpath) {
    if( FT_Init_FreeType(&ft) ) {
        return NULL;
    }
    if( FT_New_Face(ft, fontpath, 0, &ft_face) ) {
        FT_Done_FreeType(ft);
        return NULL;
    }
    font_face = cairo_ft_font_face_create_for_ft_face(ft_face, FT_LOAD_DEFAULT);
    if( font_face == NULL ) {
        cairo_font_face_destroy(font_face);
        FT_Done_Face(ft_face);
        FT_Done_FreeType(ft);
        return NULL;
    }
    return font_face;
}

cairo_font_face_t* ft_get_cairo_font_face(void)
{
    return font_face;
}

void ft_done(void) {
	if (font_face) cairo_font_face_destroy(font_face);
    if (ft_face) FT_Done_Face(ft_face);
    if (ft) FT_Done_FreeType(ft);
}

FT_UInt ft_get_name_index(char *glyph_name)
{
    return FT_Get_Name_Index(ft_face, glyph_name);
}

