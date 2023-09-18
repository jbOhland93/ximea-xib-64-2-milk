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
    if (mp_image != nullptr)
    {
        ImageStreamIO_closeIm(mp_image);
        delete mp_image;
    }
}

void *ImageProcessor::getBufferPtr()
{
    return ImageStreamIO_get_image_d_ptr(mp_image);
}

int ImageProcessor::getPayloadSize()
{
    return m_width*m_height*2;
}

void ImageProcessor::updateImage()
{
    ImageStreamIO_UpdateIm(mp_image);
}

void ImageProcessor::setImSize(int width, int height)
{
    initializeStream(width, height);
    m_width = width;
    m_height = height;
}

void ImageProcessor::initializeStream(int width, int height)
{
    if (mp_image != nullptr)
        ImageStreamIO_closeIm(mp_image);
    else
        mp_image = new IMAGE;

    int naxis = 2;
    uint32_t * imsize = new uint32_t[naxis]();
    imsize[0] = width;
    imsize[1] = height;
    uint8_t atype = _DATATYPE_UINT16;
    int shared = 1; // image will be in shared memory
    int NBkw = 0; // No keywords for now. Do this later.
    int circBufSize = 10;
    ImageStreamIO_createIm_gpu(mp_image,
                            mp_streamname,
                            naxis,
                            imsize,
                            atype,
                            -1, // -1 = Host, >=0 = device
                            shared,
                            10,
                            NBkw,
                            MATH_DATA,
                            circBufSize);
    delete imsize;
}
