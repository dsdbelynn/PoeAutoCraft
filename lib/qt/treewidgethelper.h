#ifndef TREEWIDGET_H
#define TREEWIDGET_H

/*
 * QTreeWidget助手
 * 实现QTreeWidget类的单元格编辑与批量拷贝粘贴
 * hxcsmol[at]gmail.com
 *
 * 历史：
 *
 */

#include <QtWidgets>

class TreeWidgetHelper
{
public:
    TreeWidgetHelper(QTreeWidget* widget);

    //通用接口
    int getTopItemCount();
    QTreeWidgetItem* makeTopItem(int index);
    static QTreeWidgetItem* makeItem(QTreeWidgetItem* parent, int index);

    void removeTopItem(int index);
    void removeTopItems(int index, int num = 0);
    static void removeItem(QTreeWidgetItem* parent, int index);
    static void removeItems(QTreeWidgetItem* parent, int index, int num = 0);

    //替换构造接口
    void beginBuild();
    QTreeWidgetItem* buildTopItem();
    QTreeWidgetItem* buildItem(QTreeWidgetItem* parent);
    bool isNewBuildItem(QTreeWidgetItem* item);
    void finishBuild();

private:
    void finishBuildItem(QTreeWidgetItem* parent);

private:
    enum EUserRole { ROLE_BUILD_NUM = 100, ROLE_BUILD_NEW };

    QTreeWidget* tree;
    int topBuildNum;
};

#endif // TREEWIDGET_H
