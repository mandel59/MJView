#ifndef FT_CAIRO_H
#define FT_CAIRO_H

cairo_font_face_t* ft_init(char *fontpath);
cairo_font_face_t* ft_get_cairo_font_face(void);
void ft_done(void);
FT_UInt ft_get_name_index(char *glyph_name);

#endif

