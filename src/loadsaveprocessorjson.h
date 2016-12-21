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
#include "basedevice.h"
#include "qaeswrap.h"


class loadSaveProcessorJson :  public baseDevice, public iLoadSaveProcessor
{
    Q_OBJECT

public:
    loadSaveProcessorJson(QObject *parent = 0, bool encrypt = false);
    ~loadSaveProcessorJson();

    Q_PROPERTY(QString JsonFilePath READ getJsonFilePath WRITE setJsonFilePath NOTIFY jsonFilePathChanged)

    enum state: quint64{
        stateNotReady        = 0x0000000000000001ULL,      //Save&Load处理器未就绪
        stateReady           = 0x0000000000000002ULL,      //Save&Load处理器就绪
        stateOccupied        = 0x0000000000000004ULL,      //Save&Load处理器占用中
        stateReadFile        = 0x0000000000000008ULL,      //Save&Load处理器正在读文件
        stateWriteFile       = 0x0000000000000010ULL,      //Save&Load处理器正在写文件
    };
    virtual QString getStateString(int lang = langCHN ) const{
        switch(lang){
        case langCHN:
            switch(getState()){
            case stateNotReady:
                return tr("Save&Load处理器未就绪");
            case stateReady:
                return tr("Save&Load处理器就绪");
            case stateOccupied:
                return tr("Save&Load处理器占用中");
            case stateReadFile:
                return tr("Save&Load处理器正在读文件");
            case stateWriteFile:
                return tr("Save&Load处理器正在写文件");
            default:
                return tr("数据错误");
            }
        case langEN:
            switch(getState()){
            case stateNotReady:
                return QString("Save&Load processor not ready!");
            case stateReady:
                return QString("Save&Load processor ready!");
            case stateOccupied:
                return QString("Save&Load processor occupied");
            case stateReadFile:
                return QString("Save&Load processor reading");
            case stateWriteFile:
                return QString("Save&Load processor writing");
            default:
                return QString("error code not found");
            }
        default:
            return QString::null;
        }
     };
    enum errorType : quint64 {
        errorFlieFomatWrong              = 0x0000000000000001ULL,       //Json文件格式错误
        errorFileNotFound                = 0x0000000000000002ULL,       //Json文件找不到，新建
        errorFileOpenFail                = 0x0000000000000004ULL,       //Json文件打开失败
        errorFileWriteFail               = 0x0000000000000008ULL,       //Json文件写入失败
        errorLoadFail                    = 0x0000000000000010ULL,       //读取时遇到错误
        errorSaveFail                    = 0x0000000000000020ULL,       //保存时遇到错误
        errorFileNameError               = 0x0000000000000040ULL,       //Json文件名称错误
    };
protected:
    virtual QString getErrorString(quint64 errorCode, int lang = langCHN ) const{
        switch(lang){
        case langCHN:
            switch(errorCode){
                case errorFlieFomatWrong:
                    return tr("Json文件格式错误");
                case errorFileNotFound:
                    return tr("Json文件找不到，新建");
                case errorFileOpenFail:
                    return tr("Json文件打开失败");
                case errorFileWriteFail:
                    return tr("Json文件写入失败");
                case errorLoadFail:
                    return tr("读取时遇到错误");
                case errorSaveFail:
                    return tr("保存时遇到错误");
                case errorFileNameError:
                    return tr("Json文件名称错误");
                default:
                    return tr("数据错误");
            }
        case langEN:
            switch(errorCode){
                case errorFlieFomatWrong:
                    return QString("Json format error");
                case errorFileNotFound:
                    return QString("Json file not found, new");
                case errorFileOpenFail:
                    return QString("Json file open fail");
                case errorFileWriteFail:
                    return QString("Json file write fail");
                case errorLoadFail:
                    return QString("error during reading");
                case errorSaveFail:
                    return QString("error during writing");
                case errorFileNameError:
                    return QString("file name error");
                default:
                    return QString("error code not found");
            }
        default:
            return QString::null;
        }
    };
public:

    virtual int loadParameters(const QString& paraName, QString *paraValue);
    virtual int saveParameters(const QString& paraName, const QString& paraValue);

    virtual int moveToInstance(const QString& ObjType, const QString& InstID);
    virtual int createNewInstance(const QString& ObjType, const QString& InstID);
    virtual int moveBackToParent();

    //不实现iLoadSave
    virtual int load(iLoadSaveProcessor *processor){return 0;}
    virtual int save(iLoadSaveProcessor *processor){return 0;}

    virtual int transactionStart();
    virtual int transactionEnd();
    virtual int loadFile(const QString& fileName = QString::null);
    virtual int saveFile(const QString& fileName = QString::null);
    virtual void setPassWord(const QString& pswd){
        _password = pswd;
        delete _aes;
        _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);
    }

    void    setJsonFilePath(const QString &name);
    QString getJsonFilePath(void);


private:
    bool _needEncrypt;
    QString _password;
    QString _salt;
    QAesWrap* _aes;
    QString _JsonFilePath;//for QML
    QString _JsonFilePathWithoutProtocol;//for C++
    QList<QJsonObject*> _JsonParent;//json parent objects
    QList<QString> _JsonPathName;//json descent path name

    QJsonObject* getParent();
    QString getPathName();
    int popParent();
    int pushParent(QString name);

    int readJsonFile();
    int writeJsonFile();

    void debugPrint(QJsonObject* obj);
signals:
    void jsonRefresh         (void);
    void jsonFilePathChanged (void);
};

#endif // LOADSAVEPROCESSORJSON_H
