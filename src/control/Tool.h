/*
 * Xournal++
 *
 * A tool
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __TOOL_H__
#define __TOOL_H__

#include <String.h>
#include <XournalType.h>

// Has to be in the same order as in Action.h: ActionType!
enum ToolType
{
    TOOL_NONE           =  0,

    // First valid tool, often used starting Index 0
    TOOL_PEN            =  1,
    TOOL_ERASER         =  2,
    TOOL_HILIGHTER      =  3,
    TOOL_TEXT           =  4,
    TOOL_IMAGE          =  5,
    TOOL_SELECT_RECT    =  6,
    TOOL_SELECT_REGION  =  7,
    TOOL_SELECT_OBJECT  =  8,
    TOOL_VERTICAL_SPACE =  9,
    TOOL_HAND           = 10,
    /*
    TOOL_DRAW_RECT    	= 11,
    TOOL_DRAW_CIRCLE	= 12,
    TOOL_DRAW_ARROW 	= 13
    */
};

// The count of tools
#define TOOL_COUNT 10
//#define TOOL_COUNT 13


String toolTypeToString(ToolType type);
ToolType toolTypeFromString(String type);

enum ToolSize
{
    TOOL_SIZE_VERY_FINE = 0,
    TOOL_SIZE_FINE,
    TOOL_SIZE_MEDIUM,
    TOOL_SIZE_THICK,
    TOOL_SIZE_VERY_THICK,
    // None has to be at the end, because this enum is used as memory offset
    TOOL_SIZE_NONE
};

String toolSizeToString(ToolSize size);
ToolSize toolSizeFromString(String size);

enum EraserType
{
    ERASER_TYPE_NONE = 0,
    ERASER_TYPE_DEFAULT,
    ERASER_TYPE_WHITEOUT,
    ERASER_TYPE_DELETE_STROKE
};

String eraserTypeToString(EraserType type);
EraserType eraserTypeFromString(String type);

enum PageInsertType
{
    PAGE_INSERT_TYPE_PLAIN = 1,
    PAGE_INSERT_TYPE_LINED,
    PAGE_INSERT_TYPE_RULED,
    PAGE_INSERT_TYPE_GRAPH,
    PAGE_INSERT_TYPE_COPY,
    PAGE_INSERT_TYPE_PDF_BACKGROUND
};

String pageInsertTypeToString(PageInsertType type);
PageInsertType pageInsertTypeFromString(String type);

class Tool
{
public:
	Tool(String name, ToolType tool, int color, bool enableColor, bool enableSize,
	     bool enableRuler, bool enableRectangle, bool enableCircle, bool enableArrow,
	     bool enableShapreRecognizer, double* thickness);
	virtual ~Tool();

	String getName();

	int getColor();
	void setColor(int color);

	ToolSize getSize();
	void setSize(ToolSize size);

	bool isShapeRecognizer();
	void setShapeRecognizer(bool enabled);
	bool isRuler();
	bool isRectangle();
	bool isCircle();
	bool isArrow();
	void setRuler(bool enabled);
	void setRectangle(bool enabled);
	void setCircle(bool enabled);
	void setArrow(bool enabled);

	bool isEnableColor();
	bool isEnableSize();
	bool isEnableRuler();
	bool isEnableRectangle();
	bool isEnableCircle();
	bool isEnableArrow();
	bool isEnableShapeRecognizer();

	double getThickness(ToolSize size);

private:
	Tool(const Tool& t);
	void operator = (const Tool& t);

private:
	XOJ_TYPE_ATTRIB;

	String name;
	ToolType type;

	int color;
	ToolSize size;
	double* thickness;

	bool shapeRecognizer;
	bool ruler;
	bool rectangle;
	bool circle;
	bool arrow;

	bool enableColor;
	bool enableSize;
	bool enableRuler;
	bool enableRectangle;
	bool enableCircle;
	bool enableArrow;
	bool enableShapeRecognizer;

	friend class ToolHandler;
};

#endif /* __TOOL_H__ */
