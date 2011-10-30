#ifndef FT_CAIRO_H
#define FT_CAIRO_H

int ft_init(char *fontpath);
void ft_done(void);
int ft_load_glyph_name(char *glyph_name);
void ft_translate(cairo_t *cr, double em);
void ft_draw_outline(cairo_t *cr);

#endif

