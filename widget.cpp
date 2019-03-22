/*******************
1）热像仪1控制软件
2）负责连接热像仪2并控制
3）解析主控命令
4）连接并解析测试软件
*********************/

#include "widget.h"
#include "ui_widget.h"
#include <windows.h>
#include <QSettings>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    bytesWritten = 0;
    nextBlockSize = 0;
    index = 0;
    data_len = 0;
    init_flag = false;

    ReadSetTing();
    uartInit();

    ui->textBrowser->document()->setMaximumBlockCount(1500);

    camera_ctr_one = false;
    camera_ctr_two = false;

    //热像仪2 tcp 客户端 9000
    socket = new QTcpSocket();
    tm = new QTimer(this);
    tcp_connect_flag = false;
    connect(tm, SIGNAL(timeout()), this, SLOT(TCP_Connect()));
    tm->start(3000);
    ui->textBrowser->append("热像仪2 tcp 客户端建立完成");

    //主控服务端 9002
    server_master = new QTcpServer();
    tcp_server_master = new QTcpSocket();
    int masterport = ui->masterport_lineEdit->text().toInt();
    server_master->listen(QHostAddress::Any, masterport);
    connect(server_master,SIGNAL(newConnection()),this,SLOT(TCP_master_Connect()));

}

Widget::~Widget()
{
//    flir_close();
    WriteSetTing();
    delete ui;
}

void Widget::uartInit()
{
    serial = new QSerialPort;
    QString uartPort = ui->uart_lineEdit->text();
    serial->setPortName("com" + uartPort);
    bool open_flag;
    open_flag = serial->open(QIODevice::ReadWrite);
    if(!open_flag)
    {

        qDebug()<<"open fail";
        return;
    }
    else
    {

    }

    serial->setBaudRate(QSerialPort::Baud115200);//设置波特率为115200
    serial->setDataBits(QSerialPort::Data8);//设置数据位8
    serial->setParity(QSerialPort::NoParity); //校验位设置为0
    serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
    serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制

    connect(serial, SIGNAL(readyRead()), this, SLOT(uartReadData()));
}

//读取程序启动状态信息
int Widget::ReadSetTing()
{
    //设置配置文件的目录和位置，如果有，则不动，没有，会自动创建
    QSettings setting("setting.ini",QSettings::IniFormat);
    QString ip;
    QString port;
    QString masterport;
    QString testport;
    QString uartPort;
    if(setting.contains(tr("net/ip_one"))&&setting.contains(tr("net/port_one")))//如果已经存在这个文件，那就进行读取
    {
        ip = setting.value("net/ip_one").toString();//将读取出的数据进行使用
        port = setting.value("net/port_one").toString();
        masterport = setting.value("net/masterport").toString();//将读取出的数据进行使用
        testport = setting.value("net/testport").toString();
        uartPort = setting.value("net/uartPort").toString();

        ui->ip_lineEdit->setText(ip);
        ui->port_lineEdit->setText(port);
        ui->masterport_lineEdit->setText(masterport);
        ui->testport_lineEdit->setText(testport);
        ui->uart_lineEdit->setText(uartPort);
    }
    return 0;
}

//写程序状态信息
int Widget::WriteSetTing()
{
    QSettings setting("setting.ini",QSettings::IniFormat);
    QString ip;
    QString port;
    QString masterport;
    QString testport;
    QString uartPort;
    setting.beginGroup(tr("net"));//节点开始

    ip = ui->ip_lineEdit->text();
    port = ui->port_lineEdit->text();
    masterport = ui->masterport_lineEdit->text();
    testport = ui->testport_lineEdit->text();
    uartPort = ui->uart_lineEdit->text();

    setting.setValue("ip_one",ip);//设置key和value，也就是参数和值
    setting.setValue("port_one",port);
    setting.setValue("masterport",masterport);//设置key和value，也就是参数和值
    setting.setValue("testport",testport);
    setting.setValue("uartPort",uartPort);

    setting.endGroup();//节点结束

    return 0;
}


/*************************************** 主控 tcp *********************************************************/

void Widget::TCP_master_Connect()
{
    tcp_server_master = server_master->nextPendingConnection();

    connect(tcp_server_master, SIGNAL(readyRead()), this, SLOT(socket_master_Read_Data()));
//    connect(tcp_server_master, SIGNAL(disconnected()), this, SLOT(TcpClose()));
    ui->textBrowser->append("测试服务端连接成功");
}

