#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<dirent.h>
#include<linux/a.out.h>
#include<stdio.h>
#include<QTimer>
#include<QDebug>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<QMessageBox>
#include<QComboBox>
#include<QTableWidget>
#include<QString>
#include<stdio.h>
#include<sys/vfs.h>
#include<QHBoxLayout>
#include<QProgressBar>
#include<QTimer>
#include<pwd.h>
QVector<_G>CPU_data;
QVector<double >Mem_usage;
QVector< QVector<double>  >CPU_usage(4);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
  for(int i =0;i<149;++i){
     for(int j = 0;j < 4;++j){
               CPU_usage[j].push_back(0);
     }
   }
  for(int i = 0;i < 149;++i){
    Mem_usage.push_back(0);
  }
   SetProcess();
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeProcess()));
    connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(TableSort(int)));
    ChangeSys();
   SetCore();
    QTimer * timer2  =  new QTimer;
    connect(timer2,SIGNAL(timeout()),this,SLOT(SetCoreLabel()));
    timer2->start(2000);
    SetCoreLabel();
    SetMem();
    QTimer * timer3  =  new QTimer;
    connect(timer3,SIGNAL(timeout()),this,SLOT(SetMemLabel()));
    timer3->start(2000);
    SetMemLabel();
    QTimer * timer4  =  new QTimer;
    connect(timer4,SIGNAL(timeout()),this,SLOT(ChangeGraph()));
    timer4->start(500);
    ChangeGraph();
    QTimer * timer5  =  new QTimer;
    connect(timer5,SIGNAL(timeout()),this,SLOT(ChangeMemGraph()));
    timer5->start(500);
    ChangeMemGraph();
}
void MainWindow::ChangeMemGraph(){
    FILE * fd;
    fd = fopen("/proc/meminfo","r");
    if(fd ==NULL)return ;
    _M Mem_info;
    char buffer[256];
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.total);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.free);
    fgets(buffer,sizeof(buffer),fd);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.buffer);
    fgets(buffer,sizeof(buffer),fd);
     sscanf(buffer,"%*s %ld",&Mem_info.cache);
     QVector<double>time(150);
     for(int i = 0;i < 150;++i){
         time[i] =0.5*i;
     }
     if(Mem_usage.size()>= 150){
         Mem_usage.pop_front();
     }
     double tmp =100.0* (Mem_info.total - Mem_info.cache - Mem_info.free) / Mem_info.total;
     Mem_usage.push_back(tmp);

     ui->widget_2->graph(0)->setData(time,Mem_usage);
     ui->widget_2->replot();
}

void MainWindow::ChangeGraph(){
    FILE * fd;

    char buffer[5][256];


        if((fd = fopen("/proc/stat","r")) == NULL)
        {
            qDebug()<< "Error in open stat file";
            return ;
        }
        fgets(buffer[0],sizeof(buffer[0]),fd);
        for(int i = 0;i < 4;++i){
            fgets(buffer[i+1],sizeof(buffer[i+1]),fd);
        }
        _G tmp;
        for(int i = 0;i < 4;++i){
           sscanf(buffer[i+1],"%*s %ld %ld %ld %ld",&tmp.user[i],&tmp.nice[i],&tmp.sys[i],&tmp.idle[i]);
       }
       fclose(fd);
       _G prior = CPU_data.front();
       CPU_data.pop_front();
        CPU_data.push_back(tmp);
         for(int i =0;i < 4;++i){
           if(prior.idle[i] + prior.nice[i] + prior.sys[i] + prior.user[i] ==tmp.idle[i] + tmp.nice[i] + tmp.sys[i] + tmp.user[i] ){
               if(CPU_usage[i].size() > 150)CPU_usage[i].pop_front();
               CPU_usage[i].push_back(0);
           }
           else{
               if(CPU_usage[i].size() > 150)CPU_usage[i].pop_front();
               double p =  (prior.nice[i] + prior.sys[i] + prior.user[i]  - tmp.nice[i] -tmp.sys[i] -tmp.user[i] )* 100.0   /(prior.idle[i] + prior.nice[i] + prior.sys[i] + prior.user[i]-(tmp.idle[i] + tmp.nice[i] + tmp.sys[i] + tmp.user[i] ));
               CPU_usage[i].push_back(p);
           }
       }
         QVector<double>time(150);
         for(int i = 0;i < 150;++i){
             time[i] = 0.5 * i;

         }
         ui->widget->graph(0)->setData(time,CPU_usage[0]);
         ui->widget->graph(1)->setData(time,CPU_usage[1]);
         ui->widget->graph(2)->setData(time,CPU_usage[2]);
         ui->widget->graph(3)->setData(time,CPU_usage[3]);
         ui->widget->replot();
}

