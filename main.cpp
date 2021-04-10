#include <iostream>
#include <QApplication>
#include <qt5/QtWidgets/QMainWindow>
#include "qprocesswindow.hpp"

#include <pwd.h>
static unsigned needs_for_threads;
static unsigned needs_for_sort;
static unsigned proc_format_needs;
static unsigned task_format_needs;
#define needs_for_format (proc_format_needs|task_format_needs)
#define needs_for_select (PROC_FILLSTAT | PROC_FILLSTATUS)

int main(int argc, char ** argv)
{

    QApplication app(argc, argv);
    QProcessWindow w;
    
    w.showMaximized();
    w.setWindowTitle("Process Monitor");

    return app.exec();
}