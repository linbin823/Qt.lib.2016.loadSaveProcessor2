#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "loadsaveprocessorjson.h"

/*
 * 构造函数
 * 输入参数：父QObject
 * 返回数值：无
 * 功能描述：
 * 1、初始化内部变量
 * 2、default json file path
 * 3、初始化Json root object
 */
loadSaveProcessorJson::loadSaveProcessorJson(QObject *parent, bool encrypt):QObject(parent),_needEncrypt(encrypt)
{
    _password = QString("fusion");
    _salt = QString("fusion");
    _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);

    setJsonFilePath( QString(QCoreApplication::applicationDirPath() + "/configuration.json"));

    _JsonPathName.clear();
    _JsonPathName.append( QString("JsonRoot"));
    _JsonParent.clear();
    _JsonParent.append( new QJsonObject() );
}

/*
 * 析构函数
 * 输入参数：无
 * 返回数值：无
 * 功能描述：
 */
loadSaveProcessorJson::~loadSaveProcessorJson()
{
    delete _aes;
    for(int i =0; i< _JsonParent.size(); i++){
        delete _JsonParent.at(i);
    }
    _JsonParent.clear();
    _JsonPathName.clear();
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
QString loadSaveProcessorJson::readParameters(const QString&& paraName){
    if( !isValid() ){
        //qDebug() << "load parameters from " << paraName << " error, parent not valid";
        return QString::null;
    }

    return getParent()->value(paraName).toString(QString::null);
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
int loadSaveProcessorJson::writeParameters(const QString&& paraName, const QString&& value){
    if( !isValid() ){
        //qDebug() << "save parameters to " << paraName << " error, parent not valid";
        return -1;
    }

    QJsonValue temp(value);
    getParent()->insert(paraName, temp );
    return 0;
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
int loadSaveProcessorJson::moveToInstance(const QString&& ObjType, const QString&& InstID){
    if ( !isValid() ) {
        //qDebug() << "moveToInstance to " << ObjType << InstID << " error, parent not valid";
        return -1;
    }

//    qDebug()<<"loadSaveProcessorJson::moveToInstance-before"<<getPathName()<<getParent()->keys();
    pushParent( ObjType + "::::" + InstID );
//    qDebug()<<"loadSaveProcessorJson::moveToInstance-after"<<getPathName()<<getParent()->keys();
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
int loadSaveProcessorJson::moveBackToParent(){
    if ( !isValid() ) {
        //qDebug() << "moveBackToParent error, parent not valid";
        return -1;
    }

//    qDebug()<<"loadSaveProcessorJson::moveBackToParent-before"<<getPathName()<<getParent()->keys();
    popParent();
//    qDebug()<<"loadSaveProcessorJson::moveBackToParent-after"<<getPathName()<<getParent()->keys();
    return 0;
}

//get current object's ObjType and InstID
QString loadSaveProcessorJson::currentObjType()const {
    return getPathName().split("::::").at(0);
}

QString loadSaveProcessorJson::currentInstID()const {
    QStringList ret = getPathName().split("::::");
    if (ret.size() != 2) return QString::null;
    return ret.at(1);
}

//get all sub-object's InstIDs of a certain objType
QStringList loadSaveProcessorJson::instIDs(const QString&& ObjType)const {
    QStringList ret, nameList;
    foreach (QString name, getParent()->keys()) {
        nameList = name.split("::::");
        if (nameList.size() != 2) continue;
        if (nameList.at(0) != ObjType) continue;
        ret << nameList.at(1);
    }
    return ret;
}

//get all sub-object's ObjTypes
QStringList loadSaveProcessorJson::objTypes()const {
    QStringList ret, nameList;
    foreach (QString name, getParent()->keys()) {
        nameList = name.split("::::");
        if (nameList.size() != 2) continue;
        ret << nameList.at(0);
    }
    return ret;
}

//get all parametersName
QStringList loadSaveProcessorJson::parametersName()const {
    QStringList ret, nameList;
    foreach (QString name, getParent()->keys()) {
        nameList = name.split("::::");
        if (nameList.size() != 1) continue;
        ret << nameList.at(0);
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
int loadSaveProcessorJson::init(){
    _JsonPathName.clear();
    _JsonPathName.append( QString("JsonRoot"));
    _JsonParent.clear();
    _JsonParent.append( new QJsonObject() );
    return 0;
}

/*
 * 读取Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 2、文件内容错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorJson::loadFile(const QString& fileName){
    if(fileName != NULL){
        setJsonFilePath( fileName );
    }
    int ret = readJsonFile();
    //qDebug()<<"loadSaveProcessorJson::loadFile"<<ret;
    return ret;
}

/*
 * 保存Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0
 * 2、文件内容错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorJson::saveFile(const QString &fileName){
    if(fileName !=  NULL){
        setJsonFilePath( fileName );
    }
    return writeJsonFile();
}

/*
 * 保存Json格式字符串
 * 输入参数：
 * 1、QByteArray&  return value
 * 返回数值：
 * 1、成功0
 * 功能描述：
 * 1、实施保存工作流程：a、init the processor b、root instance save parameters to processor c、saveFile
 * 2、实施实施读取工作流程：a、loadFile b、root instance load parameters from processor
 */
int loadSaveProcessorJson::saveByteArray(QByteArray& result){
    while( popParent()==0 ){
        //清空parent层
    };

    QJsonDocument doc( *(_JsonParent.at(0)) );
    result = doc.toJson() ;
    return 0;
}


/*
 * 读取Json格式字符串
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
int loadSaveProcessorJson::loadByteArray(const QByteArray& source){
    QJsonParseError ok;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(source, &ok);
    if(ok.error == QJsonParseError::NoError){
        *(_JsonParent.at(0)) = doc.object();
        return 0;
    }
    else{
        //qDebug() << "loadByteArray error"<<ok.errorString();
        return -1;
    }
}

//finished to implement iLoadSaveProcessor

/*
 * 设置文件路径
 * 输入参数：QString &name
 * 返回数值：无
 * 功能描述：
 * 1、写入无协议Json文件路径
 * 2、写入有协议Json文件路径，for QML
 * 3、每次写入都会检测文件名称
 */
void loadSaveProcessorJson::setJsonFilePath(const QString &name){

    _JsonFilePathWithoutProtocol = QDir::toNativeSeparators(name);
    _JsonFilePath = QDir::toNativeSeparators( "file:" + name );
    emit jsonFilePathChanged();
}

/*
 * 读取文件路径
 * 输入参数：无
 * 返回数值：QString &name
 * 功能描述：
 * 1、返回有协议文件路径，for QML
 */
QString loadSaveProcessorJson::getJsonFilePath(void){
    return _JsonFilePath;
}

/*
 * 返回parent堆顶元素
 * 输入参数：无
 * 返回QJsonObject*
 * 功能描述：
 * 1、返回parent堆顶元素
 */
QJsonObject* loadSaveProcessorJson::getParent() const{
    return _JsonParent.last();
}
/*
 * 返回PathName堆顶元素
 * 输入参数：无
 * 返回QString
 * 功能描述：
 * 1、返回PathName堆顶元素
 */
QString loadSaveProcessorJson::getPathName() const{
    return _JsonPathName.last();
}
/*
 * parent堆弹出
 * 输入参数：无
 * 返回：0成功、-1 parent到底
 * 功能描述：
 * 1、弹出parent堆最上面的一个元素
 * 2、把弹出的元素写入到新的堆顶元素中去
 * 3、删除弹出元素
 */
int loadSaveProcessorJson::popParent(){

    if (_JsonParent.size()==1 ){
        //到底了
        return -1;
    }
    QJsonObject* lastObj = getParent();
    QString lastName = getPathName();
    _JsonParent.removeLast();
    _JsonPathName.removeLast();

    getParent()->remove(lastName);
    //insert new or update value
    getParent()->insert(lastName, QJsonValue(*lastObj) );
    delete lastObj;
    return 0;
}
/*
 * parent堆压入
 * 输入参数：参数名称
 * 返回：0成功
 * 功能描述：
 * 1、把参数名称所对应的object压入parent堆
 * 2、不是对象,overwrite it
 */
int loadSaveProcessorJson::pushParent(const QString &&name){
    QJsonValue temp = getParent()->value( name );
    QJsonObject* tempObj;
    if(temp.isObject()){
        //是对象，则压栈
        tempObj = new QJsonObject();
        *tempObj = temp.toObject();
        _JsonParent.append( tempObj );
        _JsonPathName.append(name);
        return 0;
    }
    else{
        //不是对象,overwrite
        tempObj = new QJsonObject();
        _JsonParent.append( tempObj );
        _JsonPathName.append(name);
        return 0;
    }
}



//is JsonParent valid
bool loadSaveProcessorJson::isValid(){
    if(_JsonParent.size() < 1 || _JsonPathName.size() < 1 ){
        return false;
    }
    return true;
}

/*
 * 读取Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，file not found-1，打开失败-2，格式错误-3
 * 功能描述：
 * 1、判断文件是已存在且正确，否则失败。
 * 2、读取文件到root JsonObj对象
 */
int loadSaveProcessorJson::readJsonFile(){

    QFile file(_JsonFilePathWithoutProtocol);
    if( !file.exists()) {
        //qDebug() << "read json file error: file not exists";
        return -1;
    }
    if(!file.open(QIODevice::ReadOnly)){
        file.close();
        //qDebug() << "read json file error: file open fail";
        return -2;
    }
    QByteArray orientalBytes = file.readAll();
    file.close();

    int ret;
    if(_needEncrypt){
        QByteArray decrypedBytes = _aes->decrypt(orientalBytes,QAesWrap::AES_ECB);
        ret = loadByteArray( decrypedBytes );
    }
    else{
        ret = loadByteArray( orientalBytes );
    }
    if(ret != 0){
        //qDebug() << "read json file error: file format error";
        return -3;
    }

    emit jsonRefresh();
    return 0;
}

/*
 * 写入Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，写入错误-1
 * 功能描述：
 * 1、把root JsonObj对象写入文件
 * 2、发送文件更新信号
 */
int loadSaveProcessorJson::writeJsonFile(){

    QFile file(_JsonFilePathWithoutProtocol);
    if( !file.open(QIODevice::WriteOnly) )
    {
        //qDebug() << "write json file error: file open fail";
        return -1;
    }

    QByteArray orientalBytes;
    saveByteArray(orientalBytes);
    if(_needEncrypt){
        QByteArray encrypedBytes = _aes->encrypt(orientalBytes,QAesWrap::AES_ECB);
        //qDebug()<<"loadSaveProcessorXml::writeXmlFile"<<orientalBytes<<"||"<<encrypedBytes<<orientalBytes.size()<<encrypedBytes.size();
        //qDebug()<<_aes->decrypt(encrypedBytes, QAesWrap::AES_ECB);
        file.write( encrypedBytes, encrypedBytes.length());
    }
    else{
        file.write( orientalBytes.data(), orientalBytes.length());
    }
    file.close();
    emit jsonRefresh();

    return 0;
}

void loadSaveProcessorJson::debugPrint(QJsonObject* obj){
    QJsonObject::Iterator it;
    qDebug() << "debugPrint:";
    for(it = obj->begin(); it!=obj->end(); ++it){
        if(it.value().isObject() ){
            qDebug() << "QJsonObject: "<<it.key();
            QJsonObject temp = it.value().toObject();
            debugPrint( &temp );
            qDebug() <<"QJsonObject: "<<it.key()<<" End:";
        }
        else{
            qDebug() <<it.key()<<it.value().toString();
        }
    }
    qDebug() <<"debugPrint End";
}
