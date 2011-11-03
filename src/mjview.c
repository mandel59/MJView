/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * mjview.c
 * Copyright (C) Ryusei Yamaguchi 2011 <mandel59@m59-dynabook-u>
 * 
 */
#include "mjview.h"

#include <glib/gi18n.h>



/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/mjview/ui/mjview.ui" */
#define UI_FILE "src/mjview.ui"
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

static void
mjview_init (Mjview *object)
{

}

static void
mjview_finalize (GObject *object)
{

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
