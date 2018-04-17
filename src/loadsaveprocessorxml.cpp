#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QStringList>
#include <QDomNodeList>
#include "loadsaveprocessorxml.h"

/*
 * 构造函数
 * 输入参数：
 * 1、父QObject
 * 2、是否需要AES加密
 * 返回数值：无
 * 功能描述：
 * 1、初始化内部变量
 * 2、初始化本地xml文件
 */
loadSaveProcessorXml::loadSaveProcessorXml(QObject *parent, bool encrypt):QObject(parent),_needEncrypt(encrypt)
{
    _password = QString("fusion");
    _salt = QString("fusion");
    _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);

    setResXmlFilePath( QString(QCoreApplication::applicationDirPath() + "/configuration.xml"));
}

/*
 * 析构函数
 * 输入参数：无
 * 返回数值：无
 * 功能描述：
 */
loadSaveProcessorXml::~loadSaveProcessorXml(){
    delete _aes;
}


//start to implement iLoadSaveProcessor

/*
 * 读实例内的参数
 * 输入参数：
 * 参数1：参数的名称
 * 返回数值：
 * 参数1：返回参数值
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（readParameters） c、返回父实例（MoveBackToParent）
 * 2、读取当前parent位置下的指定paraName的paraValue
 * 3、找不到的话paraValue = null
 */
QString loadSaveProcessorXml::readParameters(const QString&& paraName){
    if( !isValid() ){
        qDebug() << "load parameters from " << paraName << " error, parent not valid";
        return QString::null;
    }

    QDomElement temp;
    int ret = getElement(temp, paraName);//* 返回数值： 成功0，输入参数错误-1，找不到-2
    if( ret == 0 ){
        if( !temp.childNodes().isEmpty() ){//找到了，且有文本
            return temp.childNodes().at(0).toText().nodeValue();
        }
        //qDebug() << "load parameters from " << paraName << " error, name found but text is empty.";
    }
    //qDebug() << "load parameters from " << paraName << " error, name not found";
    return QString::null;
}

/*
 * 写实例内的参数
 * 输入参数：
 * 参数1：写：参数的名称
 * 参数2：写：传入参数值
 * 返回数值：
 * 1、成功0
 * 2、未准备好 -1
 * 功能描述：
 * 1、子实例写入流程：a、创建新实例（CreateNewInstance） b、写入参数（writeParameters） c、返回父实例（MoveBackToParent）
 * 2、把的paraName和value保存到当前parent位置下
 * 3、若参数存在，则覆盖。若不存在，则新建
 */
int loadSaveProcessorXml::writeParameters(const QString&& paraName, const QString&& value){
    if( !isValid() ){
        //qDebug() << "save parameters to " << paraName << " error, parent not valid";
        return -1;
    }
    QDomElement temp;
    int ret = getElement(temp,paraName);//* 返回数值： 成功0，找不到-1
    if( ret == 0 ){
        if( !temp.childNodes().isEmpty() ){
            temp.childNodes().at(0).toText().setNodeValue( value );
            return 0;//找到了
        }
        QDomText newOne = _resXml.createTextNode( value );
        temp.appendChild( newOne );
        return 0;
    }
    else{
        //qDebug() << "save parameters to " << paraName << " ok, name not found, create a new one";
        QDomElement newElement  = _resXml.createElement( paraName );
        QDomText    newOne      = _resXml.createTextNode( value );
        newElement.appendChild( newOne );
        getParent().appendChild( newElement );
        return 0;//找不到,create new one
    }
}

