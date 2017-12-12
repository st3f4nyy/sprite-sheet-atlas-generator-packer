#pragma once

#include <QWidget>
#include <QComboBox>
#include "resolution.h"

class ExportSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ExportSettings(QWidget *parent = nullptr);
    Resolution getBaseResolution() const;

signals:
    void resolutionChanged(const Resolution& res);

private slots:
    void onBaseResolutionChangeInCombobox(const QString &);

private:
    void populateBaseResolutions();


private:
    QComboBox* mBaseResolutionComboBox;
    Resolution mBaseResolution;
};
