#ifndef MANAGEREXAMPLE_H
#define MANAGEREXAMPLE_H

#include "iloadsaveprocessor.h"
#include "iloadsave.h"
#include "deviceexample.h"


class managerExample : public QObject, public iLoadSave
{
    Q_OBJECT
public:
    managerExample(QObject *parent = 0);

    virtual void load(iLoadSaveProcessor *processor);
    virtual void save(iLoadSaveProcessor *processor);

    int para1;
    QString para2;
    float para3;

    QList<deviceExample*> pDeviceList;

};

#endif // MANAGEREXAMPLE_H
