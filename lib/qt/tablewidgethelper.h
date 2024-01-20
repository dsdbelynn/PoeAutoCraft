#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

/*
 * QTableWidget助手
 * 实现QTableWidget类的单元格编辑与批量拷贝粘贴
 * hxcsmol[at]gmail.com
 *
 * 历史：
 * 1.0.1 hxc 根据以前的工作初步构建
 *
 */

#include <QtWidgets>

//是否支持使用控件示例作为单元格对象（做了一定的支持，但未验证，建议禁用）
//#define SUPPORT_TABLEWIDGET_EDITOR_ITEM

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#define SUPPORT_TABLEWIDGET_USER_EDITING
#endif

class TableWidgetSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit TableWidgetSpinBox(QWidget *parent);

    void setFixWidth(int width) { fixWidth = width; }

protected:
    virtual int valueFromText(const QString &text) const;
    virtual QString textFromValue(int val) const;

private:
    int fixWidth;
};
class TableWidgetUintSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit TableWidgetUintSpinBox(QWidget *parent);

    void setFixWidth(int width) { fixWidth = width; }
    void setFixBase(int base) { fixBase = base; }

public:
    QValidator::State validate(QString &input, int &pos) const;
    virtual double valueFromText(const QString &text) const;
    virtual QString textFromValue(double val) const;

private:
    int fixWidth;
    int fixBase;
};

class TableWidgetDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum ECellType {
        TYPE_NONE, TYPE_CHECK, TYPE_EDIT, TYPE_SPIN, TYPE_UINTSPIN, TYPE_DOUBLESPIN,
        TYPE_DATE, TYPE_TIME, TYPE_DATETIME, TYPE_LIST, TYPE_VLIST, TYPE_DOUBLE,
        //以下暂时不完整支持
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
        TYPE_LINEEDIT, TYPE_SPINBOX, TYPE_DOUBLESPINBOX, TYPE_COMBOBOX
#endif
    };
    enum ECellRole { ROLE_VALUE = 100, ROLE_TYPE, ROLE_DATA, ROLE_DATA2, ROLE_DATA3, ROLE_DATA4, ROLE_DATA5 };

    explicit TableWidgetDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    static void copyToClipboard(QTableWidget* table);
    static void pasteFromClipboard(QTableWidget* table);
    static void deleteSelectedContent(QTableWidget* table);

    static QString doubleToText(double fval);
    static QString intSpinToText(int ival, int base, const QString& prefix, const QString& suffix);
    static QString uintSpinToText(uint uval, int base, const QString& prefix, const QString& suffix);
    static QString doubleSpinToText(double fval, int prec, const QString& prefix, const QString& suffix);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    static void setItemData(QAbstractItemModel* model, const QModelIndex& index, const QString& text, const QVariant& value);
    static void setItemData(QAbstractItemModel* model, int row, int col, const QString& text, const QVariant& value);

#ifdef SUPPORT_TABLEWIDGET_USER_EDITING
public:
    mutable int rowEditing = -1;
    mutable int colEditing = -1;
#endif
};

class TableWidgetFilter : public QObject
{
    Q_OBJECT
public:
    explicit TableWidgetFilter(QTableWidget* widget, bool allowPaste = false);

    void enablePaste(bool enable) { paste = enable; }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    QTableWidget* table;
    bool paste;
};

class TableWidgetHelper
{
public:
    TableWidgetHelper(QTableWidget* widget);

    void initTableWidgetAll();
    void initTableWidgetDelegate();
    void initTableWidgetClipboard(bool allowPaste = true);

    static void initTableWidgetAll(QTableWidget* widget);
    static void initTableWidgetDelegate(QTableWidget* widget);
    static void initTableWidgetClipboard(QTableWidget* widget, bool allowPaste = true);

    int getRowCount() const;
    void setRowCount(int rows);

    int getColCount() const;
    void setColCount(int cols);

    void setSpan(int row, int col, int rows, int cols);
    void clearSpans();

    QTableWidgetItem* makeVerticalHeaderItem(int row, const QString& text);
    QTableWidgetItem* makeHorizontalHeaderItem(int col, const QString& text);

