/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * mjview.h
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

#ifndef _MJVIEW_
#define _MJVIEW_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MJVIEW_TYPE_APPLICATION             (mjview_get_type ())
#define MJVIEW_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MJVIEW_TYPE_APPLICATION, Mjview))
#define MJVIEW_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MJVIEW_TYPE_APPLICATION, MjviewClass))
#define MJVIEW_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MJVIEW_TYPE_APPLICATION))
#define MJVIEW_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MJVIEW_TYPE_APPLICATION))
#define MJVIEW_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MJVIEW_TYPE_APPLICATION, MjviewClass))

typedef struct _MjviewClass MjviewClass;
typedef struct _Mjview Mjview;

struct _MjviewClass
{
	GtkApplicationClass parent_class;
};

struct _Mjview
{
	GtkApplication parent_instance;
};

GType mjview_get_type (void) G_GNUC_CONST;
Mjview *mjview_new (void);

/* Callbacks */

G_END_DECLS

#endif /* _APPLICATION_H_ */