void Widget::socket_master_Read_Data()
{
    QByteArray buffer;
    buffer = tcp_server_master->readAll();
    header* head = (header*)buffer.data();
    qDebug()<<"read:"<<buffer.toHex();
    if(head->type == 0x52)
    {
        switch (head->opcode) {
        //0x01 记录开始和停止
        case 0x01:
        {
            QString str;
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x21;
            ba[3] = 0x00;
            ba[4] = 0x02;
            if(buffer.at(8) == 0x01)
            {
                str = "热像仪1";
                ba[5] = 0x01;
            }
            if(buffer.at(8) == 0x02)
            {
                str = "热像仪2";
                ba[5] = 0x02;
            }
            if(buffer.at(8) == 0x03)
            {
                str = "热像仪1和热像仪2";
                ba[5] = 0x03;
            }
            if(buffer.at(9) == 0x01)
            {
                str = str + "开始记录";
                ba[6] = 0x01;
            }
            if(buffer.at(9) == 0x02)
            {
                str = str + "停止记录";
                ba[6] = 0x02;
            }
            serial->write(ba);
            ui->textBrowser->append(str);
            break;
        }
        case 0x04:
        {
            QString str;
            if(buffer.at(8) == 0x01)
            {
                str = "热像仪1";
            }
            if(buffer.at(8) == 0x02)
            {
                str = "热像仪2";
            }
            if(buffer.at(8) == 0x03)
            {
                str = "热像仪1和热像仪2";
            }
            str = str + "NUC校正";
            ui->textBrowser->append(str);
            break;
        }
        case 0x05:
        {
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x13;
            ba[3] = 0x00;
            ba[4] = 0x01;
            ba[5] = 0xff;
            serial->write(ba);
            ui->textBrowser->append("数据记录仪维护自检");
            break;
        }
        case 0x06:
        {
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x11;
            ba[3] = 0x00;
            ba[4] = 0x01;
            ba[5] = buffer.at(8);
            serial->write(ba);
            if(buffer.at(8) == 0x01)
                ui->textBrowser->append("关机");
            if(buffer.at(8) == 0x02)
                ui->textBrowser->append("重启");
            break;
        }
        case 0x0b:
        {
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x15;
            ba[3] = 0x00;
            ba[4] = 0x01;
            ba[5] = 0xff;
            serial->write(ba);
            ui->textBrowser->append("读中波容量");
            break;
        }
        case 0x0c:
        {
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x17;
            ba[3] = 0x00;
            ba[4] = 0x01;
            ba[5] = 0xff;
            serial->write(ba);
            ui->textBrowser->append("读长波容量");
            break;
        }
        case 0x0d:
        {
            QByteArray ba;
            ba[0] = 0x07;
            ba[1] = 0x55;
            ba[2] = 0x19;
            ba[3] = 0x00;
            ba[4] = 0x01;
            ba[5] = 0xff;
            serial->write(ba);
            ui->textBrowser->append("读模拟视频容量");
            break;
        }
        default:
            break;
        }

    }

}

void Widget::uartReadData()
{
    QByteArray buffer;
    buffer = serial->readAll();
    qDebug()<<"uart read:"<<buffer.toHex();
    switch (buffer.at(2)) {
    case 0x22:
        switch (buffer.at(5))
        {
        case 0x01:
            qDebug("开始记录成功");
            break;
        case 0x02:
            qDebug("开始记录不成功");
            break;
        case 0x03:
            qDebug("停止记录成功");
            break;
        case 0x04:
            qDebug("停止记录不成功");
            break;
        default:
            break;
        }
        break;
    case 0x12:
        switch (buffer.at(5))
        {
        case 0x01:
            qDebug("关机成功");
            break;
        case 0x02:
            qDebug("关机不成功");
            break;
        case 0x03:
            qDebug("重启成功");
            break;
        case 0x04:
            qDebug("重启不成功");
            break;
        default:
            break;
        }
        break;
    case 0x14:
        switch (buffer.at(5))
        {
        case 0x01:
            qDebug("自检成功");
            break;
        case 0x02:
            qDebug("自检不成功");
            break;
        default:
            break;
        }
        break;
    case 0x16:
        qDebug("中波容量");
        break;
    case 0x18:
        break;
    case 0x1a:
        break;
        default:
            break;
    }
}

