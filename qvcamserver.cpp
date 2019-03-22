#include "qvcamserver.h"

QVCamServer::QVCamServer()
{
    object = new QAxObject;
    object->setControl("VCamServer.VirtualCam");

}

QVCamServer::~QVCamServer()
{
    object->destroyed();
}

/***********************************************************************************************
*函数名 : init
*函数功能描述 :  初始化驱动
*函数参数 : 0 手动 1 自动
*函数返回值 : 1-成功 0-失败
***********************************************************************************************/
int QVCamServer::init(uint aut)
{
    uint result = object->dynamicCall("Init(uint)", aut).toUInt();
    return result;
}

uint QVCamServer::ScanCams()
{
    uint result = object->dynamicCall("ScanCams()").toUInt();
    return result;
}

uint QVCamServer::ConnectToCamScanned(int nIndexScannedCam)
{
    uint result = object->dynamicCall("ConnectToCamScanned(int)", nIndexScannedCam).toUInt();
    return result;
}

uint QVCamServer::SetDoNuc(uint bDoNuc)
{
    uint result = object->dynamicCall("SetDoNuc(uint)", bDoNuc).toUInt();
    return result;
}

uint QVCamServer::SetNucType(int nType)
{
    uint result = object->dynamicCall("SetNucType(int)", nType).toUInt();
    return result;
}

uint QVCamServer::SetMethode(int nMethode)
{
    uint result = object->dynamicCall("SetMethode(int)", nMethode).toUInt();
    return result;
}


uint QVCamServer::SetKeepGain(uint bKeepGain)
{
    uint result = object->dynamicCall("SetKeepGain(uint)", bKeepGain).toUInt();
    return result;
}

uint QVCamServer::SetAverageFrames(int nFrames)
{
    uint result = object->dynamicCall("SetAverageFrames(int)", nFrames).toUInt();
    return result;
}

uint QVCamServer::SetSaveAfterUpdate(uint bSaveAfterUpdate)
{
    uint result = object->dynamicCall("SetSaveAfterUpdate(uint)", bSaveAfterUpdate).toUInt();
    return result;
}

uint QVCamServer::UpdateNuc()
{
    uint result = object->dynamicCall("UpdateNuc()").toUInt();
    return result;
}

uint QVCamServer::SetMultiTi(int nbrMultiTI)
{
    uint result = object->dynamicCall("SetMultiTi(int)", nbrMultiTI).toUInt();
    return result;
}

uint QVCamServer::SetIntegration(uint dwIntegration, uint dwDelay, int nChannel, uint bAutoFrequency)
{
    QVariantList params;
    params<<dwIntegration;
    params<<dwDelay;
    params<<nChannel;
    params<<bAutoFrequency;
    uint result = object->dynamicCall("SetIntegration(uint, uint, int, uint)", params).toUInt();
    return result;
}

uint QVCamServer::SetCurrentNuc(int nNuc)
{
    uint result = object->dynamicCall("SetCurrentNuc(int)", nNuc).toUInt();
    return result;
}

uint QVCamServer::SetCurrentFilter(uint dwFilter)
{
    uint result = object->dynamicCall("SetCurrentFilter(uint)", dwFilter).toUInt();
    return result;
}

uint QVCamServer::SetFrequency(double fFrequency, uint bAutoFrequency)
{
    QVariantList params;
    params<<fFrequency;
    params<<bAutoFrequency;
    uint result = object->dynamicCall("SetFrequency(double, uint)", params).toUInt();
    return result;
}

uint QVCamServer::SetCurrentIT(uint dwIT, uint bAutoFrequency)
{
    QVariantList params;
    params<<dwIT;
    params<<bAutoFrequency;
    uint result = object->dynamicCall("SetCurrentIT(uint, uint)", params).toUInt();
    return result;
}

uint QVCamServer::IsMultiTI()
{
    uint result = object->dynamicCall("IsMultiTI()").toUInt();
    return result;
}

uint QVCamServer::GetMultiNbrChannel(int &pnbrMultiTI)
{
    uint result = object->dynamicCall("GetMultiNbrChannel(int&)", pnbrMultiTI).toUInt();
    return result;
}
