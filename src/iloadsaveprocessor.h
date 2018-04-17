#ifndef ILOADSAVEPROCESSOR
#define ILOADSAVEPROCESSOR

#include <QObject>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QDebug>
#include <QByteArray>
#include <typeinfo>
#include <QHostAddress>
#include "qaeswrap.h"

//not reenterable
//not thread safe

class iLoadSaveProcessor
{
public:
    iLoadSaveProcessor(){}
    virtual ~iLoadSaveProcessor(){}
    //读写实例内的参数
    //参数1：读：参数的名称 写：参数的名称
    //参数2：读：返回参数值 写：传入参数值
    virtual QString readParameters(const QString&& paraName) = 0;
    virtual int writeParameters(const QString&& paraName, const QString&& value) = 0;

    //子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（readParameters） c、返回父实例（MoveBackToParent）
    //子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（writeParameters） d、返回父实例（MoveBackToParent）
    //参数1：ObjType 一般为类的名字
    //参数2：InstID实例标识符，一般为实例的序号
    virtual int moveToInstance(const QString&& ObjType, const QString&& InstID) = 0;
    virtual int moveBackToParent() = 0;

    //get current object's ObjType and InstID
    virtual QString currentObjType()const = 0;
    virtual QString currentInstID()const = 0;

    //get all sub-object's InstIDs of a certain objType
    virtual QStringList instIDs(const QString&& ObjType)const = 0;
    //get all sub-object's ObjTypes
    virtual QStringList objTypes()const = 0;

    //get all parametersName
    virtual QStringList parametersName()const = 0;

    //实施保存工作流程：a、init the processor b、root instance save parameters to processor c、save File/ByteArray
    //实施读取工作流程：a、load File/ByteArray b、root instance load parameters from processor
    virtual int init() =0;
    virtual int loadFile(const QString& fileName = NULL) =0;
    virtual int saveFile(const QString& fileName = NULL) =0;
    virtual int saveByteArray(QByteArray& result) = 0;
    virtual int loadByteArray(const QByteArray& source) =0;

    //修改获取保存密码
    virtual void setPassWord(const QString&& pswd) = 0;

    //easy api, do not work for enum type and QVariant type
    //readValue combined
    template<typename T>
    int readValue(const QString&& paraName, T & retVal){
        QString rawVal = readParameters( QString(paraName) );

        const static size_t __intID = typeid (int).hash_code();
        const static size_t __uintID = typeid (unsigned int).hash_code();
        const static size_t __longID = typeid (long).hash_code();
        const static size_t __ulongID = typeid (unsigned long).hash_code();
        const static size_t __boolID = typeid (bool).hash_code();
        const static size_t __doubleID = typeid (double).hash_code();
        const static size_t __floatID = typeid (float).hash_code();
        const static size_t __llID = typeid (long long).hash_code();
        const static size_t __ullID = typeid (unsigned long long).hash_code();
        const static size_t __QCharID = typeid (QChar).hash_code();
        const static size_t __QStringID = typeid (QString).hash_code();
        const static size_t __QDateTimeID = typeid (QDateTime).hash_code();
        const static size_t __QDateID = typeid (QDate).hash_code();
        const static size_t __QTimeID = typeid (QTime).hash_code();
        const static size_t __QByteArrayID = typeid (QByteArray).hash_code();
        const static size_t __QHostAddress = typeid (QHostAddress).hash_code();

        if(true == rawVal.isNull() ){
            //dont touch retVal
            qDebug() << "unwrap value error: Null value. retrun variable is not touched!";
            return -1;
        }
        bool ok;
        const size_t id = (typeid (retVal).hash_code());
        T *p = &retVal;
        if(id == __intID)
        {
            *(int *)p = rawVal.toInt(&ok);
            return 0;
        }
        else if(id == __uintID)
        {
            *(unsigned int *)p = rawVal.toUInt(&ok);
            return 0;
        }
        else if(id == __longID)
        {
            *(long *)p = rawVal.toLong(&ok);
            return 0;
        }
        else if(id == __ulongID)
        {
            *(unsigned long *)p = rawVal.toULong(&ok);
            return 0;
        }
        else if(id == __boolID)
        {
            *(bool *)p = rawVal.toInt(&ok);
            return 0;
        }
        else if(id == __doubleID)
        {
            *(double *)p = rawVal.toDouble(&ok);
            return 0;
        }
        else if(id == __floatID)
        {
            *(float *)p = rawVal.toFloat(&ok);
            return 0;
        }
        else if(id == __llID)
        {
            *(long long *)p = rawVal.toLongLong(&ok);
            return 0;
        }
        else if(id == __ullID)
        {
            *(unsigned long long *)p = rawVal.toULongLong(&ok);
            return 0;
        }
        else if(id == __QCharID)
        {
            *(QChar *)p = rawVal.at(0);
            return 0;
        }
        else if(id == __QStringID)
        {
            *(QString *)p = rawVal;
            return 0;
        }
        else if(id == __QDateTimeID)
        {
            *(QDateTime *)p = QDateTime::fromString( rawVal );
            return 0;
        }
        else if(id == __QDateID)
        {
            *(QDate *)p = QDate::fromString( rawVal );
            return 0;
        }
        else if(id == __QTimeID)
        {
            *(QTime *)p = QTime::fromString( rawVal );
            return 0;
        }
        else if(id == __QByteArrayID)
        {
            *(QByteArray *)p = QByteArray::fromBase64( rawVal.toUtf8() );
            return 0;
        }
        else if(id == __QHostAddress)
        {
            *(QHostAddress *)p = QHostAddress(rawVal);
            return 0;
        }
        qDebug() << "unwrap value error: unknow type:"<<id;
        return -2;
    }

