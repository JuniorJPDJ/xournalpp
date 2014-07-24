/*
 * Xournal++
 *
 * The configuration for a button in the Settings Dialog
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __BUTTONCONFIGGUI__
#define __BUTTONCONFIGGUI__

#include "../../control/Actions.h"
#include <XournalType.h>
#include <gdk/gdk.h>

class SettingsDialog;
class Settings;

class ButtonConfigGui
{
public:
	ButtonConfigGui(SettingsDialog* dlg, GtkWidget* w, Settings* settings,
	                int button, bool withDevice);
	virtual ~ButtonConfigGui();

public:
	void loadSettings();
	void saveSettings();

private:
	static GtkWidget* newLabel(const char* text);
	static void cbSelectCallback(GtkComboBox* widget, ButtonConfigGui* gui);
	void enableDisableTools();

private:
	XOJ_TYPE_ATTRIB;


	Settings* settings;
	int button;
	bool withDevice;

	GtkWidget* cbDevice;
	GtkWidget* cbDisableDrawing;

	GtkWidget* cbTool;
	GtkWidget* cbThickness;
	GtkWidget* colorButton;
	GtkWidget* cbEraserType;
	GtkWidget* cbDrawingType;
	GdkDeviceManager* deviceManager;
};

#endif /* __BUTTONCONFIGGUI__ */
