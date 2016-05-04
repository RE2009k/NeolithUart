/*************************************************
**
** 界面设置和初始化
**
*************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "MainWindow() obj" << endl;

    delayTime = new QDelayTime(this);

    //软件在服务初始化
    softserver = new SoftInServer(this);
    softserver->enableOffServerMessageDialog(true);

    //系统托盘菜单
    sysTrayIconserver = new SysTrayIcon(this);

    //屏幕分割
    ui->splitter_disOpt->setStretchFactor(0,10);
    ui->splitter_disOpt->setStretchFactor(1,3);

    //串口和设置初始化
    serial = new QSerialPort(this);
    uartsettings = new SettingsDialog();
    receiveDat = new QByteArray();
    receiveHandleTimer = new QTimer(this);
    receiveHandleTimer->setSingleShot(true);
    connect(
        receiveHandleTimer,
        SIGNAL(timeout()),
        this,
        SLOT(receiveDatHandle())
    );


    //工具栏设置
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();

    connect(serial,
            SIGNAL(error(QSerialPort::SerialPortError)),
            this,
            SLOT(handleError(QSerialPort::SerialPortError))
    );

    connect(serial,
            SIGNAL(readyRead()),
            this,
            SLOT(readData())
    );


    //设置发送串口数据间隔时间的有效区间
    sendIntervalValidator = new QIntValidator(20, 1000000, this);
    ui->lineEdit_sendInterval1->setValidator(sendIntervalValidator);
    ui->lineEdit_sendInterval2->setValidator(sendIntervalValidator);
    ui->lineEdit_sendInterval3->setValidator(sendIntervalValidator);
    ui->lineEdit_sendInterval4->setValidator(sendIntervalValidator);

    //置默认值
    ui->lineEdit_sendInterval1->setText("1000");
    ui->lineEdit_sendInterval2->setText("1000");
    ui->lineEdit_sendInterval3->setText("1000");
    ui->lineEdit_sendInterval4->setText("1000");

    //自动轮发
    autoSendEnFlg1 = false;     //各发送区开关
    autoSendEnFlg2 = false;
    autoSendEnFlg3 = false;
    autoSendEnFlg4 = false;
    wheelSendingRunFlg = false;   //总开关

    hasSendBytesNum = 0;
    hasRevBytesNum = 0;

    //连发总次数设置
    repeatingSendSettingEntity.remainderRepeatingTimes = 0;
    repeatingSendSettingEntity.totalRepeatTimes = 0;
    repeatingSendSettingEntity.repeatingForeverFlg = false;

    ui->linEdi_repeatingTimes->setText("0");
    repeatingTimesValidator = new QIntValidator(0, 10000, this);
    ui->linEdi_repeatingTimes->setValidator(repeatingTimesValidator);

    //显示到屏幕上的数据的行数
    revDisLineNum = 0;
    senDisLineNum = 0;
     //暂停显示
    pauseRevDis = false;
    //设置显示行数上限值
    ui->linEdi_disRevLineNum->setText ("1000");
    ui->linEdi_disSentLineNum->setText ("1000");
    setRevDisLineTotalNum = 1000;
    setSentDisLineTotalNum = 1000;

#ifdef  DEBUG_SUPPORT
    //Debug
    ui->texEdi_sendDat1->setText("FFUUwo6677%^&*($#@1100AABB");
#endif

    //字符编码模式
    revEncodeMode = Latin1mode;
    senEncodeMode = Latin1mode;
    ui->cheBox_revlatin1mode->setChecked (true);
    ui->cheBox_revhexmode->setChecked (false);
    ui->cheBox_senlatin1mode->setChecked (true);
    ui->cheBox_senhexmode->setChecked (false);

    //去使能检查异常标志字符串功能
    CheckRevExceptionSign = false;

    //发送文件设置类初始化
    senTxtSetting = new SendTxtFileSetting(this);

    //文件发送占用串口标识信号量
    txtFileSendingRunFlg = false;

    //构建设置接收数据到文件类
    pSaveFileSetting = new SaveFileSettingDialog(this);

    //不保存接收数据到文件
    saveRevDatToFileEn = false;

    qApp->dumpObjectTree();
    qApp->dumpObjectInfo();

} //MainWindow()




//析构
MainWindow::~MainWindow()
{
    delete uartsettings;
    delete softserver;
    delete sysTrayIconserver;
    delete senTxtSetting;
    delete pSaveFileSetting;

    closeSerialPort();
    delete serial;
    delete ui;

    qApp->dumpObjectTree();
    qApp->dumpObjectInfo();

}//~MainWindow()




//################################################
//打开串口
void MainWindow::openSerialPort()
{
    //注册检查
    /*if(softserver->softInServerCheck () == false) {
       return;
    }*/

    SettingsDialog::Settings p = uartsettings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        //console->setEnabled(true);
        //console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")  .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)                                .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }

}//openSerialPort()



