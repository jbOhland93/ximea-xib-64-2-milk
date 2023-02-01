#include "CamConfigurator.h"
#include "headers/global.h"

#define getP(param,field,type,description) if(m_err==XI_OK) {m_type=type; m_err = xiGetParam(m_cameraHandle, param, &field, &m_size, &m_type);if (m_err!=XI_OK)m_actionDescription=description;}
#define setPRM(method,param,value,description) if(m_err==XI_OK) {m_err = method(m_cameraHandle, param, value);if (m_err!=XI_OK)m_actionDescription=description;}
#define setInt(param,value,description) setPRM(xiSetParamInt, param, value, description)
#define setFloat(param,value,description) setPRM(xiSetParamFloat, param, value, description)

using namespace std;

CamConfigurator::CamConfigurator(HANDLE &cameraHandle, std::shared_ptr<Acquisitor> acq)
    : m_cameraHandle(cameraHandle), mp_acquisior(acq)
{
    if (!projectflags::USE_CAM)
        return; // Only configure the camera if it is supposed to be used ...

    // Camera performance
    setInt(XI_PRM_LIMIT_BANDWIDTH_MODE, XI_OFF, "Unset limit bandwidth mode");

    // Get camera sensor parameters
    getP(XI_PRM_WIDTH, m_width, XI_PRM_TYPE::xiTypeInteger, "Get sensor width");
    getP(XI_PRM_HEIGHT, m_height, XI_PRM_TYPE::xiTypeInteger, "Get sensor height");
    getP(XI_PRM_WIDTH XI_PRM_INFO_INCREMENT, m_width_inc, XI_PRM_TYPE::xiTypeInteger, "Get sensor width increment");
    getP(XI_PRM_HEIGHT XI_PRM_INFO_INCREMENT, m_height_inc, XI_PRM_TYPE::xiTypeInteger, "Get sensor height increment");
    
    // Setting "exposure" parameter to 7500 fps equivalent
    setExposureTime_us(1e6/7500.);

    // Set default ROI
    int roiSize = 400;
    setROI(roiSize, roiSize, (m_width - roiSize)/2, (m_height - roiSize)/2);
}

int CamConfigurator::getSensorWidth()
{
    return m_width;
}

int CamConfigurator::getSensorHeight()
{
    return m_height;
}

int CamConfigurator::getROIwidthIncrement()
{
    return m_width_inc;
}

int CamConfigurator::getROIheihtIncrement()
{
    return m_height_inc;
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

shared_ptr<vector<int>> CamConfigurator::setROI(int roiW, int roiH, int roiX, int roiY)
{
    if (!projectflags::USE_CAM)
        return nullptr;

    roiW = roiW/m_width_inc;
    roiW *= m_width_inc;
    roiH = roiH/m_height_inc;
    roiH *= m_height_inc;

    roiX = roiX/m_width_inc;
    roiX *= m_width_inc;
    roiY = roiY/m_height_inc;
    roiY *= m_height_inc;

    setInt(XI_PRM_OFFSET_X, 0, "Set ROI x to 0");
    setInt(XI_PRM_OFFSET_Y, 0, "Set ROI y to 0");
    setInt(XI_PRM_WIDTH, roiW, "Set ROI width");
    setInt(XI_PRM_HEIGHT, roiH, "Set ROI height");
    setInt(XI_PRM_OFFSET_X, roiX, "Set ROI x");
    setInt(XI_PRM_OFFSET_Y, roiY, "Set ROI y");

    int getW;
    int getH;
    int getX;
    int getY;
    getP(XI_PRM_WIDTH, getW, XI_PRM_TYPE::xiTypeInteger, "Get ROI width");
    getP(XI_PRM_HEIGHT, getH, XI_PRM_TYPE::xiTypeInteger, "Get ROI height");
    getP(XI_PRM_OFFSET_X, getX, XI_PRM_TYPE::xiTypeInteger, "Get ROI x");
    getP(XI_PRM_OFFSET_Y, getY, XI_PRM_TYPE::xiTypeInteger, "Get ROI y");
    shared_ptr<vector<int>> actualROI = shared_ptr<vector<int>>(new vector<int>());
    actualROI.get()->push_back(getW);
    actualROI.get()->push_back(getH);
    actualROI.get()->push_back(getX);
    actualROI.get()->push_back(getY);

    return actualROI;
}

bool CamConfigurator::setFreeRun()
{
    if (!projectflags::USE_CAM)
        return false;
    
    setInt(XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FREE_RUN, "Set timing mode");
    int getTimingMode;
    getP(XI_PRM_ACQ_TIMING_MODE, getTimingMode, XI_PRM_TYPE::xiTypeInteger, "Get timing mode");
    return getTimingMode == XI_ACQ_TIMING_MODE_FREE_RUN;
}

float CamConfigurator::setFrameRateLimit(float fps_limit)
{
    if (!projectflags::USE_CAM)
        return -1;

    setInt(XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE_LIMIT, "Set timing mode");
    int getTimingMode;
    getP(XI_PRM_ACQ_TIMING_MODE, getTimingMode, XI_PRM_TYPE::xiTypeInteger, "Get timing mode");
    if (m_err == XI_OK && getTimingMode != XI_ACQ_TIMING_MODE_FRAME_RATE_LIMIT)
    {
        m_err = 1;
        m_actionDescription = "XI_PRM_ACQ_TIMING_MODE could not be set to frame rate limit.";
        return -1;
    }

    setFloat(XI_PRM_FRAMERATE, fps_limit, "Set fps limit");
    float getFPS;
    getP(XI_PRM_FRAMERATE, getFPS, XI_PRM_TYPE::xiTypeFloat, "Get fps limit");
    return getFPS;
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