void MainWindow::SetMemLabel(){
    FILE * fd;
    if(fd ==NULL)return ;
    char buffer[256];
    long total;
    long free;
    long mbuffer;
    long cache;
    fd = fopen("/proc/meminfo","r");
    if(fd ==NULL)return ;
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&total);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&free);
    fgets(buffer,sizeof(buffer),fd);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&mbuffer);
    fgets(buffer,sizeof(buffer),fd);
     sscanf(buffer,"%*s %ld",&cache);
     fclose(fd);

     float usage =100.0 *  (total- free- mbuffer - cache) / total;
     float to;
     float us;
     char convert[10];
     sprintf(convert,"%.1f",usage);
     sscanf(convert,"%f",&usage);
     sprintf(convert,"%.1f",total * 1.0 /1024/1024);
     sscanf(convert,"%f",&to);
     sprintf(convert,"%.1f",(total - free - mbuffer-cache)*1.0 / 1024 /1024);
     sscanf(convert,"%f",&us);
     ui->label_6->setText("内存\n" +QString("%1GiB").arg(us)+ " (" + QString("%1%").arg(usage)+")  共"+ QString("%1GiB").arg(to));
}

int MainWindow::SetMem(){

    FILE * fd;
    fd = fopen("/proc/meminfo","r");
    if(fd ==NULL)return 0;
    _M Mem_info;
    char buffer[256];
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.total);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.free);
    fgets(buffer,sizeof(buffer),fd);
    fgets(buffer,sizeof(buffer),fd);
    sscanf(buffer,"%*s %ld",&Mem_info.buffer);
    fgets(buffer,sizeof(buffer),fd);
     sscanf(buffer,"%*s %ld",&Mem_info.cache);
     fclose(fd);
     QVector<double>time(150);
     for(int i = 0;i < 150;++i){
         time[i] =0.5*i;
     }
     if(Mem_usage.size()>= 150){
         Mem_usage.pop_front();
     }
     double tmp =100.0* (Mem_info.total - Mem_info.cache - Mem_info.free) / Mem_info.total;
     Mem_usage.push_back(tmp);
    ui->widget_2->addGraph();
   ui->widget_2->graph(0)->setName("Mem");
   ui->widget_2->xAxis->setLabel("Time");
   ui->widget_2->yAxis->setLabel("Usage");
   ui->widget_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   ui->widget_2->yAxis->setAutoTickCount(2);
   ui->widget_2->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop);
   ui->widget_2->legend->setVisible(true);
   ui->widget_2->yAxis->setRange(0,50);
   ui->widget_2->xAxis->setRange(0,75);
    ui->widget_2->graph(0)->setData(time,Mem_usage);
   ui->widget_2->replot();
}
int MainWindow::SetCore(){
    FILE * fd;

    char buffer[61][5][256];
    long user[61][5];
    long nice[61][5];
    long sys[61][5];
    long idle[61][5];
    long iowait[61][5];
    long soft[61][5];
    char name[61][5][5];
    float usage[60][5];
    for(int index= 0;index <2;++index){
        if((fd = fopen("/proc/stat","r")) == NULL)
        {
            qDebug()<< "Error in open stat file";
            return 0;
        }
        fgets(buffer[index][0],sizeof(buffer[index][0]),fd);
        for(int i = 0;i < 4;++i){
            fgets(buffer[index][i+1],sizeof(buffer[index][i+1]),fd);
        }
  for(int i = 0;i < 4;++i){
           _G tmp;
           sscanf(buffer[index][i+1],"%*s %ld %ld %ld %ld",&tmp.user[i],&tmp.nice[i],&tmp.sys[i],&tmp.idle[i]);
           if(CPU_data.size() >=150){
               CPU_data.pop_front();
           }
           CPU_data.push_back(tmp);
       }

        fclose(fd);
        usleep(500000);
    }

        for(int size = 0;size < CPU_data.size() - 1;++size){
            _G prior = CPU_data.front();
            CPU_data.pop_front();
            _G next = CPU_data.front();
            for(int i =0;i < 4;++i){
                if(prior.idle[i] + prior.nice[i] + prior.sys[i] + prior.user[i] ==next.idle[i] + next.nice[i] + next.sys[i] + next.user[i] ){
                    if(CPU_usage[i].size() > 150)CPU_usage[i].pop_front();
                    CPU_usage[i].push_back(0);
                }
                else{
                    if(CPU_usage[i].size() > 150)CPU_usage[i].pop_front();
                    double tmp =  (prior.nice[i] + prior.sys[i] + prior.user[i]  - next.nice[i] -next.sys[i] -next.user[i] )* 100.0   /(prior.idle[i] + prior.nice[i] + prior.sys[i] + prior.user[i]-(next.idle[i] + next.nice[i] + next.sys[i] + next.user[i] ));
                    CPU_usage[i].push_back(tmp);
                }
            }
        }

   ui->widget->addGraph();
   ui->widget->addGraph();
   ui->widget->addGraph();
   ui->widget->addGraph();

   ui->widget->graph(0)->setName("CPU1");
   ui->widget->graph(1)->setName("CPU2");
   ui->widget->graph(2)->setName("CPU3");
   ui->widget->graph(3)->setName("CPU4");
   ui->widget->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop);

   ui->widget->graph(0)->setPen(QPen(Qt::blue));
   ui->widget->graph(1)->setPen(QPen(Qt::red));
   ui->widget->graph(2)->setPen(QPen(Qt::black));
   ui->widget->graph(3)->setPen(QPen(Qt::yellow));


   // make left and bottom axes always transfer their ranges to right and top axes:
   connect(ui->widget->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->xAxis2, SLOT(setRange(QCPRange)));
   connect(ui->widget->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->yAxis2, SLOT(setRange(QCPRange)));
   // pass data points to graphs:
   QVector<double>time(150);
   for(int i = 0;i < 150;++i){
       time[i] =0.5*i;

   }
   ui->widget->graph(0)->setData(time,CPU_usage[0]);
   ui->widget->graph(1)->setData(time,CPU_usage[1]);
   ui->widget->graph(2)->setData(time,CPU_usage[2]);
   ui->widget->graph(3)->setData(time,CPU_usage[3]);
   ui->widget->xAxis->setLabel("Time");
    ui->widget->yAxis->setLabel("CPU usage");
   // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
   ui->widget->graph(0)->rescaleAxes();
   // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
   ui->widget->graph(1)->rescaleAxes(true);
   // Note: we could have also just called customPlot->rescaleAxes(); instead
   // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
   ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   ui->widget->yAxis->setAutoTickCount(3);

   ui->widget->legend->setVisible(true);
   ui->widget->yAxis->setRange(0,50);
   ui->widget->replot();

}
void MainWindow::SetCoreLabel(){

    FILE * fd;
    char buffer[5][256];
    long user[4][2];
    long nice[4][2];
    long sys[4][2];
    long idle[4][2];
    fd = fopen("/proc/stat","r");
    if(fd == NULL)return ;
    fgets(buffer[0],sizeof(buffer[0]),fd);
    fgets(buffer[1],sizeof(buffer[1]),fd);
    fgets(buffer[2],sizeof(buffer[2]),fd);
    fgets(buffer[3],sizeof(buffer[3]),fd);
    fgets(buffer[4],sizeof(buffer[4]),fd);

    sscanf(buffer[1],"%*s %ld %ld %ld %ld",&(user[0][0]),&(nice[0][0]),&(sys[0][0]),&(idle[0][0]));
    sscanf(buffer[2],"%*s %ld %ld %ld %ld",&(user[1][0]),&(nice[1][0]),&(sys[1][0]),&(idle[1][0]));
    sscanf(buffer[3],"%*s %ld %ld %ld %ld",&(user[2][0]),&(nice[2][0]),&(sys[2][0]),&(idle[2][0]));
    sscanf(buffer[4],"%*s %ld %ld %ld %ld",&(user[3][0]),&(nice[3][0]),&(sys[3][0]),&(idle[3][0]));

    fclose(fd);

    sleep(1);
    fd = fopen("/proc/stat","r");
    if(fd == NULL)return ;
    fgets(buffer[0],sizeof(buffer[0]),fd);
    fgets(buffer[1],sizeof(buffer[1]),fd);
    fgets(buffer[2],sizeof(buffer[2]),fd);
    fgets(buffer[3],sizeof(buffer[3]),fd);
    fgets(buffer[4],sizeof(buffer[4]),fd);

    sscanf(buffer[1],"%*s %ld %ld %ld %ld",&user[0][1],&nice[0][1],&sys[0][1],&idle[0][1]);
    sscanf(buffer[2],"%*s %ld %ld %ld %ld",&user[1][1],&nice[1][1],&sys[1][1],&idle[1][1]);
    sscanf(buffer[3],"%*s %ld %ld %ld %ld",&user[2][1],&nice[2][1],&sys[2][1],&idle[2][1]);
    sscanf(buffer[4],"%*s %ld %ld %ld %ld",&user[3][1],&nice[3][1],&sys[3][1],&idle[3][1]);
    char convert[10];
    float result = 0;
    fclose(fd);
    if(user[0][1]+sys[0][1]+nice[0][1]+idle[0][1] == user[0][0]+sys[0][0]+idle[0][0]+nice[0][0])ui->label_2->setText("BLUE CPU1 0%");
    else{
        sprintf(convert,"%.1f",100.0*(user[0][1]+sys[0][1]+nice[0][1]- user[0][0]-sys[0][0]-nice[0][0])/(user[0][1]+sys[0][1]+nice[0][1]+idle[0][1] - user[0][0]-sys[0][0]-idle[0][0]-nice[0][0]));
        sscanf(convert,"%f",&result);
        ui->label_2->setText("BLUE CPU1 " + QString("%1%").arg(result));
    }

    if(user[1][1]+sys[1][1]+nice[1][1]+idle[1][1] == user[1][0]+sys[1][0]+idle[1][0]+nice[1][0])ui->label_2->setText("RED CPU2 0%");
    else{
        sprintf(convert,"%.1f",100.0*(user[1][1]+sys[1][1]+nice[1][1]- user[1][0]-sys[1][0]-nice[1][0])/(user[1][1]+sys[1][1]+nice[1][1]+idle[1][1] - user[1][0]-sys[1][0]-idle[1][0]-nice[1][0]));
        sscanf(convert,"%f",&result);
        ui->label_3->setText("RED CPU2 " + QString("%1%").arg(result));
    }

    if(user[2][1]+sys[2][1]+nice[2][1]+idle[2][1] == user[2][0]+sys[2][0]+idle[2][0]+nice[2][0])ui->label_2->setText("BLACK CPU3 0%");
    else{
        sprintf(convert,"%.1f",100.0*(user[2][1]+sys[2][1]+nice[2][1]- user[2][0]-sys[2][0]-nice[2][0])/(user[2][1]+sys[2][1]+nice[2][1]+idle[2][1] - user[2][0]-sys[2][0]-idle[2][0]-nice[2][0]));
        sscanf(convert,"%f",&result);
        ui->label_4->setText("BLACK CPU3 " + QString("%1%").arg(result));
    }

    if(user[3][1]+sys[3][1]+nice[3][1]+idle[3][1] == user[3][0]+sys[3][0]+idle[3][0]+nice[3][0])ui->label_2->setText("YELLOW CPU4 0%");
    else{

        sprintf(convert,"%.1f",100.0*(user[3][1]+sys[3][1]+nice[3][1]- user[3][0]-sys[3][0]-nice[3][0])/(user[3][1]+sys[3][1]+nice[3][1]+idle[3][1] - user[3][0]-sys[3][0]-idle[3][0]-nice[3][0]));
        sscanf(convert,"%f",&result);
        ui->label_5->setText("YELLOW CPU4 " + QString("%1%").arg(result));
    }
}

