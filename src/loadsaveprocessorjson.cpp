#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "loadsaveprocessorjson.h"

/*
 * 构造函数
 * 输入参数：父QObject
 * 返回数值：无
 * 功能描述：
 * 1、进入Initialization状态，初始化内部变量
 * 2、初始化本地xml文件
 * 3、进入Ready状态
 */
loadSaveProcessorJson::loadSaveProcessorJson(QObject *parent, bool encrypt):baseDevice(parent)
{
    _password = QString("fusion");
    _salt = QString("fusion");
    _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);
    _needEncrypt = encrypt;

    setState( stateReady );
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

/*
 * 读实例内的参数
 * 输入参数：
 * 1、传入参数的名称
 * 2、返回参数值
 * 返回数值：
 * 1、成功0，状态错误-1，找不到参数-2
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
 * 2、读取当前parent位置下的指定paraName的paraValue
 * 3、找不到的话paraValue = null，返回-2
 */
int loadSaveProcessorJson::loadParameters(const QString &paraName, QString *paraValue){
    if(getState() != stateOccupied ){
        *paraValue = QString::null;
        return -1;
    }
    QJsonValue val = getParent()->value( paraName );
    if( val.isUndefined() ){
        *paraValue = QString::null;
        return -2;//找不到
    }
    else{
        *paraValue = val.toString();
        return 0;//找到了
    }
}

/*
 * 写实例内的参数
 * 输入参数：
 * 1、传入参数的名称
 * 2、传入参数值
 * 返回数值：
 * 1、成功0，状态错误-1
 * 功能描述：
 * 1、子实例写入流程：a、创建新实例（CreateNewInstance） b、写入参数（saveParameters） c、返回父实例（MoveBackToParent）
 * 2、把的paraName和paraValue保存到当前parent位置下
 * 3、若参数存在，则覆盖。若不存在，则新建
 */
int loadSaveProcessorJson::saveParameters(const QString &paraName, const QString &paraValue){
    if(getState() != stateOccupied ){
        return -1;
    }
    getParent()->insert(paraName, QJsonValue(paraValue) );
    return 0;
}

/*
 * 移动到实例
 * 输入参数：
 * 1、ObjType 一般为类的名字
 * 2、Index 实例的序号，从0开始
 * 返回数值：
 * 1、成功0，状态错误-1
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorJson::moveToInstance(const QString& ObjType, const QString& InstID){
    if(getState() != stateOccupied ){
        return -1;
    }
    QString name = ObjType + InstID;
//    qDebug()<<"loadSaveProcessorJson::moveToInstance-before"<<getPathName()<<getParent()->keys();
    pushParent( name );
//    qDebug()<<"loadSaveProcessorJson::moveToInstance-after"<<getPathName()<<getParent()->keys();
    return 0;
}

/*
 * 创建新实例(废弃)
 * 输入参数：
 * 1、ObjType 一般为类的名字
 * 2、InstID实例标识符，一般为实例的名称
 * 返回数值：
 * 1、成功0，状态错误-1
 * 功能描述：
 * 1、子实例写入流程：a、创建新实例（CreateNewInstance）b、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
 * new routine update,20161129
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorJson::createNewInstance(const QString &ObjType, const QString &InstID){
    if(getState() != stateOccupied ){
        return -1;
    }
//    QString name = ObjType + InstID;
//    QJsonObject* newone = new QJsonObject();
//    getParent()->insert(name, QJsonValue(*newone));
    return 0;
}


/*
 * 移动到父实例
 * 输入参数：无
 * 返回数值：
 * 1、成功0，状态错误-1
 * 功能描述：
 * 1、子实例读取流程：a、移动到实例（MoveToInstance） b、读取参数（loadParameters） c、返回父实例（MoveBackToParent）
 * 2、子实例写入流程：a、创建新实例（CreateNewInstance）b、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorJson::moveBackToParent(){
    if(getState() != stateOccupied ){
        return -1;
    }
//    qDebug()<<"loadSaveProcessorJson::moveBackToParent-before"<<getPathName()<<getParent()->keys();
    int ret = popParent();
//    qDebug()<<"loadSaveProcessorJson::moveBackToParent-after"<<getPathName()<<getParent()->keys();
    return ret;
}


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
 * 启动事务
 * 输入参数：无
 * 返回数值：
 * 1、成功0，启动失败-1
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
 * 3、初始化domElementParentList
 */
int loadSaveProcessorJson::transactionStart(){
    if(getState() != stateReady) return -1;
    setState( stateOccupied );
    _JsonPathName.clear();
    _JsonPathName.append( QString("JsonRoot"));
    _JsonParent.clear();
    _JsonParent.append( new QJsonObject() );

    return 0;
}
/*
 * 结束事务
 * 输入参数：无
 * 返回数值：
 * 1、成功0，结束失败-1
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
 */
