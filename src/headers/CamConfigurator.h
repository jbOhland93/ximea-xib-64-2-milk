#ifndef CAMCONFIGURATOR_H
#define CAMCONFIGURATOR_H

#include <vector>
#include <memory>
#include <m3api/xiApi.h>
#include "Acquisitor.h"

// A class that handles the acquisition of the camera
class CamConfigurator
{
    public:
        // Ctor, taking the camera handle and the acquisitor
        CamConfigurator(HANDLE& cameraHandle, std::shared_ptr<Acquisitor> acq);

        // Get the sensor data
        int getSensorWidth();
        int getSensorHeight();
        int getROIwidthIncrement();
        int getROIheihtIncrement();

        float setExposureTime_us(float exposureTime_us);
        std::shared_ptr<std::vector<int>> setROI(int roiW, int roiH, int roiX, int roiY);
        std::shared_ptr<std::vector<int>> getROI();

        // Sets the camera into freerun mode
        bool setFreeRun();
        // Sets a limit to the cameras framerate
        float setFrameRateLimit(float fps_limit);

        // Returns true if an error is present
        bool error();
        // Returns an error description that can be printed
        std::string getErrorDescription();
        // Clears any errors that are present
        void clearError();

    private:
        const HANDLE& m_cameraHandle;
        const std::shared_ptr<Acquisitor> mp_acquisior;
        // The error returned by the last command.
        XI_RETURN m_err = XI_OK;
        // The size argument, used to get parameter values
        DWORD m_size;
        // The type argument, used to get parameter values
        XI_PRM_TYPE m_type;
        // The description of the last error
        std::string m_actionDescription = "No Error";

        int m_height;
        int m_width;
        int m_height_inc;
        int m_width_inc;
};

#endif // CAMCONFIGURATOR_H
