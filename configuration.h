#pragma once
#include <QRect>

class Configuration
{
public:
    static QRect mainWindowRect();
    static void mainWindowRect(const QRect& rect);
    static QString searchFileTreePath();
    static void searchFileTreePath(const QString& val);
};
