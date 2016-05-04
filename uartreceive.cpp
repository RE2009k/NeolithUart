#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>



//读接收数据到缓存中延迟处理
void MainWindow::readData()
{
    receiveDat->append(serial->readAll());
    //先缓存数据，再调用定时器，设置一定时间后，再处理缓存中的数据
    //qDebug()<<"receive uart data cache: " <<QString(*receiveDat)<<endl;
    receiveHandleTimer->start(20); //毫秒

}//readData()




/*********************************************************************************
 * 编码处理
 * *******************************************************************************/
//按Latin1编码字符串方式解析接收的串口数据
void MainWindow::revInLatin1mode(QString *str) {
    //假定接收的数据是按本地编码方式编码的，并将其拷贝转换成Unicode方式输出
    QTextCodec *codec = QTextCodec::codecForLocale();
    *str = codec->toUnicode(*receiveDat);

}//revInLatin1mode()





//按Hex(十六进制）编码方式解析接收的串口数据
void MainWindow::revInHexmode(QString *str) {
    QByteArray revByteArray = (*receiveDat).toHex();
    qDebug() <<"revByteArray: "<<revByteArray<<endl;
    QString tmpstr;
    //遍历revByteArray
    int num = revByteArray.size();
    for(int i = 0; i < num; i++) {
        uint padat = revByteArray[i];
        //取出的是十进制的ASCII码值
        qDebug() << "padat[" << i <<"]: " << padat <<endl;
        //整数转字面意义对应的字符串
        //tmpstr.setNum(padat, 16);
        tmpstr = padat;  //?
        qDebug() << "tmpstr: " << tmpstr << endl;
        str->append (tmpstr);
        //每字节加入一个空格
        if(i%2 == 1){
            str->append (" ");
        }

    }

}//revInHexmode()




//更新已接收行数统计和检查屏幕显示行数
void MainWindow::renewRevLineNum() {
    //qDebug() << "--revDisLineNum: " << revDisLineNum << endl;
    //显示超过一定行数后自动清空接收显示区
    if(revDisLineNum >= setRevDisLineTotalNum){
        //保存接收到的数据到文件
        if(saveRevDatToFileEn == true){
            saveRevDatToFile();
        }
        //清空屏幕
        clearRevDisplay();
        revDisLineNum = 0;
    }

}//renewRevLineNum()




//处理接收的uart缓存数据
void MainWindow::receiveDatHandle(){
    QString str;
    if(revEncodeMode == Latin1mode) {
        revInLatin1mode(&str);
    }
    else {
        revInHexmode(&str);
    }

    //更新接收行数统计
    renewRevLineNum();

    //打印到接收显示区
    if(pauseRevDis == false){
        ui->texBro_receiveDat->append(str);
        revDisLineNum++;
    }

    //累加个数
    hasRevBytesNum += receiveDat->size();
    ui->lab_hasRevBytesNum->setText(QString("has received %1 bytes.").arg(hasRevBytesNum));

    //处理接收数据
    if(CheckRevExceptionSign == true) {
        checkReceiveDat(*receiveDat);
    }

    //清除接收缓存中的数据
    receiveDat->clear();

} //receiveDatHandle()



/***********************************************************************************************************
 *
 *
 * *********************************************************************************************************/
//检查接收数据中是否有设置的关键字符串
void MainWindow::checkReceiveDat(QByteArray & received){
    static bool popUpExceptionBox = true;  //静态局部变量，用于标记是否有MessageBox未关闭
    bool containError;
    char errStr[] = "[Exception]";
    //QString errStr_n("dd");
    //空
    if(received.isEmpty()){
        return;
    }
    containError = received.contains(errStr);
    //避免MessageBox连续弹出很多
    if(containError && popUpExceptionBox) {
        popUpExceptionBox = false;
        int ret = QMessageBox::critical(this, tr("Critical Error"), errStr);
        //?进程循环会继续运行下一句吗，如果不点Box对话框，然后程序就不能再运行到这里了
        if (ret == QMessageBox::Ok) {
           popUpExceptionBox = true;
        }
    }

}//checkReceiveDat()