    //writeValue combined
    template<typename T>
    int writeValue(const QString&& paraName,const T & rawVal ){
        const static size_t __intID = typeid (int).hash_code();
        const static size_t __uintID = typeid (unsigned int).hash_code();
        const static size_t __longID = typeid (long).hash_code();
        const static size_t __ulongID = typeid (unsigned long).hash_code();
        const static size_t __boolID = typeid (bool).hash_code();
        const static size_t __doubleID = typeid (double).hash_code();
        const static size_t __floatID = typeid (float).hash_code();
        const static size_t __llID = typeid (long long).hash_code();
        const static size_t __ullID = typeid (unsigned long long).hash_code();
        const static size_t __QCharID = typeid (QChar).hash_code();
        const static size_t __QStringID = typeid (QString).hash_code();
        const static size_t __QDateTimeID = typeid (QDateTime).hash_code();
        const static size_t __QDateID = typeid (QDate).hash_code();
        const static size_t __QTimeID = typeid (QTime).hash_code();
        const static size_t __QByteArrayID = typeid (QByteArray).hash_code();
        const static size_t __QHostAddress = typeid (QHostAddress).hash_code();

        const size_t id = (typeid (rawVal).hash_code());
        T value = rawVal;
        void *p = &value;

        if(id == __intID)
        {
            return writeParameters(QString(paraName), QString::number( *(int*) p ) );
        }
        else if(id == __uintID)
        {
            return writeParameters(QString(paraName), QString::number( *(unsigned int*) p ) );
        }
        else if(id == __longID)
        {
            return writeParameters(QString(paraName), QString::number( *(long*) p ) );
        }
        else if(id == __ulongID)
        {
            return writeParameters(QString(paraName), QString::number( *(unsigned long*) p ) );
        }
        else if(id == __boolID)
        {
            return writeParameters(QString(paraName), QString::number( *(bool*) p ) );
        }
        else if(id == __doubleID)
        {
            return writeParameters(QString(paraName), QString::number( *(double*) p ) );
        }
        else if(id == __floatID)
        {
            return writeParameters(QString(paraName), QString::number( *(float*) p ) );
        }
        else if(id == __llID)
        {
            return writeParameters(QString(paraName), QString::number( *(long long*) p ) );
        }
        else if(id == __ullID)
        {
            return writeParameters(QString(paraName), QString::number( *(unsigned long long*) p ) );
        }
        else if(id == __QCharID)
        {
            return writeParameters(QString(paraName), QString( *(QChar*) p ) );
        }
        else if(id == __QStringID)
        {
            return writeParameters(QString(paraName),  QString(*(QString*) p) );
        }
        else if(id == __QDateTimeID)
        {
            return writeParameters(QString(paraName), ((QDateTime*)p)->toString() );
        }
        else if(id == __QDateID)
        {
            return writeParameters(QString(paraName), ((QDate*) p)->toString() );
        }
        else if(id == __QTimeID)
        {
            return writeParameters(QString(paraName), ((QTime*) p)->toString() );
        }
        else if(id == __QByteArrayID)
        {
            return writeParameters(QString(paraName), QString( ((QByteArray*) p)->toBase64() ) );
        }
        else if(id == __QHostAddress)
        {
            return writeParameters(QString(paraName), ((QHostAddress*)p)->toString() );
        }
        qDebug() << "wrap value error: unknow type:"<<id;
        return -1;
    }
};



#endif // ILOADSAVEPROCESSOR
