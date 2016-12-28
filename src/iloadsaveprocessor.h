#ifndef ILOADSAVEPROCESSOR
#define ILOADSAVEPROCESSOR

#include <QObject>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QDebug>
#include <QByteArray>
#include <typeinfo>
#include "qaeswrap.h"
#include "Logger.h"

//not reenterable
//not thread safe

class iLoadSaveProcessor
{
public:
    iLoadSaveProcessor(){}
    virtual ~iLoadSaveProcessor(){}//多态的析构函数必须是virtual的
    //读写实例内的参数
    //参数1：读：参数的名称 写：参数的名称
    //参数2：读：返回参数值 写：传入参数值
    virtual QString loadParameters(const QString&& paraName) = 0;
    virtual int saveParameters(const QString&& paraName, const QString&& value) = 0;

    //子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
    //子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
    //参数1：ObjType 一般为类的名字
    //参数2：InstID实例标识符，一般为实例的序号
    virtual int moveToInstance(const QString&& ObjType, const QString&& InstID) =0;
    virtual int moveBackToParent() =0;

    //实施保存工作流程：a、init the processor b、root instance save parameters to processor c、save File/ByteArray
    //实施读取工作流程：a、load File/ByteArray b、root instance load parameters from processor
    virtual int init() =0;
    virtual int loadFile(const QString& fileName = NULL) =0;
    virtual int saveFile(const QString& fileName = NULL) =0;
    virtual int saveByteArray(QByteArray& result) = 0;
    virtual int loadByteArray(const QByteArray& source) =0;

    //修改获取保存密码
    virtual void setPassWord(const QString&& pswd) = 0;

    //unwrap value from QString
    template<typename T>
    int unwrapVal(const QString&& rawVal, T & retVal){
        const static size_t __intID = typeid (int).hash_code();
        const static size_t __uintID = typeid (unsigned int).hash_code();
        const static size_t __boolID = typeid (bool).hash_code();
        const static size_t __doubleID = typeid (double).hash_code();
        const static size_t __floatID = typeid (float).hash_code();
        const static size_t __llID = typeid (long long).hash_code();
        const static size_t __ullID = typeid (unsigned long long).hash_code();
        const static size_t __QStringID = typeid (QString).hash_code();
        const static size_t __QDateTimeID = typeid (QDateTime).hash_code();
        const static size_t __QDateID = typeid (QDate).hash_code();
        const static size_t __QTimeID = typeid (QTime).hash_code();
        const static size_t __QByteArrayID = typeid (QByteArray).hash_code();

        if(true == rawVal.isNull() ){
            //dont touch retVal
            LOG_DEBUG() << "unwrap value error: Null value. retrun variable is not touched!";
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
            *(QByteArray *)p = QByteArray::fromBase64( rawVal.toLocal8Bit() );
            return 0;
        }
        LOG_DEBUG() << "unwrap value error: unknow type:"<<id;
        return -2;
    }

    //wrap value to QString
    template<typename T>
    QString wrapVal(T & rawVal ){
        const static size_t __intID = typeid (int).hash_code();
        const static size_t __uintID = typeid (unsigned int).hash_code();
        const static size_t __boolID = typeid (bool).hash_code();
        const static size_t __doubleID = typeid (double).hash_code();
        const static size_t __floatID = typeid (float).hash_code();
        const static size_t __llID = typeid (long long).hash_code();
        const static size_t __ullID = typeid (unsigned long long).hash_code();
        const static size_t __QStringID = typeid (QString).hash_code();
        const static size_t __QDateTimeID = typeid (QDateTime).hash_code();
        const static size_t __QDateID = typeid (QDate).hash_code();
        const static size_t __QTimeID = typeid (QTime).hash_code();
        const static size_t __QByteArrayID = typeid (QByteArray).hash_code();

        const size_t id = (typeid (rawVal).hash_code());
        void *p = &rawVal;

        if(id == __intID)
        {
            int temp = *(int*) p;
            return QString::number( temp );
        }
        else if(id == __uintID)
        {
            unsigned int temp = *(unsigned int*) p;
            return QString::number( temp );
        }
        else if(id == __boolID)
        {
            bool temp = *(bool*) p;
            return QString::number( temp );
        }
        else if(id == __doubleID)
        {
            double temp = *(double*) p;
            return QString::number( temp );
        }
        else if(id == __floatID)
        {
            float temp = *(float*) p;
            return QString::number( temp );
        }
        else if(id == __llID)
        {
            long long temp = *(long long*) p;
            return QString::number( temp );
        }
        else if(id == __ullID)
        {
            unsigned long long temp = *(unsigned long long*) p;
            return QString::number( temp );
        }
        else if(id == __QStringID)
        {
            return *(QString*) p;
        }
        else if(id == __QDateTimeID)
        {
            QDateTime temp = *(QDateTime*) p;
            return temp.toString();
        }
        else if(id == __QDateID)
        {
            QDate temp = *(QDate*) p;
            return temp.toString();
        }
        else if(id == __QTimeID)
        {
            QTime temp = *(QTime*) p;
            return temp.toString();
        }
        else if(id == __QByteArrayID)
        {
            QByteArray temp = *(QByteArray*) p;
            return QString( temp.toBase64() );
        }
        LOG_DEBUG() << "wrap value error: unknow type:"<<id;
        return QString::null;
    }
};



#endif // ILOADSAVEPROCESSOR
