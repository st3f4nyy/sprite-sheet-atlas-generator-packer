#include <QApplication>
#include <QTreeWidget>
#include <QTreeView>
#include <QDirModel>
#include <QStyleFactory>
#include <QMainWindow>
#include <QDesktopWidget>
#include "filesystemexplorer.h"
#include "configuration.h"

class MainApp
{
public:
  MainApp(QApplication* qapp)
  {
      mQApp = qapp;
      mMainWindow = new QMainWindow();
      setDarkTheme();
  }

  int run()
  {
      FileSystemExplorer* tree = new FileSystemExplorer();
      mMainWindow->setGeometry(Configuration::mainWindowRect());
      mMainWindow->setWindowTitle("Sprite Sheet Atlas Generator Packer");
      mMainWindow->setCentralWidget(tree);
      mMainWindow->show();
      int ret = mQApp->exec();
      Configuration::mainWindowRect(mMainWindow->geometry());
      return ret;
  }

  static int main(int argc, char *argv[])
  {
      MainApp app(new QApplication(argc,argv));
      return app.run();
  }

private:

  void setDarkTheme()
  {
      mQApp->setStyle(QStyleFactory::create("fusion"));

      QPalette palette;
      palette.setColor(QPalette::Window, QColor(53,53,53));
      palette.setColor(QPalette::WindowText, Qt::white);
      palette.setColor(QPalette::Base, QColor(15,15,15));
      palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
      palette.setColor(QPalette::ToolTipBase, Qt::white);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(53,53,53));
      palette.setColor(QPalette::ButtonText, Qt::white);
      palette.setColor(QPalette::BrightText, Qt::red);

      palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
      palette.setColor(QPalette::HighlightedText, Qt::black);

      palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
      palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

      mQApp->setPalette(palette);
  }

private:
  QApplication* mQApp;
  QMainWindow* mMainWindow;
};

int main(int argc, char *argv[])
{
    return MainApp::main(argc,argv);
}
