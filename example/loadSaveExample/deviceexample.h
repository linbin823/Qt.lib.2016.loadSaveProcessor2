#ifndef DEVICEEXAMPLE_H
#define DEVICEEXAMPLE_H

#include "iloadsaveprocessor.h"
#include "basedevice.h"

class deviceExample : public baseDevice
{
public:
    deviceExample(QObject *parent = 0);


    virtual int load(iLoadSaveProcessor *processor);
    virtual int save(iLoadSaveProcessor *processor);

    virtual QString getStateString(int lang) const{return QString::null;}

    int para1;
    QString para2;
    float para3;

protected:
    virtual QString getErrorString(quint64 errorCode, int lang) const{return QString::null;}

};

#endif // DEVICEEXAMPLE_H
