#include "deviceexample.h"

deviceExample::deviceExample(QObject *parent):baseDevice(parent)
{

    para1 = 12;
    para2 = "sdsd";
    para3 = 23.2314;
}


int deviceExample::load(iLoadSaveProcessor *processor){
    QString value;
    processor->loadParameters( QString("para1") , &value );
    para1 = value.toInt();
    processor->loadParameters( QString("para2") , &value );
    para2 = value;
    processor->loadParameters( QString("para3") , &value );
    para3 = value.toFloat();
    return 0;
}

int deviceExample::save(iLoadSaveProcessor *processor){
    int ret;
    ret = processor->saveParameters( QString("para1"), QString::number( para1 ) );
    if(ret < 0) return -1;
    ret = processor->saveParameters( QString("para2"), para2 );
    if(ret < 0) return -1;
    ret = processor->saveParameters( QString("para3"), QString::number( para3 ) );
    return 0;
}
