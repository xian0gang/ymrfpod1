#ifndef QVCAMSERVER_H
#define QVCAMSERVER_H


#include <QAxObject>
#include <qglobal.h>
class QVCamServer
{
public:
    QVCamServer();
    ~QVCamServer();
    int init(uint aut);
//    uint SetFrequency (double fFrequency, uint bAutoFrequency);
    uint GetFrequency (double& pfFrequency);
    uint GetIT(int nChannel, uint& pdwIt, uint& pdwDelay);
    uint GetCurrentIT(uint& pdwIt, uint& pdwDelay);
    uint GetCurrentFilter(uint& pdwFilter);
    uint GetCurrentNUC(int& pnNuc);
    uint GetMaxFrequency(double& pfMaxFrequency);
    uint IsMultiTI();
//    uint SetCurrentIT(uint dwIT, uint bAutoFrequency);

    uint ScanCams ();
    uint ConnectToCamScanned(int nIndexScannedCam);
    uint SetDoNuc (uint bDoNuc); //Set NUC to be Caluclate
    uint SetNucType (int nType);//Choose 1 pt NUC type
    //Choose Black body method
    //Note that you can only do one point NUC with BB method or shutter one
    uint SetMethode(int nMethode);
    uint SetKeepGain (uint bKeepGain);//Choose Keep previous Gain
    uint SetAverageFrames (int nFrames);//Set Average number of frames
    uint SetSaveAfterUpdate(uint bSaveAfterUpdate);//Don’t save after NUC calcul
    uint UpdateNuc ();
    uint SetMultiTi(int nbrMultiTI);
    //设置一个通道积分时间
    uint SetIntegration(uint dwIntegration, uint dwDelay, int nChannel, uint bAutoFrequency);
    uint SetCurrentNuc(int nNuc);//设置NUC号
    uint SetCurrentFilter(uint dwFilter);//设置滤光片位置号
    uint SetFrequency (double fFrequency, uint bAutoFrequency);//设置帧频
    uint SetCurrentIT (uint dwIT, uint bAutoFrequency);//设置一个单积分时间
    uint GetMultiNbrChannel (int& pnbrMultiTI);



private:
    QAxObject *object;
};

#endif // QVCAMSERVER_H
