#include <iostream>
#include <unistd.h>

// Ximea includes
#include <m3api/xiApi.h>
#include <memory>
#include <cstring>

// Application includes
#include "headers/UserInputHandler.h"
#include "headers/Acquisitor.h"

// Ximea defines
#define HandleResult(res,place) if (res!=XI_OK) {printf("Error after %s (%d)\n",place,res);goto finish;}

using namespace std;

// === Helper functions ===

// Tries to connect to the camera and returns an error code.
XI_RETURN connectToCamera(HANDLE& cameraHandle);

XI_RETURN exampleAqcuisition(HANDLE& cameraHandle);

// === Main function ===
int main()
{
    HANDLE xiH;
    XI_RETURN stat = connectToCamera(xiH);
    if (stat == XI_OK)
    {
        exampleAqcuisition(xiH);

        xiCloseDevice(xiH);

    // )================================
        //UserInputHandler ui;
        //std::thread uiThread = thread(&UserInputHandler::core, &ui);
        //uiThread.join();

        //Acquisitor acq;
        //cout << "Is acquiring: " << acq.isAcquiring() << "\n";
        //cout << "Starting acq result: " << acq.startAcquisition() << "\n";
        //cout << "Is acquiring: " << acq.isAcquiring() << "\n";
        //usleep(1000000);
        //cout << "Stop\n";
        //cout << "Is acquiring: " << acq.isAcquiring() << "\n";
        //acq.stopAcquisition();
        //cout << "Is acquiring: " << acq.isAcquiring() << "\n";
    }
    else
    {
        printf("An error occured (%d)\n", stat);
        return 0;
    }
}

int connectToCamera(HANDLE& cameraHandle)
{
    XI_RETURN stat = XI_OK;
    stat = xiOpenDevice(0, &cameraHandle);
    if (stat != XI_OK)
    {
        printf("Error after %s (%d)\n","Open device",stat);
        return stat;
    }

    cout << "\nSetting Parameters:\n\n";
    
    // Setting "exposure" parameter to 7500 fps equivalent
    float exposure_us = 1e6/7500.;
    stat = xiSetParamInt(cameraHandle, XI_PRM_EXPOSURE, exposure_us);
    if (stat != XI_OK)
    {
        printf("Error after %s (%d)\n","xiSetParam (exposure set)",stat);
        return stat;
    }
    cout << "\nSet exposure time to " << exposure_us << " us";

    // Filled by xiGetParam
    INOUT DWORD size;
    // Has to match the type we need
    OUT XI_PRM_TYPE type = XI_PRM_TYPE::xiTypeFloat;
    stat = xiGetParam(cameraHandle, XI_PRM_EXPOSURE, &exposure_us, &size, &type);
    cout << ", camera returned " << exposure_us << " us.\n\n";

    // Camera performance
    xiSetParamInt(cameraHandle, XI_PRM_LIMIT_BANDWIDTH_MODE, XI_OFF);
    int limit_bandwidth_mode = 0;
    xiGetParamInt(cameraHandle, XI_PRM_LIMIT_BANDWIDTH_MODE, &limit_bandwidth_mode);
    cout << "XI_PRM_LIMIT_BANDWIDTH_MODE set to ";
    if (limit_bandwidth_mode == XI_OFF)
        cout << "off\n\n";
    else
        cout << "on\n\n";

    // Get camera ROI
    int roiX;
    int roiY;
    int roiW;
    int roiH;
    type = XI_PRM_TYPE::xiTypeInteger;
    int height_inc = 0;
    stat = xiGetParamInt(cameraHandle, XI_PRM_HEIGHT XI_PRM_INFO_INCREMENT, &height_inc);
    cout << "INCREMENT HEIGHT = " << height_inc << " (height must be divisible by this)\n";
    int width_inc = 0;
    stat = xiGetParamInt(cameraHandle, XI_PRM_WIDTH XI_PRM_INFO_INCREMENT, &width_inc);
    cout << "INCREMENT WIDTH = " << width_inc << " (width must be divisible by this\n";

    stat = xiGetParam(cameraHandle, XI_PRM_OFFSET_X, &roiX, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_OFFSET_Y, &roiY, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_WIDTH, &roiW, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_HEIGHT, &roiH, &size, &type);
    cout << "ROI before: " << roiX << ", " << roiY << "; " << roiW << "x"<<roiH<<"\n";

    int cX = roiX + roiW/2;
    int cY = roiY + roiH/2;
    int roiSize = 400;
    roiH = roiSize/height_inc;
    roiH *= height_inc;
    roiW = roiSize/width_inc;
    roiW *= width_inc;
    roiX = cX - roiW/2;
    roiY = cY - roiH/2;

    stat = xiSetParamInt(cameraHandle, XI_PRM_WIDTH, roiW);
    stat = xiSetParamInt(cameraHandle, XI_PRM_HEIGHT, roiH);
    stat = xiSetParamInt(cameraHandle, XI_PRM_OFFSET_X, roiX);
    stat = xiSetParamInt(cameraHandle, XI_PRM_OFFSET_Y, roiY);
    // Read back
    stat = xiGetParam(cameraHandle, XI_PRM_OFFSET_X, &roiX, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_OFFSET_Y, &roiY, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_WIDTH, &roiW, &size, &type);
    stat = xiGetParam(cameraHandle, XI_PRM_HEIGHT, &roiH, &size, &type);
    cout << "ROI after: " << roiX << ", " << roiY << "; " << roiW << "x"<<roiH<<"\n";

    cout << "\nCamera parameters set!\n\n";
    return stat;
}

XI_RETURN exampleAqcuisition(HANDLE& cameraHandle)
{
    // image buffer
    XI_IMG image;
    memset(&image,0,sizeof(image));
    image.size = sizeof(XI_IMG);

    printf("Starting example acquisition...\n");
    XI_RETURN stat = xiStartAcquisition(cameraHandle);
    if (stat != XI_OK)
    {
        printf("Error after %s (%d)\n","xiStartAcquisition",stat);
        return stat;
    }

    #define EXPECTED_IMAGES 10
    printf("Acquiring %d images:\n", EXPECTED_IMAGES);
    for (int images=0;images < EXPECTED_IMAGES;images++)
    {
            // getting image from camera
            stat = xiGetImage(cameraHandle, 5000, &image);
            if (stat != XI_OK)
            {
                printf("Error after %s (%d)\n","xiGetImage",stat);
                return stat;
            }
            unsigned char pixel = *(unsigned char*)image.bp;
            printf("Image %d (%dx%d) received from camera. First pixel value: %d\n", images, (int)image.width, (int)image.height, pixel);
    }

    printf("Stopping acquisition...\n");
    return xiStopAcquisition(cameraHandle);
}