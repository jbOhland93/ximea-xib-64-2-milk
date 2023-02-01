#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(int width, int height)
{
    init_ImageStreamIO();
    setImSize(width, height);
}

ImageProcessor::~ImageProcessor()
{
    // This does not work, for some reason ...
    // The stream still apears in milk-streamCTRL.
    ImageStreamIO_closeIm(&m_image);
}

void *ImageProcessor::getBufferPtr()
{
    void* bufferPointer;
    ImageStreamIO_writeBuffer(&m_image, &bufferPointer);
    return bufferPointer;
}

int ImageProcessor::getPayloadSize()
{
    return m_width*m_height*2;
}

void ImageProcessor::updateImage()
{
    ImageStreamIO_UpdateIm(&m_image);
}

void ImageProcessor::setImSize(int width, int height)
{
    initializeStream(width, height);
    m_width = width;
    m_height = height;
}

void ImageProcessor::initializeStream(int width, int height)
{
    int naxis = 2;
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
