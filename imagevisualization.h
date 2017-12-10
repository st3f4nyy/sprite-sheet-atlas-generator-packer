#pragma once

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include "exportsettings.h"

class ImageVisualization : public QWidget
{
    Q_OBJECT
public:
    ImageVisualization(ExportSettings* exportSettings,QWidget* parent = Q_NULLPTR);

public slots:
    //void setSize(const QRect& size);

private:
    QRect mSize;
    QPixmap* mPixmap;
    QLabel* mDisplayLabel;
    ExportSettings* mExportSettings;
};

