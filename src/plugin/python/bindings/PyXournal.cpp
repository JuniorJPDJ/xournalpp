#include "PyXournal.h"

#include "PySelection.h"
#include "PyUndoRedoHandler.h"
#include "PyDocument.h"

#include "../../../control/Control.h"
#include "../../../gui/XournalView.h"
#include "../../../gui/widgets/XournalWidget.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

static Control* PyXournal_control = NULL;

#include "structmember.h"
typedef struct
{
	PyObject_HEAD
	Control* control;
	PyObject* undoRedoHandler;
	PyObject* document;
	PyObject* selection;
} PyXournal;


// TODO: info pygtk: http://faq.pygtk.org/index.py?req=show&file=faq23.015.htp

void initxournal();

void PyXournal_initPython(Control* control)
{
	PyXournal_control = control;

	initxournal();
	initselection();
	initdocument();
	initundoredohandler();
}

static void PyXournal_dealloc(PyXournal* self)
{
	self->ob_type->tp_free((PyObject*) self);
}

static PyObject*
PyXournal_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyXournal* self;

	self = (PyXournal*) type->tp_alloc(type, 0);
	if (self != NULL)
	{
		self->control = PyXournal_control;
		self->undoRedoHandler = NULL;
		self->document = NULL;
		self->selection = NULL;
	}

	return (PyObject*) self;
}

static int PyXournal_init(PyXournal* self, PyObject* args, PyObject* kwds)
{
	return 0;
}

static PyMemberDef PyXournal_members[] =
{
	{ NULL } /* Sentinel */
};

static PyObject*
PyXournal_newFile(PyXournal* self, PyObject* args)
{
	PyObject* force = NULL;

	if (!PyArg_ParseTuple(args, "|O", &force))
	{
		PyErr_SetString(PyExc_AttributeError, "[Object] (should be Boolean)");
		return NULL;
	}

	if (force != NULL && !PyBool_Check(force))
	{
		PyErr_SetString(PyExc_AttributeError, "[Boolean]");
		return NULL;
	}

	if (force != NULL && PyObject_IsTrue(force))
	{
		self->control->getUndoRedoHandler()->clearContents();
	}

	if (self->control->newFile())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_saveFile(PyXournal* self, PyObject* args)
{
	char* path = NULL;

	if (!PyArg_ParseTuple(args, "s", &path))
	{
		PyErr_SetString(PyExc_AttributeError, "String");
		return NULL;
	}

	self->control->getDocument()->setFilename(path);

	if (self->control->save(true))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_openFile(PyXournal* self, PyObject* args)
{
	char* path = NULL;
	int scrollToPage = -1;

	if (!PyArg_ParseTuple(args, "s|i", &path, &scrollToPage))
	{
		PyErr_SetString(PyExc_AttributeError, "String [int]");
		return NULL;
	}

	if (self->control->openFile(path, scrollToPage))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_setSelectedTool(PyXournal* self, PyObject* args)
{
	int tool = -1;

	if (!PyArg_ParseTuple(args, "i", &tool))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	if (tool < TOOL_PEN || tool > TOOL_HAND)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "Tool out of range, please use the constants TOOL_*");
		return NULL;
	}

	self->control->getToolHandler()->selectTool((ToolType) tool);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_getSelectedTool(PyXournal* self)
{
	ToolType tt = self->control->getToolHandler()->getToolType();

	return PyInt_FromLong(tt);
}

static PyObject*
PyXournal_setToolSize(PyXournal* self, PyObject* args)
{
	int size = -1;

	if (!PyArg_ParseTuple(args, "i", &size))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	if (size < TOOL_SIZE_VERY_FINE || size > TOOL_SIZE_VERY_THICK)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "Toolsize out of range, please use the constants TOOL_SIZE_*");
		return NULL;
	}

	self->control->getToolHandler()->setSize((ToolSize) size);


	Py_RETURN_NONE;
}

static PyObject*
PyXournal_getToolSize(PyXournal* self)
{
	ToolSize ts = self->control->getToolHandler()->getSize();

	return PyInt_FromLong(ts);
}