/*************************************** 热像仪2 tcp*********************************************************/
//热像仪2
void Widget::TCP_Connect()
{
    if(!tcp_connect_flag)
    {
//        qDebug()<<"取消连接";
        socket->abort();

//        qDebug()<<"开始连接";
        QString ip = ui->ip_lineEdit->text();
        QString port = ui->port_lineEdit->text();
        socket->connectToHost(ip, port.toInt());

        if(!socket->waitForConnected(300))
        {
//            qDebug("连接热像仪2服务器失败！");
            ui->textBrowser->append("连接热像仪2服务器失败！");
            tcp_connect_flag = false;
        }
        else
        {
            qDebug("连接热像仪2服务器成功！");
            ui->textBrowser->append("连接热像仪2服务器成功！");
            tcp_connect_flag = true;
            tm->stop();
        }

        connect(socket,SIGNAL(readyRead()),this,SLOT(socket_Read_Data()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(repeat_connect_tcp()));
    }
}

//热像仪2
void Widget::socket_Read_Data()
{
    QByteArray buffer;
    buffer = socket->readAll();
    qDebug()<<"read:"<<buffer;
    //热像仪2 连接
    if((buffer.indexOf("FLIR_init2") >= 0) ||
        (buffer.indexOf("FLIR_connect2") >= 0) ||
            (buffer.indexOf("FLIR_close2") >= 0) ||
            (buffer.indexOf("FLIR_getfrequency2") >= 0) ||
            (buffer.indexOf("FLIR_setfrequency2") >= 0) ||
            (buffer.indexOf("FLIR_filter2") >= 0) ||
            (buffer.indexOf("FLIR_getit2") >= 0) ||
            (buffer.indexOf("FLIR_setit2") >= 0) ||
            (buffer.indexOf("FLIR_SetMultiTi2") >= 0) ||
            (buffer.indexOf("FLIR_warning2") >= 0) ||
            (buffer.indexOf("FLIR_GetMultiNbrChannel2") >= 0) ||
            (buffer.indexOf("FLIR_setnuc2") >= 0))
    {
        tcp_server->write(buffer);
    }
}

//热像仪2
void Widget::repeat_connect_tcp()
{
    tm->start(3000);
    tcp_connect_flag = false;
}


/*************************************** 热像仪1 控制 *********************************************************/
//初始化
int Widget::flir_init()
{
    int ret=vcam.ScanCams();
    ret=vcam.ConnectToCamScanned(0);
    if (!ret)
    {
        Sleep(2000);
        ret=vcam.ScanCams();
        ret=vcam.ConnectToCamScanned(0);
    }
    Sleep(1000);


    QString str2 = "FLIR_init1";
    QByteArray data(str2.toLatin1());
    tcp_server->write(data);

    init_flag = true;
}

int Widget::flir_connect(int num)
{
    int aa = 0;
    ITchannels = vcam.GetMultiNbrChannel((int&)aa);

    if(ITchannels != 4)
    {
        int    nChannel =0x04; //积分通道
        int success3 = vcam.SetMultiTi(nChannel);
        if(success3 == 0)
        {
            ui->textBrowser->append("开启多积分通道失败");
        }
        else
        {
            ui->textBrowser->append("开启多积分通道成功");
        }
    }

    QString str3 = "FLIR_GetMultiNbrChannel1:";
    str3 = str3 + QString::number(ITchannels, 10);
    QByteArray data3(str3.toLatin1());
    tcp_server->write(data3);

}


int Widget::flir_close()
{

}



int Widget::flir_getfrequency()
{

}

int Widget::flir_setfrequency(int frequency)
{

}

int Widget::flir_setfilter(int index)
{

}

//获取积分时间
int Widget::flir_getit(int index)
{

}

int Widget::flir_setit(int index, int it)
{
    uint dwIntegration = it;
    uint dwDelay = 0x00;

    uint bAutoFrequency = FALSE;
    uint success2 = vcam.SetIntegration(dwIntegration,dwDelay,index,bAutoFrequency);
    return success2;
}

int Widget::flir_setnuc()
{
    uint hr;
    uint ret;

    //计算1point NUC
    uint bNUCIsToDo = 1; // NUC is to be calculate
    int nNucType = 1; // 1 pt NUC type
    int nMethode = 2; // BB method
    uint bKeepGain = 0; // Keep previous Gain option
    int nNumberOfAverageFrame = 10; // Number of frame
    uint bSave = FALSE; //Save after calcul option


    ret=vcam.ScanCams();
    ret=vcam.ConnectToCamScanned(0);
    if (!ret)
    {
        Sleep(2000);
        ret=vcam.ScanCams();
        ret=vcam.ConnectToCamScanned(0);
    }
    Sleep(1000);

    //Set NUC to be Caluclate
    hr = vcam.SetDoNuc(bNUCIsToDo);
    qDebug()<<hr;
    //Choose 1 pt NUC type
    hr = vcam.SetNucType(nNucType);
    qDebug()<<hr;
    //Choose Black body method
    //Note that you can only do one point NUC with BB method or shutter one
    hr = vcam.SetMethode(nMethode);
    qDebug()<<hr;
    //Choose Keep previous Gain
    hr = vcam.SetKeepGain(bKeepGain);
    qDebug()<<hr;
    //Set Average number of frames
    hr = vcam.SetAverageFrames(nNumberOfAverageFrame);
    qDebug()<<hr;
    //Don’t save after NUC calcul
    hr = vcam.SetSaveAfterUpdate(bSave);
    qDebug()<<hr;
    //Calculate NUC and update informations
    hr =vcam.UpdateNuc();
    qDebug()<<hr;

    if(hr == 0)
    {
        ui->textBrowser->append("非均匀性矫正失败！");
    }
    else
    {
        ui->textBrowser->append("非均匀性矫正成功！");
    }
    return hr;
}
