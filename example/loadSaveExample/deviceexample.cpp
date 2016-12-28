#include "deviceexample.h"

deviceExample::deviceExample(QObject *parent):QObject(parent)
{

    para1 = 12;
    para2 = "sdsd";
    para3 = 23.2314;
}


int deviceExample::load(iLoadSaveProcessor *processor){
    processor->unwrapVal( processor->loadParameters( "para1" ) , para1 );
    processor->unwrapVal( processor->loadParameters( "para2" ) , para2 );
    processor->unwrapVal( processor->loadParameters( "para3" ) , para3 );
    return 0;
}

int deviceExample::save(iLoadSaveProcessor *processor){
    processor->saveParameters( "para1", processor->wrapVal( para1 ) );
    processor->saveParameters( "para2", processor->wrapVal( para2 ) );
    processor->saveParameters( "para3", processor->wrapVal( para3 ) );
    return 0;
}
