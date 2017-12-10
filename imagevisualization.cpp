#include "imagevisualization.h"
#include <QVBoxLayout>

ImageVisualization::ImageVisualization(ExportSettings* exportSettings,QWidget* parent) : QWidget(parent)
{
    mExportSettings = exportSettings;
    QVBoxLayout* layout = new QVBoxLayout(this);
    mDisplayLabel = new QLabel(this);
    mPixmap = new QPixmap(400,400);
    mDisplayLabel->setPixmap(*mPixmap);
    layout->addWidget(mDisplayLabel);
    this->setLayout(layout);
}
