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
#include <QFileInfo>
#include "iloadsaveprocessor.h"

class loadSaveProcessorXml :  public QObject, public iLoadSaveProcessor
{
    Q_OBJECT
public:
    loadSaveProcessorXml(QObject *parent = 0, bool encrypt = false);
    ~loadSaveProcessorXml();

    Q_PROPERTY(QString resXmlFilePath READ getResXmlFilePath WRITE setResXmlFilePath NOTIFY resXmlFilePathChanged)

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

    void    setResXmlFilePath(const QString &name);
    QString getResXmlFilePath(void) const;

private:

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

    //is xmlParent valid
    bool isValid();

private:

    bool _needEncrypt;
    QString _password;
    QString _salt;
    QAesWrap* _aes;

    QString _resXmlFilePath;//for QML
    QString _resXmlFilePathWithoutProtocol;//for C++
    QDomDocument _resXml;//DomDoc
    QList<QDomElement> _domElementParentList;//element父目录清单

signals:
    void resXmlRefresh   (void);
    void resXmlFilePathChanged (void);

};

#endif // loadSaveProcessorXml_H
