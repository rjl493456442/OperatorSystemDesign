#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int GetMem();
    int GetProcess(pid_t p);
    int GetCpuTotal();
    int GetUser(pid_t p);
    int SetFileSys();
    int SetMem();
    int SetCore();

public slots:
    void SetProcess();
    void ChangeSys();
private slots:
    void on_pushButton_2_clicked();
    void ChangeProcess();
    void Test();
    void TableSort(int);
    void SetCoreLabel();
    void SetMemLabel();
    void ChangeGraph();
    void ChangeMemGraph();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
typedef struct  Pstruct{
    int utime;
    int stime;
    int cutime;
    int cstime;
   int total;
}_P;
typedef struct  Gstruct{
    long user[4];
    long nice[4];
    long sys[4];
    long idle[4];
}_G;
typedef struct  Mstrutt{
 long total;
 long buffer;
 long free;
 long cache;
}_M;
#endif // MAINWINDOW_H
