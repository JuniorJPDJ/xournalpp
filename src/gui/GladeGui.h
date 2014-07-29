/*
 * Xournal++
 *
 * Abstract GUI class, which loads the glade objects
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __GLADEGUI_H__
#define __GLADEGUI_H__

#include <gtk/gtk.h>

#include <XournalType.h>

class GladeSearchpath;

class GladeGui
{
public:
	GladeGui(GladeSearchpath* gladeSearchPath, const char* glade,
	         const char* mainWnd);
	virtual ~GladeGui();

	virtual void show(GtkWindow* parent) = 0;

	operator GtkWindow* ();
	operator GdkWindow* ();

	GtkWidget* get(const char* name);
	GtkWidget* loadIcon(const char* name);
	GdkPixbuf* loadIconPixbuf(const char* filename);

	GtkWidget* getWindow();
	GladeSearchpath* getGladeSearchPath();

private:
	XOJ_TYPE_ATTRIB;

	/**
	 * The Glade resources
	 */
	GtkBuilder* builder;

	/**
	 * Our search paths
	 */
	GladeSearchpath* gladeSearchPath;

protected:
	/**
	 * This window
	 */
	GtkWidget* window;
};

#endif /* __GLADEGUI_H__ */
