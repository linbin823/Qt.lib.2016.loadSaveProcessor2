#ifndef LOADSAVEPROCESSORXML_H
#define LOADSAVEPROCESSORXML_H

#include <QDebug>
#include <QList>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QCoreApplication>
#include <QDir>
#include <QtXmlPatterns>
#include <QFileInfo>
#include "iloadsaveprocessor.h"
#include "basedevice.h"
#include "qaeswrap.h"

class loadSaveProcessorXml :  public baseDevice, public iLoadSaveProcessor
{
    Q_OBJECT
public:
    loadSaveProcessorXml(QObject *parent = 0, bool encrypt = false);
    ~loadSaveProcessorXml();

    Q_PROPERTY(QString resXmlFilePath READ getResXmlFilePath WRITE setResXmlFilePath NOTIFY resXmlFilePathChanged)

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
     }
    enum errorType : quint64 {
        errorFlieFomatWrong              = 0x0000000000000001ULL,       //xml文件格式错误
        errorFileNotFound                = 0x0000000000000002ULL,       //xml文件找不到，新建
        errorFileOpenFail                = 0x0000000000000004ULL,       //xml文件打开失败
        errorFileWriteFail               = 0x0000000000000008ULL,       //xml文件写入失败
        errorLoadFail                    = 0x0000000000000010ULL,       //读取时遇到错误
        errorSaveFail                    = 0x0000000000000020ULL,       //保存时遇到错误
        errorFileNameError               = 0x0000000000000040ULL,       //xml文件名称错误
    };
protected:
    virtual QString getErrorString(quint64 errorCode, int lang = langCHN ) const{
        switch(lang){
        case langCHN:
            switch(errorCode){
                case errorFlieFomatWrong:
                    return tr("xml文件格式错误");
                case errorFileNotFound:
                    return tr("xml文件找不到，新建");
                case errorFileOpenFail:
                    return tr("xml文件打开失败");
                case errorFileWriteFail:
                    return tr("xml文件写入失败");
                case errorLoadFail:
                    return tr("读取时遇到错误");
                case errorSaveFail:
                    return tr("保存时遇到错误");
                case errorFileNameError:
                    return tr("xml文件名称错误");
                default:
                    return tr("数据错误");
            }
        case langEN:
            switch(errorCode){
                case errorFlieFomatWrong:
                    return QString("xml format error");
                case errorFileNotFound:
                    return QString("file not found, new");
                case errorFileOpenFail:
                    return QString("file open fail");
                case errorFileWriteFail:
                    return QString("file write fail");
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
    }

public:

    virtual int loadParameters(const QString& paraName, QString *paraValue);
    virtual int saveParameters(const QString& paraName, const QString & paraValue);
    virtual int moveToInstance(const QString& ObjType, const QString &index);
    virtual int createNewInstance(const QString & ObjType, const QString & InstID);
    virtual int moveBackToParent();


    //不实现iLoadSave
    virtual int load(iLoadSaveProcessor *processor){return 0;}
    virtual int save(iLoadSaveProcessor *processor){return 0;}

    virtual int transactionStart();
    virtual int transactionEnd();
    virtual int loadFile(const QString& fileName = NULL);
    virtual int saveFile(const QString& fileName = NULL);
    virtual void setPassWord(const QString& pswd){
        _password = pswd;
        delete _aes;
        _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);
    }

    void    setResXmlFilePath(const QString &name);
    QString getResXmlFilePath(void) const;

private:

    bool _needEncrypt;
    QString _password;
    QString _salt;
    QAesWrap* _aes;

    QString _resXmlFilePath;//for QML
    QString _resXmlFilePathWithoutProtocol;//for C++
    QDomDocument _resXml;//DomDoc
    QList<QDomElement> _domElementParentList;//element父目录清单

    int initXmlFile();
    int readXmlFile();
    int writeXmlFile();

    int getElement(QDomElement& res, QString tagName, QString id = QString::null);
    int setElement(QDomElement& res, QString tagName, QString id = QString::null);

    inline QDomElement getParent(){
        if (_domElementParentList.size() ==0 )
            return _resXml.childNodes().at(0).toElement();
        return _domElementParentList.last();
    }
    inline int popParent(){
        if (_domElementParentList.size() ==1 )
            return -1;
        _domElementParentList.removeLast();
        return 0;
    }
    inline int pushParent(QDomElement newone){
        _domElementParentList.append(newone);
        return 0;
    }


signals:
    void resXmlRefresh   (void);
    void resXmlFilePathChanged (void);

};

#endif // loadSaveProcessorXml_H
