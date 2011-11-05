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


G_DEFINE_TYPE (Mjview, mjview, GTK_TYPE_APPLICATION);

/* Create a new window loading a file */
static void
mjview_new_window (GApplication *app,
                           GFile        *file)
{
	GtkWidget *window;

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
        if (!window)
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
  gint i;

  for (i = 0; i < n_files; i++)
    mjview_new_window (application, files[i]);
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

static void
mjview_finalize (GObject *object)
{
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

cairo_surface_t *glyph_surface = NULL;

void
drawglyph (cairo_surface_t *surface, const gchar *glyphname)
{
	int height = cairo_image_surface_get_height (surface);
	cairo_t *cr = cairo_create (surface);
	cairo_font_face_t* face = ft_get_cairo_font_face ();
	if (!face) {
		g_warning ("null font face");
	}
	cairo_clear (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_font_face (cr, face);
	cairo_set_font_size (cr, height);
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
entry1_activate_drawing_area (GtkEntry *entry, gpointer user_data)
{
	GtkDrawingArea *da = GTK_DRAWING_AREA (user_data);
	gchar mjname[9];
	g_strlcpy (mjname, gtk_entry_get_text (entry), 9);
	mjname[0] = 'm'; mjname[1] = 'j';
	drawglyph(glyph_surface, mjname);
	GtkWidget *wda = GTK_WIDGET (da);
	gint width = gtk_widget_get_allocated_width (wda);
	gint height = gtk_widget_get_allocated_height (wda);
	gtk_widget_queue_draw_area (wda, 0, 0, width, height);
}

void
entry1_activate_textview1 (GtkEntry *entry, gpointer user_data)
{
	GtkTextView *tv = GTK_TEXT_VIEW (user_data);
	gchar mjname[9];
	g_strlcpy (mjname, gtk_entry_get_text (entry), 9);
	mjname[0] = 'M'; mjname[1] = 'J';
	sqlite3_stmt *stmt;
	if (sqlite3_prepare(mjdb, "select * from mj_info where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			gchar *a;
			a = g_strdup_printf("%s\nsvg version: %s\n汎用電子: %s\n平成明朝: %s\n%s stroke(s)\nhistory: %s\n",
		                    sqlite3_column_text(stmt, 0),
		                    sqlite3_column_text(stmt, 1),
		                    sqlite3_column_text(stmt, 2),
		                    sqlite3_column_text(stmt, 3),
		                    sqlite3_column_text(stmt, 4),
		                    sqlite3_column_text(stmt, 5));
			gtk_text_buffer_set_text (gtk_text_view_get_buffer (tv), a, -1);
			g_free(a);
		}
		sqlite3_finalize (stmt);
	}
}

void
entry1_activate_textview2 (GtkEntry *entry, gpointer user_data)
{
	GtkTextView *tv = GTK_TEXT_VIEW (user_data);
	gchar mjname[9];
	g_strlcpy (mjname, gtk_entry_get_text (entry), 9);
	mjname[0] = 'M'; mjname[1] = 'J';
	sqlite3_stmt *stmt;
	gchar *a;
	a = g_strdup ("");
	if (sqlite3_prepare(mjdb, "select * from mj_ucs where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			gchar *b = g_strdup_printf("U+%4X (%s)", sqlite3_column_int(stmt, 1), sqlite3_column_text(stmt, 2));
			g_free(a);
			a = b;
		}
		sqlite3_finalize (stmt);
	}
	if (sqlite3_prepare(mjdb, "select reading from mj_reading where mj = ?;",
	                    60, &stmt, NULL) ) {
		g_warning ("can't open stmt");
	} else {
		sqlite3_bind_text(stmt, 1, mjname, -1, SQLITE_TRANSIENT);
		gchar *b = g_strconcat (a, "\nreadings:");
		g_free(a);
		a = b;
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			gchar *b = g_strjoin("\n", a, sqlite3_column_text(stmt, 0));
			g_free(a);
			a = b;
		}
		sqlite3_finalize (stmt);
	}
	gtk_text_buffer_set_text (gtk_text_view_get_buffer (tv), a, -1);
	g_free(a);
}

void
entry1_activate_icon_view (GtkEntry *entry, gpointer user_data)
{
	GtkIconView *iv = GTK_ICON_VIEW (user_data);
	GtkTreeModel *model = gtk_icon_view_get_model (iv);
	
}
