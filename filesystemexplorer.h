#ifndef FILESYSTEMEXPLORER_H
#define FILESYSTEMEXPLORER_H

#include <QTreeWidget>
#include <QTreeView>
#include <QDirModel>
#include <QVBoxLayout>
#include <QObject>
#include <QMenu>

class FileSystemExplorer : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemExplorer(QWidget* parent = Q_NULLPTR);
    void setBasePath(const QString& basePath);

public slots:
    void onCustomContextMenu(const QPoint& point);
    void addSelectedFileInTree();
    void addSelectedFilesInFolderTree();
    void setSelectedFolderInTreeAsRoot();
    void selectRootFolderInTree();
    void onTreeElementClicked(const QModelIndex &index);
    void onTreeElementDoubleClicked(const QModelIndex &index);

private:
    void initTreeViewActions();
    void removeAllTreeActions();
    void setTreeActionsForFile();
    void setTreeActionsForFolder();


private:
    QTreeView* mTreeView;
    QDirModel* mDirModel;
    QString mCurrentBasePath;
    QVBoxLayout* mLayout;
    QMenu* mTreeMenu;
    QFileInfo mLastSelectedFile;

    QAction* mTreeActionAddFile;
    QAction* mTreeActionAddFilesInFolder;
    QAction* mTreeActionSetFolderAsRoot;
    QAction* mTreeActionSelectRootDir;
    QAction* mTreeActionSeparator;

};

#endif // FILESYSTEMEXPLORER_H
