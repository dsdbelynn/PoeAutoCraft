#include "treewidgethelper.h"

/*
 * QTreeWidget助手
 * 实现QTreeWidget类的单元格编辑与批量拷贝粘贴
 * hxcsmol[at]gmail.com
 *
 * 历史：
 *
 *
 */
TreeWidgetHelper::TreeWidgetHelper(QTreeWidget* widget) : tree(widget), topBuildNum(0)
{
}

int TreeWidgetHelper::getTopItemCount()
{
    return tree->topLevelItemCount();
}
QTreeWidgetItem* TreeWidgetHelper::makeTopItem(int index)
{
    QTreeWidgetItem* item = tree->topLevelItem(index);
    if(nullptr == item)
    {
        item = new QTreeWidgetItem(tree);
        if(nullptr != item)
        {
            tree->addTopLevelItem(item);
        }
    }
    return item;
}

QTreeWidgetItem* TreeWidgetHelper::makeItem(QTreeWidgetItem* parent, int index)
{
    QTreeWidgetItem* item = parent->child(index);
    if(nullptr == item)
    {
        item = new QTreeWidgetItem(parent);
        if(nullptr != item)
        {
            parent->addChild(item);
        }
    }
    return item;
}

void TreeWidgetHelper::removeTopItem(int index)
{
    QTreeWidgetItem* item = tree->takeTopLevelItem(index);
    if(nullptr != item)
    {
        delete item;
    }
}
void TreeWidgetHelper::removeItem(QTreeWidgetItem* parent, int index)
{
    QTreeWidgetItem* item = parent->takeChild(index);
    if(nullptr != item)
    {
        delete item;
    }
}

void TreeWidgetHelper::removeTopItems(int index, int num)
{
    int end = num <= 0 ? tree->topLevelItemCount() : index + num;
    for(int i=end-1; i>=index; --i)
    {
        QTreeWidgetItem* item = tree->takeTopLevelItem(i);
        if(nullptr != item)
        {
            delete item;
        }
    }
}
void TreeWidgetHelper::removeItems(QTreeWidgetItem* parent, int index, int num)
{
    int end = num <= 0 ? parent->childCount() : index + num;
    for(int i=end-1; i>=index; --i)
    {
        QTreeWidgetItem* item = parent->takeChild(i);
        if(nullptr != item)
        {
            delete item;
        }
    }
}


void TreeWidgetHelper::beginBuild()
{
    topBuildNum = 0;
}
QTreeWidgetItem* TreeWidgetHelper::buildTopItem()
{
    bool bnew = (topBuildNum >= tree->topLevelItemCount());
    QTreeWidgetItem* item = makeTopItem(topBuildNum);
    if(nullptr != item)
    {
        item->setData(0, Qt::UserRole + ROLE_BUILD_NUM, 0);
        item->setData(0, Qt::UserRole + ROLE_BUILD_NEW, bnew);
        topBuildNum++;
    }
    return item;
}
QTreeWidgetItem* TreeWidgetHelper::buildItem(QTreeWidgetItem* parent)
{
    int index = parent->data(0, Qt::UserRole + ROLE_BUILD_NUM).toInt();
    bool bnew = (index >= parent->childCount());
    QTreeWidgetItem* item = makeItem(parent, index);
    if(nullptr != item)
    {
        item->setData(0, Qt::UserRole + ROLE_BUILD_NUM, 0);
        item->setData(0, Qt::UserRole + ROLE_BUILD_NEW, bnew);
        parent->setData(0, Qt::UserRole + ROLE_BUILD_NUM, index + 1);
    }
    return item;
}
bool TreeWidgetHelper::isNewBuildItem(QTreeWidgetItem* item)
{
    return item->data(0, Qt::UserRole + ROLE_BUILD_NEW).toBool();
}
void TreeWidgetHelper::finishBuildItem(QTreeWidgetItem* parent)
{
    int num = parent->data(0, Qt::UserRole + ROLE_BUILD_NUM).toInt();
    int cnt = parent->childCount();
    if(num < cnt)
    {
        removeItems(parent, num);
    }
    cnt = parent->childCount();
    cnt = cnt < num ? cnt : num;
    for(int i=0; i<cnt; ++i)
    {
        QTreeWidgetItem* item = parent->child(i);
        if(nullptr != item && item->childCount() > 0)
        {
            finishBuildItem(item);
        }
    }
}
void TreeWidgetHelper::finishBuild()
{
    int cnt = tree->topLevelItemCount();
    if(topBuildNum < cnt)
    {
        removeTopItems(topBuildNum);
    }
    cnt = tree->topLevelItemCount();
    cnt = cnt < topBuildNum ? cnt : topBuildNum;
    for(int i=0; i<cnt; ++i)
    {
        QTreeWidgetItem* item = tree->topLevelItem(i);
        if(nullptr != item && item->childCount() > 0)
        {
            finishBuildItem(item);
        }
    }
}


