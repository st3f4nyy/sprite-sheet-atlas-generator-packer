#include "mainwindow.h"
#include <QApplication>
#include <QTreeWidget>
#include <QTreeView>
#include <QDirModel>
#include "filesystemexplorer.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    FileSystemExplorer* tree = new FileSystemExplorer();

    //QTreeWidgetItem* item1 = new QTreeWidgetItem(tree);
    //item1->setText(0,QString("hi"));


    w.setCentralWidget(tree);


    w.show();

    return a.exec();
}
