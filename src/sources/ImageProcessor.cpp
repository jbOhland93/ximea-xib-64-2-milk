#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
{
    initializeStream();
}

ImageProcessor::~ImageProcessor()
{
    // This does not work, for some reason ...
    // The stream still apears in milk-streamCTRL.
    ImageStreamIO_closeIm(&m_image);

    /*void* bufferLocation;
    ImageStreamIO_writeBuffer(&m_image, &bufferLocation);
    for (int i = 0; i < 10; i++)
        *bufferLocation = i;
        bufferLocation++;*/
}

void ImageProcessor::initializeStream()
{
    int naxis = 2;
    int width = 10;
    int height = 10;
    uint32_t * imsize = new uint32_t[naxis]();
    imsize[0] = width;
    imsize[1] = height;
    uint8_t atype = _DATATYPE_UINT16;
    int shared = 1; // image will be in shared memory
    int NBkw = 0; // No keywords for now. Do this later.
    int circBufSize = 10;
    ImageStreamIO_createIm(&m_image,
                            mp_streamname,
                            naxis,
                            imsize,
                            atype, // called atype
                            shared,
                            NBkw,
                            circBufSize);
    delete imsize;
}
