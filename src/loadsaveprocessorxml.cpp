#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "loadsaveprocessorxml.h"

/*
 * 构造函数
 * 输入参数：
 * 1、父QObject
 * 2、是否需要AES加密
 * 返回数值：无
 * 功能描述：
 * 1、进入Initialization状态，初始化内部变量
 * 2、初始化本地xml文件
 * 3、进入Ready状态
 */
loadSaveProcessorXml::loadSaveProcessorXml(QObject *parent, bool encrypt):baseDevice(parent)
{
    _password = QString("fusion");
    _salt = QString("fusion");
    _aes = new QAesWrap(_password.toUtf8(), _salt.toUtf8(), QAesWrap::AES_256);
    _needEncrypt = encrypt;

    setState( stateReady );
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
 * 3、找不到的话paraValue = -1，返回-2
 */
int loadSaveProcessorXml::loadParameters(const QString& paraName, QString * paraValue){
    if(getState() != stateOccupied ){
        *paraValue = QString::null;
        return -1;
    }
    QDomElement temp;
    int ret = getElement(temp,paraName);//* 返回数值： 成功0，输入参数错误-1，找不到-2
    if( ret == 0 ){
        if( !temp.childNodes().isEmpty() ){
            *paraValue = temp.childNodes().at(0).toText().nodeValue();
            return 0;//找到了，且有文本
        }
    }
    *paraValue = QString::null;
    return -2;//找不到
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
int loadSaveProcessorXml::saveParameters(const QString &paraName, const QString &paraValue){
    if(getState() != stateOccupied ){
        return -1;
    }
    QDomElement temp;
    int ret = getElement(temp,paraName);//* 返回数值： 成功0，找不到-1
    //qDebug()<<"loadSaveProcessorXml::saveParameters"<<ret;
    //qDebug()<<"loadSaveProcessorXml::saveParameters"<<domElementParentList.size();
    if( ret == 0 ){
        if( !temp.childNodes().isEmpty() ){
            temp.childNodes().at(0).toText().setNodeValue( paraValue );
            return 0;//找到了
        }
        QDomText newOne = _resXml.createTextNode( paraValue );
        temp.appendChild( newOne );
        return 0;
    }
    else{
        QDomElement newElement  = _resXml.createElement( paraName );
        QDomText    newOne      = _resXml.createTextNode( paraValue );
        newElement.appendChild( newOne );
        getParent().appendChild( newElement );
        return 0;//找不到,create new one
    }
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
 * 20161120优化，子实例写入流程可以简化：
 * 2、子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorXml::moveToInstance(const QString &ObjType, const QString &index){
    if(getState() != stateOccupied ){
        return -1;
    }
    QDomNodeList newList = getParent().elementsByTagName(ObjType);
//    if(newList.isEmpty()){
//        return -2;
//    }
//    if(index.isNull()){
//        pushParent(newList.at(0).toElement());
//    }
    for(int i=0; i<newList.size(); i++){
        QDomElement newone = newList.at(i).toElement();
        if(index.isNull()){
            pushParent(newone);
            return 0;
        }
        else if( newone.attribute("id") == index){
            pushParent(newone);
            return 0;
        }
    }
    //找不到，建新再压栈
    QDomElement newone;
    setElement(newone, ObjType, index);
    pushParent( newone );
    return 0;
}
/*
 * 创建新实例(废弃)
 * 输入参数：
 * 1、ObjType 一般为类的名字
 * 2、InstID实例标识符，一般为实例的名称
 * 返回数值：
 * 1、new 0，existed! 1, 状态错误-1
 * 功能描述：
 * 1、子实例写入流程：a、创建新实例（CreateNewInstance）b、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
 * 20161120优化，子实例写入流程可以简化：
 * 2、子实例写入流程：a、移动到实例（MoveToInstance） c、写入参数（saveParameters） d、返回父实例（MoveBackToParent）
 */
int loadSaveProcessorXml::createNewInstance(const QString &ObjType, const QString &InstID){
    if(getState() != stateOccupied ){
        return -1;
    }
    QDomElement newone;
    return setElement(newone, ObjType, InstID);
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
int loadSaveProcessorXml::moveBackToParent(){
    if(getState() != stateOccupied ){
        return -1;
    }
    popParent();
    return 0;
}

/*
 * 初始化xml文件(废弃)
 * 输入参数：无
 * 返回数值：
 * 1、成功0，失败-1
 * 功能描述：
 * 1、初始化一个空的xml，并且写入基本信息
 * 2、写入文件
 * 3、发送文件更新信号
 */
int loadSaveProcessorXml::initXmlFile(){

    quint64 lastState = getState();
    setState( stateWriteFile );//进入指定写入文件模式

    _resXml.clear();

    QString strHeader( "version=\"1.0\" encoding=\"UTF-8\"" );
    _resXml.appendChild( _resXml.createProcessingInstruction("xml", strHeader) );

    QDomElement rootElement = _resXml.createElement( "config" );
    rootElement.setAttribute( "id", "test v1.0" );
    _resXml.appendChild( rootElement );

    int ret = writeXmlFile();
    setState( lastState );//进入上个模式

    emit resXmlRefresh();

    return ret;

}
/*
 * 读取xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，失败-1
 * 功能描述：
 * 1、判断文件是已存在且正确，否则失败。
 * 2、读取文件到DOM对象
 * 3、读取时进入ReadFile模式，发生错误后返回到NotReady模式，读取成功后进入原有模式
 */
int loadSaveProcessorXml::readXmlFile(){

    quint64 lastState = getState();
    setState( stateReadFile );//进入读取文件模式
    //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<lastState;

    QFile file(_resXmlFilePathWithoutProtocol);
    if( !file.exists()) {
        setError( errorFileNotFound );
//        20161120优化，取消初始化文件功能
//        if (initXmlFile() != 0){
//            setError( errorFileOpenFail );
//            //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<"errorFileOpenFail";
//            setState( stateNotReady );//进入停止模式
//            return -1;
//        }
        setState( stateNotReady );//进入停止模式
        return -1;
    }
    if(!file.open(QIODevice::ReadOnly)){
        //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<"xml file open failed!\n"<<file.errorString();
        setError( errorFileOpenFail );
        file.close();
        setState( stateNotReady );//进入停止模式
        return -1;
    }

    QByteArray orientalBytes = file.readAll();
//    QDataStream data(&file);
//    char* buffer = (char*)malloc(file.size() * sizeof(char) );
//    data.readRawData(buffer, file.size() );
//    QByteArray orientalBytes = QByteArray(buffer, file.size() );
//    delete buffer;
    file.close();

    bool ok;
    QString errMsg;
    if(_needEncrypt){
        QByteArray decrypedBytes = _aes->decrypt(orientalBytes,QAesWrap::AES_ECB);
        //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<orientalBytes<<"||"<<decrypedBytes;
        //qDebug()<<orientalBytes.length()<<decrypedBytes.length();
        ok = _resXml.setContent(decrypedBytes,false,&errMsg);
    }
    else{
        ok = _resXml.setContent(orientalBytes,false,&errMsg);
    }
    if(!ok){
        setError( errorFlieFomatWrong );
        file.close();
        setState( stateNotReady );//进入停止模式
        return -1;
    }

    //qDebug()<<"loadSaveProcessorXml::readXmlFile"<<resXml.firstChildElement().nodeName();
    _domElementParentList.clear();
    _domElementParentList.append( _resXml.firstChildElement() );
    //qDebug()<<domElementParentList.at(0).toElement().nodeName();
    emit resXmlRefresh();
    setState( lastState );//进入上个模式
    return 0;
}

/*
 * 写入xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，写入错误-1
 * 功能描述：
 * 1、把DOM对象写入文件
 * 2、发送文件更新信号
 * 3、读取时进入WriteFile模式，发生错误后返回到NotReady模式，读取成功后进入原有模式
 */
int loadSaveProcessorXml::writeXmlFile(){
    quint64 lastState = getState();
    setState( stateWriteFile );//进入写文件模式

    QFile file(_resXmlFilePathWithoutProtocol);
    if( !file.open(QIODevice::WriteOnly) )
    {
        setError( errorFileWriteFail );
        //qDebug()<<"xml file write error!";
        setState( stateNotReady );//进入停止模式
        return -1;
    }

    QByteArray orientalBytes;
    QTextStream out( &orientalBytes );
    out.setCodec("UTF-8");
    _resXml.save(out,4,QDomNode::EncodingFromTextStream);//4是子项目缩进长度

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

    emit resXmlRefresh();
    setState( lastState );//进入上个模式

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
    QDomNodeList selection = getParent().elementsByTagName(tagName);
    if(!selection.isEmpty()){
        if(id.isNull() ){
            res = selection.at(0).toElement();
            return 0;
        }
        for(int i=0; i<selection.count(); i++){
            res = selection.at(i).toElement();
            if(id == res.attribute("id")){
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
    QDomNodeList selection = getParent().elementsByTagName(tagName);
    if( !selection.isEmpty() ){
        if( id.isNull() ){
            res = selection.at(0).toElement();
            return 1;//已存在，不新建
        }
        for(int i=0; i<selection.count(); i++){
            if( id == selection.at(i).toElement().attribute("id") ){
                res = selection.at(i).toElement();
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
//    QFileInfo test = QFileInfo(name);
//    if( !test.isFile() ){
//        setError( errorFileNameError );
//        return;
//    }
    _resXmlFilePathWithoutProtocol = QDir::toNativeSeparators(name);
    _resXmlFilePath = QDir::toNativeSeparators( "file:" + name );
    //qDebug()<<_resXmlFilePath;
    //qDebug()<<_resXmlFilePathWithoutProtocol;
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
/*
 * 读取xml文件
 * 输入参数：无
 * 返回数值：
 * 1、成功0，失败-1，文件内容错误-21
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
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
 * 1、成功0，写入错误-1
 * 功能描述：
 * 1、实施保存工作流程：a、启动事务(transactionStart) b、顶层实例顺序调用每个子实例的保存函数 c、子实例顺序调更下级的子实例保存函数 d、所有保存完毕后，顶层实例调用保存文件（saveFile） e、结束事务（transactionEnd）
 * 2、实施读取工作流程：a、启动事务(transactionStart) b、顶层实例调用读取文件（loadFile），可以是默认文件，也可以指定文件 c、顶层实例顺序调用每个子实例的读取函数 d、子实例顺序调更下级的子实例读取函数  e、结束事务（transactionEnd）
 */
int loadSaveProcessorXml::saveFile(const QString& fileName){
    if(fileName != NULL){
        setResXmlFilePath( fileName );
    }
    return writeXmlFile();
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
int loadSaveProcessorXml::transactionStart(){
    if(getState() != stateReady) return -1;
    setState( stateOccupied );
    _resXml.clear();
    QDomElement root = _resXml.createElement("root");
    _resXml.appendChild( root );
    _domElementParentList.clear();
    _domElementParentList.append( root );
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
int loadSaveProcessorXml::transactionEnd(){
    if(getState() != stateOccupied) return -1;
    setState( stateReady );
    return 0;
}
