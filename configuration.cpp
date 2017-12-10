#include "configuration.h"
#include <QSettings>
#include <QDesktopWidget>
#include <QDir>

static const QString MAIN_WINDOW_RECT = QString("MAIN_WINDOW_RECT");
static const QString TREE_FILE_PATH = QString("TREE_FILE_PATH");
static const QString BASE_RESOLUTION = QString("BASE_RESOLUTION");

QSettings sSettings(QSettings::UserScope,"meduseware","sprite sheet atlas generator packer");

QSettings& get() { return sSettings; }

QRect Configuration::mainWindowRect()
{
    auto rect = get().value(MAIN_WINDOW_RECT).toRect();
    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();

    if(rect.height() + rect.top()>screenHeight)
        rect.moveTop(screenHeight/4);
    if(rect.width() + rect.left()> screenWidth)
        rect.moveLeft(screenWidth/4);
    if(rect.top()<1)
        rect.setTop(screenHeight/4);
    if(rect.left()<1)
        rect.setLeft(screenWidth/4);
    if(rect.width()<300)
        rect.setWidth(screenWidth/2);
    if(rect.height()<300)
        rect.setHeight(screenHeight/2);

    return rect;
}

void Configuration::mainWindowRect(const QRect& rect)
{
    get().setValue(MAIN_WINDOW_RECT,rect);
}

QString Configuration::searchFileTreePath()
{
    QString strinPath = get().value(TREE_FILE_PATH).toString();
    if(strinPath.length()>0 && QDir(strinPath).exists())
    {
        return strinPath;
    } else
    {
        return QDir::homePath();
    }
}

void Configuration::searchFileTreePath(const QString& val)
{
    get().setValue(TREE_FILE_PATH,val);
}

void Configuration::baseResolution(const Resolution& res)
{
    get().setValue(BASE_RESOLUTION,res.name());
}

Resolution Configuration::baseResolution()
{
    QString name = get().value(BASE_RESOLUTION).toString();
    return Resolution::getResolutionByName(name);
}
