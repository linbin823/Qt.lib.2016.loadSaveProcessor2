#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "loadsaveprocessorxml.h"
#include "loadsaveprocessorjson.h"
#include "managerexample.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setValues();
    void getValues();

private slots:

    void on_PB_load_clicked();
    void on_PB_Save_clicked();

private:
    Ui::MainWindow *ui;
    iLoadSaveProcessor* processor;
    managerExample* manager;

};

#endif // MAINWINDOW_H
