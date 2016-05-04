#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>


//保存接收数据到文件
bool MainWindow::saveRevDatToFile (){
    QString saveFilePath;
    saveFilePath = pSaveFileSetting->getRevDatSaveFilePath();

    //打开文件
    QFile file(saveFilePath);
    bool openFlag;
    openFlag = file.open(QIODevice::Append | QIODevice::Text);
    if(openFlag == false)
    {
       QMessageBox::critical(this, tr("Error"), "NeolithUart cannot open the selected file.");
       return false;
    }

    //写入数据
    QTextStream stream(&file);
    //写入固定字符串内容
    //stream << "== NeolithUart Dump File ==\r" << endl;

    //写入接收数据
    //ui->texBro_receiveDat->append ("~~^.^~~");
    stream << ui->texBro_receiveDat->toPlainText ()<<endl;

    //关闭文件
    file.close();

    //QMessageBox::information(this, tr("Success!"), "NeolithUart output RevDat to file was successful!");

    return true;

}//saveRevDatToFile();

