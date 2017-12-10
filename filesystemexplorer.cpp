#include "filesystemexplorer.h"

#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QStringListModel>
#include "configuration.h"

FileSystemExplorer::FileSystemExplorer(QWidget* parent) :
    QWidget(parent)
{
    mTreeLabel = new QLabel(this);
    mTreeView = new QTreeView(this);
    mSelectedListView = new QListView(this);
    setBasePath(Configuration::searchFileTreePath());
    this->initTreeViewActions();


    this->initLayouts();
}

void FileSystemExplorer::initLayouts()
{
    QHBoxLayout* completeLayout = new QHBoxLayout(this);
    QVBoxLayout* treeLayout = new QVBoxLayout();
    treeLayout->addWidget(mTreeLabel);
    treeLayout->addWidget(mTreeView);
    completeLayout->addLayout(treeLayout);
    completeLayout->addWidget(mSelectedListView);
    this->setLayout(completeLayout);
}

void FileSystemExplorer::initTreeViewActions()
{
    connect(mTreeView,&QTreeView::clicked,this,&FileSystemExplorer::onTreeElementClicked);
    connect(mTreeView,&QTreeView::expanded,this,&FileSystemExplorer::onTreeElementClicked);
    connect(mTreeView,&QTreeView::doubleClicked,this,&FileSystemExplorer::onTreeElementDoubleClicked);

    mTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTreeView,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(onCustomContextMenuTree(const QPoint &)));
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
    Configuration::searchFileTreePath(path);
    mTreeLabel->setText(mCurrentBasePath);
    mTreeLabel->setMaximumWidth(mTreeView->width());
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

void FileSystemExplorer::onCustomContextMenuTree(const QPoint &point)
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
    if(!mSelectedFiles.contains(mLastSelectedFile))
    {
        mSelectedFiles.append(mLastSelectedFile);
        this->updateSelectedFileList();
    }
}

void FileSystemExplorer::addSelectedFilesInFolderTree()
{
    const auto fileInfoList = QDir(mLastSelectedFile.absoluteFilePath()).entryInfoList();
    bool changed = false;
    for(int i=0; i<fileInfoList.size(); i++)
    {
        const auto file = fileInfoList.at(i);
        if(file.isFile() && !mSelectedFiles.contains(file))
        {
            mSelectedFiles.append(file);
            changed = true;
        }
    }
    if(changed) this->updateSelectedFileList();
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

void FileSystemExplorer::updateSelectedFileList()
{
    QAbstractItemModel* model = mSelectedListView->model();
    if(model==Q_NULLPTR)
    {
        model = new QStringListModel();
        mSelectedListView->setModel(model);
    }
    model->removeRows(0,model->rowCount());
    for(int i=0; i<mSelectedFiles.length();i++)
    {
        const auto file = mSelectedFiles.at(i);
        const auto fileName = file.completeBaseName()
                .append(".")
                .append(file.completeSuffix());
        model->insertRow(i);
        model->setData(model->index(i,0),QVariant(fileName));
    }
}