static PyObject*
PyXournal_mousePressed(PyXournal* self, PyObject* args)
{
	double x = -1;
	double y = -1;

	if (!PyArg_ParseTuple(args, "dd", &x, &y))
	{
		PyErr_SetString(PyExc_AttributeError, "double double");
		return NULL;
	}

	MainWindow* win = self->control->getWindow();
	if(!win)
	{
		PyErr_SetString(PyExc_AttributeError, "Window not yet initialized!");
		return NULL;
	}

	int pageNo = self->control->getCurrentPageNo();
	XournalView* xournal = win->getXournal();
	double zoom = xournal->getZoom();
	PageView* v = xournal->getViewFor(pageNo);

	GdkEventButton event;
	memset(&event, 0, sizeof(GdkEventButton));
	event.type = GDK_BUTTON_PRESS;
	event.window = (GdkWindow*)*win;
	event.send_event = true;
	event.time = 0;
	event.x = x * zoom + v->getX();
	event.y = y * zoom + v->getY();
	event.axes = NULL;
	event.state = GDK_MOD2_MASK;
	event.button = 1;
	event.device = gdk_device_get_core_pointer();
	event.x_root = 0;
	event.y_root = 0;

	gtk_widget_event(xournal->getWidget(), (GdkEvent*)&event);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_mouseMoved(PyXournal* self, PyObject* args)
{
	double x = -1;
	double y = -1;

	if (!PyArg_ParseTuple(args, "dd", &x, &y))
	{
		PyErr_SetString(PyExc_AttributeError, "double double");
		return NULL;
	}

	MainWindow* win = self->control->getWindow();
	if(!win)
	{
		PyErr_SetString(PyExc_AttributeError, "Window not yet initialized!");
		return NULL;
	}

	int pageNo = self->control->getCurrentPageNo();
	XournalView* xournal = win->getXournal();
	double zoom = xournal->getZoom();
	PageView* v = xournal->getViewFor(pageNo);

	GdkEventMotion event;
	memset(&event, 0, sizeof(GdkEventMotion));
	event.type = GDK_MOTION_NOTIFY;
	event.window = (GdkWindow*)*win;
	event.send_event = true;
	event.time = 0;
	event.x = x * zoom + v->getX();
	event.y = y * zoom + v->getY();
	event.axes = NULL;
	event.state = GDK_MOD2_MASK;
	event.is_hint = 0;
	event.device = gdk_device_get_core_pointer();
	event.x_root = 0;
	event.y_root = 0;

	gtk_widget_event(xournal->getWidget(), (GdkEvent*)&event);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_mouseReleased(PyXournal* self)
{
	MainWindow* win = self->control->getWindow();
	if(!win)
	{
		PyErr_SetString(PyExc_AttributeError, "Window not yet initialized!");
		return NULL;
	}

	GdkEventButton event;
	memset(&event, 0, sizeof(GdkEventButton));
	event.type = GDK_BUTTON_RELEASE;
	event.window = (GdkWindow*)*win;
	event.send_event = true;
	event.time = 0;
	event.x = 0;
	event.y = 0;
	event.axes = NULL;
	event.state = GDK_MOD2_MASK;
	event.button = 1;
	event.device = gdk_device_get_core_pointer();
	event.x_root = 0;
	event.y_root = 0;

	gtk_widget_event(win->getXournal()->getWidget(), (GdkEvent*)&event);

	Py_RETURN_NONE;
}

#define GETTER_IMPL(attrib, constructor) \
	if(self->attrib) { \
		Py_IncRef(self->attrib); \
		return self->attrib; \
	} \
	\
	PyObject * instance = constructor((PyObject *)self); \
	if (instance) { \
		self->attrib = instance; \
		Py_IncRef(self->attrib); \
		return instance; \
	} \
	\
	Py_RETURN_NONE


static PyObject*
PyXournal_getUndoRedoHandler(PyXournal* self)
{
	GETTER_IMPL(undoRedoHandler, newPyUndoRedoHandler);
}

static PyObject*
PyXournal_getSelection(PyXournal* self)
{
	GETTER_IMPL(selection, newPySelection);
}

static PyObject*
PyXournal_getDocument(PyXournal* self)
{
	GETTER_IMPL(document, newPyDocument);
}

static PyObject*
PyXournal_getSelectedPage(PyXournal* self)
{
	int pageNo = self->control->getCurrentPageNo();

	return PyLong_FromLong(pageNo);
}

static PyObject*
PyXournal_selectPage(PyXournal* self, PyObject* args)
{
	int page = -1;

	if (!PyArg_ParseTuple(args, "i", &page))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	self->control->getScrollHandler()->scrollToPage(page, 0);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_setRulerEnabled(PyXournal* self, PyObject* args)
{
	PyObject* enable = NULL;

	if (!PyArg_ParseTuple(args, "O", &enable))
	{
		PyErr_SetString(PyExc_AttributeError, "[Object] (should be Boolean)");
		return NULL;
	}

	if (!PyBool_Check(enable))
	{
		PyErr_SetString(PyExc_AttributeError, "[Boolean]");
		return NULL;
	}

	self->control->setRulerEnabled(PyObject_IsTrue(enable));

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_isRulerEnabled(PyXournal* self)
{
	if (self->control->getToolHandler()->isRuler())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_setShapeRecognizerEnabled(PyXournal* self, PyObject* args)
{
	PyObject* enable = NULL;

	if (!PyArg_ParseTuple(args, "O", &enable))
	{
		PyErr_SetString(PyExc_AttributeError, "[Object] (should be Boolean)");
		return NULL;
	}

	if (!PyBool_Check(enable))
	{
		PyErr_SetString(PyExc_AttributeError, "[Boolean]");
		return NULL;
	}

	self->control->setShapeRecognizerEnabled(PyObject_IsTrue(enable));

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_isShapeRecognizerEnabled(PyXournal* self)
{
	if (self->control->getToolHandler()->isShapeRecognizer())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_getToolColor(PyXournal* self)
{
	int color = self->control->getToolHandler()->getColor();

	return PyInt_FromLong(color);
}

static PyObject*
PyXournal_setToolColor(PyXournal* self, PyObject* args)
{
	int color = -1;

	if (!PyArg_ParseTuple(args, "i", &color))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	self->control->getToolHandler()->setColor(color);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_getCurrentPageBackground(PyXournal* self)
{
	PageRef page = self->control->getCurrentPage();

	if(!page.isValid())
	{
		PyErr_SetString(PyExc_RuntimeError,
		                "getCurrentPageBackground::No current page!");
		return NULL;
	}

	return PyInt_FromLong(page->getBackgroundType());
}

static PyObject*
PyXournal_setCurrentPageBackground(PyXournal* self, PyObject* args)
{
	int backgroundType = -1;

	if (!PyArg_ParseTuple(args, "i", &backgroundType))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	if (backgroundType < BACKGROUND_TYPE_NONE ||
	    backgroundType > BACKGROUND_TYPE_GRAPH)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "Backgroundtype out of range, please use the constants BACKGROUND_TYPE_*");
		return NULL;
	}

	if (backgroundType == BACKGROUND_TYPE_PDF)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "BACKGROUND_TYPE_PDF connot be set with setCurrentPageBackground");
		return NULL;
	}

	if (backgroundType == BACKGROUND_TYPE_IMAGE)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "BACKGROUND_TYPE_PDF connot be set with setCurrentPageBackground");
		return NULL;
	}

	PageRef page = self->control->getCurrentPage();

	if(!page.isValid())
	{
		PyErr_SetString(PyExc_RuntimeError,
		                "setCurrentPageBackground::No current page!");
		return NULL;
	}

	page->setBackgroundType((BackgroundType)backgroundType);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_getEraserType(PyXournal* self)
{
	EraserType type = self->control->getToolHandler()->getEraserType();
	return PyInt_FromLong(type);
}

static PyObject*
PyXournal_setEraserType(PyXournal* self, PyObject* args)
{
	int eraserType = -1;

	if (!PyArg_ParseTuple(args, "i", &eraserType))
	{
		PyErr_SetString(PyExc_AttributeError, "int");
		return NULL;
	}

	if (eraserType < ERASER_TYPE_DEFAULT || eraserType > ERASER_TYPE_DELETE_STROKE)
	{
		PyErr_SetString(PyExc_AttributeError,
		                "Erasertype out of range, please use the constants BACKGROUND_TYPE_*");
		return NULL;
	}

	self->control->getToolHandler()->setEraserType((EraserType)eraserType);

	Py_RETURN_NONE;
}

static PyObject*
PyXournal_copy(PyXournal* self, PyObject* args)
{
	if(self->control->copy())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_cut(PyXournal* self, PyObject* args)
{
	if(self->control->cut())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject*
PyXournal_paste(PyXournal* self, PyObject* args)
{
	if(self->control->paste())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyMethodDef PyXournal_methods[] =
{
	{ "setSelectedTool", (PyCFunction) PyXournal_setSelectedTool, METH_VARARGS, "Selects a tool (see constatns TOOL_*" },
	{ "getSelectedTool", (PyCFunction) PyXournal_getSelectedTool, METH_NOARGS, "Return the selected tool" },
	{ "newFile", (PyCFunction) PyXournal_newFile, METH_VARARGS, "Create a new document" },
	{ "saveFile", (PyCFunction) PyXournal_saveFile, METH_VARARGS, "Save a document as" },
	{ "openFile", (PyCFunction) PyXournal_openFile, METH_VARARGS, "Opens a file" },
	{ "mousePressed", (PyCFunction) PyXournal_mousePressed, METH_VARARGS, "Simulate a mouse press on the current view" },
	{ "mouseMoved", (PyCFunction) PyXournal_mouseMoved, METH_VARARGS, "Simulate a mouse move on the current view" },
	{ "mouseReleased", (PyCFunction) PyXournal_mouseReleased, METH_NOARGS, "Simulate a mouse release" },
	{ "getUndoRedoHandler", (PyCFunction) PyXournal_getUndoRedoHandler, METH_NOARGS, "Gets the Undo- / Redohandler" },
	{ "getDocument", (PyCFunction) PyXournal_getDocument, METH_NOARGS, "Return the Xournal Document" },
	{ "getSelectedPage", (PyCFunction) PyXournal_getSelectedPage, METH_NOARGS, "Gets the selected page ID (first Page: 0)" },
	{ "selectPage", (PyCFunction) PyXournal_selectPage, METH_VARARGS, "Sets the selected page (first Page: 0)" },
	{ "setRulerEnabled", (PyCFunction) PyXournal_setRulerEnabled, METH_VARARGS, "Enables the ruler" },
	{ "isRulerEnabled", (PyCFunction) PyXournal_isRulerEnabled, METH_NOARGS, "Return true if the ruler is enabled" },
	{ "setShapeRecognizerEnabled", (PyCFunction) PyXournal_setShapeRecognizerEnabled, METH_VARARGS, "Enables the shape recognizer" },
	{ "isShapeRecognizerEnabled", (PyCFunction) PyXournal_isShapeRecognizerEnabled, METH_NOARGS, "Return true if the shape recognizer is enabled" },
	{ "setToolColor", (PyCFunction) PyXournal_setToolColor, METH_VARARGS, "Sets the tool Color in RGB" },
	{ "getToolColor", (PyCFunction) PyXournal_getToolColor, METH_NOARGS, "Returns the tool color in RGB" },
	{ "setToolSize", (PyCFunction) PyXournal_setToolSize, METH_VARARGS, "Sets the current tool size" },
	{ "getToolSize", (PyCFunction) PyXournal_getToolSize, METH_NOARGS, "Return the selected tool size" },
	{ "getCurrentPageBackground", (PyCFunction) PyXournal_getCurrentPageBackground, METH_NOARGS, "Return the background type of the current page (see BACKGROUND_TYPE_*)" },
	{ "setCurrentPageBackground", (PyCFunction) PyXournal_setCurrentPageBackground, METH_VARARGS, "Set the background type of the current page (see BACKGROUND_TYPE_*)\nDon't use BACKGROUND_TYPE_PDF or BACKGROUND_TYPE_PDF" },
	{ "setEraserType", (PyCFunction) PyXournal_setEraserType, METH_VARARGS, "Set the eraser type (ERASER_TYPE_*)" },
	{ "getEraserType", (PyCFunction) PyXournal_getEraserType, METH_VARARGS, "Returns the eraser type (ERASER_TYPE_*)" },
	{ "copy", (PyCFunction) PyXournal_copy, METH_NOARGS, "Copy to clipboard" },
	{ "cut", (PyCFunction) PyXournal_cut, METH_NOARGS, "Copy to clipboard and delete after" },
	{ "paste", (PyCFunction) PyXournal_paste, METH_NOARGS, "Paste from clipboard (if possible)" },
	{ "getSelection", (PyCFunction) PyXournal_getSelection, METH_NOARGS, "Return the Selection or None" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	//	{ "xxxxxxxxxxxxx", (PyCFunction) xxxxxxxxxxxxxxxxx, METH_VARARGS, "Xxxxxxxxxxxxxxxx" },
	{ NULL } /* Sentinel */
};

static PyTypeObject XournalType =
{
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"xournal.Xournal", /*tp_name*/
	sizeof(PyXournal), /*tp_basicsize*/
	0, /*tp_itemsize*/
	(destructor)PyXournal_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	0, /*tp_getattr*/
	0, /*tp_setattr*/
	0, /*tp_compare*/
	0, /*tp_repr*/
	0, /*tp_as_number*/
	0, /*tp_as_sequence*/
	0, /*tp_as_mapping*/
	0, /*tp_hash */
	0, /*tp_call*/
	0, /*tp_str*/
	0, /*tp_getattro*/
	0, /*tp_setattro*/
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"Xournal object", /* tp_doc */
	0, /* tp_traverse */
	0, /* tp_clear */
	0, /* tp_richcompare */
	0, /* tp_weaklistoffset */
	0, /* tp_iter */
	0, /* tp_iternext */
	PyXournal_methods, /* tp_methods */
	PyXournal_members, /* tp_members */
	0, /* tp_getset */
	0, /* tp_base */
	0, /* tp_dict */
	0, /* tp_descr_get */
	0, /* tp_descr_set */
	0, /* tp_dictoffset */
	(initproc)PyXournal_init, /* tp_init */
	0, /* tp_alloc */
	PyXournal_new, /* tp_new */
};

static PyMethodDef module_methods[] = { { NULL } /* Sentinel */
};


bool PyXournal_Check(PyObject* obj)
{
	return Py_TYPE(obj) == &XournalType;
}

Control* PyXournal_getControl(PyObject* obj)
{
	if(!PyXournal_Check(obj))
	{
		return NULL;
	}
	return ((PyXournal*)obj)->control;
}

#define ADD_CONST(tool) PyDict_SetItemString(XournalType.tp_dict, #tool, PyLong_FromLong(tool));

void initxournal()
{
	PyObject* m;

	if (PyType_Ready(&XournalType) < 0)
		return;

	ADD_CONST(TOOL_PEN);
	ADD_CONST(TOOL_ERASER);
	ADD_CONST(TOOL_HILIGHTER);
	ADD_CONST(TOOL_TEXT);
	ADD_CONST(TOOL_IMAGE);
	ADD_CONST(TOOL_SELECT_RECT);
	ADD_CONST(TOOL_SELECT_REGION);
	ADD_CONST(TOOL_SELECT_OBJECT);
	ADD_CONST(TOOL_VERTICAL_SPACE);
	ADD_CONST(TOOL_HAND);

	ADD_CONST(TOOL_SIZE_VERY_FINE);
	ADD_CONST(TOOL_SIZE_FINE);
	ADD_CONST(TOOL_SIZE_MEDIUM);
	ADD_CONST(TOOL_SIZE_THICK);
	ADD_CONST(TOOL_SIZE_VERY_THICK);

	ADD_CONST(BACKGROUND_TYPE_NONE);
	ADD_CONST(BACKGROUND_TYPE_PDF);
	ADD_CONST(BACKGROUND_TYPE_IMAGE);
	ADD_CONST(BACKGROUND_TYPE_LINED);
	ADD_CONST(BACKGROUND_TYPE_RULED);
	ADD_CONST(BACKGROUND_TYPE_GRAPH);

	ADD_CONST(ERASER_TYPE_DEFAULT);
	ADD_CONST(ERASER_TYPE_WHITEOUT);
	ADD_CONST(ERASER_TYPE_DELETE_STROKE);

	m = Py_InitModule3("xournal", module_methods, "Xournal API modul");

	if (m == NULL)
	{
		return;
	}

	Py_INCREF(&XournalType);
	PyModule_AddObject(m, "Xournal", (PyObject*) &XournalType);
}

#ifdef __cplusplus
}
#endif
