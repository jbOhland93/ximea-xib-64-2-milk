#include "CamConfigurator.h"
#include "headers/global.h"

#define getP(param,field,type,description) if(m_err==XI_OK) {m_type=type; m_err = xiGetParam(m_cameraHandle, param, &field, &m_size, &m_type);if (m_err!=XI_OK)m_actionDescription=description;}
#define setInt(param,value,description) if(m_err==XI_OK) {m_err = xiSetParamInt(m_cameraHandle, param, value);if (m_err!=XI_OK)m_actionDescription=description;}

using namespace std;

CamConfigurator::CamConfigurator(HANDLE &cameraHandle, std::shared_ptr<Acquisitor> acq)
    : m_cameraHandle(cameraHandle), mp_acquisior(acq)
{
    if (!projectflags::USE_CAM)
        return; // Only configure the camera if it is supposed to be used ...

    // Setting "exposure" parameter to 7500 fps equivalent
    float exposure_us = 1e6/7500.;
    setInt(XI_PRM_EXPOSURE, exposure_us, "Set exposure time");

    // Camera performance
    setInt(XI_PRM_LIMIT_BANDWIDTH_MODE, XI_OFF, "Unset limit bandwidth mode");

    // Get camera sensor parameters
    getP(XI_PRM_WIDTH, m_width, XI_PRM_TYPE::xiTypeInteger, "Get sensor width");
    getP(XI_PRM_HEIGHT, m_height, XI_PRM_TYPE::xiTypeInteger, "Get sensor height");
    getP(XI_PRM_WIDTH XI_PRM_INFO_INCREMENT, m_width_inc, XI_PRM_TYPE::xiTypeInteger, "Get sensor width increment");
    getP(XI_PRM_HEIGHT XI_PRM_INFO_INCREMENT, m_height_inc, XI_PRM_TYPE::xiTypeInteger, "Get sensor height increment");
        
    // Set default ROI
    int roiSize = 400;
    int roiX;
    int roiY;
    int roiW;
    int roiH;  
    roiH = roiSize/m_height_inc;
    roiH *= m_height_inc;
    roiW = roiSize/m_width_inc;
    roiW *= m_width_inc;
    roiX = (m_width - roiW)/2;
    roiY = (m_height - roiH)/2;

    setInt(XI_PRM_WIDTH, roiW, "Set ROI width");
    setInt(XI_PRM_HEIGHT, roiH, "Set ROI height");
    setInt(XI_PRM_OFFSET_X, roiX, "Set ROI x");
    setInt(XI_PRM_OFFSET_Y, roiY, "Set ROI y");
}

float CamConfigurator::setExposureTime_us(float exposureTime_us)
{
    if (!projectflags::USE_CAM)
        return -1; // Only configure the camera if it is supposed to be used ...

    setInt(XI_PRM_EXPOSURE, exposureTime_us, "Set exposure time");
    if (m_err != XI_OK)
        return -1;

    float setTime;
    getP(XI_PRM_EXPOSURE, setTime, XI_PRM_TYPE::xiTypeFloat, "Get exposure time");
    if (m_err != XI_OK)
        return -1.;
    return setTime;
}

bool CamConfigurator::error()
{
    return m_err != XI_OK;
}

std::string CamConfigurator::getErrorDescription()
{
    if (!projectflags::USE_CAM)
        return "Camera unused (projectflags::USE_CAM == false).\n";
        
    string msg = m_actionDescription;
    msg.append(" (");
    msg.append(to_string(m_err));
    msg.append(").\n");
    return msg;
}

void CamConfigurator::clearError()
{
    m_err = XI_OK;
}
