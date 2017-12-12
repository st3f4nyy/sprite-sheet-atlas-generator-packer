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
    void selectedFilesChanged(QStringList seletectedFiles);
    void resolutionChanged(const Resolution& resolution);
    void renderButtonPressed(bool checked);

private:
    void setResolution();
    QImage getTransparencyImage();
    void drawTransparency();
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
    QSize mContainerSize;
};

