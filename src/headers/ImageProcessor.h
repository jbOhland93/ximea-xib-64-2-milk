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
        ImageProcessor();
        // Destructor, deleting the milk stream
        ~ImageProcessor();
    private:
        char * mp_streamname = (char*)"my_deletable_steam2";
        IMAGE m_image;

        void initializeStream();
};

#endif // IMAGEPROCESSOR_H
