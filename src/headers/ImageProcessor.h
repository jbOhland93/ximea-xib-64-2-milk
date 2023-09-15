#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#ifndef __cplusplus
    #define __cplusplus
#endif

// Include ISIO headers
#include "ImageStreamIO.h"
#include "ImageStruct.h"

// A class that processes the images acquired by the acquisitor
class ImageProcessor
{
    public:
        // Ctor, initializing the milk stream
        ImageProcessor(int width, int height);
        // Destructor, deleting the milk stream
        ~ImageProcessor();

        // Sets the region of interest of the camera
        void setImSize(int width, int height);

        // Returns the pointer to the current image buffer
        void* getBufferPtr();
        // Returns the size of the allocated buffer
        int getPayloadSize();

        // Posts the semaphores and increases the circular buffer
        void updateImage();
    private:
        char * mp_streamname = (char*)"ximeaCam";
        IMAGE m_image;
        int m_width;
        int m_height;

        ImageProcessor();
        void initializeStream(int width, int height);
};

#endif // IMAGEPROCESSOR_H
