/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * mjview.c
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
#include "mjview.h"
#include "ft_cairo.h"

#include <glib/gi18n.h>
#include <sqlite3.h>


#if 1
#define UI_FILE PACKAGE_DATA_DIR"/mjview/ui/mjview.ui"
#define FONT_FILE PACKAGE_DATA_DIR"/mjview/fonts/ipamjm.ttf"
#define DB_FILE PACKAGE_DATA_DIR"/mjview/db/mj.db"
#else
/* For testing propose use the local (not installed) files */
#define UI_FILE "src/mjview.ui"
#define FONT_FILE PACKAGE_TOP_BUILDDIR"/data/ipamjm.ttf"
#define DB_FILE PACKAGE_TOP_BUILDDIR"/data/mj.db"
#endif

#define TOP_WINDOW "window"

GtkWidget *window = NULL;
GtkListStore *liststore1 = NULL;
GtkEntry *entry1 = NULL;
GtkIconView *iconview1 = NULL;
GtkToggleButton *togglebutton1 = NULL;
GtkFileFilter *filefilter1 = NULL;

G_DEFINE_TYPE (Mjview, mjview, GTK_TYPE_APPLICATION);

/* Create a new window loading a file */
static void
mjview_new_window (GApplication *app,
                           GFile        *file)
{
	GtkBuilder *builder;
	GError* error = NULL;

	/* Load UI from file */
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_critical ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals (builder, NULL);

	/* Get the window object from the ui file */
	window = GTK_WIDGET (gtk_builder_get_object (builder, TOP_WINDOW));
	liststore1 = GTK_LIST_STORE (gtk_builder_get_object (builder, "liststore1"));
	entry1 = GTK_ENTRY (gtk_builder_get_object (builder, "entry1"));
	iconview1 = GTK_ICON_VIEW (gtk_builder_get_object (builder, "iconview1"));
	togglebutton1 = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "togglebutton1"));
	filefilter1 = GTK_FILE_FILTER (gtk_builder_get_object (builder, "filefilter1"));
	if (!(window && liststore1 && entry1 && iconview1 && togglebutton1))
	{
		g_critical ("Widget \"%s\" is missing in file %s.",
		            TOP_WINDOW,
		            UI_FILE);
	}
	g_object_unref (builder);

	
	gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (app));
	if (file != NULL)
	{
		/* TODO: Add code here to open the file in the new window */
	}
	gtk_widget_show_all (GTK_WIDGET (window));
}


/* GApplication implementation */
static void
mjview_activate (GApplication *application)
{
  mjview_new_window (application, NULL);
}

static void
mjview_open (GApplication  *application,
                     GFile        **files,
                     gint           n_files,
                     const gchar   *hint)
{
#if 0
	gint i;
	for (i = 0; i < n_files; i++)
		mjview_new_window (application, files[i]);
#endif
	mjview_new_window (application, NULL);
}

sqlite3 *mjdb = NULL;

static void
mjview_init (Mjview *object)
{
	if ( !ft_init (FONT_FILE) ) {
		g_critical ("Font \"%s\" is missing in file %s.",
				"ipamj",
				FONT_FILE);
	}
	if ( sqlite3_open(DB_FILE, &mjdb) ) {
		g_critical ("Database \"%s\" is missing in file %s.",
				"mjdb",
				DB_FILE);
	}
}

cairo_surface_t *glyph_surface = NULL;
cairo_surface_t *small_glyph_surface = NULL;

static void
mjview_finalize (GObject *object)
{
	if (glyph_surface) cairo_surface_destroy (glyph_surface);
	if (small_glyph_surface) cairo_surface_destroy (small_glyph_surface);
	sqlite3_close(mjdb);
	ft_done();
	G_OBJECT_CLASS (mjview_parent_class)->finalize (object);
}

static void
mjview_class_init (MjviewClass *klass)
{
	G_APPLICATION_CLASS (klass)->activate = mjview_activate;
	G_APPLICATION_CLASS (klass)->open = mjview_open;

	G_OBJECT_CLASS (klass)->finalize = mjview_finalize;
}

Mjview *
mjview_new (void)
{
	g_type_init ();

	return g_object_new (mjview_get_type (),
	                     "application-id", "org.gnome.mjview",
	                     "flags", G_APPLICATION_HANDLES_OPEN,
	                     NULL);
}

