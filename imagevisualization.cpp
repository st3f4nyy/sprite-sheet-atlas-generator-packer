#include "imagevisualization.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <tuple>

ImageVisualization::ImageVisualization(ExportSettings* exportSettings,
                                       FileSystemExplorer* explorer,
                                       QWidget* parent) :
    QWidget(parent),mDisplayLabel(nullptr)
{
    mExportButton = new QPushButton("Export",this);
    mRenderButton = new QPushButton("Render",this);
    mExportSettings = exportSettings;
    mFileSystemExplorer = explorer;
    connect(mFileSystemExplorer,&FileSystemExplorer::selectedFilesChanged,
            this,&ImageVisualization::selectedFilesChanged);
    connect(mExportSettings,&ExportSettings::resolutionChanged,
            this,&ImageVisualization::resolutionChanged);
    connect(mRenderButton,&QPushButton::clicked,
            this,&ImageVisualization::renderButtonPressed);
    mContainerSize = QSize(parent->geometry().width(),parent->geometry().height());

    setResolution();
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(mDisplayLabel);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(mRenderButton);
    buttonLayout->addWidget(mExportButton);
    layout->addLayout(buttonLayout);
    this->setLayout(layout);
}

void ImageVisualization::onContainerResize(const QSize& size)
{
    mContainerSize = size;
    setPixmap(mCurrentImage);
}

void ImageVisualization::setResolution()
{
    Resolution res = mExportSettings->getBaseResolution();
    mImageCreator.setResolution(res);
    if(mDisplayLabel==nullptr) mDisplayLabel=new QLabel(this);
    this->drawTransparency();
}

void ImageVisualization::setPixmap(const QImage& image)
{
    int size = mContainerSize.height() * 0.75;
    mDisplayLabel->setPixmap(QPixmap::fromImage(image)
                .scaled(size,size, Qt::KeepAspectRatio));
}

void ImageVisualization::selectedFilesChanged(QStringList seletectedFiles)
{
    const auto errors = mImageCreator.setFiles(seletectedFiles);
    if(errors.empty()==false)
        QMessageBox::critical(this,"error",errors.at(0));
}

void ImageVisualization::resolutionChanged(const Resolution&)
{
    this->setResolution();
}

QImage ImageVisualization::getTransparencyImage()
{
    Resolution res = mExportSettings->getBaseResolution();
    QImage img(res.res(),res.res(),QImage::Format_ARGB32);
    auto fillRect = [&] (const QRect& rect, const QColor& color) {
        for(int x=rect.left(); x <= rect.right() && x < img.width(); x++)
        {
            for(int y=rect.top(); y <= rect.bottom() && y < img.height(); y++)
            {
                img.setPixelColor(x,y,color);
            }
        }
    };
    QColor black (100,100,100);
    QColor white (200,200,200);
    bool toggle=true;
    const int BLOCK_SIZE=16;
    for(int x=0; x*BLOCK_SIZE < img.width(); x++)
    {
        toggle=!toggle;
        for(int y=0 ; y*BLOCK_SIZE < img.height() ; y++)
        {
            QRect rect= QRect(QPoint(x*BLOCK_SIZE,y*BLOCK_SIZE),
                              QPoint(x*BLOCK_SIZE+BLOCK_SIZE,y*BLOCK_SIZE+BLOCK_SIZE));
            if(toggle) fillRect(rect,black);
            else fillRect(rect,white);
            toggle=!toggle;
        }
    }
    return img;
}

void ImageVisualization::drawTransparency()
{
    mCurrentImage = getTransparencyImage();
    setPixmap(mCurrentImage);
}

void ImageVisualization::renderButtonPressed(bool){
    std::tuple<QString,const QImage*> r = mImageCreator.render();
    QString error = std::get<0>(r);
    const QImage* image = std::get<1>(r);
    if(error.length()!=0)
    {
        QMessageBox::critical(this,"Error",error);
    }
    else
    {
        QImage finalImg = getTransparencyImage();
        for(int x=0; x < finalImg.width(); x++)
        {
            for(int y=0; y < finalImg.height(); y++)
            {
                const auto imgColor = image->pixelColor(x,y);
                if(imgColor.alpha()!=0)
                    finalImg.setPixelColor(x,y,imgColor);
            }
        }
        setPixmap(finalImg);
    }

}
