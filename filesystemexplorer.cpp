#include "filesystemexplorer.h"

#include <QDir>
#include <QDockWidget>
#include <QFileDialog>

FileSystemExplorer::FileSystemExplorer(QWidget* parent) :
    QWidget(parent)
{
    mTreeView = new QTreeView(this);
    mTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTreeView,&QTreeView::clicked,this,&FileSystemExplorer::onTreeElementClicked);
    connect(mTreeView,&QTreeView::expanded,this,&FileSystemExplorer::onTreeElementClicked);
    connect(mTreeView,&QTreeView::doubleClicked,this,&FileSystemExplorer::onTreeElementDoubleClicked);
    this->initTreeViewActions();


    mLayout = new QVBoxLayout(this);
    mLayout->addWidget(mTreeView);
    this->setLayout(mLayout);
    mTreeView->show();
}

void FileSystemExplorer::initTreeViewActions()
{
    connect(mTreeView,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(onCustomContextMenu(const QPoint &)));
    setBasePath(QDir::homePath());
    mTreeMenu = new QMenu(this);

    mTreeActionAddFile = new QAction("Add file",this);
    connect(mTreeActionAddFile,&QAction::triggered,this,
            &FileSystemExplorer::addSelectedFileInTree);

    mTreeActionAddFilesInFolder =
            new QAction("Add files in folder",this);
    connect(mTreeActionAddFilesInFolder,&QAction::triggered,this,
            &FileSystemExplorer::addSelectedFilesInFolderTree);

    mTreeActionSetFolderAsRoot =
            new QAction("Set folder as root",this);
    connect(mTreeActionSetFolderAsRoot,&QAction::triggered,this,
            &FileSystemExplorer::setSelectedFolderInTreeAsRoot);

    mTreeActionSelectRootDir =
            new QAction("Select other root folder",this);
    connect(mTreeActionSelectRootDir,&QAction::triggered,this,
            &FileSystemExplorer::selectRootFolderInTree);


    mTreeActionSeparator = mTreeMenu->addSeparator();
}

void FileSystemExplorer::setBasePath(const QString& path)
{
    mCurrentBasePath = path;
    mDirModel = new QDirModel(this);
    mDirModel->setFilter(QDir::AllEntries|
                         QDir::NoDot|QDir::Readable);
    mDirModel->setSorting(QDir::Name|QDir::DirsFirst|QDir::LocaleAware|QDir::IgnoreCase);
    mTreeView->setModel(mDirModel);
    mTreeView->setRootIndex(mDirModel->index(mCurrentBasePath));
    mTreeView->hideColumn(1);
    mTreeView->hideColumn(2);
    mTreeView->hideColumn(3);
}

void FileSystemExplorer::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = mTreeView->indexAt(point);
    if (index.isValid()) {
        mLastSelectedFile = mDirModel->fileInfo(index);
        if(mLastSelectedFile.isDir()){
            setTreeActionsForFolder();
        } else {
            setTreeActionsForFile();
        }
        mTreeMenu->exec(mTreeView->mapToGlobal(point));
    }
}

void FileSystemExplorer::onTreeElementClicked(const QModelIndex &index)
{
    QFileInfo fileClicked = mDirModel->fileInfo(index);
    if(fileClicked.baseName().compare("")==0)
    {
        setBasePath(fileClicked.absoluteFilePath());
    }
}

void FileSystemExplorer::onTreeElementDoubleClicked(const QModelIndex &index)
{
    QFileInfo fileClicked = mDirModel->fileInfo(index);
    if(fileClicked.isDir())
    {
        setBasePath(fileClicked.absoluteFilePath());
    }
}

void FileSystemExplorer::removeAllTreeActions()
{
    mTreeMenu->removeAction(mTreeActionSeparator);
    mTreeMenu->removeAction(mTreeActionAddFile);
    mTreeMenu->removeAction(mTreeActionAddFilesInFolder);
    mTreeMenu->removeAction(mTreeActionSetFolderAsRoot);
    mTreeMenu->removeAction(mTreeActionSelectRootDir);
}

void FileSystemExplorer::setTreeActionsForFile()
{
    this->removeAllTreeActions();
    mTreeMenu->addAction(mTreeActionAddFile);
}

void FileSystemExplorer::setTreeActionsForFolder()
{
    this->removeAllTreeActions();
    mTreeMenu->addAction(mTreeActionAddFilesInFolder);
    mTreeMenu->addAction(mTreeActionSetFolderAsRoot);
    mTreeMenu->addAction(mTreeActionSeparator);
    mTreeMenu->addAction(mTreeActionSelectRootDir);
}

void FileSystemExplorer::addSelectedFileInTree()
{
    std::printf("file\n");
}

void FileSystemExplorer::addSelectedFilesInFolderTree()
{
    std::printf("folder\n");
}

void FileSystemExplorer::setSelectedFolderInTreeAsRoot()
{
    setBasePath(mLastSelectedFile.absoluteFilePath());
}

void FileSystemExplorer::selectRootFolderInTree()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    mLastSelectedFile.absoluteFilePath(),
                                                    QFileDialog::ShowDirsOnly);
    if(!dir.isEmpty())
    {
        setBasePath(dir);
    }

}