void MainWindow::ChangeSys(){
    this->SetFileSys();
}

void MainWindow::ChangeProcess(){
    this->SetProcess();

}
int MainWindow::SetFileSys(){
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setShowGrid(false);
    ui->tableWidget_2->setRowCount(0);

        FILE * fp;
        char path[100][100];
        fp = popen("/bin/mount","r");
        int all = 0;
        if(fp == NULL){
            printf("Failed to run command\n");
            exit(1);
        }
        while(fgets(path[all],sizeof(path[all]) -1,fp)!= NULL){
            ++all;
        }
        pclose(fp);
        int index;
        struct statfs sfs;
        char position[100];
        char type[50];
        char devname[50];
        for(index = 0;index < all;++index){
            sscanf(path[index],"%s %*s %s %*s %s",devname,position,type);
            int i = statfs(position,&sfs);
            int per;
            if(sfs.f_blocks == 0){
                continue;
            }
            ui->tableWidget_2->insertRow(0);
            per = 100 *(sfs.f_blocks - sfs.f_bfree)/ (sfs.f_blocks)+1;
            QTableWidgetItem * p = new QTableWidgetItem;
            QTableWidgetItem * all = new QTableWidgetItem;
            QTableWidgetItem * used = new QTableWidgetItem;
            QTableWidgetItem * free = new QTableWidgetItem;

            p->setData(Qt::EditRole,QVariant(per));
           //all
            if(4 * sfs.f_blocks < 1024){
               ui->tableWidget_2->setItem(0,3,new QTableWidgetItem(QString("%1KB").arg(4 * sfs.f_blocks )));
           }
           else if(4 * sfs.f_blocks >= 1024 && 4 * sfs.f_blocks < 1024 * 1024){
               ui->tableWidget_2->setItem(0,3,new QTableWidgetItem(QString("%1MB").arg(4 * sfs.f_blocks /1024)));
           }
           else{
               ui->tableWidget_2->setItem(0,3,new QTableWidgetItem(QString("%1GB").arg(4 * sfs.f_blocks /1024/1024)));
           }
            //used
           if(4 * (sfs.f_blocks-sfs.f_bfree) < 1024){
               ui->tableWidget_2->setItem(0,4,new QTableWidgetItem(QString("%1KB").arg(4 * (sfs.f_blocks-sfs.f_bfree) )));
           }
           else if(4 * (sfs.f_blocks -sfs.f_bfree) >= 1024 && 4 *(sfs.f_blocks - sfs.f_bfree) < 1024 * 1024){
               ui->tableWidget_2->setItem(0,4,new QTableWidgetItem(QString("%1MB").arg(4 * (sfs.f_blocks-sfs.f_bfree) /1024)));
           }
           else{
               ui->tableWidget_2->setItem(0,4,new QTableWidgetItem(QString("%1GB").arg(4 * (sfs.f_blocks-sfs.f_bfree) /1024/1024)));
           }
           //available
           if(4 * sfs.f_bavail < 1024){

              ui->tableWidget_2->setItem(0,5,new QTableWidgetItem(QString("%1KB").arg(4 * sfs.f_bavail)));

           }
           else if(4 * sfs.f_bavail >= 1024 && 4 * sfs.f_bavail < 1024 * 1024){
               ui->tableWidget_2->setItem(0,5,new QTableWidgetItem(QString("%1MB").arg(4 * sfs.f_bavail /1024)));
           }
           else{
               ui->tableWidget_2->setItem(0,5,new QTableWidgetItem(QString("%1GB").arg(4 * sfs.f_bavail/1024/1024)));
           }
           QProgressBar * progress = new QProgressBar;
           progress->setValue(per);
           QTableWidgetItem * itm = new QTableWidgetItem;
           ui->tableWidget_2->setCellWidget(0,6,progress);
           ui->tableWidget_2->setItem(0,0,new QTableWidgetItem(devname));
           ui->tableWidget_2->setItem(0,1,new QTableWidgetItem(position));
           ui->tableWidget_2->setItem(0,2,new QTableWidgetItem(type));


        }
}

