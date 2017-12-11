#include "exportsettings.h"
#include "configuration.h"
#include <QFormLayout>
#include <QLabel>

ExportSettings::ExportSettings(QWidget *parent) : QWidget(parent)
{
    this->populateBaseResolutions();

    QFormLayout* layout = new QFormLayout(this);
    this->setLayout(layout);
    layout->addRow(new QLabel("Base resolution"),mBaseResolutionComboBox);
}

void ExportSettings::populateBaseResolutions()
{
    mBaseResolutionComboBox = new QComboBox(this);
    mBaseResolution = Configuration::baseResolution();
    const auto ress = Resolution::resolutions();
    for(auto i: ress)
    {
        mBaseResolutionComboBox->addItem(i.name());
    }
    mBaseResolutionComboBox->setCurrentText(mBaseResolution.name());
    connect(mBaseResolutionComboBox,&QComboBox::currentTextChanged,
            this,&ExportSettings::onBaseResolutionChange);
}

void ExportSettings::onBaseResolutionChange(const QString &name)
{
    mBaseResolution = Resolution::getResolutionByName(name);
    Configuration::baseResolution(mBaseResolution);
}


