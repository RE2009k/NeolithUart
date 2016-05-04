#include "mainwindow.h"
#include "ui_mainwindow.h"


//终止文件发送
void MainWindow::stopFileSendingOnUart() {
    txtFileSendingRunFlg = false;

}


//在串口上发送TXT文件
void MainWindow::sendTxtFileOnUart() {
    //启动串口发送互斥检查

    //检查有没有正在进行轮发，手工发是不互斥的
    if(true == wheelSendingRunFlg){
        return;
    }

    //正在发送文件
    if(true == txtFileSendingRunFlg) {
        //忽略操作
        return;
    }

    //设置文件发送 占用串口的信号量
    txtFileSendingRunFlg = true;

    //读取发送文件的配置
    QString senTxtPathStr;
    senTxtPathStr = senTxtSetting->getSendingTxtFilePath();
    qDebug() << "sendTxtFileOnUart: " << senTxtPathStr;


    //打开要发送的文件
    QFile file(senTxtPathStr);
    if(false == file.exists ()) {
        //throw false;
        return;
    }
    //打开文件RefUCF
    if((file.open(QIODevice::ReadOnly)) == false) {
       return;
    }


    QTextStream stream(&file);
    QString lineStr;
    //循环读取文件行字符串
    do {
        //读取(不包括"\n")一行文本
        lineStr = stream.readLine();

        //串口命令过滤和解析
        int parserOut;
        parserOut = ComCmdParser(lineStr);
        if(parserOut !=0) {
            continue;
        }

        //Latin1编码模式下的发送
        if(senEncodeMode == Latin1mode) {
            sendInLatin1mode(lineStr);
        }
        //十六进制串口数据发送
        else {
            sendInHexmode(lineStr);
        }

    //直到读到文件末尾
    } while(!stream.atEnd() && (txtFileSendingRunFlg == true));

    //关闭文件
    file.close();


}//sendTxtFile()






/*******************************************
 *串口文件中的命令解析
 * 参数说明：
 * 0 无效控制命令
 * 1 功能1
 * 2
 * -1 发生错误
 ******************************************/
int MainWindow::ComCmdParser(const QString &str){
    //正则表达式解析控制行和数据行
    //QString RexExpPattern("^\\s*NET\\s+\"(.*)\"\\s+LOC\\s*=\\s*\"(.*)\"\\s*;\\s*.*$");

    //==解析间隔时间命令==
    //#INTV = 5000;  任意字符
    QString RexExpPattern_INTV("^\\s*#INTV\\s*=\\s*(\\d+)\\s*.*$");
    QRegExp RegExp_INTV(RexExpPattern_INTV);

    int pos = RegExp_INTV.indexIn(str);
    qDebug() <<"RegExp INTV pos: " <<pos;
    //匹配
    if(pos != -1) {
        //取命令中的延时时长值
        QString numStr = RegExp_INTV.cap(1);
        int num_value;
        num_value = numStr.toInt();
        qDebug() << "#INTVL num_value: " << num_value <<endl;

        //延时指定时长
        delayTime->delay_ms(num_value);

        return 1;
    }



    //==解析文件循环发送命令==





    //该行是数据行
    return 0;

}//ComCmdParser()








