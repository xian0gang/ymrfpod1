/*******************
1）热像仪1控制软件
2）负责连接热像仪2并控制
3）解析主控命令
4）连接并解析测试软件
*********************/

#include "widget.h"
#include "ui_widget.h"

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
    server_master->listen(QHostAddress::Any, 9002);
    connect(server_master,SIGNAL(newConnection()),this,SLOT(TCP_master_Connect()));

    //测试软件服务端 9001
    server = new QTcpServer();
    tcp_server = new QTcpSocket();
    server->listen(QHostAddress::Any, 9001);
    connect(server,SIGNAL(newConnection()),this,SLOT(NewConnect()));

}

Widget::~Widget()
{
//    flir_close();
    delete ui;
}


/*************************************** 主控 tcp *********************************************************/
////主控
void Widget::TCP_master_Connect()
{
    tcp_server_master = server_master->nextPendingConnection();

    connect(tcp_server_master, SIGNAL(readyRead()), this, SLOT(socket_master_Read_Data()));
//    connect(tcp_server_master, SIGNAL(disconnected()), this, SLOT(TcpClose()));
    ui->textBrowser->append("测试服务端连接成功");
}

////主控
void Widget::socket_master_Read_Data()
{
    QByteArray buffer;
    buffer = tcp_server_master->readAll();
    qDebug()<<"read:"<<buffer.toHex();
    if(buffer.at(0) == 0x52)
    {
        switch (buffer.at(1)) {
        //0x01 记录开始和停止
        case 0x01:
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
            if(buffer.at(9) == 0x01)
            {
                str = str + "开始记录";
            }
            if(buffer.at(9) == 0x02)
            {
                str = str + "停止记录";
            }
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
            ui->textBrowser->append("数据记录仪维护自检");
            break;
        }
        case 0x06:
        {
            if(buffer.at(8) == 0x01)
                ui->textBrowser->append("关机");
            if(buffer.at(8) == 0x02)
                ui->textBrowser->append("重启");
            break;
        }
        default:
            break;
        }

    }

}