void MainWindow::Test(){
    qDebug()<<"Test";
}

void MainWindow::TableSort(int column){
    int order = ui->tableWidget->horizontalHeader()->sortIndicatorOrder();
    switch (column) {
    case 0:
//name
        if(order){
            ui->tableWidget->sortByColumn(0, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(0,Qt::AscendingOrder);
        }
        break;
    case 1:
//%cpu
        if(order){
            ui->tableWidget->sortByColumn(1, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(1,Qt::AscendingOrder);
        }
        break;

    case 2:
//user
        if(order){
            ui->tableWidget->sortByColumn(2, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(2,Qt::AscendingOrder);
        }
        break;
    case 3:
//id
        if(order){
            ui->tableWidget->sortByColumn(3, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(3,Qt::AscendingOrder);
        }
        break;
    case 4:
//mem
        if(order){
            ui->tableWidget->sortByColumn(4, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(4,Qt::AscendingOrder);
        }
        break;
    case 5:
//status
        if(order){
            ui->tableWidget->sortByColumn(5, Qt::DescendingOrder );
        }
        else{
            ui->tableWidget->sortByColumn(5,Qt::AscendingOrder);
        }
        break;
    }
}

int MainWindow::GetMem(){
    FILE * fd;
    fd = fopen("/proc/meminfo","r");
    int total;
    char name[50];
    if(fd == 0)qDebug() <<"meminfo open error";
    char buffer[1024];
   fgets(buffer,1024,fd);
   sscanf(buffer,"%s %d",name,&total);
   fclose(fd);
   return total;
}
int MainWindow::GetProcess(pid_t p){
    char filename[50];
    sprintf(filename,"/proc/%d/status",p);
    FILE * fd;
    if((fd == fopen(filename,"r")) == 0)qDebug() <<"proc status file open error";
    int vmrss;
    char buffer[256];
    char name[50];
    int phy_mem;
    for(int i= 0;i < 15;++i){
        fgets(buffer,256,fd);
    }
    fgets(buffer,256,fd);
    sscanf(buffer,"%s %d",name,&phy_mem);
    fclose(fd);
    return phy_mem;
}

void MainWindow::SetProcess(){
    int process_count = 0;
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setShowGrid(false);
    QVector<_P> first(65536);
    QVector<_P> second(65536);
    bool ok;
    DIR *dir;
    FILE *fd;
    struct dirent *dirName;
    char stat[2], nice[3];              //分别保存进程当前状态和nice数
    char vmsize[20], rmsize[20];      //进程所用虚拟内存大小以及所占实际物理内存大小
    char processName[50];          //进程名
    char fileName[50];
    int utime;
    int stime;
    int cutime;
    int cstime;
    int utime2;
    int stime2;
    int cutime2;
    int cstime2;
    char pid[20];        
    struct passwd * pwd = new struct passwd;
    int index = ui->comboBox->currentIndex();
    ui->tableWidget->setRowCount(0);

    if((dir = opendir("/proc")) ==NULL)return;
    while((dirName = readdir(dir)) != NULL){
          if(atoi(dirName->d_name)){
              int num = atoi(dirName->d_name);
              sprintf(fileName,"/proc/%s/stat",dirName->d_name);

              if((fd = fopen(fileName,"r") ) ==NULL){
                  qDebug() <<"filename" <<fileName << "\n";
                  perror("ERROR!!");
              }
              fscanf(fd,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%d%d%d%d%*s%*s%*s%*s%*s%*s%*s", &(second[num].utime),&(second[num].stime),&(second[num].cutime),&(second[num].cstime));
                second[num].total = GetCpuTotal();
                fclose(fd);
          }
    }
    sleep(1);
    if((dir = opendir("/proc")) ==NULL)return;
    while((dirName = readdir(dir)) != NULL){
          if(atoi(dirName->d_name)){
              ++process_count;
              //process
              int num = atoi(dirName->d_name);
              sprintf(fileName,"/proc/%s/stat",dirName->d_name);
              if((fd = fopen(fileName,"r") ) ==NULL){
                  perror("ERROR2!");
              }
              fscanf(fd,"%s%s%s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%d%d%d%d%*s%*s%*s%*s%*s%s%s",pid, processName, stat, &(first[num].utime),&(first[num].stime),&(first[num].cutime),&(first[num].cstime),vmsize,rmsize);
              fclose(fd);
              int uid = this->GetUser(atoi(dirName->d_name));
              struct passwd * pwd = new struct passwd;
              pwd = getpwuid(uid);
                if(pwd == NULL){
                    qDebug()<<"DEBUG"<<"\n";
                }
              first[num].total = GetCpuTotal();
              if(index == 1){
                  if(stat[0] != 'R'){
                      continue;
                  }
              }
              else if(index == 2){
                  if(uid != 1000)continue;
              }
              QString name =QString(QLatin1String(processName));
              QString pid_string = QString(QLatin1String(pid));
              ui->tableWidget->insertRow(0);
              ui->tableWidget->setItem(0,2,new QTableWidgetItem(pwd->pw_name));
              ui->tableWidget->setItem(0,0,new QTableWidgetItem(name.mid(1,name.length()-2)));
              QTableWidgetItem * p = new QTableWidgetItem;
              QTableWidgetItem * per = new QTableWidgetItem;
              QTableWidgetItem * mem = new QTableWidgetItem;
              p->setData(Qt::EditRole,QVariant(pid_string.toInt()));
              ui->tableWidget->setItem(0,3,p);

              int all1 = first[num].utime + first[num].cstime + first[num].cutime + first[num].stime;
             int all2 = second[num].utime + second[num].cstime + second[num].cutime + second[num].stime;
             float p_per = 100.0 * (all2 - all1)/ (second[num].total - first[num].total);
             per->setData(Qt::EditRole,QVariant(p_per));
             ui->tableWidget->setItem(0,1,per);
             mem->setData(Qt::EditRole,QVariant(atof(rmsize)/1024));
             ui->tableWidget->setItem(0, 4,mem);
              switch(stat[0])             //根据进程的状态，将其转换为更友好的形式显示
                  {
                      case 'S' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem(tr("Sleep")));
                      break;
                      case 'R' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem("Running"));
                      break;
                      case 'Z' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem("Zombie"));
                      break;
                      case 'D' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem(tr("Disk Sleep")));
                      break;
                      case 'T' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem("Stop"));
                      break;
                      case 'X' :
                      ui->tableWidget->setItem(0, 5, new QTableWidgetItem("Dead"));
                      break;
                      default:
                      break;
                  }

              ui->label->setText(tr("Process number : ") + QString::number(process_count));
          }
    }


}
int MainWindow::GetCpuTotal(){
    FILE *fd;
    char buffer[256];

    fd = fopen("/proc/stat","r");
    if(!fd){
        qDebug()<<"Error";
        return -1;
    }
    fgets(buffer,256,fd);
    int user,nice,sys,idle;
    sscanf(buffer,"%*s %d %d %d %d",&user,&nice,&sys,&idle);
    fclose(fd);
    return user + nice + sys + idle;
}
int MainWindow::GetUser(pid_t p){
    char filename[50];
    sprintf(filename,"/proc/%d/status",p);
    FILE * fd;
    if((fd = fopen(filename,"r")) == 0)qDebug() <<"user status file open error";
    int vmrss;
    char buffer[256];
    char name[50];
    int uid;
    for(int i= 0;i < 7;++i){
        fgets(buffer,256,fd);
    }
    fgets(buffer,256,fd);
    sscanf(buffer,"%s %d",name,&uid);
    fclose(fd);
    return uid;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    SetProcess();
}


void MainWindow::on_pushButton_clicked()
{
    if(ui->tableWidget->currentItem() == NULL)return;
    int row = ui->tableWidget->currentRow();
    int column = 3;
    int pid = ui->tableWidget->item(row,column)->text().toInt();
   QMessageBox::StandardButton rb = QMessageBox::question(this,"Kill Process","Are you sure to kill this Process",QMessageBox::Yes | QMessageBox::No ,QMessageBox::No );
    if(rb == QMessageBox::Yes){
        int retval = kill(pid,SIGKILL);
        sleep(1);
        SetProcess();
    }
}
