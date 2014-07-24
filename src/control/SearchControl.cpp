#include "SearchControl.h"
#include "../model/Text.h"
#include "../model/Layer.h"
#include "../view/TextView.h"

SearchControl::SearchControl(PageRef page, XojPopplerPage* pdf)
{
	XOJ_INIT_TYPE(SearchControl);

	this->page = page;
	this->pdf = pdf;
	this->results = NULL;
}

SearchControl::~SearchControl()
{
	XOJ_CHECK_TYPE(SearchControl);

	freeSearchResults();

	XOJ_RELEASE_TYPE(SearchControl);
}

void SearchControl::freeSearchResults()
{
	XOJ_CHECK_TYPE(SearchControl);

	if (this->results)
	{
		for (GList* l = this->results; l != NULL; l = l->next)
		{
			delete (XojPopplerRectangle*) l->data;
		}
		g_list_free(this->results);
		this->results = NULL;
	}
}

void SearchControl::paint(cairo_t* cr, GdkRectangle* rect, double zoom,
                          GdkRGBA color)
{
	XOJ_CHECK_TYPE(SearchControl);

	// set the line always the same size on display
	cairo_set_line_width(cr, 1 / zoom);

	for (GList* l = this->results; l != NULL; l = l->next)
	{
		XojPopplerRectangle* rect = (XojPopplerRectangle*) l->data;
		cairo_rectangle(cr, rect->x1, rect->y1, rect->x2 - rect->x1,
		                rect->y2 - rect->y1);
		gdk_cairo_set_source_rgba(cr, &color);
		cairo_stroke_preserve(cr);
		cairo_set_source_rgba(cr, color.red, color.green,
		                      color.blue, 0.3);
		cairo_fill(cr);
	}
}

bool SearchControl::search(const char* text, int* occures, double* top)
{
	XOJ_CHECK_TYPE(SearchControl);

	freeSearchResults();

	if (text == NULL)
	{
		return true;
	}

	if (this->pdf)
	{
		this->results = this->pdf->findText(text);
	}

	int selected = this->page->getSelectedLayerId();
	ListIterator<Layer*> it = this->page->layerIterator();

	while (it.hasNext() && selected)
	{
		Layer* l = it.next();

		ListIterator<Element*> eit = l->elementIterator();
		while (eit.hasNext())
		{
			Element* e = eit.next();

			if (e->getType() == ELEMENT_TEXT)
			{
				Text* t = (Text*) e;

				GList* textResult = TextView::findText(t, text);
				this->results = g_list_concat(this->results, textResult);
			}
		}

		selected--;
	}

	if (occures)
	{
		*occures = g_list_length(this->results);
	}
	if (top)
	{
		if (this->results == NULL)
		{
			*top = 0;
		}
		else
		{

			double min = ((XojPopplerRectangle*) this->results->data)->y1;
			for (GList* l = this->results->next; l != NULL; l = l->next)
			{
				XojPopplerRectangle* rect = (XojPopplerRectangle*) l->data;
				min = MIN(min, rect->y1);
			}

			*top = min;
		}
	}

	return this->results != NULL;
}
