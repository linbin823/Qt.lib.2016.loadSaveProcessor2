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

private slots:
    void on_pushButton_clicked();
    void setState(quint64 state);
    void on_PB_load_clicked();
    void updateDisplay();

private:
    Ui::MainWindow *ui;
    iLoadSaveProcessor* processor;
    managerExample* manager;

};

#endif // MAINWINDOW_H
