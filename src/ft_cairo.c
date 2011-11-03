#include <config.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cairo-ft.h>

FT_Library ft;
FT_Face ft_face;
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
    cairo_font_face_destroy(font_face);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft);
}

FT_UInt ft_get_name_index(char *glyph_name)
{
    return FT_Get_Name_Index(ft_face, glyph_name);
}

