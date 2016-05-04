/**************************************************
**
** Copyright (C) 2012 Denis Shienkov
**
*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <qDebug>
#include <QString>
#include <QTextCodec>
#include <QIntValidator>
#include <QFileDialog>
#include "./SoftInServer/SoftInServer.h"
#include "./TrayIcon/trayiconserver.h"
#include "./SendTxtFileSetting/sendtxtfilesetting.h"
#include <QCloseEvent>
#include <QDialog>
#include "settingsdialog.h"
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include "QDelayTime/qdelaytime.h"
#include "./SaveFileSetting/savefilesettingdialog.h"


QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE


//界面\初始化支持调试
#define DEBUG_SUPPORT   1

class SettingsDialog;
class SendTxtFileSetting;
enum  EncodeMode { Latin1mode = 0, Hexmode = 1};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event); //extra qualification

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void clearDisplay();        //清空发送和接收显示
    void receiveDatHandle();    //处理接收的uart缓存数据

    void on_pusBut_sendDat1_clicked();

    void on_pusBut_sendDat2_clicked();

    void on_pusBut_sendDat3_clicked();

    void on_pusBut_sendDat4_clicked();

    void on_pusBut_pauseDisplay_clicked();

    void on_cheBox_autosend1_clicked(bool checked);

    void on_cheBox_autosend2_clicked(bool checked);

    void on_cheBox_autosend3_clicked(bool checked);

    void on_checBox_autosend4_clicked(bool checked);

    void on_pusBut_startAutoSend_clicked();


private:
    void initActionsConnections();
    //停止文件发送
    void stopFileSendingOnUart();

    void sendRegion1Dat();  //发送“发送数据区1”的数据
    void sendRegion2Dat();  //发送“发送数据区2”的数据
    void sendRegion3Dat();  //发送“发送数据区3”的数据
    void sendRegion4Dat();  //发送“发送数据区4”的数据

    //检查接收数据
    void checkReceiveDat(QByteArray & received);
    //保存接收数据到文件
    bool saveRevDatToFile();
    //设置屏幕上能显示的行数上限值
    //void setDisLineNum();

    void clearRevDisplay();
    void clearSentDisplay();

    //两种编码模式下的发送串口数据
    void sendInLatin1mode(QString const &uartstr);
    void sendInHexmode(QString const &uartstr);

    //两种编码模式下的串口数据接收
    void revInLatin1mode(QString *str);
    void revInHexmode(QString *str);

    //更新已发送行数统计和检查屏幕显示行数
    void renewSentLineNum();
    void renewRevLineNum();

    //设置显示当前轮发的次数（序号）
    void displayCurWheelSenOrder(const quint32 times);

    //在串口上发送TXT文件
    void sendTxtFileOnUart();

    //解析串口发送文件中的命令
    int ComCmdParser(const QString &str);



private slots:
    void startRegion1AutoSend();  //启动发送区1 自动发
    void startRegion2AutoSend();  //启动发送区2 自动发
    void startRegion3AutoSend();  //启动发送区3 自动发
    void startRegion4AutoSend();  //启动发送区4 自动发
    void goToAutoSendEndPoint(); //自动发送循环中的一次循环的端点

    void on_pusBut_clr1_clicked();

    void on_pusBut_clr2_clicked();

    void on_pusBut_clr3_clicked();

    void on_pusBut_clr4_clicked();

    void on_linEdi_repeatingTimes_editingFinished();

    void on_cheBox_RevException_clicked(bool checked);

    void on_pusBut_saveToFile_clicked();

    void on_linEdi_disRevLineNum_editingFinished();

    void on_linEdi_disSentLineNum_editingFinished();

    void on_cheBox_revhexmode_clicked(bool checked);

    void on_cheBox_revlatin1mode_clicked(bool checked);

    void on_cheBox_senhexmode_clicked(bool checked);

    void on_cheBox_senlatin1mode_clicked(bool checked);

    void on_pusBut_sendFrFile_clicked();

    void on_pB_sendfile_clicked();


    void on_checkBox_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    SettingsDialog *uartsettings;  //串口设置
    QSerialPort *serial;  //串口
    SoftInServer *softserver;        //软件在服务注册
    SysTrayIcon *sysTrayIconserver;  //系统托盘图标处理
    SendTxtFileSetting *senTxtSetting;  //发送文本文件设置
    //延时函数
    QDelayTime *delayTime;
    SaveFileSettingDialog *pSaveFileSetting;


    QByteArray *receiveDat;     //从串口上接收的数据
    QTimer *receiveHandleTimer; //延时处理接收缓存中的串口数据
    quint32 setRevDisLineTotalNum;  //设置接收显示的上限
    quint32 setSentDisLineTotalNum;  //设置发送显示的上限
    quint32 revDisLineNum;  //从串口上接收并显示在屏幕上的数据的行号
    quint32 senDisLineNum; //发送并显示到屏幕上的行数
    bool pauseRevDis;  //true 暂停接收数据的屏幕显示
    QIntValidator* sendIntervalValidator;  //发送间隔时间设置校验器

    //四个发送数据区的Auto-Send使能
    bool autoSendEnFlg1;
    bool autoSendEnFlg2;
    bool autoSendEnFlg3;
    bool autoSendEnFlg4;


    quint64 hasSendBytesNum; //已发送字节数
    quint64 hasRevBytesNum;   //已接收字节数


    struct repeatingSendSetting{
        quint32 remainderRepeatingTimes;   //连发（当前剩余）次数
        quint32 totalRepeatTimes;   //总的连发次数
        bool repeatingForeverFlg;   //true 不计次数地连发, false 按照总次数连发
    } repeatingSendSettingEntity; //轮发模式下 总循环次数设置

    QIntValidator* repeatingTimesValidator;  //轮发模式下 总循环次数设置 的校验器

    bool  CheckRevExceptionSign;  //检查接收中的异常标识字符串

    //收发数据的字符编码模式
    EncodeMode  revEncodeMode;
    EncodeMode  senEncodeMode;

    //自动轮发占用串口标识信号量
    bool wheelSendingRunFlg;
    //文件发送占用串口标识信号量
    bool txtFileSendingRunFlg;    //true  正在发送文件

    //设置是否保存接收到的数据到文件
    bool saveRevDatToFileEn;


};

#endif // MAINWINDOW_H
