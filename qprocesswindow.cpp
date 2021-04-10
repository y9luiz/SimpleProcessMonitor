#include "qprocesswindow.hpp"
#include <iostream>
#include <QVBoxLayout> 
#include <QHBoxLayout>
#include <QtWidgets/QPushButton> 
#include <signal.h>
#include <QTimer>
#include <proc/sysinfo.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <QtWidgets/QLineEdit>
QProcessWindow::QProcessWindow(QWidget *parent)
{
    table = new QTableWidget();
    table->setColumnCount(NUMBER_COLS);
    QVBoxLayout * layout = new QVBoxLayout;
    QPushButton* button = new QPushButton("Kill Process");
    button->setMaximumWidth(this->width()/5);
    QHBoxLayout * hlayout = new QHBoxLayout;
    layout->addWidget(table);
    hlayout->addWidget(button);

    layout->addLayout(hlayout);
    lineEdit_filters = new QLineEdit*[NUMBER_FILTERS];
    QStringList place_holders_filters;

    place_holders_filters<<"PID FILTER"<<"USER FILTER"<<"Command FILTER";
    for(int i=0;i<NUMBER_FILTERS;i++)
    {
        lineEdit_filters[i] = new QLineEdit();
        lineEdit_filters[i]->setPlaceholderText(place_holders_filters[i]);
        layout->addWidget(lineEdit_filters[i]);
    }
    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout(layout);

    this->setCentralWidget(window);
    QStringList str_list;
    str_list<<"PID"<<"USER"<<"STATE"<<"CPU %"<<"MEM %"<<" Command ";
    table->setHorizontalHeaderLabels(str_list);
    updateTableProcess();
    QTableView * view = (QTableView*)table->verticalHeader();
    view->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //evento clicar em uma celula
    connect(table, &QTableWidget::cellClicked, this, [=](int r ,int c){this->table->selectRow(r);});
    connect(table, &QTableWidget::itemSelectionChanged, this, [=](){int r = this->table->currentRow(); this->table->selectRow(r);});

    //evento clicar no botao de kill
    connect(button, &QPushButton::clicked, this, [=](){
            int selected_row = this->table->currentRow();
            int selected_col = this->table->currentColumn();
            int pid = table->takeItem(selected_row,selected_col)->text().toInt();
            kill(pid,SIGKILL);
            this->table->removeRow(selected_row);
            updateTableProcess();
        });
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &QProcessWindow::updateTableProcess);
    timer->start(1000);
}

std::vector<QStringList> QProcessWindow::getProcInformations()
{
    proc=openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS ,NULL);
    if(!proc)
    {
        throw std::exception();
    }
    std::vector<QStringList> data;
    meminfo();
    double up_time,idle_sec;
    uptime(&up_time,&idle_sec);
    QString pid_filter = this->lineEdit_filters[0]->text();
    QString user_filter = this->lineEdit_filters[1]->text();
    QString command_filter = this->lineEdit_filters[2]->text();
    bool filters_empty = pid_filter.isEmpty() && user_filter.isEmpty() && command_filter.isEmpty();
    while( info = readproc(proc,NULL)){
        QString task_id = QString::number( info->tgid);
        QString username =    getpwuid(info->euid)->pw_name;
        QString command = info->cmd;
        QString state;
        state.append(info->state);
        QStringList process_info;
        long int total_main_mem = kb_main_total;
        double mem_perc = info->vm_rss*100.0/total_main_mem;
        unsigned long long total_time = info->utime + info->stime;
        total_time+=info->cutime+info->cstime;
        double seconds = up_time - (info->start_time/Hertz);
        double cpu_usage = 100* ((total_time/Hertz)/seconds);
        bool display_pid = QString::compare(task_id, pid_filter)==0;
        bool display_username = QString::compare(username, user_filter)==0;
        bool display_command = QString::compare(command, command_filter)==0;
        
        if(display_pid || display_username || display_command || filters_empty){

            process_info<<task_id<<username<<state<<QString::number(cpu_usage)<<QString::number(mem_perc)<<command;
            data.push_back(process_info);

        }
        
        freeproc(info);
    }
    return data;
}

void QProcessWindow::paintEvent(QPaintEvent * e){
    auto process_info = getProcInformations();
    int item_width = table->width()/NUMBER_COLS;
    for(int i =0;i<NUMBER_COLS;i++)
    {
            table->setColumnWidth(i,item_width);
    }
    
       
}

void QProcessWindow::updateTableProcess()
{
    int selected_row = this->table->currentRow();
    table->setRowCount(0);
    auto process_info = getProcInformations();
    // para cada processo
    for(int i =0;i<process_info.size();i++)
    {
        table->insertRow(table->rowCount());
        //tenho uma lista de informações dentro dele
        int col = 0;
        for( auto &it:process_info[i])
        {
            table->setItem(table->rowCount()-1,col,new QTableWidgetItem(it));
            col++;
        }
    }
    table->selectRow(selected_row);
}