﻿#include "managerexample.h"

managerExample::managerExample(QObject *parent ):QObject(parent)
{
    for(int i=0; i<=5; i++){
        deviceExample* temp = new deviceExample(this);
        pDeviceList.append(temp);
    }
    para1 = 12;
    para2 = "sdsd";
    para3 = 23.2314;
}

int managerExample::load(iLoadSaveProcessor *processor){
    iLoadSaveProcessor::unwrapVal(  processor->loadParameters( "para1"), para1 );
    iLoadSaveProcessor::unwrapVal(  processor->loadParameters( "para2"), para2 );
    iLoadSaveProcessor::unwrapVal(  processor->loadParameters( "para3"), para3 );
    //qDebug()<<value<<para3;

    for(int i=0; i<=5; i++){
        int ret = processor->moveToInstance( QString("deviceExample") , QString::number( i ) );
        if(ret != 0) continue;
        pDeviceList.at(i)->load( processor );
        processor->moveBackToParent();
    }
    return 0;

}

int managerExample::save(iLoadSaveProcessor *processor){
    //qDebug()<<"managerExample::save"<<QString("para1")<<QString::number( para1 );
    processor->saveParameters( "para1", iLoadSaveProcessor::wrapVal( para1 ) );
    processor->saveParameters( "para2", iLoadSaveProcessor::wrapVal( para2 ) );
    processor->saveParameters( "para3", iLoadSaveProcessor::wrapVal( para3 ) );
    int ret;
    for(int i=0; i<=5; i++){
//        ret = processor->createNewInstance( QString("deviceExample") , QString::number( i ) );
//        if(ret < 0) continue;
        ret = processor->moveToInstance(    QString("deviceExample") , QString::number( i ) );
        if(ret < 0) continue;
        ret = pDeviceList.at(i)->save( processor );
        if(ret < 0) continue;
        processor->moveBackToParent();
    }
    return 0;
}
