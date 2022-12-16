#include <iostream>
#include <unistd.h>

// Ximea includes
#include <m3api/xiApi.h>
#include <memory>
#include <cstring>

// Application includes
#include "headers/UserInputHandler.h"
#include "headers/Acquisitor.h"
#include "headers/CamConfigurator.h"

using namespace std;

// === Helper functions ===

// Tries to connect to the camera and returns an error code.
XI_RETURN connectToCamera(HANDLE& cameraHandle);

// === Main function ===
int main()
{
    // Establish a camera connection
    HANDLE xiH;
    XI_RETURN stat = connectToCamera(xiH);
    // If the connection is successfull, launch the CLI.
    if (stat == XI_OK)
    {
        // Close the stderr stream at this point.
        // This is to avoid the xiAPI to spam status messages into the CLI.
        fclose(stderr);

        // Initiate the acquisitor
        std::shared_ptr<Acquisitor> p_acq = std::shared_ptr<Acquisitor>( new Acquisitor(xiH) );
        // Initiate the cam configurator, which sets the default parameters
        std::shared_ptr<CamConfigurator> p_cconf = std::shared_ptr<CamConfigurator>( new CamConfigurator(xiH, p_acq) );
        // Start the CLI
        UserInputHandler ui(p_acq, p_cconf);
        std::thread uiThread = thread(&UserInputHandler::core, &ui);
        uiThread.join();

        xiCloseDevice(xiH);
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
    return XI_OK;
}
