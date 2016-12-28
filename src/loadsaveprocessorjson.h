#ifndef LOADSAVEPROCESSORJSON_H
#define LOADSAVEPROCESSORJSON_H

#include <QDebug>
#include <QList>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include "iloadsaveprocessor.h"


class loadSaveProcessorJson :  public QObject , public iLoadSaveProcessor
{
    Q_OBJECT

public:
    loadSaveProcessorJson(QObject *parent = 0, bool encrypt = false);
    virtual ~loadSaveProcessorJson();

    Q_PROPERTY(QString JsonFilePath READ getJsonFilePath WRITE setJsonFilePath NOTIFY jsonFilePathChanged)

//start to implement iLoadSaveProcessor
    //读写实例内的参数
    //参数1：读：参数的名称 写：参数的名称
    //参数2：读：返回参数值 写：传入参数值
    virtual QString loadParameters(const QString&& paraName);
    virtual int saveParameters(const QString&& paraName, const QString&& value);

    //子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
    //子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
    //参数1：ObjType 一般为类的名字
    //参数2：InstID实例标识符，一般为实例的序号
    virtual int moveToInstance(const QString&& ObjType, const QString&& InstID);
    virtual int moveBackToParent();

    //实施保存工作流程：a、init the processor b、root instance save parameters to processor c、save File/ByteArray
    //实施读取工作流程：a、load File/ByteArray b、root instance load parameters from processor
    virtual int init();
    virtual int loadFile(const QString& fileName = NULL);
    virtual int saveFile(const QString& fileName = NULL);
    virtual int saveByteArray(QByteArray& result);
    virtual int loadByteArray(const QByteArray& source);


    //修改获取保存密码
    virtual void setPassWord(const QString&& pswd){
        _password = pswd;
        delete _aes;
        _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);
    }
//finished to implement iLoadSaveProcessor


    void    setJsonFilePath(const QString &name);
    QString getJsonFilePath(void);


private:
    QJsonObject* getParent();
    QString getPathName();
    int popParent();
    int pushParent(const QString&& name);

    //is JsonParent valid
    bool isValid();

    int readJsonFile();
    int writeJsonFile();

    void debugPrint(QJsonObject* obj);

private:
    bool _needEncrypt;
    QString _password;
    QString _salt;
    QAesWrap* _aes;
    QString _JsonFilePath;//for QML
    QString _JsonFilePathWithoutProtocol;//for C++
    QList<QJsonObject*> _JsonParent;//json parent objects
    QList<QString> _JsonPathName;//json descent path name

signals:
    void jsonRefresh         (void);
    void jsonFilePathChanged (void);
};

#endif // LOADSAVEPROCESSORJSON_H
