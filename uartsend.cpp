#include "mainwindow.h"
#include "ui_mainwindow.h"


//=================================================
//启动发送数据区1的自动轮发
void MainWindow::startRegion1AutoSend() {
    if(wheelSendingRunFlg) {
        if(autoSendEnFlg1) {
            sendRegion1Dat();
            int delyTime;
            delyTime = ui->lineEdit_sendInterval1->text().toInt();
            QTimer::singleShot(delyTime, this, SLOT(startRegion2AutoSend()));
        }
        else {
            startRegion2AutoSend();
        }
    }

}//startRegion1AutoSend()



//启动发送数据区2的自动轮发
void MainWindow::startRegion2AutoSend() {
    if(wheelSendingRunFlg){
        if(autoSendEnFlg2) {
            sendRegion2Dat();
            int delyTime;
            delyTime = ui->lineEdit_sendInterval2->text().toInt();
            QTimer::singleShot(delyTime, this,SLOT(startRegion3AutoSend()));
        }
        else {
            startRegion3AutoSend();
        }
    }

}//startRegion1AutoSend()



//启动发送数据区3的自动轮发
void MainWindow::startRegion3AutoSend() {
    if(wheelSendingRunFlg){
        if(autoSendEnFlg3) {
            sendRegion3Dat();
            int delyTime;
            delyTime = ui->lineEdit_sendInterval3->text().toInt();
            QTimer::singleShot(delyTime, this, SLOT(startRegion4AutoSend()));
        }
        else {
            startRegion4AutoSend();
        }
    }

}//startRegion1AutoSend()



//启动发送数据区4的自动轮发
void MainWindow::startRegion4AutoSend() {
    if(wheelSendingRunFlg){
        if(autoSendEnFlg4) {
            sendRegion4Dat();
            int delyTime;
            delyTime = ui->lineEdit_sendInterval4->text().toInt();
            QTimer::singleShot(delyTime, this, SLOT(goToAutoSendEndPoint()));
        }
        else {
            goToAutoSendEndPoint();
        }
    }
}//startRegion1AutoSend()



//自动发送循环中的一次循环的端点
//功能： 检查总循环次数是否溢出
void MainWindow::goToAutoSendEndPoint() {
    if(wheelSendingRunFlg){
        //检查是否还有有效的发送区域
        if(false == (autoSendEnFlg1 || autoSendEnFlg2 || autoSendEnFlg3 || autoSendEnFlg4)) {
            wheelSendingRunFlg = false;
            ui->pusBut_startAutoSend->setText("Start AutoSend");
            return;
        }

        //不限次数的连续发送
        if(repeatingSendSettingEntity.repeatingForeverFlg == true) {
            startRegion1AutoSend();
        }
        //有次数限制
        else {
            quint32 times = --repeatingSendSettingEntity.remainderRepeatingTimes;
            qDebug() << "goToAutoSendEndPoint times: " << times << endl;
            //显示剩余次数
            displayCurWheelSenOrder(times);
            //还有剩余次数未完
            if(times > 0 ){
                startRegion1AutoSend();
            }
            else {
                wheelSendingRunFlg = false;
                ui->pusBut_startAutoSend->setText("Start AutoSend");
                return;
            }
        }
    }
} //goToAutoSendEndPoint();
//------------------------------------------------------------------------------------------



//======================================================
//写数据到串口上
void MainWindow::writeData(const QByteArray &data)
{
    qint64 writeBytesNum = 0;
    writeBytesNum = serial->write(data);
    //qDebug() <<writeBytesNum << hasSendBytesNum <<endl;
    if(writeBytesNum > 0){
        hasSendBytesNum += writeBytesNum;
    }

    ui->lab_hasSendBytesNum->setText(QString("has sent %1 bytes.").arg(hasSendBytesNum));

}//writeData()
//=======================================================


