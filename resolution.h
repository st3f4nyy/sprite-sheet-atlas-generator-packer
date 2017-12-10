#pragma once
#include <QString>

class Resolution
{
public:
    Resolution(unsigned int res, const QString& name);
    unsigned int res() const;
    QString name() const;

    static Resolution getResolutionByName(QString name);
    static QList<Resolution> resolutions();
    static Resolution getDefault();
    static const Resolution R32;
    static const Resolution R64;
    static const Resolution R128;
    static const Resolution R256;
    static const Resolution R512;
    static const Resolution R1K;
    static const Resolution R2K;
    static const Resolution R4K;
    static const Resolution R8K;
    static const Resolution R16K;
private:
    unsigned int mRes;
    QString mName;
};
