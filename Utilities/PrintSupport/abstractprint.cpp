#include "abstractprint.h"
#include "QPrintDialog"
#include "QPrinter"
#include "QPainter"
#include "QStandardPaths"
#include<QTextDocument>
#include <QFileDialog>
#include <QDesktopServices>

//TODO : Add Print JSON

AbstractPrint::AbstractPrint(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;

}

AbstractPrint::~AbstractPrint()
{
    mutex.lock ();
    abort = true;
    condition.wakeOne ();
    mutex.unlock ();
    wait();
}

void AbstractPrint::openPrintDialog()
{
    if(!isRunning ()){
        start (LowPriority);
    }else{
        restart = true;
        condition.wakeOne ();
    }
}

void AbstractPrint::run()
{
    forever{
        if(abort)
            return;
        if(restart)
            break;


        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOrientation (QPrinter::Landscape);

        auto fileName = QStandardPaths::writableLocation (QStandardPaths::DesktopLocation)+"/out%1.pdf";

        auto incrementFileName = fileName.arg ("");
        auto count = 1;

        while(QFile(incrementFileName).exists ()){
            incrementFileName = fileName.arg ("_"+QString::number (count));
            ++count;
        }

        qDebug() << incrementFileName;
        printer.setOutputFileName(incrementFileName);

        QString html;

        for(int row = 0 ; row < 100 ; ++row){
            html += "<p> ("+QString::number (row)+") Lorem ipsum dolor sit amet, consectitur adipisci elit.</p>";
        }

        QTextDocument doc;
        doc.setHtml(html);
        doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
        doc.print(&printer);

        QDesktopServices::openUrl (QUrl(incrementFileName));

        mutex.lock ();
        if(!restart)
            condition.wait (&mutex);
        restart = false;
        qDebug() << "Pausing Printer Thread : " << currentThreadId ();
        mutex.unlock ();
    }
}
