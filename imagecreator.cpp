#include "imagecreator.h"
#include "configuration.h"
#include <QStringList>
#include <QImageReader>
#include <algorithm>

class ImageDescriptor
{
public:
    ImageDescriptor(QImage* image, const QString& path)
    {
        this->croped=QRect(-1,-1,-1,-1);
        this->position=QRect(-1,-1,-1,-1);
        this->path=path;
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
    if(mImage!=nullptr) delete mImage;
    mImage = new QImage(mWorkingResolution.res(),
                        mWorkingResolution.res(),
                        QImage::Format_ARGB32);
    mImage->fill(Qt::transparent);
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
        cropAt=-1;
        for( y=0 ; y < size.height() && cropAt<0; y++ )
        for( x=0 ; x < size.width()  && cropAt<0; x++ )
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) {cropAt=y;}
        }
        cropRect.setTop(cropAt);
    }
    {   // calculate LEFT Crop
        cropAt=-1;
        for( x=0; x < size.width() && cropAt<0 ; x++ )
        for( y=0; y < size.height() && cropAt<0; y++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) cropAt=x;
        }
        cropRect.setLeft(cropAt);
    }
    {
        // calculate RIGHT Crop
        cropAt=-1;
        for(x=size.width()-1; x>=0 && cropAt<0 ; x--)
        for(y=0; y < size.height() && cropAt<0 ; y++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) cropAt=x;
        }
        cropRect.setRight(cropAt);
    }
    {
        // calculate BOTTOM Crop
        cropAt=-1;
        for(y=size.height()-1; y >=0 && cropAt<0 ; y--)
        for(x=0; x < size.width() && cropAt<0 ; x++)
        {
            const auto pixel = image->pixelColor(x,y);
            if(pixel.alpha()!=0) cropAt=y;
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
            mFetchedImages.append(new ImageDescriptor(image,path));
        }
        delete reader;
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


QString ImageCreator::getErrorMessage(Error error, ImageDescriptor* des)
{
    switch(error)
    {

    case INSUFICIENT_SPACE:
        return QString("Not enough texture space to put the image %1").arg(des->path);
    case EMPTY_IMAGE:
        return QString("The image is empty %1").arg(des->path);
    default:
    case NO_ERROR: return "";
    }
}

ImageCreator::RenderRet ImageCreator::render(const QStringList& files,
                                             const ExportSettings* settings,
                                             std::function<void(int,QString)> showProgress)
{
    mShowProgress = showProgress;
    this->setResolution(settings->getBaseResolution());
    this->setFiles(files);
    for(auto des : mFetchedImages) this->cropEmptyFromImage(des);
    this->orderImagesBySize();
    for(auto des : mFetchedImages)
    {
        auto error = addImage(des);
        if(error!=NO_ERROR)
            return RenderRet(getErrorMessage(error,des),nullptr);
    }
    return RenderRet("",mImage);
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
    rect.setWidth(rect.width()-1);rect.setHeight(rect.height()-1);
    imageDescriptor->position=rect;
    return NO_ERROR;
}

QRect ImageCreator::searchForEmptySpace(ImageDescriptor* imageDescriptor)
{
    QImage* targetImage= imageDescriptor->image;

    auto findNextCleanPixel = [&] (QPoint point) -> QPoint {
        int x=point.x(), y= point.y();
        for(; x < mImage->width() && x+ targetImage->width() <= mImage->width(); x++)
        {
            for(; y < mImage->height() && y + targetImage->height() <= mImage->height(); y++)
            {
                bool used=false;
                int tx1=x, ty1=y, /*tx2=x+targetImage->width()-1,*/ty2=y+targetImage->height()-1;
                for(auto imgDes : mFetchedImages)
                {
                    int x1,x2,y1,y2; imgDes->position.getCoords(&x1,&y1,&x2,&y2);
                    used = tx1 >= x1 && tx1 <= x2 &&
                           ((ty1 >= y1 && ty1 <=y2) || (ty2 >= y1 && ty2 <=y2));
                    if(used)
                    {
                        y=y2+1;
                        break;
                    }
                }
                if(!used) return QPoint(x,y);
            }
            y=0;
        }


        return QPoint(mImage->width()+1,mImage->height()+1); // invalid
    };



    QPoint point = findNextCleanPixel(QPoint(0,0));
    if(point.x() < mImage->width() && point.y() < mImage->height())
    {
        QPoint topLeft = QPoint(point.x(),point.y());
        QPoint bottomRight =
                QPoint(point.x()+targetImage->width()-1,
                       point.y()+targetImage->height()-1);
        return QRect(topLeft,bottomRight);
    }
    return QRect(0,0,0,0);
}
