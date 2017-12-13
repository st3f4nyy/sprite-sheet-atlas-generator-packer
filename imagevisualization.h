#pragma once

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QWindow>
#include <QMainWindow>
#include "exportsettings.h"
#include "filesystemexplorer.h"
#include "imagecreator.h"



class ImageVisualization : public QWidget
{
    Q_OBJECT
public:
    ImageVisualization(ExportSettings* exportSettings,
                       FileSystemExplorer* explorer,
                       QWidget* parent = Q_NULLPTR);

    void onContainerResize(const QSize& size);

private slots:
    void resolutionChanged(const Resolution& resolution);
    void renderButtonPressed(bool checked);

private:
    void setResolution(const Resolution& res);
    QImage getTransparencyImage(const Resolution& res);
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
};

