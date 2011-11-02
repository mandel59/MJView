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
    
    cairo_font_face_t* face = ft_init(ipamjm);
    if(face == NULL) {
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
    cairo_set_font_face(cr, face);
    cairo_set_font_size(cr, svg_em);
    cairo_font_extents_t extents;
    cairo_font_extents(cr, &extents);
    cairo_glyph_t glyph;
    glyph.index = ft_get_name_index(glyph_name);
    glyph.x = 0;
    glyph.y = extents.height - extents.descent;
    cairo_show_glyphs(cr, &glyph, 1);
    
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    ft_done();
}

