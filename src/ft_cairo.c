#include <stdlib.h>
#include <stdio.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ft;
FT_Face face;

int ft_init(char *fontpath) {
    if( FT_Init_FreeType(&ft) || FT_New_Face(ft, fontpath, 0, &face) ) {
        return -1;
    }
    return 0;
}

void ft_done(void) {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

int ft_load_glyph_name(char *glyph_name)
{
    FT_UInt index = FT_Get_Name_Index(face, glyph_name);
    if( index == 0 ) return -1;
    return FT_Load_Glyph(face, index, FT_LOAD_NO_SCALE) ? -1 : 0 ;
}

void ft_translate(cairo_t *cr, double em)
{
    FT_Short upm = face->units_per_EM;
    cairo_scale(cr, em / upm, - em / upm);
    cairo_translate(cr, 0, -face->ascender);
    cairo_set_source_rgb(cr, 0, 0, 0);
}

void ft_draw_outline(cairo_t *cr)
{
    FT_Outline outline = face->glyph->outline;
    short c, p, start, end;
    start = 0;
    for(c = 0; c < outline.n_contours; c++) {
        end = outline.contours[c];
        FT_Vector* points = outline.points + start;
        char*      tags   = outline.tags   + start;
        cairo_move_to(cr, points[0].x, points[0].y);
        p = 1;
        while(p <= end - start) {
            if( tags[p] & 0x1 ) {
                cairo_line_to(cr, points[p].x, points[p].y);
                p++;
            } else if( tags[p] & 0x2 ) {
                FT_Vector point_end;
                if( p + 2 > end - start) {
                    point_end = points[0];
                } else {
                    point_end = points[p+2];
                }
                cairo_curve_to(cr,
                    points[p  ].x, points[p  ].y,
                    points[p+1].x, points[p+1].y,
                    point_end  .x, point_end  .y);
                p += 3;
            } else {
                FT_Vector point_end;
                if( p + 1 > end - start) {
                    point_end = points[0];
                } else {
                    point_end = points[p+1];
                }
                cairo_curve_to(cr,
                    (points[p-1].x + points[p].x * 2) / 3.0, (points[p-1].y + points[p].y * 2) / 3.0,
                    (point_end  .x + points[p].x * 2) / 3.0, (point_end  .y + points[p].y * 2) / 3.0,
                    point_end.x, point_end.y);
                p += 2;
            }
        }
        cairo_close_path(cr);
        start = end + 1;
    }
}

