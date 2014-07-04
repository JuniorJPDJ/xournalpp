/*
 * Xournal++
 *
 * A job to export PDF
 *
 * @author Xournal Team
 * http://xournal.sf.net
 *
 * @license GPL
 */

#ifndef __PDFEXPORTJOB_H__
#define __PDFEXPORTJOB_H__

#include "BlockingJob.h"
#include <String.h>
#include <XournalType.h>

class Control;
class PdfExportJob: public BlockingJob
{
public:
	PdfExportJob(Control* control);

protected:
	virtual ~PdfExportJob();

public:
	void run(bool noThreads);
	virtual void afterRun();

public:
	bool showFilechooser();

private:
	XOJ_TYPE_ATTRIB;

	String filename;

	String errorMsg;
};

#endif /* __PDFEXPORTJOB_H__ */
