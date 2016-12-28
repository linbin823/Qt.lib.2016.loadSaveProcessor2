#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    processor = new loadSaveProcessorXml(this,false);
    manager = new managerExample(this);
    getValues();
}

MainWindow::~MainWindow()
{
    delete processor;
    delete manager;
    delete ui;
}

void MainWindow::on_PB_load_clicked()
{

    QString ret = QFileDialog::getOpenFileName(this, tr("Load File"),
                                               QDir::currentPath(),
                                               tr("config Fille (*.*)"));
    if(ret == QString::null) return;
    processor->loadFile( ret );
    processor->moveToInstance( QString("managerExample"), QString::number( 1 ) );
    manager->load( processor );
    processor->init();

    getValues();
}

void MainWindow::on_PB_Save_clicked()
{
    QString ret = QFileDialog::getSaveFileName(this, tr("Save File"),
                                               QDir::currentPath(),
                                               tr("config Fille (*.*)"));
    if(ret == QString::null) return;

    processor->init();
    processor->moveToInstance(    QString("managerExample"), QString::number( 1 ) );
    manager->save(processor);
    processor->moveBackToParent();
    processor->saveFile( ret );

    setValues();
}

void MainWindow::setValues(){
    manager->para1 = ui->Mpara1->text().toInt();
    manager->para2 = ui->Mpara2->text();
    manager->para3 = ui->Mpara1->text().toFloat();

    manager->pDeviceList.at(0)->para1 = ui->D1para1->text().toInt();
    manager->pDeviceList.at(0)->para2 = ui->D1para2->text();
    manager->pDeviceList.at(0)->para3 = ui->D1para3->text().toFloat();

    manager->pDeviceList.at(1)->para1 = ui->D2para1->text().toInt();
    manager->pDeviceList.at(1)->para2 = ui->D2para2->text();
    manager->pDeviceList.at(1)->para3 = ui->D2para3->text().toFloat();

    manager->pDeviceList.at(2)->para1 = ui->D3para1->text().toInt();
    manager->pDeviceList.at(2)->para2 = ui->D3para2->text();
    manager->pDeviceList.at(2)->para3 = ui->D3para3->text().toFloat();

    manager->pDeviceList.at(3)->para1 = ui->D4para1->text().toInt();
    manager->pDeviceList.at(3)->para2 = ui->D4para2->text();
    manager->pDeviceList.at(3)->para3 = ui->D4para3->text().toFloat();

    manager->pDeviceList.at(4)->para1 = ui->D5para1->text().toInt();
    manager->pDeviceList.at(4)->para2 = ui->D5para2->text();
    manager->pDeviceList.at(4)->para3 = ui->D5para3->text().toFloat();

    manager->pDeviceList.at(5)->para1 = ui->D6para1->text().toInt();
    manager->pDeviceList.at(5)->para2 = ui->D6para2->text();
    manager->pDeviceList.at(5)->para3 = ui->D6para3->text().toFloat();
}

void MainWindow::getValues(){
    ui->Mpara1->setText( QString::number(manager->para1) );
    ui->Mpara2->setText( manager->para2 );
    ui->Mpara3->setText( QString::number(manager->para3) );

    ui->D1para1->setText( QString::number(manager->pDeviceList.at(0)->para1) );
    ui->D1para2->setText( manager->pDeviceList.at(0)->para2 );
    ui->D1para3->setText( QString::number(manager->pDeviceList.at(0)->para3) );

    ui->D2para1->setText( QString::number(manager->pDeviceList.at(1)->para1) );
    ui->D2para2->setText( manager->pDeviceList.at(1)->para2 );
    ui->D2para3->setText( QString::number(manager->pDeviceList.at(1)->para3) );

    ui->D3para1->setText( QString::number(manager->pDeviceList.at(2)->para1) );
    ui->D3para2->setText( manager->pDeviceList.at(2)->para2 );
    ui->D3para3->setText( QString::number(manager->pDeviceList.at(2)->para3) );

    ui->D4para1->setText( QString::number(manager->pDeviceList.at(3)->para1) );
    ui->D4para2->setText( manager->pDeviceList.at(3)->para2 );
    ui->D4para3->setText( QString::number(manager->pDeviceList.at(3)->para3) );

    ui->D5para1->setText( QString::number(manager->pDeviceList.at(4)->para1) );
    ui->D5para2->setText( manager->pDeviceList.at(4)->para2 );
    ui->D5para3->setText( QString::number(manager->pDeviceList.at(5)->para3) );

    ui->D6para1->setText( QString::number(manager->pDeviceList.at(5)->para1) );
    ui->D6para2->setText( manager->pDeviceList.at(5)->para2 );
    ui->D6para3->setText( QString::number(manager->pDeviceList.at(5)->para3) );
}