//关闭串口
void MainWindow::closeSerialPort()
{
    if(serial->isOpen()){
        serial->close();
    }
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));

}//closeSerialPort()



//关于
void MainWindow::about()
{
    QMessageBox::about(this, tr("The NeolithUart v1.0"),tr("<b>Welcom to QiongLai.</b>"));

} //about()



//处理错误
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}



//初始化动作的信号槽连接
void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));

    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));

    //connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close())); //是隐藏，而不是退出
    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ui->actionConfigure, SIGNAL(triggered()), uartsettings, SLOT(show()));

    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearDisplay()));

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}//initActionsConnections()



//清空接收显示
void MainWindow::clearRevDisplay () {
    //清空显示
    ui->texBro_receiveDat->clear();

}//clearRevDisplay()



//清空发送显示
void MainWindow::clearSentDisplay () {
    ui->texBro_sentDat->clear();

}//clearSentDisplay()



//清空数据显示
void MainWindow::clearDisplay(){
    clearRevDisplay();
    clearSentDisplay();

    //清零计数值
    hasRevBytesNum = 0;
    ui->lab_hasRevBytesNum->setText(QString("0 bytes."));

    //清零计数值
    hasSendBytesNum = 0;
    ui->lab_hasSendBytesNum->setText(QString("0 bytes."));

}//clearDisplay()


//手动发送数据区1的字符
void MainWindow::on_pusBut_sendDat1_clicked()
{
    sendRegion1Dat();

}//on_pusBut_sendDat1_clicked()



//手动发送数据区域2的字符
void MainWindow::on_pusBut_sendDat2_clicked()
{
    sendRegion2Dat();

}//on_pusBut_sendDat2_clicked()



//手动发送数据区域3的字符
void MainWindow::on_pusBut_sendDat3_clicked()
{
    sendRegion3Dat();

}//on_pusBut_sendDat3_clicked()



//手动发送数据区域4的字符
void MainWindow::on_pusBut_sendDat4_clicked()
{
    sendRegion4Dat();

}//on_pusBut_sendDat4_clicked()




//=================================================
//暂停/继续显示
void MainWindow::on_pusBut_pauseDisplay_clicked()
{
    if(pauseRevDis == true) {
        pauseRevDis = false;
        ui->pusBut_pauseDisplay->setText("Pause");
    }
    else {
        pauseRevDis = true;
        ui->pusBut_pauseDisplay->setText("Continue");
    }

}//on_pusBut_pauseDisplay_clicked()



//=====================================================
void MainWindow::on_cheBox_autosend1_clicked(bool checked)
{
    autoSendEnFlg1 = checked;
}

void MainWindow::on_cheBox_autosend2_clicked(bool checked)
{
    autoSendEnFlg2 = checked;
}

void MainWindow::on_cheBox_autosend3_clicked(bool checked)
{
    autoSendEnFlg3 = checked;

}

void MainWindow::on_checBox_autosend4_clicked(bool checked)
{
    autoSendEnFlg4 = checked;

}


//启动自动轮发模式
void MainWindow::on_pusBut_startAutoSend_clicked()
{
    //正在发的过程中需停止
    if(wheelSendingRunFlg == true){
        wheelSendingRunFlg = false;
        ui->pusBut_startAutoSend->setText("Start AutoSend");
        //更新连发设置到初始状态
        on_linEdi_repeatingTimes_editingFinished();
    }
    //当且至少有一个发送区域被使能参加自动发送
    else if(true == (autoSendEnFlg1 || autoSendEnFlg2 || autoSendEnFlg3 || autoSendEnFlg4) && (false == txtFileSendingRunFlg)){
        wheelSendingRunFlg = true;
        //更新发送次数设置
        on_linEdi_repeatingTimes_editingFinished();
        //启动自动发送
        startRegion1AutoSend();
        ui->pusBut_startAutoSend->setText("Stop AutoSend");
    }


} //on_pusBut_startAutoSend_clicked()




//=================================================
void MainWindow::on_pusBut_clr1_clicked()
{
   ui->texEdi_sendDat1->clear();
}


void MainWindow::on_pusBut_clr2_clicked()
{
    ui->texEdi_sendDat2->clear();
}


void MainWindow::on_pusBut_clr3_clicked()
{
    ui->texEdi_sendDat3->clear();
}


