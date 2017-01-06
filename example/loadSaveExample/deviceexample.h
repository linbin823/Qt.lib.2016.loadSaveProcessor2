#ifndef DEVICEEXAMPLE_H
#define DEVICEEXAMPLE_H

#include "iloadsaveprocessor.h"
#include "iloadsave.h"

class deviceExample : public QObject, public iLoadSave
{
public:
    deviceExample(QObject *parent = 0);


    virtual void load(iLoadSaveProcessor *processor);
    virtual void save(iLoadSaveProcessor *processor);

    int para1;
    QString para2;
    float para3;

};

#endif // DEVICEEXAMPLE_H