//更新已发送行数统计和检查屏幕显示行数
void MainWindow::renewSentLineNum() {
    //显示超过一定行数后自动清空接收显示区
    if(senDisLineNum >= setSentDisLineTotalNum){
        clearSentDisplay();
        senDisLineNum = 0;
    }
    senDisLineNum++;
    //qDebug() << "--senDisLineNum: " << senDisLineNum <<endl;
}




//=======================================================
//按Latin1字符编码模式发送串口数据
void MainWindow::sendInLatin1mode(QString const &uartstr) {
    writeData(uartstr.toLocal8Bit());
    renewSentLineNum();
    ui->texBro_sentDat->append(uartstr);

}//sendInLatin1mode()


//按十六进制编码模式发送串口数据
void MainWindow::sendInHexmode(QString  const &uartstr) {
    QString toBeSendStr(uartstr);
    QString toBeScreenStr;
    bool toUIntOK;
    uint dat;
    QByteArray * bytearray = new QByteArray();
    QString tempStr = "";

    bytearray->resize (1);
    tempStr = toBeSendStr.left(2);
    while (!tempStr.isEmpty ()) {
        //qDebug() << "sendRegion1Dat()  tempStr: " << tempStr << endl;
        //按字符字面表示转为16进制整型数
        dat = tempStr.toUInt (&toUIntOK, 16);
        //qDebug() << "sendRegion1Dat()  dat: " << dat <<endl;
        //------------转换成功---------
        if(true == toUIntOK) {
            //写到ByteArray中
            (*bytearray)[0]= dat;
            qDebug() << "(*bytearray).toHex(): " << (*bytearray).toHex() <<endl;
            //发送
            writeData(*bytearray);
            //将已发送数据显示到窗口
            toBeScreenStr.append(tempStr + " ");
        }
        else {
            //qDebug() << "to be sent data not hex, fail." <<endl;
            ;
        }

        //移除已发送字符
        toBeSendStr.remove(0,2);
        //准备下一次循环
        tempStr = toBeSendStr.left(2);
    }

    renewSentLineNum();
    //将已发送的字符显示到窗口
    ui->texBro_sentDat->append(toBeScreenStr);

}//sendInHexmode()
//======================================================



//发送数据区1的数据
void MainWindow::sendRegion1Dat(){
    QString writeToUartStr;
    writeToUartStr = ui->texEdi_sendDat1->toPlainText();

    //Latin1编码模式下的发送
    if(senEncodeMode == Latin1mode) {
        sendInLatin1mode(writeToUartStr);
    }
    //十六进制串口数据发送
    else {
        sendInHexmode(writeToUartStr);
    }

}//sendRegion1Dat()




//发送数据区2的数据
void MainWindow::sendRegion2Dat(){
    QString writeToUartStr;
    writeToUartStr = ui->texEdi_sendDat2->toPlainText();

    //Latin1编码模式下的发送
    if(senEncodeMode == Latin1mode) {
        sendInLatin1mode(writeToUartStr);
    }
    //十六进制串口数据发送
    else {
        sendInHexmode(writeToUartStr);
    }

}


//发送数据区3的数据
void MainWindow::sendRegion3Dat(){
    QString writeToUartStr;
    writeToUartStr = ui->texEdi_sendDat3->toPlainText();

    //Latin1编码模式下的发送
    if(senEncodeMode == Latin1mode) {
        sendInLatin1mode(writeToUartStr);
    }
    //十六进制串口数据发送
    else {
        sendInHexmode(writeToUartStr);
    }

}


//发送数据区4的数据
void MainWindow::sendRegion4Dat(){
    QString writeToUartStr;
    writeToUartStr = ui->texEdi_sendDat4->toPlainText();

    //Latin1编码模式下的发送
    if(senEncodeMode == Latin1mode) {
        sendInLatin1mode(writeToUartStr);
    }
    //十六进制串口数据发送
    else {
        sendInHexmode(writeToUartStr);
    }

}
//======================================================









