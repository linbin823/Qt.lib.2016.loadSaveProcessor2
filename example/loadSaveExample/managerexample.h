#ifndef MANAGEREXAMPLE_H
#define MANAGEREXAMPLE_H

#include "iloadsaveprocessor.h"
#include "basedevice.h"
#include "deviceexample.h"


class managerExample: public baseDevice
{
    Q_OBJECT
public:
    managerExample(QObject *parent = 0);

    virtual int load(iLoadSaveProcessor *processor);
    virtual int save(iLoadSaveProcessor *processor);

    virtual QString getStateString(int lang) const{return QString::null;}


    int para1;
    QString para2;
    float para3;

    QList<deviceExample*> pDeviceList;

protected:
    virtual QString getErrorString(quint64 errorCode, int lang) const{return QString::null;}

};

#endif // MANAGEREXAMPLE_H