static void
cairo_clear (cairo_t *cr)
{
	cairo_save (cr);
	cairo_set_source_rgba (cr, 0, 0, 0, 0);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);
	cairo_restore (cr);
}

void
drawglyph (cairo_surface_t *surface, const gchar *glyphname, double em)
{
	cairo_t *cr = cairo_create (surface);
	cairo_font_face_t* face = ft_get_cairo_font_face ();
	if (!face) {
		g_warning ("null font face");
	}
	cairo_clear (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_font_face (cr, face);
	cairo_set_font_size (cr, em);
	cairo_font_extents_t extents;
	cairo_font_extents (cr, &extents);
	cairo_glyph_t glyph;
	glyph.index = ft_get_name_index ((char*)glyphname);
	glyph.x = 0;
	glyph.y = extents.height - extents.descent;
	cairo_show_glyphs (cr, &glyph, 1);
	cairo_destroy (cr);
}

gboolean
drawingarea1_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	cairo_set_source_surface (cr, glyph_surface, 0, 0);
	cairo_paint (cr);
	return FALSE;
}

gboolean
drawingarea1_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	int width = gtk_widget_get_allocated_width (widget);
	int height = gtk_widget_get_allocated_height (widget);
	if (glyph_surface == NULL) {
		glyph_surface
			= cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	}
	return TRUE;
}

void
iconview1_select_drawing_area (GtkIconView *iconview, gpointer user_data)
{
	GList *list = gtk_icon_view_get_selected_items (iconview);
	if(g_list_length(list) < 1) {
		g_list_free (list);
		return;
	}
	GtkTreeModel *model = GTK_TREE_MODEL ( gtk_icon_view_get_model (iconview) );
	GtkTreePath *path = g_list_first(list)->data;
	GtkTreeIter iter;
	gtk_tree_model_get_iter (model, &iter, path);
	gchar *mjname;
	gtk_tree_model_get (model, &iter, 1, &mjname, -1);
	g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (list);
	drawglyph(glyph_surface, mjname, cairo_image_surface_get_height (glyph_surface));
	GtkWidget *da = GTK_WIDGET (user_data);
	gint width = gtk_widget_get_allocated_width (da);
	gint height = gtk_widget_get_allocated_height (da);
	gtk_widget_queue_draw_area (da, 0, 0, width, height);

	g_free(mjname);
}

void
iconview1_select_textview1 (GtkIconView *iconview, gpointer user_data)
{
	GList *list = gtk_icon_view_get_selected_items (iconview);
	if(g_list_length(list) < 1) {
		g_list_free (list);
		return;
	}
	GtkTreeModel *model = GTK_TREE_MODEL ( gtk_icon_view_get_model (iconview) );
	GtkTreePath *path = g_list_first(list)->data;
	GtkTreeIter iter;
	gtk_tree_model_get_iter (model, &iter, path);
	gchar *mjname;
	gtk_tree_model_get (model, &iter, 1, &mjname, -1);
	g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (list);
	
	mjname[0] = 'M'; mjname[1] = 'J'; mjname[8] = '\0';
	GtkTextView *tv = GTK_TEXT_VIEW (user_data);
	sqlite3_stmt *stmt;
	if (sqlite3_prepare(mjdb, "select * from mj_info where mj = ?;", 60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			gchar *a;
			a = g_strdup_printf("%s\nSVG Version: %s\nHanyo Denshi: %05d\nHeisei Mincho: %s\n%s stroke(s)\nHistory:\n%s",
			                    sqlite3_column_text(stmt, 0),
			                    sqlite3_column_text(stmt, 1),
			                    sqlite3_column_int (stmt, 2),
			                    sqlite3_column_text(stmt, 3),
			                    sqlite3_column_text(stmt, 4),
			                    sqlite3_column_text(stmt, 5));
			gtk_text_buffer_set_text (gtk_text_view_get_buffer (tv), a, -1);
			g_free(a);
		}
		sqlite3_finalize (stmt);
	}
	g_free(mjname);
}

