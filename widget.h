#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//#include "FLIR_LIB.h"
#include <QLibrary>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTcpSocket>
#include <QTimer>
#include <QTcpServer>
#include <QProcess>
#include <QFile>
#include "qvcamserver.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    int WriteSetTing();
    int ReadSetTing();

private slots:

    int flir_init();
    int flir_connect(int auto);
    int flir_close();
    int flir_getfrequency();
    int flir_setfrequency(int frequency);
    int flir_setfilter(int index);
    int flir_getit(int index);
    int flir_setit(int index, int it);
    int flir_setnuc();

    void TCP_Connect();
    void socket_Read_Data();
    void repeat_connect_tcp();

    void TCP_master_Connect();
    void socket_master_Read_Data();

    //测试服务端
    void NewConnect();
    void ReadMessage();
    void TcpClose();


private:
    Ui::Widget *ui;
    QLibrary *mylib;      //加载动态库
    QTcpSocket *socket;
//    QTcpSocket *masterControl_tcp;
    bool tcp_connect_flag;
    bool tcp_masterConnect_flag;
    QTimer *tm;
    QTimer *tm_master;
    QVCamServer vcam;
    bool init_flag;

    //测试服务端
    QTcpServer *server;
    QTcpSocket *tcp_server;

    QTcpServer *server_master;
    QTcpSocket *tcp_server_master;
    bool camera_ctr_one;
    bool camera_ctr_two;
    int bytesWritten;
    quint64 nextBlockSize;

    quint32 data_len;
    quint32 index;
    QByteArray array;
    quint32 length;


    int ITchannels;
};

#endif // WIDGET_H
