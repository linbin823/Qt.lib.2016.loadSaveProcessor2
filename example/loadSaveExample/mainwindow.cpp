#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    processor = new loadSaveProcessorXml(this,false);
    manager = new managerExample(this);

//    connect(processor,SIGNAL(msgStateChanged(quint64)),this,SLOT(setState(quint64)));
//    connect(processor,SIGNAL(msgErrorSet(quint64)),this,SLOT(setState(quint64)));
//    connect(processor,SIGNAL(msgErrorReset(quint64)),this,SLOT(setState(quint64)));
    setState(0);
    updateDisplay();
}

MainWindow::~MainWindow()
{
    delete processor;
    delete manager;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString ret = QFileDialog::getSaveFileName(this, tr("Save File"),
                                               QDir::currentPath(),
                                               tr("config Fille (*.*)"));
    if(ret == QString::null) return;
    if( processor->transactionStart() )
        return;

//    processor->createNewInstance( QString("managerExample"), QString::number( 1 ) );
    processor->moveToInstance(    QString("managerExample"), QString::number( 1 ) );
    manager->save(processor);
    processor->moveBackToParent();
    processor->saveFile( ret );
    processor->transactionEnd();

    updateDisplay();
}

void MainWindow::setState(quint64 state){
//    ui->error->setText( processor->getErrorStringList().join(";"));
//    ui->state->setText( processor->getStateString());
}

void MainWindow::on_PB_load_clicked()
{

    QString ret = QFileDialog::getOpenFileName(this, tr("Load File"),
                                               QDir::currentPath(),
                                               tr("config Fille (*.*)"));
    if(ret == QString::null) return;
    if( processor->transactionStart() )
        return;
    processor->loadFile( ret );
    processor->moveToInstance( QString("managerExample"), QString::number( 1 ) );
    manager->load( processor );
    processor->moveBackToParent();
    processor->transactionEnd();
    updateDisplay();
}

void MainWindow::updateDisplay(){
    ui->Mpara1->setText( QString::number(manager->para1 ) );
    ui->Mpara2->setText( manager->para2 );
    ui->Mpara3->setText( QString::number(manager->para3 ) );

    ui->Dpara1->setText( QString::number(manager->pDeviceList.at(0)->para1 ) );
    ui->Dpara2->setText( manager->pDeviceList.at(0)->para2 );
    ui->Dpara3->setText( QString::number(manager->pDeviceList.at(0)->para3 ) );
}