/*
 * 移动到实例
 * 输入参数：
 * 参数1：ObjType 一般为类的名字
 * 参数2：InstID实例标识符，一般为实例的序号
 * 返回数值：
 * 1、成功0
 * 2、未准备好 -1
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（readParameters） c、返回父实例（MoveBackToParent）
 * 2、子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（writeParameters） d、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorXml::moveToInstance(const QString&& ObjType, const QString&& InstID){
    if( !isValid() ){
        //qDebug() << "moveToInstance to " << ObjType << InstID << " error, parent not valid";
        return -1;
    }

    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        QDomElement ele = list.at(i).toElement();
        if (InstID.isNull()) {
            pushParent(ele);
            return 0;
        } else if (InstID == ele.attribute("id")) {
            pushParent(ele);
            return 0;
        }
    }

    //找不到，建新再压栈
    QDomElement newone;
    setElement(newone, ObjType, InstID);
    pushParent( newone );
    //qDebug() << "moveToInstance to " << ObjType << InstID << " ok, instance not found, create a new one.";
    return 0;
}

/*
 * 移动到父实例
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 2、未准备好 -1
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（readParameters） c、返回父实例（MoveBackToParent）
 * 2、子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（writeParameters） d、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorXml::moveBackToParent(){
    if( !isValid() ){
        //qDebug() << "moveBackToParent error, parent not valid";
        return -1;
    }

    popParent();
    return 0;
}

//get current object's ObjType and InstID
QString loadSaveProcessorXml::currentObjType()const {
    return getParent().tagName();
}

QString loadSaveProcessorXml::currentInstID()const {
    return getParent().attribute("id");
}

//get all sub-object's InstIDs of a certain objType
QStringList loadSaveProcessorXml::instIDs(const QString&& ObjType)const {
    QStringList ret;

    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        QDomElement ele = list.at(i).toElement();
        if (ele.tagName() == ObjType) {
            QString id = ele.attribute("id");
            if( id != QString() ) {
                ret << id;
            }
        }
    }
    ret.removeDuplicates();
    return ret;
}

//get all sub-object's ObjTypes
QStringList loadSaveProcessorXml::objTypes()const {
    QStringList ret;
    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        QDomElement ele = list.at(i).toElement();
        ret << ele.tagName();
    }
    ret.removeDuplicates();
    return ret;
}

//get all parametersName
QStringList loadSaveProcessorXml::parametersName()const {
    QStringList ret;

    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        ret << list.at(i).toElement().tagName();
    }
    return ret;
}
/*
 * init
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 * 3、初始化_JsonParent _JsonPathName
 */
int loadSaveProcessorXml::init(){
    _resXml.clear();
    QDomElement root = _resXml.createElement("root");
    _resXml.appendChild( root );
    _domElementParentList.clear();
    _domElementParentList.append( root );
    return 0;
}

/*
 * 读取xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 2、文件内容错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorXml::loadFile(const QString& fileName){
    if(fileName != NULL){
        setResXmlFilePath( fileName );
    }
    int ret = readXmlFile();
    //qDebug()<<ret;
    return ret;
}
/*
 * 保存xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 2、文件内容错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorXml::saveFile(const QString& fileName){
    if(fileName != NULL){
        setResXmlFilePath( fileName );
    }
    return writeXmlFile();
}

/*
 * 保存xml格式字符串
 * 输入参数：
 * 1、QByteArray&  return value
 * 返回数值：
 * 1、成功0
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorXml::saveByteArray(QByteArray& result){
    QTextStream out( &result );
    out.setCodec("UTF-8");
    _resXml.save(out,4,QDomNode::EncodingFromTextStream);//4是子项目缩进长度

    return 0;
}


/*
 * 读取xml格式字符串
 * 输入参数：
 * 1、QByteArray&  source value
 * 返回数值：
 * 1、成功0
 * 2、文件内容错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 * 3、can't load encrypted string
 */
int loadSaveProcessorXml::loadByteArray(const QByteArray& source){
    bool ok;
    QString errMsg;
    ok = _resXml.setContent(source,false,&errMsg);
    if(!ok){
        //qDebug() << "loadByteArray error"<<errMsg;
        return -1;
    }

    _domElementParentList.clear();
    _domElementParentList.append( _resXml.firstChildElement() );
    emit resXmlRefresh();
    return 0;
}

//finished to implement iLoadSaveProcessor

/*
 * 读取xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，失败-1
 * 功能描述：
 * 1、判断文件是已存在且正确，否则失败。
 * 2、读取文件到DOM对象
 */