/*************************************** 测试服务端 *********************************************************/
//测试服务端
void Widget::NewConnect()
{
    qDebug("测试服务端 connect");
    tcp_server = server->nextPendingConnection();

    connect(tcp_server, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(tcp_server, SIGNAL(disconnected()), this, SLOT(TcpClose()));
    ui->textBrowser->append("测试服务端连接成功");
}


//测试服务端
void Widget::TcpClose()
{
    tcp_server->deleteLater();
    qDebug() <<"TcpClose";
    tcp_masterConnect_flag = false;
    ui->textBrowser->append("测试服务端断开连接");
}

//测试服务端
void Widget::ReadMessage()
{
    QByteArray ba;
    ba = tcp_server->readAll();
    qDebug()<<"read:"<<ba;
    ui->textBrowser->append(ba);

//测试软件控制命令
    if(ba == "FLIR_Camera_Ctr")
    {
        qDebug()<<"FLIR_Camera_Ctr";
        camera_ctr_one = true;
        QString str2 = "Ctr success";
        QByteArray data2(str2.toLatin1());
        tcp_server->write(data2);
        return;
    }


    if(ba == "FLIR_Camera_quit")
    {
        qDebug()<<"FLIR_Camera_quit";
        camera_ctr_one = false;
        QString str2 = "quit success";
        QByteArray data2(str2.toLatin1());
        tcp_server->write(data2);

        tcp_server->close();
        tcp_masterConnect_flag = false;
        return;
    }


    /*********************************    热像仪2   **************************************/
    //热像仪2 连接
        if((ba.indexOf("FLIR_init2") >= 0) ||
            (ba.indexOf("FLIR_connect2") >= 0) ||
                (ba.indexOf("FLIR_close2") >= 0) ||
                (ba.indexOf("FLIR_getfrequency2") >= 0) ||
                (ba.indexOf("FLIR_setfrequency2") >= 0) ||
                (ba.indexOf("FLIR_filter2") >= 0) ||
                (ba.indexOf("FLIR_getit2") >= 0) ||
                (ba.indexOf("FLIR_setit2") >= 0) ||
                (ba.indexOf("FLIR_SetMultiTi2") >= 0) ||
                (ba.indexOf("FLIR_warning2") >= 0) ||
                (ba.indexOf("FLIR_setnuc2") >= 0))
        {
            socket->write(ba);
            ui->textBrowser->append("热像仪2数据转发");
            return;
        }
/*********************************    热像仪1   **************************************/
    //热像仪1 模块初始化
    if(ba == "FLIR_init1")
    {
        qDebug()<<"FLIR_Init";

        flir_init();
        ui->textBrowser->append("初始化完成");
        return;
    }

    if(init_flag)
    {
        //热像仪1 设置频率
        if(ba.indexOf("FLIR_setfrequency1:") >= 0)
        {
            QString str = ba;
            QString num = str.section(':', 1, 1);
            int success  = flir_setfrequency(num.toInt());
            QString str2 = "FLIR_setfrequency1:";
            str2 = str2 + QString::number(success, 10);
            QByteArray data(str2.toLatin1());
            tcp_server->write(data);
            if(success == 0)
            {
                ui->textBrowser->append("频率设置失败");
            }
            else
            {
                ui->textBrowser->append("频率设置成功");
            }
        }

        //热像仪1 获取频率
        if(ba.indexOf("FLIR_getfrequency1") >= 0)
        {
            qDebug()<<"FLIR_Camera_GetFrequency";
            int frequency = flir_getfrequency();
            QString str = "FLIR_getfrequency1:";
            str = str + QString::number(frequency, 10);
            QByteArray data(str.toLatin1());
            tcp_server->write(data);
            if(frequency == 0)
            {
                ui->textBrowser->append("频率获取失败");
            }
            else
            {
                ui->textBrowser->append("频率获取成功");
            }
        }

        //热像仪1 连接
        if(ba.indexOf("FLIR_connect1:") >= 0)
        {
            QString str = ba;
            QString num = str.section(':', 1, 1);
            flir_connect(num.toInt());
            ui->textBrowser->append("连接热像仪成功");
        }

        //滤片
        if(ba.indexOf("FLIR_filter1:") >= 0)
        {

            QString str = ba;
            QString num = str.section(':', 1, 1);
            qDebug()<<num.toInt();
            int success = flir_setfilter(num.toInt());
            QString str2 = "FLIR_filter1:";
            str2 = str2 + QString::number(success, 10);
            QByteArray data(str2.toLatin1());
            tcp_server->write(data);
            if(success == 0)
            {
                ui->textBrowser->append("切换滤片失败");
            }
            else
            {
                ui->textBrowser->append("切换滤片成功");
            }

        }

        if(ba.indexOf("FLIR_close1") >= 0)
        {
            qDebug()<<"FLIR_close1";
            flir_close();
            ui->textBrowser->append("驱动注销成功，可关闭程序");
        }

        if(ba.indexOf("FLIR_getit1:") >= 0)
        {
            QString str = ba;
            QString num = str.section(':', 1, 1);
            qDebug()<<num.toInt();

            int it = flir_getit(num.toInt());
            str = str + ":" + QString::number(it, 10);
            QByteArray data(str.toLatin1());
            tcp_server->write(data);
            if(it == 0)
            {
                ui->textBrowser->append("获取积分时间为0或者失败");
            }
            else
            {
                ui->textBrowser->append("获取积分时间成功");
            }
        }

        if(ba.indexOf("FLIR_setit1:") >= 0)
        {
            QString str = ba;
            QString index = str.section(':', 1, 1);
            QString it = str.section(':', 2, 2);
            qDebug()<<index.toInt();
            qDebug()<<it.toInt();

            int success = flir_setit(index.toInt(), it.toInt());

            QString str2 = "FLIR_setit1:";
            str2 = str2 + QString::number(success, 10);
            QByteArray data(str2.toLatin1());
            tcp_server->write(data);

            QString txt = "设置通道";
            if(success == 0)
            {
                txt = txt + index + "的积分时间失败";
            }
            else
            {
                txt = txt + index + "的积分时间成功";
            }
            ui->textBrowser->append(txt);
        }

        if(ba.indexOf("FLIR_SetMultiTi:") >= 0)
        {
            QString str = ba;
            QString num = str.section(':', 1, 1);
            QString index = str.section(':', 2, 2);
            int    nChannel = num.toInt(); //积分通道
            int success = vcam.SetMultiTi(nChannel);
            if(success == 0)
            {
                ui->textBrowser->append("开启积分通道失败");
            }
            else
            {
                ui->textBrowser->append("开启积分通道成功");
            }
            QString str2 = "FLIR_SetMultiTi:";
            str2 = str2 + index + ":" + QString::number(success, 10);
            QByteArray data(str2.toLatin1());
            tcp_server->write(data);
        }

        if(ba.indexOf("FLIR_setnuc1") >= 0)
        {
            int success = flir_setnuc();
            QString str2 = "FLIR_setnuc1:";
            str2 = str2 + QString::number(success, 10);
            QByteArray data(str2.toLatin1());
            tcp_server->write(data);
        }

    }
    else
    {
        QString str2 = "FLIR_warning:init";
        QByteArray data(str2.toLatin1());
        tcp_server->write(data);
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
        socket->connectToHost("192.168.1.100", 9000);

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
//    FLIR_LIBInitialize();

//    mylib = new QLibrary("FLIR_LIB");
//    mylib->load();
//    if(!mylib->isLoaded())
//    {
//        qDebug()<<"load FLIR_Matlab_SDK_Lib failed";
//    }
//    typedef void (*fun)();
//    fun FLIR_Init = (fun)mylib->resolve("?FLIR_Init@@YAXXZ");
//    FLIR_Init();
//    ui->textBrowser->append("模块初始化完成");
////    vcam = new QVCamServer;


//    int ret=vcam.ScanCams();
//    ret=vcam.ConnectToCamScanned(0);
//    if (!ret)
//    {
//        Sleep(2000);
//        ret=vcam.ScanCams();
//        ret=vcam.ConnectToCamScanned(0);
//    }
//    Sleep(1000);


//    QString str2 = "FLIR_init1";
//    QByteArray data(str2.toLatin1());
//    tcp_server->write(data);

//    init_flag = true;
}

int Widget::flir_connect(int num)
{
//    typedef void (*fun1)(int nargout, mwArray& bSuccess, const mwArray& bAutomaticConnection);
//    fun1 FLIR_Camera_Connect = (fun1)mylib->resolve("?FLIR_Camera_Connect@@YAXHAAVmwArray@@ABV1@@Z");
//    int connect = 1;

//    mwArray mwa(1,1,mxINT32_CLASS);
//    mwArray mwb(1,1,mxINT32_CLASS);
//    //bAutomaticConnection 1 自动连接
//    //bAutomaticConnection 0 手动连接
//    int a[1] = {num};
//    mwb.SetData(a,1);
//    FLIR_Camera_Connect(connect, mwa, mwb);
//    int success = mwa.Get(1,1);
////    qDebug()<<success;
//    int success2 = mwb.Get(1,1);
////    qDebug()<<success2;


//    int aa = 0;
//    ITchannels = vcam.GetMultiNbrChannel((int&)aa);

//    if(ITchannels != 4)
//    {
//        LONG    nChannel =0x04; //积分通道
//        int success3 = vcam.SetMultiTi(nChannel);
//        if(success3 == 0)
//        {
//            ui->textBrowser->append("开启多积分通道失败");
//        }
//        else
//        {
//            ui->textBrowser->append("开启多积分通道成功");
//        }
//    }

//    QString str3 = "FLIR_GetMultiNbrChannel1:";
//    str3 = str3 + QString::number(ITchannels, 10);
//    QByteArray data3(str3.toLatin1());
//    tcp_server->write(data3);

}


int Widget::flir_close()
{
//    typedef void (*fun1)();
//    fun1 FLIR_Close = (fun1)mylib->resolve("?FLIR_Close@@YAXXZ");
//    FLIR_Close();
//    init_flag = false;
}



int Widget::flir_getfrequency()
{
//    typedef void (*fun1)(int nargout, mwArray& bSuccess, const mwArray& bAutomaticConnection);
//    fun1 FLIR_Camera_GetFrequency = (fun1)mylib->resolve("?FLIR_Camera_GetFrequency@@YAXHAAVmwArray@@0@Z");
//    int connect = 2;

//    mwArray mwa(1,1,mxINT32_CLASS);
//    mwArray mwb(1,1,mxINT32_CLASS);
//    FLIR_Camera_GetFrequency(connect, mwa, mwb);
//    int success = mwa.Get(1,1);
//    qDebug()<<success;
//    int success2 = mwb.Get(1,1);
//    qDebug()<<success2;
//    if(success == 0)
//    {
//        qDebug("get Frequency failed");
//        return 0;
//    }
////    ui->frequency_label->setText(QString::number(success2,10));
//    return  success2;
}

int Widget::flir_setfrequency(int frequency)
{
//    typedef void (*fun1)(int nargout,
//                         mwArray& bSuccess,
//                         mwArray& bSucceededToSetFrequency,
//                         mwArray& fActualFrequency,
//                         const mwArray& fFrequency,
//                         const mwArray& bAutoFrequency);
//    fun1 FLIR_Camera_SetFrequency = (fun1)mylib->resolve("?FLIR_Camera_SetFrequency@@YAXHAAVmwArray@@00ABV1@1@Z");
//    int connect = 2;

//    mwArray mwa(1,1,mxINT32_CLASS);
//    mwArray mwb(1,1,mxINT32_CLASS);
//    mwArray mwc(1,1,mxINT32_CLASS);
//    mwArray mwd(1,1,mxINT32_CLASS);
//    mwArray mwe(1,1,mxINT32_CLASS);
//    int a[1] = {0};
//    mwd.SetData(a,1);
//    int b[1] = {frequency};
//    mwc.SetData(b,1);

//    FLIR_Camera_SetFrequency(connect, mwa, mwb, mwe, mwc, mwd);
//    int success = mwa.Get(1,1);
//    int success2 = mwb.Get(1,1);

//    return success2;
}

int Widget::flir_setfilter(int index)
{
//    typedef void (*fun1)(int nargout,
//                         mwArray& bSuccess,
//                         const mwArray& dwFilterIndex);
//    fun1 FLIR_Camera_SetFilter = (fun1)mylib->resolve("?FLIR_Camera_SetFilter@@YAXHAAVmwArray@@ABV1@@Z");
//    int connect = 1;

//    mwArray mwa(1,1,mxINT32_CLASS);
//    mwArray mwb(1,1,mxINT32_CLASS);
//    int b[1] = {index};
//    mwb.SetData(b,1);

//    FLIR_Camera_SetFilter(connect, mwa, mwb);
//    int success = mwa.Get(1,1);

//    return success;
}

//获取积分时间
int Widget::flir_getit(int index)
{
//    typedef void (*fun1)(int nargout,
//                         mwArray& bSuccess,
//                         mwArray& fIT,
//                         const mwArray& nChannel);
//    fun1 FLIR_Camera_GetIT = (fun1)mylib->resolve("?FLIR_Camera_GetIT@@YAXHAAVmwArray@@0ABV1@@Z");
//    int connect = 1;

//    mwArray mwa(1,1,mxINT32_CLASS);
//    mwArray mwb(1,1,mxINT32_CLASS);
//    mwArray mwc(1,1,mxINT32_CLASS);
//    int c[1] = {index};
//    mwc.SetData(c,1);

//    FLIR_Camera_GetIT(connect, mwa, mwb, mwc);
//    int success = mwa.Get(1,1);
//    int it = mwb.Get(1,1);

//    if(success == 0)
//    {
////        ui->textBrowser->append("获取积分时间失败！");
//        return 0;
//    }

//    return it;
}

int Widget::flir_setit(int index, int it)
{
//    ULONG dwIntegration = it;
//    ULONG dwDelay = 0x00;

//    ULONG bAutoFrequency = FALSE;
//    uint success2 = vcam.SetIntegration(dwIntegration,dwDelay,index,bAutoFrequency);
//    return success2;
}

int Widget::flir_setnuc()
{
//    uint hr;
//    uint ret;

//    //计算1point NUC
//    uint bNUCIsToDo = TRUE; // NUC is to be calculate
//    int nNucType = 1; // 1 pt NUC type
//    int nMethode = 2; // BB method
//    uint bKeepGain = FALSE; // Keep previous Gain option
//    int nNumberOfAverageFrame = 10; // Number of frame
//    uint bSave = FALSE; //Save after calcul option


//    ret=vcam.ScanCams();
//    ret=vcam.ConnectToCamScanned(0);
//    if (!ret)
//    {
//        Sleep(2000);
//        ret=vcam.ScanCams();
//        ret=vcam.ConnectToCamScanned(0);
//    }
//    Sleep(1000);

//    //Set NUC to be Caluclate
//    hr = vcam.SetDoNuc(bNUCIsToDo);
//    qDebug()<<hr;
//    //Choose 1 pt NUC type
//    hr = vcam.SetNucType(nNucType);
//    qDebug()<<hr;
//    //Choose Black body method
//    //Note that you can only do one point NUC with BB method or shutter one
//    hr = vcam.SetMethode(nMethode);
//    qDebug()<<hr;
//    //Choose Keep previous Gain
//    hr = vcam.SetKeepGain(bKeepGain);
//    qDebug()<<hr;
//    //Set Average number of frames
//    hr = vcam.SetAverageFrames(nNumberOfAverageFrame);
//    qDebug()<<hr;
//    //Don’t save after NUC calcul
//    hr = vcam.SetSaveAfterUpdate(bSave);
//    qDebug()<<hr;
//    //Calculate NUC and update informations
//    hr =vcam.UpdateNuc();
//    qDebug()<<hr;

//    if(hr == 0)
//    {
//        ui->textBrowser->append("非均匀性矫正失败！");
//    }
//    else
//    {
//        ui->textBrowser->append("非均匀性矫正成功！");
//    }
//    return hr;
}
