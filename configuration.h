#pragma once
#include <QRect>
#include <resolution.h>

class Configuration
{
public:
    static QRect mainWindowRect();
    static void mainWindowRect(const QRect& rect);
    static QString searchFileTreePath();
    static void searchFileTreePath(const QString& val);
    static void baseResolution(const Resolution& res);
    static Resolution baseResolution();
};