int loadSaveProcessorJson::transactionEnd(){
    if(getState() != stateOccupied) return -1;
    setState( stateReady );
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
 * 1、成功0，失败-1，文件内容错误-2
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
 */
int loadSaveProcessorJson::loadFile(const QString &fileName){
    if(getState() != stateOccupied ){
        return -1;
    }
    if(fileName != QString::null){
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
 * 1、成功0，写入错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
 */
int loadSaveProcessorJson::saveFile(const QString &fileName){
    if(getState() != stateOccupied ){
        return -1;
    }
    if(fileName !=  QString::null){
        setJsonFilePath( fileName );
    }
    return writeJsonFile();
}

/*
 * 读取Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，失败-2，打开失败-3，格式错误-4
 * 功能描述：
 * 1、判断文件是已存在且正确，否则失败。
 * 2、读取文件到root JsonObj对象
 * 3、读取时进入ReadFile模式，发生错误后返回到NotReady模式，读取成功后进入原有模式
 */
int loadSaveProcessorJson::readJsonFile(){

    quint64 lastState = getState();
    setState( stateReadFile );//进入读取文件模式

    QFile file(_JsonFilePathWithoutProtocol);
    if( !file.exists()) {
        setError( errorFileNotFound );
        setState( stateNotReady );//进入停止模式
        return -2;
    }
    if(!file.open(QIODevice::ReadOnly)){
        //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<"xml file open failed!\n"<<file.errorString();
        setError( errorFileOpenFail );
        file.close();
        setState( stateNotReady );//进入停止模式
        return -3;
    }

    QByteArray orientalBytes = file.readAll();
    file.close();

    QJsonParseError ok;
    QJsonDocument doc;
    if(_needEncrypt){
        QByteArray decrypedBytes = _aes->decrypt(orientalBytes,QAesWrap::AES_ECB);
        //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<orientalBytes<<"||"<<decrypedBytes;
        //qDebug()<<orientalBytes.length()<<decrypedBytes.length();
        doc = QJsonDocument::fromJson(decrypedBytes, &ok);
        if(ok.error == QJsonParseError::NoError){
            *(_JsonParent.at(0)) = doc.object();
        }
        else{
            setError( errorFlieFomatWrong );
            setState( stateNotReady );//进入停止模式
            return -4;
        }
    }
    else{
        doc = QJsonDocument::fromJson(orientalBytes, &ok);
        if(ok.error == QJsonParseError::NoError){
            *(_JsonParent.at(0)) = doc.object();
        }
        else{
            setError( errorFlieFomatWrong );
            setState( stateNotReady );//进入停止模式
            return -4;
        }
    }

    emit jsonRefresh();
    setState( lastState );//进入上个模式
    return 0;
}

/*
 * 写入Json文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，写入错误-2
 * 功能描述：
 * 1、把root JsonObj对象写入文件
 * 2、发送文件更新信号
 * 3、读取时进入WriteFile模式，发生错误后返回到NotReady模式，读取成功后进入原有模式
 */
int loadSaveProcessorJson::writeJsonFile(){
    quint64 lastState = getState();
    setState( stateWriteFile );//进入写文件模式

    while( popParent()==0 ){
        //清空parent层
    };

    QFile file(_JsonFilePathWithoutProtocol);
    if( !file.open(QIODevice::WriteOnly) )
    {
        setError( errorFileWriteFail );
        setState( stateNotReady );//进入停止模式
        return -2;
    }

    QJsonDocument doc( *(_JsonParent.at(0)) );
    QByteArray orientalBytes = doc.toJson() ;
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
    setState( lastState );//进入上个模式

    return 0;
}
/*
 * 返回parent堆顶元素
 * 输入参数：无
 * 返回QJsonObject*
 * 功能描述：
 * 1、返回parent堆顶元素
 */
QJsonObject* loadSaveProcessorJson::getParent(){
    return _JsonParent.last();
}
/*
 * 返回PathName堆顶元素
 * 输入参数：无
 * 返回QString
 * 功能描述：
 * 1、返回PathName堆顶元素
 */
QString loadSaveProcessorJson::getPathName(){
    return _JsonPathName.last();
}
/*
 * parent堆弹出
 * 输入参数：无
 * 返回：0成功、-1parent到底
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
//    getParent()->insert(lastName, QJsonValue(*lastObj) );
//    qDebug()<<"to pop"<<lastName<<lastObj->size();
//    qDebug()<<"parent"<<getPathName()<<getParent()->size();
//    debugPrint( lastObj );
//    debugPrint( _JsonRoot );
    getParent()->insert(lastName, QJsonValue(*lastObj) );
//    debugPrint( _JsonRoot );
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
int loadSaveProcessorJson::pushParent(QString name){
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
//obsolete
void loadSaveProcessorJson::debugPrint(QJsonObject* obj){
    QJsonObject::Iterator it;
    qDebug()<<"-------------debugPrint Start";
    for(it = obj->begin(); it!=obj->end(); ++it){
        if(it.value().isObject() ){
            qDebug()<<"------------------Objec Start"<<it.key();
            QJsonObject temp = it.value().toObject();
            debugPrint( &temp );
            qDebug()<<"------------------end of Object"<<it.key();
        }
        else{
            qDebug()<<it.key()<<it.value().toString();
        }
    }
    qDebug()<<"-------------debugPrint End";
}
