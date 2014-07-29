/*
 * Xournal++
 *
 * Control to display a single color button
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __SELECTCOLOR_H__
#define __SELECTCOLOR_H__

#include <gtk/gtk.h>
#include <cairo.h>

G_BEGIN_DECLS

#define SELECT_COLOR(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, selectcolor_get_type(), SelectColor)
#define SELECT_COLOR_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, selectcolor_get_type(), SelectColorClass)
#define IS_SELECT_COLOR(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, selectcolor_get_type())

typedef struct _SelectColor SelectColor;
typedef struct _SelectColorClass SelectColorClass;

struct _SelectColor
{
	//	GtkWidget widget;
	GtkMisc widget;

	gint color;
	gboolean circle;

	gint size;
};

struct _SelectColorClass
{
	GtkMiscClass parent_class;
};

GType selectcolor_get_type(void);
GtkWidget* selectcolor_new(gint color);
void selectcolor_set_color(GtkWidget* sc, gint color);
void selectcolor_set_circle(GtkWidget* sc, gboolean circle);
void selectcolor_set_size(GtkWidget* sc, gint size);

G_END_DECLS

#endif /* __SELECTCOLOR_H__ */
