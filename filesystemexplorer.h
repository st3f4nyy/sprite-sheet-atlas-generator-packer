#pragma once

#include <QTreeWidget>
#include <QTreeView>
#include <QDirModel>
#include <QVBoxLayout>
#include <QObject>
#include <QMenu>
#include <QLabel>
#include <QListView>
#include <QList>

class FileSystemExplorer : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemExplorer(QWidget* parent = Q_NULLPTR);
    void setBasePath(const QString& basePath);
    QStringList getSelectedFilesPath();

signals:
    void selectedFilesChanged(QStringList seletectedFiles);

private slots:
    void onCustomContextMenuTree(const QPoint& point);
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
    void initLayouts();
    void updateSelectedFileList();
    void sendUpdateFileListChange();

private:
    QLabel* mTreeLabel;
    QTreeView* mTreeView;
    QDirModel* mDirModel;
    QString mCurrentBasePath;
    QMenu* mTreeMenu;
    QFileInfo mLastSelectedFile;

    QAction* mTreeActionAddFile;
    QAction* mTreeActionAddFilesInFolder;
    QAction* mTreeActionSetFolderAsRoot;
    QAction* mTreeActionSelectRootDir;
    QAction* mTreeActionSeparator;

    QListView* mSelectedListView;
    QList<QFileInfo> mSelectedFiles;

};

