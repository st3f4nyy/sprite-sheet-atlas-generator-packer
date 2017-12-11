#include "imagecreator.h"
#include "configuration.h"
#include <QStringList>
#include <QImageReader>
#include <algorithm>

class ImageDescriptor
{
public:
    ImageDescriptor(QImage* image)
    {
        this->image = image;
    }

    ~ImageDescriptor() {
        delete image;
        image=nullptr;
    }

    QRect croped;
    QImage* image;
    QString path;
    QRect position;
};


static QStringList getSupportedReadFormats()
{
    QStringList r;
    r.append("BMP");
    r.append("GIF");
    r.append("JPG");
    r.append("JPEG");
    r.append("PNG");
    r.append("PBM");
    r.append("PGM");
    r.append("PPM");
    r.append("XBM");
    r.append("XPM");
    return r;
}

static QStringList getSupportedWriteFormats()
{
    QStringList r;
    r.append("BMP");
    r.append("JPG");
    r.append("JPEG");
    r.append("PNG");
    r.append("PPM");
    r.append("XBM");
    r.append("XPM");
    return r;
}

static QStringList SUPPORTED_READ_FORMATS = getSupportedReadFormats();
static QStringList SUPPORTED_WRITE_FORMAT = getSupportedWriteFormats();

const QStringList& ImageCreator::supportedReadFormats()
{
    return SUPPORTED_READ_FORMATS;
}

const QStringList& ImageCreator::supportedWriteFormats()
{
    return SUPPORTED_WRITE_FORMAT;
}

ImageCreator::ImageCreator()
{
    mImage=nullptr;
    mWorkingResolution = Configuration::baseResolution();
}

void ImageCreator::clearFetchedImages()
{
    for(ImageDescriptor* descriptor : mFetchedImages) delete descriptor;
    mFetchedImages.clear();
}

void ImageCreator::setResolution(const Resolution &res)
{
    mWorkingResolution = res;
}

const Resolution& ImageCreator::getResolution()
{
    return mWorkingResolution;
}

void ImageCreator::cropEmptyFromImage(ImageDescriptor* descriptor)
{
    QImage* image= descriptor->image;
    const auto size = image->size();
    QRect cropRect;
    int x,y,cropAt;
    {   // calculate TOP Crop
        cropAt=0;
        for( y=0 ; y < size.height() ; y++ )
        for( x=0 ; x < size.width()  ; x++ )
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) {cropAt=y; break;}
        }
        cropRect.setTop(cropAt);
    }
    {   // calculate LEFT Crop
        for( x=0; x < size.width(); x++ )
        for( y=0; y < size.height(); y++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) {cropAt=x; ;break;}
        }
        cropRect.setLeft(cropAt);
    }
    {
        // calculate RIGHT Crop
        for(x=size.width()-1; x>=0 ; x--)
        for(y=0; y < size.height(); y++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) {cropAt=x; ;break;}
        }
        cropRect.setRight(cropAt);
    }
    {
        // calculate BOTTOM Crop
        for(y=size.height()-1; y >=0 ; y--)
        for(x=0; x < size.width(); x++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) {cropAt=y; break;}
        }
        cropRect.setBottom(cropAt);
    }

    // crop
    if(cropRect.left()!=0
       || cropRect.top()!=0
       || cropRect.right()!=image->width()
       || cropRect.height()!=image->height())
    {
        QImage copy = image->copy(cropRect);
        *image = copy;
        descriptor->croped= cropRect;
    }
}

QStringList ImageCreator::setFiles(const QStringList &filePaths)
{
    QStringList errors;
    this->clearFetchedImages();
    for(auto path : filePaths)
    {
        QImageReader* reader = new QImageReader();
        QImage* image = new QImage();
        reader->setFileName(path);
        if(reader->read(image)==false)
        {
            delete image; image=nullptr;
            errors.append(reader->errorString());
        } else
        {
            mFetchedImages.append(new ImageDescriptor(image));
        }
    }

    return errors;
}

void ImageCreator::orderImagesBySize()
{
    auto lessThan = [] (const ImageDescriptor* ad, const ImageDescriptor* bd) -> bool {
        QImage* a = ad->image; QImage* b = bd->image;
        int sizea = a->size().width() * a->size().height();
        int sizeb = b->size().width() * b->size().height();
        return sizea < sizeb;
    };
    std::sort(mFetchedImages.begin(),mFetchedImages.end(),lessThan);
}


std::tuple<ImageCreator::Error,const QImage*> ImageCreator::render()
{
    if(mImage!=nullptr) delete mImage;
    mImage = new QImage(mWorkingResolution.res(),
                        mWorkingResolution.res(),
                        QImage::Format_ARGB32);
    for(auto des : mFetchedImages) this->cropEmptyFromImage(des);
    this->orderImagesBySize();
    for(auto des : mFetchedImages)
    {
        auto error = addImage(des);
        if(error!=NO_ERROR)
            return std::tuple<ImageCreator::Error,const QImage*>(error,nullptr);
    }
    return std::tuple<ImageCreator::Error,const QImage*>(NO_ERROR,mImage);
}

ImageCreator::Error ImageCreator::addImage(ImageDescriptor* imageDescriptor)
{
    QImage* img = imageDescriptor->image;
    if(img->width()<=0 || img->height()<=0) return EMPTY_IMAGE;
    QRect rect = searchForEmptySpace(imageDescriptor);
    if(rect.width()<=0 || rect.height()<=0) return INSUFICIENT_SPACE;
    for(int x = 0; x < rect.width(); x++)
    {
        for(int y=0; y < rect.height(); y++)
        {
            const auto pixel = img->pixelColor(x,y);
            mImage->setPixelColor(rect.x()+x,rect.y()+y,pixel);
        }
    }
    imageDescriptor->position=rect;
    return NO_ERROR;
}

QRect ImageCreator::searchForEmptySpace(ImageDescriptor* imageDescriptor)
{
    auto findNextCleanPixel = [this] (QPoint point) -> QPoint {
        int x=point.x(), y= point.y();
        while(y<=mImage->height())
        {
            while(x<=mImage->width())
            {
                const auto pixel = mImage->pixelColor(x,y);
                if(pixel.alpha()==0)
                {
                    return QPoint(x,y);
                } else
                {
                    x++;
                    if(x>mImage->width())
                    {
                        x = 0;
                        y++;
                    }
                }
            }
        }
        return QPoint(mImage->width()+1,mImage->height()+1); // invalid
    };

    auto enoughSpace = [this](QPoint point, QImage* image) -> bool {
        if(point.x()+image->width()>mImage->width()) return false;
        if(point.y()+image->height()>mImage->height()) return false;

        for(int x=0; x<image->width(); x++)
        {
            for(int y=0; y < image->height() ; y++)
            {
                const auto pixel = mImage->pixelColor(x+point.x(),y+point.y());
                if(pixel.alpha()!=0) return false;
            }
        }

        return true;
    };

    for(QPoint point(0,0);
        point.x() <= mImage->width() && point.y() <= mImage->height()
        && point.x()>0 && point.y()>0;
        point = findNextCleanPixel(point))
    {
        if(enoughSpace(point,imageDescriptor->image))
        {
            QPoint topLeft = QPoint(point.x(),point.y());
            QPoint bottomRight =
                    QPoint(point.x()+imageDescriptor->image->width(),
                           point.y()+imageDescriptor->image->height());
            return QRect(topLeft,bottomRight);
        }
    }

    return QRect(0,0,0,0);
}
