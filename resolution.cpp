#include "resolution.h"
#include <QList>

const Resolution Resolution::R32(32,"32x32");
const Resolution Resolution::R64(64,"64x64");
const Resolution Resolution::R128(128,"128x128");
const Resolution Resolution::R256(256,"256x256");
const Resolution Resolution::R512(512,"512x512");
const Resolution Resolution::R1K(1024,"1Kx1K");
const Resolution Resolution::R2K(2048,"2Kx2K");
const Resolution Resolution::R4K(4096,"4Kx4K");
const Resolution Resolution::R8K(8192,"8Kx8K");
const Resolution Resolution::R16K(16384,"16Kx16K");
QList<Resolution> Resolution::resolutions()
{
    QList<Resolution> r;
    r.append(Resolution::R32);
    r.append(Resolution::R64);
    r.append(Resolution::R128);
    r.append(Resolution::R512);
    r.append(Resolution::R1K);
    r.append(Resolution::R2K);
    r.append(Resolution::R4K);
    r.append(Resolution::R8K);
    r.append(Resolution::R16K);
    return r;
}

Resolution Resolution::getDefault()
{
    return R512;
}

Resolution Resolution::getResolutionByName(QString name)
{
    const auto ress = resolutions();
    for(auto i: ress)
    {
        if(i.name().compare(name)==0)
            return i;
    }
    return getDefault();
}

Resolution::Resolution()
{
    *this=getDefault();
}

Resolution::Resolution(unsigned int res, const QString &name)
{
    mRes = res;
    mName = name;
}

unsigned int Resolution::res() const
{
    return mRes;
}

QString Resolution::name() const
{
    return mName;
}


