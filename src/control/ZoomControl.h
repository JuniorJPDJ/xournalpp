/*
 * Xournal++
 *
 * Controls the zoom level
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __ZOOMCONTROL_H__
#define __ZOOMCONTROL_H__

//Hardcode max and min zoom
//this should probably be user-adjustable in future
#define MAX_ZOOM 5
#define MIN_ZOOM .3

#include <gtk/gtk.h>

#include <XournalType.h>

class ZoomListener
{
public:
	virtual void zoomChanged(double lastZoom) = 0;
	virtual void zoomRangeValuesChanged();
};

class ZoomControl
{
public:
	ZoomControl();
	virtual ~ZoomControl();

	void zoomIn();
	void zoomOut();

	void zoomFit();
	void zoom100();

	double getZoom();
	void setZoom(double zoom);

	void setZoom100(double zoom);
	void setZoomFit(double zoom);

	double getZoomFit();
	double getZoom100();

	void addZoomListener(ZoomListener* listener);

	void initZoomHandler(GtkWidget* widget);

	// Current zoom center
	gdouble zoom_center_x;
	gdouble zoom_center_y;

protected:
	void fireZoomChanged(double lastZoom);
	void fireZoomRangeValueChanged();

	static bool onScrolledwindowMainScrollEvent(GtkWidget* widget,
	                                            GdkEventScroll* event, ZoomControl* zoom);

private:
	XOJ_TYPE_ATTRIB;

	GList* listener;

	double zoom;

	double lastZoomValue;

	bool zoomFitMode;

	double zoom100Value;
	double zoomFitValue;
};

#endif /* __ZOOMCONTROL_H__ */