void MainWindow::on_pusBut_clr4_clicked()
{
    ui->texEdi_sendDat4->clear();
}




//设置连发总次数
void MainWindow::on_linEdi_repeatingTimes_editingFinished()
{
    quint32 num =  ui->linEdi_repeatingTimes->text().toUInt();
    if(num == 0) {
        repeatingSendSettingEntity.repeatingForeverFlg = true;
    }
    else {
        repeatingSendSettingEntity.repeatingForeverFlg = false;
     }

    repeatingSendSettingEntity.remainderRepeatingTimes = num;
    repeatingSendSettingEntity.totalRepeatTimes = num;

    //更新显示
    displayCurWheelSenOrder(num);
    //UInt == uint32

}//on_linEdi_repeatingTimes_editingFinished()




//开关：是否自动检查接收自串口的数据中的“异常”标识字符串
void MainWindow::on_cheBox_RevException_clicked(bool checked)
{
    CheckRevExceptionSign = checked;
}



//########################################################
// 保存接收数据到文件
//########################################################
void MainWindow::on_pusBut_saveToFile_clicked()
{
    //打开设置界面
    pSaveFileSetting->show();

}//on_pusBut_saveToFile_clicked()



/************************************************************************************************************
 *
 *
 * **********************************************************************************************************/
//设置接收显示行数上限值
void MainWindow::on_linEdi_disRevLineNum_editingFinished()
{
    setRevDisLineTotalNum = ui->linEdi_disRevLineNum->text ().toInt ();
}


//设置发送显示行数上限值
void MainWindow::on_linEdi_disSentLineNum_editingFinished()
{
    setSentDisLineTotalNum = ui->linEdi_disSentLineNum->text ().toInt ();

}




/***********************************************************************************
 *  收发显示（字符编码）模式设置
 *
***********************************************************************************/
//接收
void MainWindow::on_cheBox_revhexmode_clicked(bool checked)
{
    if(checked == false) {
        ui->cheBox_revlatin1mode->setChecked (true);
        revEncodeMode = Latin1mode;
    }
    else {
        ui->cheBox_revlatin1mode->setChecked (false);
        revEncodeMode = Hexmode;
    }
}


void MainWindow::on_cheBox_revlatin1mode_clicked(bool checked)
{
    if(checked == false) {
        ui->cheBox_revhexmode->setChecked (true);
        revEncodeMode = Hexmode;
    }
    else {
        ui->cheBox_revhexmode->setChecked (false);
        revEncodeMode = Latin1mode;
    }
}


//发送
void MainWindow::on_cheBox_senhexmode_clicked(bool checked)
{
    if(checked == false) {
        ui->cheBox_senlatin1mode->setChecked (true);
        senEncodeMode = Latin1mode;
    }
    else {
        ui->cheBox_senlatin1mode->setChecked (false);
        senEncodeMode = Hexmode;
    }
}



void MainWindow::on_cheBox_senlatin1mode_clicked(bool checked)
{
    if(checked == false) {
        ui->cheBox_senhexmode->setChecked (true);
        senEncodeMode = Hexmode;
    }
    else {
        ui->cheBox_senhexmode->setChecked (false);
        senEncodeMode = Latin1mode;
    }
}

//###################################################################

//设置显示当前轮发的次数（序号）
void MainWindow::displayCurWheelSenOrder(const quint32 times){
    quint32 ordertime = repeatingSendSettingEntity.totalRepeatTimes - times;
    ui->lab_curSenTimes->setText(QString("%1/").arg(ordertime));

}//setCurWheelSenTimes()




/****************************************************************
 * 关闭程序事件的处理
 ***************************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    //显示提示弹窗
    sysTrayIconserver->showMessageBox("NeolithUart hiding here!");
    //隐藏程序界面
    this->hide();

} //"closeEvent()"



//启动文件发送配置和发送使能
void MainWindow::on_pusBut_sendFrFile_clicked()
{
    senTxtSetting->show ();

}



//启动文件发送/停止发送
void MainWindow::on_pB_sendfile_clicked()
{
    //正在发送文件，将执行终止发送
    if(txtFileSendingRunFlg) {
        ui->pB_sendfile->setText(tr("Start send file"));
        stopFileSendingOnUart();
    }
    //没有发送文件, 将启动发送文件
    else {
        ui->pB_sendfile->setText(tr("Stop file-Sending"));
        sendTxtFileOnUart();
    }

}//on_pB_sendfile_clicked()


//设置是否保存接收数据到文件
void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked) {
        saveRevDatToFileEn = true;
    }
    else {
        saveRevDatToFileEn = false;
    }

}//on_checkBox_clicked()