int loadSaveProcessorXml::readXmlFile(){

    QFile file(_resXmlFilePathWithoutProtocol);
    if( !file.exists()) {
        //qDebug() << "readXmlFile error, file not exist.";
        return -1;
    }
    if(!file.open(QIODevice::ReadOnly)){
        //qDebug() << "readXmlFile error, file open failed.";
        file.close();
        return -1;
    }
    QByteArray orientalBytes = file.readAll();
    file.close();

    if(_needEncrypt){
        QByteArray decrypedBytes = _aes->decrypt(orientalBytes,QAesWrap::AES_ECB);
        return loadByteArray(decrypedBytes);
    }
    return loadByteArray(orientalBytes);
}

/*
 * 写入xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，写入错误-1
 * 功能描述：
 * 1、把DOM对象写入文件
 * 2、发送文件更新信号
 */
int loadSaveProcessorXml::writeXmlFile(){
    QByteArray orientalBytes;
    saveByteArray(orientalBytes);

    QFile file(_resXmlFilePathWithoutProtocol);
    if( !file.open(QIODevice::WriteOnly) )
    {
        //qDebug() << "writeXmlFile error, file open failed.";
        return -1;
    }
    if(_needEncrypt){
        QByteArray encrypedBytes = _aes->encrypt(orientalBytes,QAesWrap::AES_ECB);
        file.write( encrypedBytes, encrypedBytes.length());
    }
    else{
        file.write( orientalBytes.data(), orientalBytes.length());
    }
    file.close();

    emit resXmlRefresh();

    return 0;
}

/*
 * 搜索xml元素
 * 输入参数：
 * 1、QDomElement 返回该xml元素的指针，找不到返回getParent()
 * 2、QString 被搜索元素的tagName
 * 3、QString 被搜索元素的id，可以不指定
 * 返回数值：
 * 1、成功0，找不到-1
 * 功能描述：
 */
int loadSaveProcessorXml::getElement(QDomElement& res, QString tagName, QString id){
    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        QDomElement ele = list.at(i).toElement();
        if (ele.tagName() == tagName) {
            if (id.isNull()) {
                res = ele;
                return 0;
            } else if (id == ele.attribute("id")) {
                res = ele;
                return 0;
            }
        }
    }
    res = getParent();
    return -1;
}

/*
 * 新增xml元素
 * 输入参数：
 * 1、QDomElement 返回new xml元素的reference
 * 2、QString 新元素的tagName
 * 3、QString 新元素的id，可以不指定
 * 返回数值：
 * 1、成功0，已存在1
 * 功能描述：
 * 1、在搜索范围内搜索
 * 2、若该元素存在，则直接返回
 * 3、若该元素不存在，则新建一个返回
 * 4、元素id可以不指定，此时返回tagName相同的第一个元素
 */
int loadSaveProcessorXml::setElement(QDomElement& res, QString tagName, QString id){
    QDomNodeList list = getParent().childNodes();
    for(int i = 0; i < list.size(); i++) {
        QDomElement ele = list.at(i).toElement();
        if (ele.tagName() == tagName) {
            if (id.isNull()) {
                res = ele;
                return 1;//已存在，不新建
            } else if (id == ele.attribute("id")) {
                res = ele;
                return 1;//已存在，不新建
            }
        }
    }
    res = _resXml.createElement( tagName );
    if( !id.isNull() ){
        res.setAttribute( "id", id );
    }
    getParent().appendChild( res );//追加新建
    return 0;
}
/*
 * 设置文件路径
 * 输入参数：QString &name
 * 返回数值：无
 * 功能描述：
 * 1、写入无协议xml文件路径
 * 2、写入有协议xml文件路径，for QML
 * 3、每次写入都会检测文件名称
 */
void loadSaveProcessorXml::setResXmlFilePath(const QString &name){
    _resXmlFilePathWithoutProtocol = QDir::toNativeSeparators(name);
    _resXmlFilePath = QDir::toNativeSeparators( "file:" + name );
    emit resXmlFilePathChanged();
}

/*
 * 读取文件路径
 * 输入参数：无
 * 返回数值：QString &name
 * 功能描述：
 * 1、返回有协议文件路径，for QML
 */
QString loadSaveProcessorXml::getResXmlFilePath(void) const{
    return _resXmlFilePath;
}

//is JsonParent valid
bool loadSaveProcessorXml::isValid(){
    return true;
}
