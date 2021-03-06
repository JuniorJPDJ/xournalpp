/*
 * Xournal++
 *
 * XML Writer helper class
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <OutputStream.h>
#include <XournalType.h>

class XMLAttribute
{
public:
	XMLAttribute(const char* name);
	virtual ~XMLAttribute();

public:
	virtual void writeOut(OutputStream* out) = 0;

	const char* getName();

private:
	XOJ_TYPE_ATTRIB;


	char* name;
};
