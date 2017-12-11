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

    const QStringList& supportedReadFormats();
    const QStringList& supportedWriteFormats();

    ImageCreator();
    void setResolution(const Resolution& res);
    const Resolution& getResolution();
    QStringList setFiles(const QStringList& files);
    std::tuple<Error,const QImage*> render();

private:
    void clearFetchedImages();
    void orderImagesBySize();
    void cropEmptyFromImage(ImageDescriptor* image);
    QRect searchForEmptySpace(ImageDescriptor* imageDescriptor);
    Error addImage(ImageDescriptor* imageDescriptor);

private:
    Resolution mWorkingResolution;
    QList<ImageDescriptor*> mFetchedImages;
    QImage* mImage;
};


