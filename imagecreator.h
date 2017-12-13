#pragma once
#include <QStringList>
#include <QImage>
#include <QRect>
#include <tuple>
#include "resolution.h"

class ImageDescriptor;

class ImageCreator
{
public:
    enum Error
    {
      NO_ERROR, INSUFICIENT_SPACE, EMPTY_IMAGE
    };
    typedef std::tuple<QString,const QImage*> RenderRet;

    const QStringList& supportedReadFormats();
    const QStringList& supportedWriteFormats();

    ImageCreator();
    RenderRet render(const QStringList& files,const Resolution& res);
private:
    const Resolution& getResolution();
    void setResolution(const Resolution& res);
    QStringList setFiles(const QStringList& files);
    void clearFetchedImages();
    void orderImagesBySize();
    void cropEmptyFromImage(ImageDescriptor* image);
    QRect searchForEmptySpace(ImageDescriptor* imageDescriptor);
    Error addImage(ImageDescriptor* imageDescriptor);
    QString getErrorMessage(Error error, ImageDescriptor* des);

private:
    Resolution mWorkingResolution;
    QList<ImageDescriptor*> mFetchedImages;
    QImage* mImage;
};


