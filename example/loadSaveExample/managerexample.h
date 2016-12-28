#ifndef MANAGEREXAMPLE_H
#define MANAGEREXAMPLE_H

#include "iloadsaveprocessor.h"
#include "deviceexample.h"


class managerExample : public QObject
{
    Q_OBJECT
public:
    managerExample(QObject *parent = 0);

    virtual int load(iLoadSaveProcessor *processor);
    virtual int save(iLoadSaveProcessor *processor);

    int para1;
    QString para2;
    float para3;

    QList<deviceExample*> pDeviceList;

};

#endif // MANAGEREXAMPLE_H
