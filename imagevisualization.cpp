#include "imagevisualization.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <tuple>

ImageVisualization::ImageVisualization(ExportSettings* exportSettings,
                                       FileSystemExplorer* explorer,
                                       QWidget* parent) :
    QWidget(parent),mDisplayLabel(nullptr),mProgressDialog(nullptr),mRender(nullptr)
{
    mExportButton = new QPushButton("Export",this);
    mRenderButton = new QPushButton("Render",this);
    mExportSettings = exportSettings;
    mFileSystemExplorer = explorer;
    connect(mRenderButton,&QPushButton::clicked,
            this,&ImageVisualization::renderButtonPressed);
    mContainerSize = QSize(parent->geometry().width(),parent->geometry().height());

    setResolution(Resolution::R256);
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

void ImageVisualization::setResolution(const Resolution& res)
{
    if(mDisplayLabel==nullptr) mDisplayLabel=new QLabel(this);
    mTransParencyImage = getTransparencyImage(res);
    if(mCurrentImage.isNull()) mCurrentImage=mTransParencyImage;
    setPixmap(mCurrentImage);
}

void ImageVisualization::setPixmap(const QImage& image)
{
    int size = mContainerSize.height() * 0.75;
    if(&mCurrentImage != &image) mCurrentImage=image;
    mDisplayLabel->setPixmap(QPixmap::fromImage(image)
                .scaled(size,size, Qt::KeepAspectRatio));
}

QImage ImageVisualization::getTransparencyImage(const Resolution& res)
{
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
    const int BLOCK_SIZE=res.res()/16;
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

void ImageVisualization::renderButtonPressed(bool){
    if(mRender==nullptr)
    {
        mRender = new ImageVisualizationRender(mFileSystemExplorer->getSelectedFilesPath(),
                                               mExportSettings);
        connect(mRender,&ImageVisualizationRender::showPogress,
                this,&ImageVisualization::showRenderingProgress);
        connect(mRender,&ImageVisualizationRender::renderEnded,
                this,&ImageVisualization::renderingProgressEnded);
        connect(mRender,&ImageVisualizationRender::error,
                this,&ImageVisualization::showRenderingProgressError);
        mRender->start();
    }
}

class CustomProgressDialog : public QProgressDialog
{
public:
    CustomProgressDialog(QString title,int start, int end, QWidget* parent):
        QProgressDialog("","",start,end,parent)
    {
        this->setWindowModality(Qt::WindowModal);
        this->setWindowTitle(title);
        this->setCancelButton(nullptr);
        this->setAutoClose(false);
        this->show();
    }

    void closeEvent(QCloseEvent *event) override
    {
        event->ignore();
    }
};

void ImageVisualization::showRenderingProgress(int progress, QString msg)
{
    if(mProgressDialog==nullptr)
    {
        mProgressDialog = new CustomProgressDialog ("Rendering",0,100,this);
    }
    mProgressDialog->setLabelText(msg);
    mProgressDialog->setValue(progress);
}

void ImageVisualization::renderingProgressEnded()
{
    if(mProgressDialog!=nullptr)
    {
        mProgressDialog->hide();
        delete mProgressDialog; mProgressDialog=nullptr;
    }
    if(mRender!=nullptr)
    {
        const QImage* finalImage = mRender->getImageWithTransparency();
        if(finalImage!=nullptr)
        {
            this->setPixmap(*finalImage);
        }
        delete mRender; mRender=nullptr;
    }
}

void ImageVisualization::showRenderingProgressError(QString msg)
{
    this->renderingProgressEnded();
    QMessageBox::critical(this,"Rendering error",msg);
}

ImageVisualizationRender::ImageVisualizationRender(const QStringList &files,
                                                   const ExportSettings *settings):
    mImage(nullptr),mTransparencyImage(nullptr),mImageWithTransparency(nullptr),
    mFiles(new QStringList(files)),mSettings(settings)
{
}

ImageVisualizationRender::~ImageVisualizationRender()
{
    if(mFiles!=nullptr) delete mFiles;
    this->resetState();
}

void ImageVisualizationRender::resetState()
{
    if(mImageWithTransparency!=nullptr) delete mImageWithTransparency;
    if(mTransparencyImage!=nullptr) delete mTransparencyImage;
    if(mImage!=nullptr) delete mImage;
}

void ImageVisualizationRender::run()
{
    this->resetState();
    this->showPogress(0,"Starting base image rendering");
    auto showBaseImageProgress = [&](int progress, QString msg) {
        this->showPogress(progress/90,msg);
    };
    ImageCreator imageCreator;
    ImageCreator::RenderRet ret = imageCreator.render(*mFiles,mSettings,showBaseImageProgress);
    QString error = std::get<0>(ret);
    const QImage* retImage = std::get<1>(ret);
    if(error.isEmpty()==false)
    {
        this->error(error);
    }
    else
    {
        mImage = new QImage(*retImage);
        this->showPogress(95,"Generating transparency");
        mTransparencyImage = new QImage(
                    ImageVisualization::getTransparencyImage(mSettings->getBaseResolution()));
        this->showPogress(97,"Generating final image");
        mImageWithTransparency = new QImage(
                this->generateImageWithTransparency(mTransparencyImage,mImage));
        this->renderEnded();
    }
}

QImage ImageVisualizationRender::generateImageWithTransparency(const QImage* transparency,
                                                             const QImage* other)
{
    QImage finalImg(*transparency);
    for(int x=0; x < finalImg.width(); x++)
    {
        for(int y=0; y < finalImg.height(); y++)
        {
            const auto imgColor = other->pixelColor(x,y);
            if(imgColor.alpha()!=0)
                finalImg.setPixelColor(x,y,imgColor);
        }
    }
    return finalImg;
}
