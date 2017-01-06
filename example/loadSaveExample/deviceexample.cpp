#include "deviceexample.h"

deviceExample::deviceExample(QObject *parent):QObject(parent)
{

    para1 = 12;
    para2 = "sdsd";
    para3 = 23.2314;
}


void deviceExample::load(iLoadSaveProcessor *processor){

    processor->readValue("para1", para1);
    processor->readValue("para2", para2);
    processor->readValue("para3", para3);
}

void deviceExample::save(iLoadSaveProcessor *processor){
    processor->writeValue( "para1", para1 );
    processor->writeValue( "para2", para2 );
    processor->writeValue( "para3", para3 );
}
