#include "deviceexample.h"

deviceExample::deviceExample(QObject *parent):QObject(parent)
{

    para1 = 12;
    para2 = "sdsd";
    para3 = 23.2314;
}


int deviceExample::load(iLoadSaveProcessor *processor){
    iLoadSaveProcessor::unwrapVal( processor->loadParameters( "para1" ) , para1 );
    iLoadSaveProcessor::unwrapVal( processor->loadParameters( "para2" ) , para2 );
    iLoadSaveProcessor::unwrapVal( processor->loadParameters( "para3" ) , para3 );
    return 0;
}

int deviceExample::save(iLoadSaveProcessor *processor){
    processor->saveParameters( "para1", iLoadSaveProcessor::wrapVal( para1 ) );
    processor->saveParameters( "para2", iLoadSaveProcessor::wrapVal( para2 ) );
    processor->saveParameters( "para3", iLoadSaveProcessor::wrapVal( para3 ) );
    return 0;
}
