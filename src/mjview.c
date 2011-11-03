/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * mjview.c
 * Copyright (C) Ryusei Yamaguchi 2011 <mandel59@m59-dynabook-u>
 * 
 */
#include "mjview.h"
#include "ft_cairo.h"

#include <glib/gi18n.h>
#include <sqlite3.h>


/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/mjview/ui/mjview.ui" */
/* #define FONT_FILE PACKAGE_DATA_DIR"/mjview/fonts/ipamjm.ttf" */
/* #define DB_FILE PACKAGE_DATA_DIR"/mjview/db/mj.db" */
#define UI_FILE "src/mjview.ui"
#define FONT_FILE PACKAGE_TOP_BUILDDIR"/data/ipamjm.ttf"
#define DB_FILE PACKAGE_TOP_BUILDDIR"/data/mj.db"

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
drawglyph (cairo_surface_t *surface, char *glyphname)
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
	glyph.index = ft_get_name_index (glyphname);
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
entry1_activate (GtkEntry *entry, gpointer user_data)
{
	GtkDrawingArea *da = GTK_DRAWING_AREA(user_data);
	GtkTreeIter iter;
	/*drawglyph(glyph_surface, gtk_entry_get_text (entry));*/
	gchar num[9];
	g_snprintf (num, 9, "mj%06d", g_random_int_range (1, 60386));
	drawglyph(glyph_surface, num);
	gtk_entry_set_text (entry, num);
	gtk_widget_queue_draw_area (da, 0, 0, gtk_widget_get_allocated_width (da), gtk_widget_get_allocated_height (da));
}