void
iconview1_select_textview2 (GtkIconView *iconview, gpointer user_data)
{
	GList *list = gtk_icon_view_get_selected_items (iconview);
	if(g_list_length(list) < 1) {
		g_list_free (list);
		return;
	}
	GtkTreeModel *model = GTK_TREE_MODEL ( gtk_icon_view_get_model (iconview) );
	GtkTreePath *path = g_list_first(list)->data;
	GtkTreeIter iter;
	gtk_tree_model_get_iter (model, &iter, path);
	gchar *mjname;
	gtk_tree_model_get (model, &iter, 1, &mjname, -1);
	g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (list);
	
	mjname[0] = 'M'; mjname[1] = 'J'; mjname[8] = '\0';
	GtkTextView *tv = GTK_TEXT_VIEW (user_data);
	sqlite3_stmt *stmt;
	GString *str;
	str = g_string_new("");
	if (sqlite3_prepare(mjdb, "select radical, strokes from mj_index where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			uint rad = sqlite3_column_int(stmt, 0);
			gunichar urad = 0x2f00 + rad - 1;
			gchar outbuf[7];
			outbuf[g_unichar_to_utf8(urad, outbuf)] = '\0';
			g_string_append_printf (str, "Radical-stroke Index: %s[%d]-%d\n", outbuf, rad, sqlite3_column_int(stmt, 1));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select reading from mj_reading where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		GPtrArray *readings = g_ptr_array_new();
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			g_ptr_array_add (readings, g_strdup ((gchar*) sqlite3_column_text(stmt, 0)));
		}
		if (readings->len > 0) {
			g_string_append (str, "Readings:\n");
			g_ptr_array_add (readings, NULL);
			gchar *readings_joined = g_strjoinv (", ", (gchar**) readings->pdata);
			g_string_append_printf (str, "%s\n", readings_joined);
			g_free (readings_joined);
		}
		g_ptr_array_free (readings, TRUE);
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select measure from mj_measure where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf (str,
			                        " * %s\n",
			                        sqlite3_column_text(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select * from mj_ucs where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			gunichar wc = sqlite3_column_int(stmt, 1);
			gchar outbuf[7];
			outbuf[g_unichar_to_utf8(wc, outbuf)] = '\0';
			gchar *implemented = sqlite3_column_int(stmt, 3) ? "yes" : "no";
			g_string_append_printf (str,
			                        "UCS: U+%4X (%s)\nUCS Reliability Class: %s\n"
			                        "UCS Implemented: %s\n",
			                        wc, outbuf, sqlite3_column_text(stmt, 2),
			                        implemented);
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select * from mj_ivs where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			gunichar ucs = sqlite3_column_int(stmt, 1);
			gunichar ivs = sqlite3_column_int(stmt, 2);
			gchar outbuf[13];
			gint ucs_len = g_unichar_to_utf8(ucs, outbuf);
			gint ivs_len = g_unichar_to_utf8(ivs, outbuf + ucs_len);
			outbuf[ucs_len + ivs_len] = '\0';
			gchar *implemented = sqlite3_column_int(stmt, 3) ? "yes" : "no";
			g_string_append_printf(str, "IVS: %4X %4X (%s)\nIVS Implemented: %s\n",
			                       ucs, ivs, outbuf, implemented);
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select code, subsumption, class from mj_x0213 where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "JIS X 0213: %s",
			                       sqlite3_column_text(stmt, 0));
			if(sqlite3_column_int(stmt, 2) == 2) {
				g_string_append_printf(str, " (subsumed: %s)", sqlite3_column_text(stmt, 1));
			}
			g_string_append (str, "\n");
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select code from mj_x0212 where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "JIS X 0212: %s\n",
			                       sqlite3_column_text(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select code from mj_koseki where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "Koseki Toitsu Moji Bango: %06d\n",
			                       sqlite3_column_int(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select code from mj_juki where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "Juki Net Toitsu Moji Code: J+%04X\n",
			                       sqlite3_column_int(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select code from mj_toki where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "Toki Toitsu Moji Bango: %08d\n",
			                       sqlite3_column_int(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select daikanwa from mj_daikanwa where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			g_string_append_printf(str, "Dai Kanwa Jiten: %s\n",
			                       sqlite3_column_text(stmt, 0));
		}
		sqlite3_finalize (stmt);
	}
	gtk_text_buffer_set_text (gtk_text_view_get_buffer (tv), str->str, -1);
	g_string_free(str, TRUE);
	
	g_free(mjname);
}

sqlite3_stmt*
build_query (const gchar* str)
{
	GPtrArray *queries = g_ptr_array_new();
	GPtrArray *binds = g_ptr_array_new();
	gchar **str_and = g_strsplit (str, " ", -1);
	gchar **itr;
	for (itr = str_and; *itr != NULL; itr++) {
		gchar *s = *itr;
		if ( *s == '\0' ) {
			continue;
		}
		if ( g_str_has_prefix(s, "MJ") || g_str_has_prefix(s, "mj") ) {
			char *c;
			unsigned long l = strtoul (s+2, &c, 10);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 10);
				if (l2 < 10) {
					l2 = (l / 10) * 10 + l2;
				} else if (l2 < 100) {
					l2 = (l / 100) * 100 + l2;
				} else if (l2 < 1000) {
					l2 = (l / 1000) * 1000 + l2;
				} else if (l2 < 10000) {
					l2 = (l / 10000) * 10000 + l2;
				}
				g_ptr_array_add(queries, " select mj from mj_info where hanyodenshi between ? and ? ");
				g_ptr_array_add(binds, g_strdup_printf("%ld", l));
				g_ptr_array_add(binds, g_strdup_printf("%ld", l2));
				continue;
			}
			g_ptr_array_add(queries, " select mj from mj_info where hanyodenshi = ? ");
			g_ptr_array_add(binds, g_strdup_printf("%ld", l));
			continue;
		}
		if ( g_str_has_prefix(s, "koseki-") || g_str_has_prefix(s, "toki-") ) {
			char *c;
			unsigned long l;
			l = strtoul (s+(*s=='k'?7:5), &c, 10);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 10);
				if (l2 < 10) {
					l2 = (l / 10) * 10 + l2;
				} else if (l2 < 100) {
					l2 = (l / 100) * 100 + l2;
				} else if (l2 < 1000) {
					l2 = (l / 1000) * 1000 + l2;
				} else if (l2 < 10000) {
					l2 = (l / 10000) * 10000 + l2;
				} else if (l2 < 100000) {
					l2 = (l / 100000) * 100000 + l2;
				}
				if (*s=='k') {
					g_ptr_array_add(queries, " select mj from mj_koseki where code between ? and ? ");
				} else {
					g_ptr_array_add(queries, " select mj from mj_toki where code between ? and ? ");
				}
				g_ptr_array_add(binds, g_strdup_printf("%ld", l));
				g_ptr_array_add(binds, g_strdup_printf("%ld", l2));
				continue;
			}
			if (*s=='k') {
				g_ptr_array_add(queries, " select mj from mj_koseki where code = ? ");
			} else {
				g_ptr_array_add(queries, " select mj from mj_toki where code = ? ");
			}
			g_ptr_array_add(binds, g_strdup_printf("%ld", l));
			continue;
		}
		if ( g_str_has_prefix(s, "MJ") || g_str_has_prefix(s, "mj") ) {
			char *c;
			unsigned long l = strtoul (s+2, &c, 10);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 10);
				if (l2 < 10) {
					l2 = (l / 10) * 10 + l2;
				} else if (l2 < 100) {
					l2 = (l / 100) * 100 + l2;
				} else if (l2 < 1000) {
					l2 = (l / 1000) * 1000 + l2;
				} else if (l2 < 10000) {
					l2 = (l / 10000) * 10000 + l2;
				}
				g_ptr_array_add(queries, " select mj from mj_info where hanyodenshi between ? and ? ");
				g_ptr_array_add(binds, g_strdup_printf("%ld", l));
				g_ptr_array_add(binds, g_strdup_printf("%ld", l2));
				continue;
			}
			g_ptr_array_add(queries, " select mj from mj_info where hanyodenshi = ? ");
			g_ptr_array_add(binds, g_strdup_printf("%ld", l));
			continue;
		}
		if ( g_str_has_prefix(s, "U+") ) {
			char *c;
			unsigned long l = strtoul (s+2, &c, 16);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 16);
				if (l2 < 0x10) {
					l2 = (l & 0xFFFFFFF0) + l2;
				} else if (l2 < 0x100) {
					l2 = (l & 0xFFFFFF00) + l2;
				} else if (l2 < 0x1000) {
					l2 = (l & 0xFFFFF000) + l2;
				}
				g_ptr_array_add(queries, " select mj from mj_ucs where ucs between ? and ? ");
				g_ptr_array_add(binds, g_strdup_printf("%ld", l));
				g_ptr_array_add(binds, g_strdup_printf("%ld", l2));
				continue;
			}
			g_ptr_array_add(queries, " select mj from mj_ucs where ucs = ? ");
			g_ptr_array_add(binds, g_strdup_printf("%ld", l));
			continue;
		}
		if ( g_str_has_prefix(s, "J+") ) {
			char *c;
			unsigned long l = strtoul (s+2, &c, 16);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 16);
				if (l2 < 0x10) {
					l2 = (l & 0xFFFFFFF0) + l2;
				} else if (l2 < 0x100) {
					l2 = (l & 0xFFFFFF00) + l2;
				} else if (l2 < 0x1000) {
					l2 = (l & 0xFFFFF000) + l2;
				}
				g_ptr_array_add(queries, " select mj from mj_juki where code between ? and ? ");
				g_ptr_array_add(binds, g_strdup_printf("%ld", l));
				g_ptr_array_add(binds, g_strdup_printf("%ld", l2));
				continue;
			}
			g_ptr_array_add(queries, " select mj from mj_juki where code = ? ");
			g_ptr_array_add(binds, g_strdup_printf("%ld", l));
			continue;
		}
		if ( *s == '<' ) {
			g_ptr_array_add(queries, " select mj from mj_info where strokes < ? ");
			g_ptr_array_add(binds, g_strdup_printf("%lu", strtoul (s+1, NULL, 10)));
			continue;
		}
		if ( *s == '=' ) {
			char *c;
			unsigned long l = strtoul (s+1, &c, 10);
			if (*c == '-' && *(c+1) != '\0') {
				unsigned long l2 = strtoul (c+1, NULL, 10);
				if (l2 < l) {
					unsigned long t = l2;
					l2 = l;
					l = t;
				}
				g_ptr_array_add(queries, " select mj from mj_info where strokes between ? and ? ");
				g_ptr_array_add(binds, g_strdup_printf("%lu", l));
				g_ptr_array_add(binds, g_strdup_printf("%lu", l2));
				continue;
			}
			g_ptr_array_add(queries, " select mj from mj_info where strokes = ? ");
			g_ptr_array_add(binds, g_strdup_printf("%lu", l));
			continue;
		}
		if ( *s == '>' ) {
			g_ptr_array_add(queries, " select mj from mj_info where strokes > ? ");
			g_ptr_array_add(binds, g_strdup_printf("%lu", strtoul (s+1, NULL, 10)));
			continue;
		}
		gunichar us;
		if (*s == '[') {
			long a = strtoul (s+1, &s, 10);
			if(a < 1 || 214 < a) continue;
			us = a + 0x2F00 - 1;
		} else {
			us = g_utf8_get_char (s);
		}
		GUnicodeScript sc = g_unichar_get_script(us);
		if (sc == G_UNICODE_SCRIPT_HAN) {
			gunichar us_c, us_c2;
			g_unichar_decompose (us, &us_c, &us_c2);
			if ( 0x2F00 <= us && us <= 0x2FD5) {
				gchar *c = g_utf8_next_char(s);
				if ( *c == '[' ) {
					if (us != strtoul (c+1, &c, 10) + 0x2F00 - 1) {
						continue;
					}
					c++;
				}
				if ( *c == '-' && *(c+1) != '\0' ) {
					unsigned long strokes = strtoul (c+1, &c, 10);
					if ( *c == '-' && *(c+1) != '\0' ) {
						unsigned long strokes2 = strtoul (c+1, &c, 10);
						g_ptr_array_add(queries, " select mj from mj_index where strokes between ? and ? and radical = ?");
						g_ptr_array_add(binds, g_strdup_printf("%ld", strokes));
						g_ptr_array_add(binds, g_strdup_printf("%ld", strokes2));
					} else {
						g_ptr_array_add(queries, " select mj from mj_index where strokes = ? and radical = ?");
						g_ptr_array_add(binds, g_strdup_printf("%ld", strokes));
					}
				} else {
					g_ptr_array_add(queries, " select mj from mj_index where radical = ? ");
				}
				g_ptr_array_add(binds, g_strdup_printf("%d", us - 0x2F00 + 1));
			} else if(us == us_c) {
				g_ptr_array_add(queries, " select mj from mj_ucs where ucs = ? ");
				g_ptr_array_add(binds, g_strdup_printf("%d", us));
			} else {
				g_ptr_array_add(queries, " select mj from mj_ucs where ucs = ? or ucs = ? ");
				g_ptr_array_add(binds, g_strdup_printf("%d", us));
				g_ptr_array_add(binds, g_strdup_printf("%d", us_c));
			}
		} else if (sc == G_UNICODE_SCRIPT_KATAKANA || sc == G_UNICODE_SCRIPT_HIRAGANA) {
			g_ptr_array_add(queries, " select mj from mj_reading where reading = ? ");
			g_ptr_array_add(binds, g_strdup(s));
		}
	}
	g_strfreev(str_and);
	if (queries->len == 0) {
		g_ptr_array_free (queries, FALSE);
		g_ptr_array_free (binds, TRUE);
		return NULL;
	}
	g_ptr_array_add(queries, NULL);
	gchar *query = g_strjoinv (" intersect ", (gchar**) queries->pdata);
	
	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare(mjdb, query, -1, &stmt, NULL)) {
		g_ptr_array_free (queries, FALSE);
		g_ptr_array_free (binds, TRUE);
		g_warning ("can't open stmt");
		return NULL;
	}
	guint i;
	for (i = 0; i < binds->len; i++) {
		sqlite3_bind_text (stmt, i+1, g_ptr_array_index(binds, i), -1, NULL);
	}
	g_ptr_array_free (queries, FALSE);
	g_ptr_array_free (binds, TRUE);
	return stmt;
}

