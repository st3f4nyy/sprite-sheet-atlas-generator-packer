#pragma once

#include <QWidget>
#include <QComboBox>
#include "resolution.h"

class ExportSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ExportSettings(QWidget *parent = nullptr);

private slots:
    void onBaseResolutionChange(const QString &);

private:
    void populateBaseResolutions();


private:
    QComboBox* mBaseResolutionComboBox;
    Resolution mBaseResolution;
};
