#ifndef DEVICEEXAMPLE_H
#define DEVICEEXAMPLE_H

#include "iloadsaveprocessor.h"

class deviceExample : public QObject
{
public:
    deviceExample(QObject *parent = 0);


    int load(iLoadSaveProcessor *processor);
    int save(iLoadSaveProcessor *processor);

    int para1;
    QString para2;
    float para3;

};

#endif // DEVICEEXAMPLE_H
