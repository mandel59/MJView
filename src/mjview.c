#include <stdlib.h>
#include <stdio.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <ft_cairo.h>

cairo_status_t write_to_stdout(void *closure, unsigned char *data, unsigned int length)
{
    if( fwrite(data, sizeof(unsigned char), length, stdout) < length ) {
        return CAIRO_STATUS_WRITE_ERROR;
    }
    return CAIRO_STATUS_SUCCESS;
}

int main(void)
{
    char *ipamjm, *glyph_name;
    ipamjm = getenv("IPAMJM");
    if( ipamjm == NULL ) {
        fputs("IPAMJM is NULL\n", stderr);
        exit(1);
    }
    glyph_name = getenv("GLYPH_NAME");
    if( glyph_name == NULL ) {
        fputs("GLYPH_NAME is NULL\n", stderr);
        exit(1);
    }
    if( ft_init(ipamjm) ) {
        fputs("ft_init error\n", stderr);
        exit(1);
    }
    
    double svg_em = 128.0;
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_svg_surface_create_for_stream((cairo_write_func_t)&write_to_stdout, NULL, svg_em, svg_em);
    if( surface == NULL ) {
        fputs("surface is NULL\n", stderr);
        exit(1);
    }
    cr = cairo_create(surface);
    
    if( ft_load_glyph_name(glyph_name) ) {
        fputs("ft_load_glyph_name error\n", stderr);
        exit(1);
    }
    
    ft_translate(cr, svg_em);
    ft_draw_outline(cr);
    cairo_fill(cr);
    
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    
    ft_done();
}