    QTableWidgetItem* makeItem(int row, int col, bool editable = false);
    QTableWidgetItem* makeItem(int row, int col, const QString& text, bool editable);
    QTableWidgetItem* makeItemEditable(int row, int col, const QString& text);
    QTableWidgetItem* makeItemReadOnly(int row, int col, const QString& text);

    bool setVerticalHeaderItemData(int row, int role, const QVariant & value);
    bool setHorizontalHeaderItemData(int col, int role, const QVariant & value);
    bool setItemData(int row, int col, int role, const QVariant & value);
    bool setItemEditable(int row, int col, bool editable);
    bool setItemChecked(int row, int col, bool checked);
    void setRowData(int row, int role, const QVariant & value, bool header = false);
    void setColData(int col, int role, const QVariant & value, bool header = false);

    QVariant getVerticalHeaderItemData(int row, int role);
    QVariant getHorizontalHeaderItemData(int col, int role);
    QVariant getItemData(int row, int col, int role);
    QString getItemText(int row, int col) const;

    bool isItemChecked(int row, int col) const;
    bool isItemSelected(int row, int col) const;
    bool isItemEditing(int row, int col) const;
    bool isRowSelected(int row) const;
    bool isColSelected(int col) const;

    int getFirstSelectedRow() const;
    int getFirstSelectedCol() const;
    int getLastSelectedRow() const;
    int getLastSelectedCol() const;

    //以下接口需要安装代理对象TableWidgetDelegate才能支持
    QTableWidgetItem* makeStaticItem(int row, int col, const QString& text);
    QTableWidgetItem* makeNormalItem(int row, int col, const QString& text);

    QTableWidgetItem* makeTextItem(int row, int col, const QString& text);
    QTableWidgetItem* makeDoubleItem(int row, int col, double val, double minVal = 1, double maxVal = 0);

    QTableWidgetItem* makeCheckItem(int row, int col, bool val, bool editable = true);
    QTableWidgetItem* makeEditItem(int row, int col, const QString& text, const QString& regExp = QString(), const QString& mask = QString());
    QTableWidgetItem* makeListItem(int row, int col, int sel, const QStringList& lst);
    QTableWidgetItem* makeListItem(int row, int col, const QVariant& val, const QList<QVariant>& vals, const QStringList& names);
    QTableWidgetItem* makeSpinItem(int row, int col, int val, int minVal = 1, int maxVal = 0, int base = 10, const QString& suffix = QString(), const QString& prefix = QString());
    QTableWidgetItem* makeUintSpinItem(int row, int col, uint val, uint minVal = 1, uint maxVal = 0, int base = 10, const QString& suffix = QString(), const QString& prefix = QString());
    QTableWidgetItem* makeDoubleSpinItem(int row, int col, double val, double minVal = 1, double maxVal = 0, int prec = 2, const QString& suffix = QString(), const QString& prefix = QString());
    QTableWidgetItem* makeDateItem(int row, int col, const QDate& val, const QString& fmt = QString());
    QTableWidgetItem* makeTimeItem(int row, int col, const QTime& val, const QString& fmt = QString());
    QTableWidgetItem* makeDateTimeItem(int row, int col, const QDateTime& val, const QString& fmt = QString());

#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    QTableWidgetItem* makeEditItem(int row, int col, const QString& text, QLineEdit* edit);
    QTableWidgetItem* makeSpinItem(int row, int col, int val, QSpinBox* spin);
    QTableWidgetItem* makeDoubleSpinItem(int row, int col, double val, QDoubleSpinBox* spin);
    QTableWidgetItem* makeComboListItem(int row, int col, int val, QComboBox* combo);
#endif

    QVariant getDelegateItemValue(int row, int col);
    void setDelegateItemValue(int row, int col, const QVariant& value);

    static QVariant getDelegateItemValue(QTableWidgetItem* item);
    static void setDelegateItemValue(QTableWidgetItem* item, const QVariant& value);

private:
    QTableWidget* table;
};

#endif // TABLEWIDGET_H
