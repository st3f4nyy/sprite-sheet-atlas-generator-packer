#pragma once

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QWindow>
#include <QMainWindow>
#include <QThread>
#include <QProgressDialog>
#include "exportsettings.h"
#include "filesystemexplorer.h"
#include "imagecreator.h"


class ImageVisualizationRender;
class ImageVisualization : public QWidget
{
    Q_OBJECT
public:
    ImageVisualization(ExportSettings* exportSettings,
                       FileSystemExplorer* explorer,
                       QWidget* parent = Q_NULLPTR);

    void onContainerResize(const QSize& size);
    static QImage getTransparencyImage(const Resolution& res);

private slots:
    void renderButtonPressed(bool checked);
    void showRenderingProgress(int progress, QString msg);
    void renderingProgressEnded();
    void showRenderingProgressError(QString msg);

private:
    void setResolution(const Resolution& res);
    void setPixmap(const QImage& pixmap);

private:
    QRect mSize;
    QLabel* mDisplayLabel;
    ExportSettings* mExportSettings;
    FileSystemExplorer* mFileSystemExplorer;
    ImageCreator mImageCreator;
    QPushButton* mExportButton;
    QPushButton* mRenderButton;
    QImage mCurrentImage;
    QImage mTransParencyImage;
    QSize mContainerSize;
    QProgressDialog* mProgressDialog;
    ImageVisualizationRender* mRender;
};

class ImageVisualizationRender : public QThread
{
    Q_OBJECT
public:
    ImageVisualizationRender(const QStringList& files,
                             const ExportSettings* settings);

    virtual ~ImageVisualizationRender();
    void run() override;
    const QImage* getImageWithTransparency() const {return mImageWithTransparency;}
    const QImage* getImage() const {return mImage;}

signals:
    void showPogress(int progress, QString msg);
    void error(QString msg);
    void renderEnded();

private:
    void resetState();
    QImage generateImageWithTransparency(const QImage* transparency,
                                       const QImage* other);

private:
    QImage* mImage;
    QImage* mTransparencyImage;
    QImage* mImageWithTransparency;
    QStringList* mFiles;
    const ExportSettings* mSettings;
};

