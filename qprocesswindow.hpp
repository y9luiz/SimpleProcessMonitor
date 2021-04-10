#include <qt5/QtWidgets/QMainWindow>
#include <qt5/QtWidgets/QTableWidget>
#include <qt5/QtWidgets/QWidget>
#include <proc/readproc.h>
#include <pwd.h>
#include <qt5/QtCore/QStringList>
#include <QObject>
#include <iostream>
#define NUMBER_COLS 6
#define NUMBER_FILTERS 3
class QProcessWindow: public QMainWindow
{
    Q_OBJECT
    public:
        QProcessWindow(QWidget *parent = 0);
        virtual ~QProcessWindow(){
            delete proc;
            delete info;
            delete table;
            delete [] lineEdit_filters;
        };
    private:
        PROCTAB* proc;
        proc_t* info;
        std::vector<QStringList> getProcInformations();
        QTableWidget * table;
        QLineEdit ** lineEdit_filters; // PID FILTER, USER FILTER COMMAND FILTER
        void paintEvent(QPaintEvent * e);
        void updateTableProcess();

    signals:
        void tableCellClicked(int,int);
};