sqlite3_stmt *step_add_stmt = NULL;

gboolean
step_add (gpointer user_data)
{
	GtkListStore *ls = liststore1;
	sqlite3_stmt *stmt = step_add_stmt;
	int em = 32;
	if (small_glyph_surface == NULL) {
		small_glyph_surface
			= cairo_image_surface_create (CAIRO_FORMAT_ARGB32, em, em);
	}
	cairo_surface_t *glyph = small_glyph_surface;
	int i = 0, max = 100;
	while (i < max && sqlite3_step(stmt) == SQLITE_ROW) {
		gchar *mjname = g_strdup((gchar*) sqlite3_column_text(stmt, 0));
		mjname[0] = 'm'; mjname[1] = 'j';
		drawglyph(glyph, mjname, cairo_image_surface_get_height (glyph));
		GdkPixbuf *buf = gdk_pixbuf_get_from_surface(glyph, 0, 0, em, em);
		GtkTreeIter iter;
		gtk_list_store_append (ls, &iter);
		gtk_list_store_set (ls, &iter,
		                    0, buf,
		                    1, mjname,
		                    -1);
		g_object_unref (buf);
		g_free (mjname);
		i++;
	}
	if (i == max) {
		return TRUE;
	}
	sqlite3_finalize (stmt);
	step_add_stmt = NULL;
	gtk_toggle_button_set_active (togglebutton1, FALSE);
	return FALSE;
}

void
entry1_activate_togglebutton1 (GtkEntry *entry, gpointer user_data)
{
	GtkToggleButton *button = GTK_TOGGLE_BUTTON(user_data);
	entry1 = entry;
	gtk_toggle_button_set_active (button, TRUE);
}

void
togglebutton1_clicked (GtkToggleButton *togglebutton, gpointer user_data)
{
	static guint sid = 0;
	if (sid != 0) {
		GSource *source = g_main_context_find_source_by_id (NULL, sid);
		if (source != NULL && !g_source_is_destroyed (source)) {
			g_source_destroy (source);
			if (step_add_stmt) sqlite3_finalize (step_add_stmt);
		}
		sid = 0;
	}
	if ( gtk_toggle_button_get_active(togglebutton) ) {
		sqlite3_stmt *stmt = build_query (gtk_entry_get_text (entry1));
		if(stmt == NULL) {
			gtk_toggle_button_set_active (togglebutton, FALSE);
			return;
		}
		GtkIconView *iv = GTK_ICON_VIEW (user_data);
		GtkListStore *ls = GTK_LIST_STORE( gtk_icon_view_get_model (iv) );
		togglebutton1 = togglebutton;
		liststore1 = ls;
		gtk_list_store_clear (ls);
		gtk_icon_view_set_pixbuf_column (iv, 0);
		step_add_stmt = stmt;
		sid = g_idle_add((GSourceFunc) step_add, NULL);
	}
}
