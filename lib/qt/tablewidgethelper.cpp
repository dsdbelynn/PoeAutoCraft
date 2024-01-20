#include "tablewidgethelper.h"
#include <cmath>

/*
 * QTableWidget助手
 * 实现QTableWidget类的单元格编辑与批量拷贝粘贴
 * hxcsmol[at]gmail.com
 *
 * 历史：
 * 1.0.1 hxc 根据以前的工作初步构建
 *
 *
 */

/*
    //常用正则
    boolExp.setPattern("0|1|true|false");
    boolExp.setCaseSensitivity(Qt::CaseInsensitive);

    byteArrayExp.setPattern("[\\x00-\\xff]*");
    charExp.setPattern(".");
    colorExp.setPattern("\\(([0-9]*),([0-9]*),([0-9]*),([0-9]*)\\)");
    doubleExp.setPattern("-?[0-9]*\\.?[0-9]*");
    pointExp.setPattern("\\((-?[0-9]*),(-?[0-9]*)\\)");
    rectExp.setPattern("\\((-?[0-9]*),(-?[0-9]*),(-?[0-9]*),(-?[0-9]*)\\)");
    signedIntegerExp.setPattern("-?[0-9]*");
    sizeExp = pointExp;
    unsignedIntegerExp.setPattern("[0-9]*");

    dateExp.setPattern("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})");
    timeExp.setPattern("([0-9]{,2}):([0-9]{,2}):([0-9]{,2})");
    dateTimeExp.setPattern(dateExp.pattern() + "T" + timeExp.pattern());
 */

/////////////////////////////////////////////////////////////////////////////
TableWidgetSpinBox::TableWidgetSpinBox(QWidget *parent) :
    QSpinBox(parent), fixWidth(0)
{
}
int TableWidgetSpinBox::valueFromText(const QString &text) const
{
    return QSpinBox::valueFromText(text);//text.toInt(nullptr, displayIntegerBase());
}
QString TableWidgetSpinBox::textFromValue(int val) const
{
    if(0 != fixWidth)
    {
        int base = displayIntegerBase();
        return base < 10 ?
                    QString("%1").arg(val, fixWidth, base, QChar('0')) :
                    QString("%1").arg(val, fixWidth, base, QChar('0')).toUpper();
    }
    else
    {
        return QSpinBox::textFromValue(val);
    }
}
/////////////////////////////////////////////////////////////////////////////
TableWidgetUintSpinBox::TableWidgetUintSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    fixWidth(0), fixBase(10)
{
    setDecimals(0);
}
QValidator::State TableWidgetUintSpinBox::validate(QString &input, int &pos) const
{
    if(10 != fixBase)
    {
        QString dat = input;
        QString fix0 = prefix();
        if(!fix0.isEmpty() && dat.startsWith(fix0))
        {
            dat = dat.mid(fix0.size());
            //pos = fix0.size();
        }
        QString fix1 = suffix();
        if(!fix1.isEmpty() && dat.endsWith(fix1))
        {
            dat = dat.left(dat.size() - fix0.size());
        }

        bool ok = false;
        uint val = dat.toUInt(&ok, fixBase);

        QValidator::State ret = QValidator::Invalid;
        if(ok && val >= minimum() && val <= maximum())
        {
            ret = QValidator::Acceptable;
        }

        input = fix0 + dat + fix1;
        return ret;
    }
    return QDoubleSpinBox::validate(input, pos);
}
double TableWidgetUintSpinBox::valueFromText(const QString &text) const
{
    if(10 != fixBase)
    {
        QString strDat = text;
        QString strSub = prefix();
        if(!strSub.isEmpty() && strDat.startsWith(strSub))
        {
            strDat = strDat.mid(strSub.size());
        }
        strSub = suffix();
        if(!strSub.isEmpty() && strDat.endsWith(strSub))
        {
            strDat = strDat.left(strDat.size() - strSub.size());
        }
        return strDat.toUInt(nullptr, fixBase);
    }
    else
    {
        return QDoubleSpinBox::valueFromText(text);
    }
}
QString TableWidgetUintSpinBox::textFromValue(double val) const
{
    if(10 != fixBase || 0 != fixWidth)
    {
        return fixBase < 10 ?
                    QString("%1").arg(static_cast<uint>(val), fixWidth, fixBase, QChar('0')) :
                    QString("%1").arg(static_cast<uint>(val), fixWidth, fixBase, QChar('0')).toUpper();
    }
    else
    {
        return QDoubleSpinBox::textFromValue(val);
    }
}
///////////////////////////////////////////////////////////////////////////
#define TABLEWIDGET_REG_DOUBLE "-?[0-9]*\\.?[0-9]*[eE]?-?[0-9]*"

TableWidgetDelegate::TableWidgetDelegate(QObject *parent) : QItemDelegate(parent)
{
}
void TableWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
    int type = index.model()->data(index, Qt::UserRole+ROLE_TYPE).toInt();
    if(TYPE_CHECK == type)
    {
        bool val = index.model()->data(index, Qt::UserRole+ROLE_VALUE).toBool();
        painter->save();
        QPoint pt = option.rect.center();
        painter->setPen(QColor(230, 230, 230));
        QRect rect(pt.x()-8, pt.y()-8, 16, 16);
        painter->drawRect(rect);
        if(0 == (index.model()->flags(index) & Qt::ItemIsEditable))
        {
            painter->fillRect(rect, QColor(245, 245, 245));
        }
        if(val)
        {
            QPen pen(Qt::black, 2);
            painter->setPen(pen);
            QPainterPath path;
            path.moveTo(pt.x()-4, pt.y());
            path.lineTo(pt.x(), pt.y()+4);
            path.lineTo(pt.x()+7, pt.y()-3);
            painter->drawPath(path);
        }
        painter->restore();
    }
}
bool TableWidgetDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        int type = model->data(index, Qt::UserRole+ROLE_TYPE).toInt();
        if(TYPE_CHECK == type && 0 != (model->flags(index) & Qt::ItemIsEditable))
        {
            QMouseEvent* mouse = dynamic_cast<QMouseEvent*>(event);
            if(mouse)
            {
                QPoint pt = option.rect.center();
                QRect rect(pt.x()-8, pt.y()-8, 16, 16);
                if(rect.contains(mouse->pos()))
                {
                    bool val = !model->data(index, Qt::UserRole+ROLE_VALUE).toBool();
                    model->setData(index, val, Qt::UserRole+ROLE_VALUE);
                    return true;
                }
            }
        }
    }
    return false;
}

QWidget* TableWidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget* widget = nullptr;
    const QAbstractItemModel* model = index.model();
    int type = model->data(index, Qt::UserRole+ROLE_TYPE).toInt();
    switch(type)
    {
    case TYPE_CHECK:
        break;
    case TYPE_DOUBLE:
    {
        QLineEdit *edit = new QLineEdit(parent);
        edit->setFrame(false);
        QRegExp regExp;
        regExp.setPattern(TABLEWIDGET_REG_DOUBLE);
        edit->setValidator(new QRegExpValidator(regExp, edit));
        widget = edit;
        widget->setProperty("VDType", TYPE_DOUBLE);
        break;
    }
    case TYPE_LIST:
    {
        QComboBox* box = new QComboBox(parent);
        box->setFrame(false);
        box->addItems(model->data(index, Qt::UserRole+ROLE_DATA).toStringList());
        widget = box;
        widget->setProperty("VDType", TYPE_LIST);
        break;
    }
    case TYPE_VLIST:
    {
        QComboBox* box = new QComboBox(parent);
        box->setFrame(false);
        box->addItems(model->data(index, Qt::UserRole+ROLE_DATA).toStringList());
        widget = box;
        widget->setProperty("VDType", TYPE_VLIST);
        break;
    }
    case TYPE_EDIT:
    {
        QLineEdit *edit = new QLineEdit(parent);
        edit->setFrame(false);
        QString strExp = model->data(index, Qt::UserRole+ROLE_DATA).toString();
        QString strMsk = model->data(index, Qt::UserRole+ROLE_DATA2).toString();
        if(!strMsk.isEmpty())
        {
            edit->setInputMask(strMsk);
        }
        else if(!strExp.isEmpty())
        {
            QRegExp regExp;
            regExp.setPattern(strExp);
            if(!regExp.isEmpty())
            {
                QValidator *validator = new QRegExpValidator(regExp, edit);
                edit->setValidator(validator);
            }
        }
        widget = edit;
        widget->setProperty("VDType", TYPE_EDIT);
        break;
    }
    case TYPE_SPIN:
    {
        QSpinBox* spin = nullptr;
        int base = model->data(index, Qt::UserRole+ROLE_DATA3).toInt();
        if(base > 10)
        {
            TableWidgetSpinBox* box = new TableWidgetSpinBox(parent);
            box->setFixWidth(base / 100);
            spin = box;
        }
        else
        {
            spin = new QSpinBox(parent);
        }
        spin->setFrame(false);
        int imin = model->data(index, Qt::UserRole+ROLE_DATA).toInt();
        int imax = model->data(index, Qt::UserRole+ROLE_DATA2).toInt();
        if(imin > imax)
        {
            imin = INT_MIN;
            imax = INT_MAX;
        }
        spin->setRange(imin, imax);
        spin->setDisplayIntegerBase(base % 100);
        spin->setSuffix(model->data(index, Qt::UserRole+ROLE_DATA4).toString());
        spin->setPrefix(model->data(index, Qt::UserRole+ROLE_DATA5).toString());
        widget = spin;
        widget->setProperty("VDType", TYPE_SPIN);
        break;
    }
    case TYPE_UINTSPIN:
    {
        TableWidgetUintSpinBox* spin = new TableWidgetUintSpinBox(parent);
        int base = model->data(index, Qt::UserRole+ROLE_DATA3).toInt();
        spin->setFixWidth(base / 100);
        spin->setFixBase(base % 100);
        spin->setFrame(false);

        unsigned int umin = model->data(index, Qt::UserRole+ROLE_DATA).toUInt();
        unsigned int umax = model->data(index, Qt::UserRole+ROLE_DATA2).toUInt();
        if(umin > umax)
        {
            umin = 0;
            umax = UINT_MAX;
        }
        spin->setRange(umin, umax);
        spin->setSuffix(model->data(index, Qt::UserRole+ROLE_DATA4).toString());
        spin->setPrefix(model->data(index, Qt::UserRole+ROLE_DATA5).toString());
        widget = spin;
        widget->setProperty("VDType", TYPE_UINTSPIN);
        break;
    }
    case TYPE_DOUBLESPIN:
    {
        QDoubleSpinBox* spin = new QDoubleSpinBox(parent);
        spin->setFrame(false);
        double fmin = model->data(index, Qt::UserRole+ROLE_DATA).toDouble();
        double fmax = model->data(index, Qt::UserRole+ROLE_DATA2).toDouble();
        if(fmin > fmax)
        {
            fmin = DBL_MIN;
            fmax = DBL_MAX;
        }
        spin->setRange(fmin, fmax);
        spin->setDecimals(model->data(index, Qt::UserRole+ROLE_DATA3).toInt());
        spin->setSuffix(model->data(index, Qt::UserRole+ROLE_DATA4).toString());
        spin->setPrefix(model->data(index, Qt::UserRole+ROLE_DATA5).toString());
        widget = spin;
        widget->setProperty("VDType", TYPE_DOUBLESPIN);
        break;
    }
    case TYPE_DATE:
    {
        QDateEdit* edit = new QDateEdit(parent);
        edit->setFrame(false);
        edit->setDisplayFormat(model->data(index, Qt::UserRole+ROLE_DATA).toString());
        edit->setCalendarPopup(true);
        widget = edit;
        widget->setProperty("VDType", TYPE_DATE);
        break;
    }
    case TYPE_TIME:
    {
        QTimeEdit* edit = new QTimeEdit(parent);
        edit->setFrame(false);
        edit->setDisplayFormat(model->data(index, Qt::UserRole+ROLE_DATA).toString());
        widget = edit;
        widget->setProperty("VDType", TYPE_TIME);
        break;
    }
    case TYPE_DATETIME:
    {
        QDateTimeEdit* edit = new QDateTimeEdit(parent);
        edit->setFrame(false);
        edit->setDisplayFormat(model->data(index, Qt::UserRole+ROLE_DATA).toString());
        //edit->setCalendarPopup(true);
        widget = edit;
        widget->setProperty("VDType", TYPE_DATETIME);
        break;
    }
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_LINEEDIT:
    {
        QLineEdit* data1 = model->data(index, Qt::UserRole+ROLE_DATA).value<QLineEdit*>();
        QLineEdit* data2 = model->data(index, Qt::UserRole+ROLE_DATA2).value<QLineEdit*>();
        if(nullptr != data1 && data1 == data2)
        {
            widget = data1;
        }
        break;
    }
    case TYPE_SPINBOX:
    {
        QSpinBox* data1 = model->data(index, Qt::UserRole+ROLE_DATA).value<QSpinBox*>();
        QSpinBox* data2 = model->data(index, Qt::UserRole+ROLE_DATA2).value<QSpinBox*>();
        if(nullptr != data1 && data1 == data2)
        {
            widget = data1;
        }
        break;
    }
    case TYPE_DOUBLESPINBOX:
    {
        QDoubleSpinBox* data1 = model->data(index, Qt::UserRole+ROLE_DATA).value<QDoubleSpinBox*>();
        QDoubleSpinBox* data2 = model->data(index, Qt::UserRole+ROLE_DATA2).value<QDoubleSpinBox*>();
        if(nullptr != data1 && data1 == data2)
        {
            widget = data1;
        }
        break;
    }
    case TYPE_COMBOBOX:
    {
        QComboBox* data1 = model->data(index, Qt::UserRole+ROLE_DATA).value<QComboBox*>();
        QComboBox* data2 = model->data(index, Qt::UserRole+ROLE_DATA2).value<QComboBox*>();
        if(nullptr != data1 && data1 == data2)
        {
            widget = data1;
        }
        break;
    }
#endif
    default:
        widget = QItemDelegate::createEditor(parent, option, index);
        break;
    }
#ifdef SUPPORT_TABLEWIDGET_USER_EDITING
    if(nullptr != widget)
    {
        rowEditing = index.row();
        colEditing = index.column();
    }
#endif
    return widget;
}
void TableWidgetDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
#ifdef SUPPORT_TABLEWIDGET_USER_EDITING
    rowEditing = -1;
    colEditing = -1;
#endif
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    const QAbstractItemModel* model = index.model();
    int type = model->data(index, Qt::UserRole+ROLE_TYPE).toInt();
    switch(type)
    {
    case TYPE_LINEEDIT:
    case TYPE_SPINBOX:
    case TYPE_DOUBLESPINBOX:
    case TYPE_COMBOBOX:
        break;
    default:
        QItemDelegate::destroyEditor(editor, index);
        break;
    }
#else
    QItemDelegate::destroyEditor(editor, index);
#endif
}

void TableWidgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QAbstractItemModel* model = index.model();
    int type = editor->property("VDType").toInt();
    switch(type)
    {
    case TYPE_DOUBLE:
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        if(nullptr != edit)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            edit->setText(doubleToText(var.toDouble()));
        }
        break;
    }
    case TYPE_LIST:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_COMBOBOX:
#endif
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if(nullptr != box)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            box->setCurrentIndex(var.toInt());
        }
        break;
    }
    case TYPE_VLIST:
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if(nullptr != box)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            QList<QVariant> vals = model->data(index, Qt::UserRole+ROLE_DATA2).toList();
            box->setCurrentIndex(vals.indexOf(var));
        }
        break;
    }
    case TYPE_EDIT:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_LINEEDIT:
#endif
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        if(nullptr != edit)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            edit->setText(var.toString());
        }
        break;
    }
    case TYPE_SPIN:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_SPINBOX:
#endif
    {
        QSpinBox* spin = qobject_cast<QSpinBox*>(editor);
        if(nullptr != spin)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            spin->setValue(var.toInt());
        }
        break;
    }
    case TYPE_UINTSPIN:
    {
        TableWidgetUintSpinBox* spin = qobject_cast<TableWidgetUintSpinBox*>(editor);
        if(nullptr != spin)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            spin->setValue(var.toUInt());
        }
        break;
    }
    case TYPE_DOUBLESPIN:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_DOUBLESPINBOX:
#endif
    {
        QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(editor);
        if(nullptr != spin)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            spin->setValue(var.toDouble());
        }
        break;
    }
    case TYPE_DATE:
    {
        QDateEdit* edit = qobject_cast<QDateEdit*>(editor);
        if(nullptr != edit)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            edit->setDate(var.toDate());
        }
        break;
    }
    case TYPE_TIME:
    {
        QTimeEdit* edit = qobject_cast<QTimeEdit*>(editor);
        if(nullptr != edit)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            edit->setTime(var.toTime());
        }
        break;
    }
    case TYPE_DATETIME:
    {
        QDateTimeEdit* edit = qobject_cast<QDateTimeEdit*>(editor);
        if(nullptr != edit)
        {
            QVariant var = model->data(index, Qt::UserRole+ROLE_VALUE);
            edit->setDateTime(var.toDateTime());
        }
        break;
    }
    default:
        QItemDelegate::setEditorData(editor, index);
        break;
    }
}

void TableWidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int type = editor->property("VDType").toInt();
    switch(type)
    {
    case TYPE_DOUBLE:
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        if(nullptr == edit || !edit->isModified())
        {
            return;
        }
        QString text = edit->text();
        const QValidator *validator = edit->validator();
        if(nullptr != validator)
        {
            int pos;
            if(validator->validate(text, pos) != QValidator::Acceptable)
            {
                return;
            }
        }
        double fval = text.toDouble();
        double fmin = model->data(index, Qt::UserRole+ROLE_DATA).toDouble();
        double fmax = model->data(index, Qt::UserRole+ROLE_DATA2).toDouble();
        if(fmin <= fmax)
        {
            if(fval < fmin || fval > fmax)
            {
                return;
            }
        }
        setItemData(model, index, doubleToText(fval), fval);
        break;
    }
    case TYPE_LIST:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_COMBOBOX:
#endif
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if(nullptr != box)
        {
            setItemData(model, index, box->currentText(), box->currentIndex());
        }
        break;
    }
    case TYPE_VLIST:
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if(nullptr != box)
        {
            QList<QVariant> vals = model->data(index, Qt::UserRole+ROLE_DATA2).toList();
            int sel = box->currentIndex();
            if(sel >= 0 && sel < vals.size())
            {
                setItemData(model, index, box->currentText(), vals[sel]);
            }
            else
            {
                setItemData(model, index, QString(), QVariant());
            }
        }
        break;
    }
    case TYPE_EDIT:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_LINEEDIT:
#endif
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        if(nullptr == edit || !edit->isModified())
        {
            return;
        }
        QString text = edit->text();
        const QValidator *validator = edit->validator();
        if(nullptr != validator)
        {
            int pos;
            if(validator->validate(text, pos) != QValidator::Acceptable)
            {
                return;
            }
        }
        setItemData(model, index, text, text);
        break;
    }
    case TYPE_SPIN:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_SPINBOX:
#endif
    {
        QSpinBox* spin = qobject_cast<QSpinBox*>(editor);
        if(nullptr != spin)
        {
            setItemData(model, index, spin->text(), spin->value());
        }
        break;
    }
    case TYPE_UINTSPIN:
    {
        TableWidgetUintSpinBox* spin = qobject_cast<TableWidgetUintSpinBox*>(editor);
        if(nullptr != spin)
        {
            setItemData(model, index, spin->text(), static_cast<uint>(spin->value()));
        }
        break;
    }
    case TYPE_DOUBLESPIN:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
    case TYPE_DOUBLESPINBOX:
#endif
    {
        QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(editor);
        if(nullptr != spin)
        {
            setItemData(model, index, spin->text(), spin->value());
        }
        break;
    }
    case TYPE_DATE:
    {
        QDateEdit* edit = qobject_cast<QDateEdit*>(editor);
        if(nullptr != edit)
        {
            setItemData(model, index, edit->text(), edit->date());
        }
        break;
    }
    case TYPE_TIME:
    {
        QTimeEdit* edit = qobject_cast<QTimeEdit*>(editor);
        if(nullptr != edit)
        {
            setItemData(model, index, edit->text(), edit->time());
        }
        break;
    }
    case TYPE_DATETIME:
    {
        QDateTimeEdit* edit = qobject_cast<QDateTimeEdit*>(editor);
        if(nullptr != edit)
        {
            setItemData(model, index, edit->text(), edit->dateTime());
        }
        break;
    }
    default:
        QItemDelegate::setModelData(editor, model, index);
        model->setData(index, model->data(index, Qt::DisplayRole), Qt::UserRole+ROLE_VALUE);
        break;
    }
}

void TableWidgetDelegate::copyToClipboard(QTableWidget* table)
{
    QList<QTableWidgetSelectionRange> list = table->selectedRanges();
    if(list.size() <= 0)
    {
        return;
    }

    QString strData;
    //just copy first selected area
    const QTableWidgetSelectionRange& range = list.at(0);
    for(int row=range.topRow(); row<=range.bottomRow(); row++)
    {
        if(row != range.topRow())
        {
            strData.append("\n");
        }
        for(int col=range.leftColumn(); col<=range.rightColumn(); ++col)
        {
            if(col != range.leftColumn())
            {
                strData.append("\t");
            }
            QTableWidgetItem* item = table->item(row, col);
            if(nullptr != item)
            {
                int type = item->data(Qt::UserRole+ROLE_TYPE).toInt();
                if(TYPE_CHECK == type)
                {
                    strData.append(item->data(Qt::UserRole+ROLE_VALUE).toBool() ? "1" : "0");
                }
                else
                {
                    strData.append(item->text());
                }
            }
        }
    }
    QApplication::clipboard()->setText(strData);
}
QString TableWidgetDelegate::doubleToText(double fval)
{
    QString str;
    if(0 == fval)
    {
        str = "0";
    }
    else
    {
        double base = std::log10(std::abs(fval));
        //大整数不要科学计数，保留有限位小数
        if(base > 0)
        {
            str = QString::number(fval, 'g', base + 7);
        }
        //大一点的小数直接显示
        else if(base >= -3)
        {
            str = QString::number(fval, 'g', 7 - base);
        }
        //小一点的小数会产生科学计数，此处暂用笨办法指定精度去零
        else if(base >= -10)
        {
            str = QString::number(fval, 'f', 7 - base);
#if QT_VERSION >= 0x050A00
            while(str.back() == QChar('0'))
#else
            while(str.size() > 0 && str.at(str.size() - 1) == QChar('0'))
#endif
            {
                str.remove(str.size() - 1, 1);
            }
        }
        //极小的小数用科学计数显示
        else
        {
            str = QString::number(fval, 'g', 7);
        }
    }
    return str;
}
QString TableWidgetDelegate::intSpinToText(int ival, int base, const QString& prefix, const QString& suffix)
{
    QString text = QString("%1").arg(ival, base/100, base%100, QChar('0')).toUpper();
    return QString("%1%2%3").arg(prefix).arg(text).arg(suffix);
}
QString TableWidgetDelegate::uintSpinToText(uint uval, int base, const QString& prefix, const QString& suffix)
{
    QString text = QString("%1").arg(uval, base/100, base%100, QChar('0')).toUpper();
    return QString("%1%2%3").arg(prefix).arg(text).arg(suffix);
}
QString TableWidgetDelegate::doubleSpinToText(double fval, int prec, const QString& prefix, const QString& suffix)
{
    return QString("%1%2%3").arg(prefix).arg(fval, 0, 'f', prec).arg(suffix);
}
//避免多次触发itemChanged
void TableWidgetDelegate::setItemData(QAbstractItemModel* model, const QModelIndex& index, const QString& text, const QVariant& value)
{
    QMap<int, QVariant> roles;
    roles.insert(Qt::DisplayRole, text);
    roles.insert(Qt::UserRole+ROLE_VALUE, value);
    model->setItemData(index, roles);
}
void TableWidgetDelegate::setItemData(QAbstractItemModel* model, int row, int col, const QString& text, const QVariant& value)
{
    QMap<int, QVariant> roles;
    roles.insert(Qt::DisplayRole, text);
    roles.insert(Qt::UserRole+ROLE_VALUE, value);
    model->setItemData(model->index(row, col), roles);
}
//无法解决itemChange信号编辑问题，暂时不能粘贴特殊类型
void TableWidgetDelegate::pasteFromClipboard(QTableWidget* table)
{
    if(QTableWidget::NoEditTriggers == table->editTriggers())
    {
        return;
    }
    QString strData = QApplication::clipboard()->text();
    if(strData.isEmpty())
    {
        return;
    }
    QAbstractItemModel* model = table->model();
    if(nullptr == model)
    {
        return;
    }
    QList<QTableWidgetSelectionRange> list = table->selectedRanges();
    if(list.size() <= 0)
    {
        return;
    }
    const QTableWidgetSelectionRange& range = list.at(0);
    int row = range.topRow();
    int col = range.leftColumn();
    QStringList lines = strData.split('\n');
    for(int i=0; i<lines.size() && row<table->rowCount(); ++i, ++row)
    {
        col = range.leftColumn();
        QStringList texts = lines[i].split('\t');
        for(int j=0; j<texts.size() && col<table->columnCount(); ++j, ++col)
        {
            QString str = texts[j];
            QTableWidgetItem* item = table->item(row, col);
            if(nullptr == item)
            {
                continue;
            }
            if(0 == (Qt::ItemIsEditable & item->flags()))
            {
                continue;
            }
            int type = item->data(Qt::UserRole+ROLE_TYPE).toInt();
            switch(type)
            {
            case TYPE_NONE:
                item->setText(str);
                break;
            case TYPE_CHECK:
            {
                bool val = (str == "true" || str == "1");
                item->setData(Qt::UserRole+ROLE_VALUE, val);
                item->setText("");
                break;
            }
            case TYPE_DOUBLE:
            {
                QRegExp regExp;
                regExp.setPattern(TABLEWIDGET_REG_DOUBLE);
                if(!regExp.isEmpty() && !regExp.exactMatch(str))
                {
                    break;
                }
                double fval = str.toDouble();
                double fmin = item->data(Qt::UserRole+ROLE_DATA).toDouble();
                double fmax = item->data(Qt::UserRole+ROLE_DATA2).toDouble();
                if(fmin <= fmax)
                {
                    if(fval < fmin || fval > fmax)
                    {
                        break;
                    }
                }
                setItemData(model, row, col, doubleToText(fval), fval);
                break;
            }
            case TYPE_EDIT:
            {
                QString strExp = item->data(Qt::UserRole+ROLE_DATA).toString();
                if(!strExp.isEmpty())
                {
                    QRegExp regExp;
                    regExp.setPattern(strExp);
                    if(!regExp.isEmpty() && !regExp.exactMatch(str))
                    {
                        break;
                    }
                }
                setItemData(model, row, col, str, str);
                //item->setData(Qt::UserRole+ROLE_VALUE, str);
                //item->setText(str);
                break;
            }
            case TYPE_SPIN:
            {
                QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                if(!prefix.isEmpty() && str.startsWith(prefix))
                {
                    str = str.mid(prefix.size());
                }
                QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                if(!suffix.isEmpty() && str.endsWith(suffix))
                {
                    str = str.mid(0, str.size() - suffix.size());
                }
                int base = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                int ival = str.toInt(nullptr, base%100);
                int imin = item->data(Qt::UserRole+ROLE_DATA).toInt();
                int imax = item->data(Qt::UserRole+ROLE_DATA2).toInt();
                if(imin > imax || (ival >= imin && ival <= imax))
                {
                    setItemData(model, row, col, intSpinToText(ival, base, prefix, suffix), ival);
                }
                break;
            }
            case TYPE_UINTSPIN:
            {
                QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                if(!prefix.isEmpty() && str.startsWith(prefix))
                {
                    str = str.mid(prefix.size());
                }
                QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                if(!suffix.isEmpty() && str.endsWith(suffix))
                {
                    str = str.mid(0, str.size() - suffix.size());
                }
                int base = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                uint uval = str.toUInt(nullptr, base%100);
                uint umin = item->data(Qt::UserRole+ROLE_DATA).toUInt();
                uint umax = item->data(Qt::UserRole+ROLE_DATA2).toUInt();
                if(umin > umax || (uval >= umin && uval <= umax))
                {
                    setItemData(model, row, col, uintSpinToText(uval, base, prefix, suffix), uval);
                }
                break;
            }
            case TYPE_DOUBLESPIN:
            {
                QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                if(!prefix.isEmpty() && str.startsWith(prefix))
                {
                    str = str.mid(prefix.size());
                }
                QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                if(!suffix.isEmpty() && str.endsWith(suffix))
                {
                    str = str.mid(0, str.size() - suffix.size());
                }
                int prec = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                double fval = str.toDouble();
                double fmin = item->data(Qt::UserRole+ROLE_DATA).toUInt();
                double fmax = item->data(Qt::UserRole+ROLE_DATA2).toUInt();
                if(fmin > fmax || (fval >= fmin && fval <= fmax))
                {
                    setItemData(model, row, col, doubleSpinToText(fval, prec, prefix, suffix), fval);
                }
                break;
            }
            case TYPE_DATE:
            {
                QDate val = QDate::fromString(str, item->data(Qt::UserRole+ROLE_DATA).toString());
                if(val.isValid())
                {
                    setItemData(model, row, col, str, val);
                }
                break;
            }
            case TYPE_TIME:
            {
                QTime val = QTime::fromString(str, item->data(Qt::UserRole+ROLE_DATA).toString());
                if(val.isValid())
                {
                    setItemData(model, row, col, str, val);
                }
                break;
            }
            case TYPE_DATETIME:
            {
                QDateTime val = QDateTime::fromString(str, item->data(Qt::UserRole+ROLE_DATA).toString());
                if(val.isValid())
                {
                    setItemData(model, row, col, str, val);
                }
                break;
            }
            case TYPE_LIST:
            {
                QStringList lst = item->data(Qt::UserRole+ROLE_DATA).toStringList();
                int sel = lst.indexOf(str);
                setItemData(model, row, col, sel >= 0 ? str : QString(), sel);
                break;
            }
            case TYPE_VLIST:
            {
                QStringList strs = item->data(Qt::UserRole+ROLE_DATA).toStringList();
                QList<QVariant> vals = item->data(Qt::UserRole+ROLE_DATA2).toList();
                int sel = strs.indexOf(str);
                if(sel >= 0 && sel < vals.size())
                {
                    setItemData(model, row, col, str, vals[sel]);
                }
                else
                {
                    setItemData(model, row, col, QString(), QVariant());
                }
                break;
            }
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
            case TYPE_LINEEDIT:
            {
                QLineEdit* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QLineEdit*>();
                QLineEdit* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QLineEdit*>();
                if(nullptr != data1 && data1 == data2)
                {
                    //insert可以使用校验，不要使用setText
                    data1->clear();
                    data1->insert(str);
                    QString res = data1->text();
                    setItemData(model, row, col, res, res);
                }
                break;
            }
            case TYPE_SPINBOX:
            {
                QSpinBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QSpinBox*>();
                QSpinBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QSpinBox*>();
                if(nullptr != data1 && data1 == data2)
                {
                    QString fix = data1->prefix();
                    if(!fix.isEmpty() && str.startsWith(fix))
                    {
                        str = str.mid(fix.size());
                    }
                    fix = data1->suffix();
                    if(!fix.isEmpty() && str.endsWith(fix))
                    {
                        str = str.mid(0, str.size() - fix.size());
                    }
                    int val = str.toInt();
                    if(val >= data1->minimum() && val <= data1->maximum())
                    {
                        data1->setValue(val);
                        setItemData(model, row, col, data1->text(), val);
                    }
                }
                break;
            }
            case TYPE_DOUBLESPINBOX:
            {
                QDoubleSpinBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QDoubleSpinBox*>();
                QDoubleSpinBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QDoubleSpinBox*>();
                if(nullptr != data1 && data1 == data2)
                {
                    QString fix = data1->prefix();
                    if(!fix.isEmpty() && str.startsWith(fix))
                    {
                        str = str.mid(fix.size());
                    }
                    fix = data1->suffix();
                    if(!fix.isEmpty() && str.endsWith(fix))
                    {
                        str = str.mid(0, str.size() - fix.size());
                    }
                    double val = str.toDouble();
                    if(val >= data1->minimum() && val <= data1->maximum())
                    {
                        data1->setValue(val);
                        setItemData(model, row, col, data1->text(), val);
                    }
                }
                break;
            }
            case TYPE_COMBOBOX:
            {
                QComboBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QComboBox*>();
                QComboBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QComboBox*>();
                if(nullptr != data1 && data1 == data2)
                {
                    if(data1->isEditable())
                    {
                        data1->setCurrentText(str);
                        setItemData(model, row, col, str, str);
                    }
                    else
                    {
                        int val = -1;
                        int num = data1->count();
                        for(int i=0; i<num; ++i)
                        {
                            if(str == data1->itemText(i))
                            {
                                val = i;
                                break;
                            }
                        }
                        data1->setCurrentIndex(val);
                        setItemData(model, row, col, val >= 0 ? str : "", val);
                    }
                }
                break;
            }
#endif
            default:
                break;
            } //switch
        } //for col
    } //for row
}
void TableWidgetDelegate::deleteSelectedContent(QTableWidget* table)
{
    if(QTableWidget::NoEditTriggers == table->editTriggers())
    {
        return;
    }
    QAbstractItemModel* model = table->model();
    if(nullptr == model)
    {
        return;
    }
    QList<QTableWidgetSelectionRange> list = table->selectedRanges();
    if(list.size() <= 0)
    {
        return;
    }
    for(int i=0; i<list.size(); ++i)
    {
        const QTableWidgetSelectionRange& range = list.at(i);
        for(int row=range.topRow(); row<=range.bottomRow(); ++row)
        {
            for(int col=range.leftColumn(); col<=range.rightColumn(); ++col)
            {
                QTableWidgetItem* item = table->item(row, col);
                if(nullptr == item)
                {
                    continue;
                }
                if(0 == (Qt::ItemIsEditable & item->flags()))
                {
                    continue;
                }
                int type = item->data(Qt::UserRole+ROLE_TYPE).toInt();
                switch(type)
                {
                case TYPE_NONE:
                    item->setText("");
                    break;
                case TYPE_CHECK:
                {
                    item->setData(Qt::UserRole+ROLE_VALUE, false);
                    item->setText("");
                    break;
                }
                case TYPE_DOUBLE:
                {
                    double fval = 0;
                    double fmin = item->data(Qt::UserRole+ROLE_DATA).toDouble();
                    double fmax = item->data(Qt::UserRole+ROLE_DATA2).toDouble();
                    if(fmin <= fmax)
                    {
                        if(fval < fmin)
                        {
                            fval = fmin;
                        }
                        else if(fval > fmax)
                        {
                            fval = fmax;
                        }
                    }
                    setItemData(model, row, col, doubleToText(fval), fval);
                    break;
                }
                case TYPE_EDIT:
                {
                    QString str;
                    QString strExp = item->data(Qt::UserRole+ROLE_DATA).toString();
                    if(!strExp.isEmpty())
                    {
                        QRegExp regExp;
                        regExp.setPattern(strExp);
                        if(!regExp.isEmpty() && !regExp.exactMatch(str))
                        {
                            break;
                        }
                    }
                    setItemData(model, row, col, str, str);
                    break;
                }
                case TYPE_SPIN:
                {
                    QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                    QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                    int base = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                    int ival = 0;
                    int imin = item->data(Qt::UserRole+ROLE_DATA).toInt();
                    int imax = item->data(Qt::UserRole+ROLE_DATA2).toInt();
                    if(imin <= imax)
                    {
                        if(ival < imin)
                        {
                            ival = imin;
                        }
                        else if(ival > imax)
                        {
                            ival = imax;
                        }
                    }
                    setItemData(model, row, col, intSpinToText(ival, base, prefix, suffix), ival);
                    break;
                }
                case TYPE_UINTSPIN:
                {
                    QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                    QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                    int base = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                    uint uval = 0;
                    uint umin = item->data(Qt::UserRole+ROLE_DATA).toUInt();
                    uint umax = item->data(Qt::UserRole+ROLE_DATA2).toUInt();
                    if(umin <= umax)
                    {
                        if(uval < umin)
                        {
                            uval = umin;
                        }
                        else if(uval > umax)
                        {
                            uval = umax;
                        }
                    }
                    setItemData(model, row, col, uintSpinToText(uval, base, prefix, suffix), uval);
                    break;
                }
                case TYPE_DOUBLESPIN:
                {
                    QString prefix = item->data(Qt::UserRole+ROLE_DATA5).toString();
                    QString suffix = item->data(Qt::UserRole+ROLE_DATA4).toString();
                    int prec = item->data(Qt::UserRole+ROLE_DATA3).toInt();
                    double fval = 0;
                    double fmin = item->data(Qt::UserRole+ROLE_DATA).toUInt();
                    double fmax = item->data(Qt::UserRole+ROLE_DATA2).toUInt();
                    if(fmin <= fmax)
                    {
                        if(fval < fmin)
                        {
                            fval = fmin;
                        }
                        else if(fval > fmax)
                        {
                            fval = fmax;
                        }
                    }
                    setItemData(model, row, col, doubleSpinToText(fval, prec, prefix, suffix), fval);
                    break;
                }
                case TYPE_DATE:
                {
                    QDate val = QDate::fromJulianDay(0);
                    if(val.isValid())
                    {
                        setItemData(model, row, col, val.toString(item->data(Qt::UserRole+ROLE_DATA).toString()), val);
                    }
                    break;
                }
                case TYPE_TIME:
                {
                    QTime val = QTime::fromMSecsSinceStartOfDay(0);
                    if(val.isValid())
                    {
                        setItemData(model, row, col, val.toString(item->data(Qt::UserRole+ROLE_DATA).toString()), val);
                    }
                    break;
                }
                case TYPE_DATETIME:
                {
                    QDateTime val = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC);
                    if(val.isValid())
                    {
                        setItemData(model, row, col, val.toString(item->data(Qt::UserRole+ROLE_DATA).toString()), val);
                    }
                    break;
                }
                case TYPE_LIST:
                {
                    QStringList lst = item->data(Qt::UserRole+ROLE_DATA).toStringList();
                    if(lst.size() > 0)
                    {
                        setItemData(model, row, col, lst[0], 0);
                    }
                    else
                    {
                        setItemData(model, row, col, QString(), -1);
                    }
                    break;
                }
                case TYPE_VLIST:
                {
                    QStringList strs = item->data(Qt::UserRole+ROLE_DATA).toStringList();
                    QList<QVariant> vals = item->data(Qt::UserRole+ROLE_DATA2).toList();
                    if(strs.size() > 0 && vals.size() > 0)
                    {
                        setItemData(model, row, col, strs[0], vals[0]);
                    }
                    else
                    {
                        setItemData(model, row, col, QString(), QVariant());
                    }
                    break;
                }
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
                case TYPE_LINEEDIT:
                {
                    QLineEdit* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QLineEdit*>();
                    QLineEdit* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QLineEdit*>();
                    if(nullptr != data1 && data1 == data2)
                    {
                        //insert可以使用校验，不要使用setText
                        data1->clear();
                        QString res = data1->text();
                        setItemData(model, row, col, res, res);
                    }
                    break;
                }
                case TYPE_SPINBOX:
                {
                    QSpinBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QSpinBox*>();
                    QSpinBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QSpinBox*>();
                    if(nullptr != data1 && data1 == data2)
                    {
                        int val = 0;
                        if(val < data1->minimum())
                        {
                            val = data1->minimum();
                        }
                        else if(val > data1->maximum())
                        {
                            val = data1->maximum();
                        }
                        data1->setValue(val);
                        setItemData(model, row, col, data1->text(), val);
                    }
                    break;
                }
                case TYPE_DOUBLESPINBOX:
                {
                    QDoubleSpinBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QDoubleSpinBox*>();
                    QDoubleSpinBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QDoubleSpinBox*>();
                    if(nullptr != data1 && data1 == data2)
                    {
                        double val = 0;
                        if(val < data1->minimum())
                        {
                            val = data1->minimum();
                        }
                        else if(val > data1->maximum())
                        {
                            val = data1->maximum();
                        }
                        data1->setValue(val);
                        setItemData(model, row, col, data1->text(), val);
                    }
                    break;
                }
                case TYPE_COMBOBOX:
                {
                    QComboBox* data1 = item->data(Qt::UserRole+ROLE_DATA).value<QComboBox*>();
                    QComboBox* data2 = item->data(Qt::UserRole+ROLE_DATA2).value<QComboBox*>();
                    if(nullptr != data1 && data1 == data2)
                    {
                        if(data1->isEditable())
                        {
                            data1->setCurrentText("");
                            setItemData(model, row, col, "", "");
                        }
                        else
                        {
                            int num = data1->count();
                            if(num > 0)
                            {
                                data1->setCurrentIndex(0);
                                setItemData(model, row, col, data1->itemText(0), 0);
                            }
                        }
                    }
                    break;
                }
#endif
                default:
                    break;
                } //switch
            } //for col
        } //for row
    } //for i
}
//////////////////////////////////////////////////////////////////////////////////
TableWidgetFilter::TableWidgetFilter(QTableWidget* widget, bool allowPaste)
    : QObject(widget), table(widget), paste(allowPaste)
{
}
bool TableWidgetFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->matches(QKeySequence::Copy))
        {
            TableWidgetDelegate::copyToClipboard(table);
            return true;
        }
        else if(paste && keyEvent->matches(QKeySequence::Paste))
        {
            TableWidgetDelegate::pasteFromClipboard(table);
            return true;
        }
        else if(keyEvent->matches(QKeySequence::Delete))
        {
            TableWidgetDelegate::deleteSelectedContent(table);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

//////////////////////////////////////////////////////////////////////////////////
TableWidgetHelper::TableWidgetHelper(QTableWidget* widget) : table(widget)
{
}
void TableWidgetHelper::initTableWidgetAll(QTableWidget* widget)
{
    initTableWidgetDelegate(widget);
    initTableWidgetClipboard(widget);
}
void TableWidgetHelper::initTableWidgetDelegate(QTableWidget* widget)
{
    widget->setItemDelegate(new TableWidgetDelegate(widget));
}
void TableWidgetHelper::initTableWidgetClipboard(QTableWidget* widget, bool allowPaste)
{
    widget->installEventFilter(new TableWidgetFilter(widget, allowPaste));
}
void TableWidgetHelper::initTableWidgetAll()
{
    initTableWidgetDelegate(table);
    initTableWidgetClipboard(table);
}
void TableWidgetHelper::initTableWidgetDelegate()
{
    initTableWidgetDelegate(table);
}
void TableWidgetHelper::initTableWidgetClipboard(bool allowPaste)
{
    initTableWidgetClipboard(table, allowPaste);
}
int TableWidgetHelper::getRowCount() const
{
    return table->rowCount();
}
void TableWidgetHelper::setRowCount(int rows)
{
    table->setRowCount(rows);
}
int TableWidgetHelper::getColCount() const
{
    return table->columnCount();
}
void TableWidgetHelper::setColCount(int cols)
{
    table->setColumnCount(cols);
}
void TableWidgetHelper::setSpan(int row, int col, int rows, int cols)
{
    table->setSpan(row, col, rows, cols);
}
void TableWidgetHelper::clearSpans()
{
    table->clearSpans();
}
QTableWidgetItem* TableWidgetHelper::makeHorizontalHeaderItem(int col, const QString& text)
{
    QTableWidgetItem* item = table->horizontalHeaderItem(col);
    if(nullptr == item)
    {
        item = new QTableWidgetItem(text);
        if(nullptr != item)
        {
            table->setHorizontalHeaderItem(col, item);
        }
    }
    else
    {
        item->setText(text);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeVerticalHeaderItem(int row, const QString& text)
{
    QTableWidgetItem* item = table->verticalHeaderItem(row);
    if(nullptr == item)
    {
        item = new QTableWidgetItem(text);
        if(nullptr != item)
        {
            table->setVerticalHeaderItem(row, item);
        }
    }
    else
    {
        item->setText(text);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeItem(int row, int col, bool editable)
{
    QTableWidgetItem* item = table->item(row, col);
    if(nullptr == item)
    {
        item = new QTableWidgetItem();
        if(nullptr != item)
        {
            table->setItem(row, col, item);
        }
    }
    if(nullptr != item)
    {
        if(editable)
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
        else
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeItem(int row, int col, const QString& text, bool editable)
{
    QTableWidgetItem* item = makeItem(row, col, editable);
    if(nullptr != item)
    {
        item->setText(text);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeItemEditable(int row, int col, const QString& text)
{
    return makeItem(row, col, text, true);
}

QTableWidgetItem* TableWidgetHelper::makeItemReadOnly(int row, int col, const QString& text)
{
    return makeItem(row, col, text, false);
}

bool TableWidgetHelper::setVerticalHeaderItemData(int row, int role, const QVariant & value)
{
    bool ret = false;
    QTableWidgetItem* item = table->verticalHeaderItem(row);
    if(nullptr != item)
    {
        item->setData(role, value);
        ret = true;
    }
    return ret;
}

bool TableWidgetHelper::setHorizontalHeaderItemData(int col, int role, const QVariant & value)
{
    bool ret = false;
    QTableWidgetItem* item = table->horizontalHeaderItem(col);
    if(nullptr != item)
    {
        item->setData(role, value);
        ret = true;
    }
    return ret;
}
QVariant TableWidgetHelper::getVerticalHeaderItemData(int row, int role)
{
    QTableWidgetItem* item = table->verticalHeaderItem(row);
    return nullptr != item ? item->data(role) : QVariant();
}
QVariant TableWidgetHelper::getHorizontalHeaderItemData(int col, int role)
{
    QTableWidgetItem* item = table->horizontalHeaderItem(col);
    return nullptr != item ? item->data(role) : QVariant();
}
bool TableWidgetHelper::setItemData(int row, int col, int role, const QVariant & value)
{
    bool ret = false;
    QTableWidgetItem* item = table->item(row, col);
    if(nullptr != item)
    {
        item->setData(role, value);
        ret = true;
    }
    return ret;
}

bool TableWidgetHelper::setItemEditable(int row, int col, bool editable)
{
    bool ret = false;
    QTableWidgetItem* item = table->item(row, col);
    if(nullptr != item)
    {
        if(editable)
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
        else
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ret = true;
    }
    return ret;
}
bool TableWidgetHelper::setItemChecked(int row, int col, bool checked)
{
    bool ret = false;
    QTableWidgetItem* item = table->item(row, col);
    if(nullptr != item)
    {
        item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        ret = true;
    }
    return ret;
}
void TableWidgetHelper::setRowData(int row, int role, const QVariant & value, bool header)
{
    int cols = table->columnCount();
    for(int col=0; col<cols; ++col)
    {
        setItemData(row, col, role, value);
    }
    if(header)
    {
        setVerticalHeaderItemData(row, role, value);
    }
}

void TableWidgetHelper::setColData(int col, int role, const QVariant & value, bool header)
{
    int rows = table->rowCount();
    for(int row=0; row<rows; ++row)
    {
        setItemData(row, col, role, value);
    }
    if(header)
    {
        setHorizontalHeaderItemData(col, role, value);
    }
}
QVariant TableWidgetHelper::getItemData(int row, int col, int role)
{
    QTableWidgetItem* item = table->item(row, col);
    return nullptr != item ? item->data(role) : QVariant();
}
QString TableWidgetHelper::getItemText(int row, int col) const
{
    QTableWidgetItem* item = table->item(row, col);
    return nullptr != item ? item->text() : QString();
}
bool TableWidgetHelper::isItemChecked(int row, int col) const
{
    QTableWidgetItem* item = table->item(row, col);
    return nullptr != item ? item->checkState() == Qt::Checked : false;
}
bool TableWidgetHelper::isItemSelected(int row, int col) const
{
    QTableWidgetItem* item = table->item(row, col);
    return nullptr != item ? item->isSelected() : false;
}
bool TableWidgetHelper::isItemEditing(int row, int col) const
{
#ifdef SUPPORT_TABLEWIDGET_USER_EDITING
    QAbstractItemDelegate* ptr = table->itemDelegate();
    if(nullptr != ptr)
    {
        TableWidgetDelegate* dat = qobject_cast<TableWidgetDelegate*>(ptr);
        if(nullptr != dat)
        {
            return dat->rowEditing == row && dat->colEditing == col;
        }
    }
    return false;
#else
    QTableWidgetItem* item = table->item(row, col);
    return nullptr != item ? table->isPersistentEditorOpen(item) : false;
#endif
}
bool TableWidgetHelper::isRowSelected(int row) const
{
    int cols = table->columnCount();
    for(int col=0; col<cols; ++col)
    {
        if(isItemSelected(row, col))
        {
            return true;
        }
    }
    return false;
}
bool TableWidgetHelper::isColSelected(int col) const
{
    int rows = table->rowCount();
    for(int row=0; row<rows; ++row)
    {
        if(isItemSelected(row, col))
        {
            return true;
        }
    }
    return false;
}
int TableWidgetHelper::getFirstSelectedRow() const
{
    QList<QTableWidgetSelectionRange> lst = table->selectedRanges();
    return lst.size() > 0 ? lst.first().topRow() : -1;
}
int TableWidgetHelper::getFirstSelectedCol() const
{
    QList<QTableWidgetSelectionRange> lst = table->selectedRanges();
    return lst.size() > 0 ? lst.first().leftColumn() : -1;
}
int TableWidgetHelper::getLastSelectedRow() const
{
    QList<QTableWidgetSelectionRange> lst = table->selectedRanges();
    return lst.size() > 0 ? lst.first().bottomRow() : -1;
}
int TableWidgetHelper::getLastSelectedCol() const
{
    QList<QTableWidgetSelectionRange> lst = table->selectedRanges();
    return lst.size() > 0 ? lst.first().rightColumn() : -1;
}

QTableWidgetItem* TableWidgetHelper::makeStaticItem(int row, int col, const QString& text)
{
    QTableWidgetItem* item = makeItem(row, col, false);
    if(nullptr != item)
    {
        item->setText(text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_NONE);
    }
    return item;
}
//本接口不支持DelegateValue，要编辑Text，请使用makeTextItem
QTableWidgetItem* TableWidgetHelper::makeNormalItem(int row, int col, const QString& text)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_NONE);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeEditItem(int row, int col, const QString& text, const QString& regExp, const QString& mask)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_EDIT);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, regExp);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, mask);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeSpinItem(int row, int col, int val, int minVal, int maxVal, int base, const QString& suffix, const QString& prefix)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(TableWidgetDelegate::intSpinToText(val, base, prefix, suffix));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_SPIN);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, minVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, maxVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3, base);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4, suffix);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5, prefix);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeUintSpinItem(int row, int col, uint val, uint minVal, uint maxVal, int base, const QString& suffix, const QString& prefix)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(TableWidgetDelegate::uintSpinToText(val, base, prefix, suffix));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_UINTSPIN);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, minVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, maxVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3, base);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4, suffix);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5, prefix);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeDoubleSpinItem(int row, int col, double val, double minVal, double maxVal, int prec, const QString& suffix, const QString& prefix)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(TableWidgetDelegate::doubleSpinToText(val, prec, prefix, suffix));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_DOUBLESPIN);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, minVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, maxVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3, prec);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4, suffix);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5, prefix);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeDateItem(int row, int col, const QDate& val, const QString& fmt)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        QString sfmt = fmt.isEmpty() ? "yyyy-MM-dd" : fmt;
        item->setText(val.toString(sfmt));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_DATE);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, sfmt);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeTimeItem(int row, int col, const QTime& val, const QString& fmt)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        QString sfmt = fmt.isEmpty() ? "HH:mm:ss" : fmt;
        item->setText(val.toString(sfmt));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_TIME);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, sfmt);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeDateTimeItem(int row, int col, const QDateTime& val, const QString& fmt)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        QString sfmt = fmt.isEmpty() ? "yyyy-MM-dd HH:mm:ss" : fmt;
        item->setText(val.toString(sfmt));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_DATETIME);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, sfmt);
    }
    return item;
}
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
QTableWidgetItem* TableWidgetHelper::makeEditItem(int row, int col, const QString& text, QLineEdit* edit)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_LINEEDIT);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, text);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, QVariant::fromValue(edit));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, QVariant::fromValue(edit));
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeSpinItem(int row, int col, int val, QSpinBox* spin)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        spin->setValue(val);
        item->setText(spin->text());
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_SPINBOX);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, QVariant::fromValue(spin));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, QVariant::fromValue(spin));
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeDoubleSpinItem(int row, int col, double val, QDoubleSpinBox* spin)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        spin->setValue(val);
        item->setText(spin->text());
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_DOUBLESPINBOX);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, QVariant::fromValue(spin));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, QVariant::fromValue(spin));
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeComboListItem(int row, int col, int val, QComboBox* combo)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        combo->setCurrentIndex(val);
        item->setText(combo->currentText());
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_COMBOBOX);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, QVariant::fromValue(combo));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, QVariant::fromValue(combo));
    }
    return item;
}
#endif
QTableWidgetItem* TableWidgetHelper::makeCheckItem(int row, int col, bool val, bool editable)
{
    QTableWidgetItem* item = makeItem(row, col, editable);
    if(nullptr != item)
    {
        item->setText("");
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_CHECK);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeTextItem(int row, int col, const QString& text)
{
    return makeEditItem(row, col, text);
}
//无单位的普通浮点（小数点位数不固定）
QTableWidgetItem* TableWidgetHelper::makeDoubleItem(int row, int col, double val, double minVal, double maxVal)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(TableWidgetDelegate::doubleToText(val));
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_DOUBLE);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, minVal);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, maxVal);
    }
    return item;
}

QTableWidgetItem* TableWidgetHelper::makeListItem(int row, int col, int sel, const QStringList& lst)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        item->setText(sel >= 0 && sel < lst.size() ? lst.at(sel) : "");
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_LIST);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, sel);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, lst);
    }
    return item;
}
QTableWidgetItem* TableWidgetHelper::makeListItem(int row, int col, const QVariant& val, const QList<QVariant>& vals, const QStringList& names)
{
    QTableWidgetItem* item = makeItem(row, col, true);
    if(nullptr != item)
    {
        int sel = vals.indexOf(val);
        item->setText(sel >= 0 && sel < names.size() ? names.at(sel) : "");
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE, TableWidgetDelegate::TYPE_VLIST);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, val);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA, names);
        item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2, vals);
    }
    return item;
}
QVariant TableWidgetHelper::getDelegateItemValue(int row, int col)
{
    return getDelegateItemValue(table->item(row, col));
}

void TableWidgetHelper::setDelegateItemValue(int row, int col, const QVariant& value)
{
    setDelegateItemValue(table->item(row, col), value);
}

QVariant TableWidgetHelper::getDelegateItemValue(QTableWidgetItem* item)
{
    return nullptr != item ? item->data(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE) : QVariant();
}
void TableWidgetHelper::setDelegateItemValue(QTableWidgetItem* item, const QVariant& value)
{
    if(nullptr != item)
    {
        int type = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_TYPE).toInt();
        switch(type)
        {
        case TableWidgetDelegate::TYPE_NONE:
            item->setText(value.toString());
            break;
        case TableWidgetDelegate::TYPE_DOUBLE:
            item->setText(TableWidgetDelegate::doubleToText(value.toDouble()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_CHECK:
            item->setText("");
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_EDIT:
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
        case TableWidgetDelegate::TYPE_LINEEDIT:
#endif
            item->setText(value.toString());
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_SPIN:
            item->setText(TableWidgetDelegate::intSpinToText(
                              value.toInt(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3).toInt(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5).toString(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_UINTSPIN:
            item->setText(TableWidgetDelegate::uintSpinToText(
                              value.toUInt(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3).toInt(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5).toString(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_DOUBLESPIN:
            item->setText(TableWidgetDelegate::doubleSpinToText(
                              value.toDouble(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA3).toInt(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA5).toString(),
                              item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA4).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_LIST:
        {
            int sel = value.toInt();
            QStringList lst = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).toStringList();
            item->setText(sel >= 0 && sel < lst.size() ? lst.at(sel) : QString());
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        }
        case TableWidgetDelegate::TYPE_VLIST:
        {
            QStringList strs = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).toStringList();
            QList<QVariant> vals = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2).toList();
            int sel = vals.indexOf(value);
            item->setText(sel >= 0 && sel < strs.size() ? strs.at(sel) : QString());
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        }
        case TableWidgetDelegate::TYPE_DATE:
            item->setText(value.toDate().toString(item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_TIME:
            item->setText(value.toTime().toString(item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
        case TableWidgetDelegate::TYPE_DATETIME:
            item->setText(value.toDateTime().toString(item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).toString()));
            item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            break;
#ifdef SUPPORT_TABLEWIDGET_EDITOR_ITEM
        case TableWidgetDelegate::TYPE_SPINBOX:
        {
            QSpinBox* data1 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).value<QSpinBox*>();
            QSpinBox* data2 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2).value<QSpinBox*>();
            if(nullptr != data1 && data1 == data2)
            {
                data1->setValue(value.toInt());
                item->setText(data1->text());
                item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            }
            break;
        }
        case TableWidgetDelegate::TYPE_DOUBLESPINBOX:
        {
            QDoubleSpinBox* data1 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).value<QDoubleSpinBox*>();
            QDoubleSpinBox* data2 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2).value<QDoubleSpinBox*>();
            if(nullptr != data1 && data1 == data2)
            {
                data1->setValue(value.toDouble());
                item->setText(data1->text());
                item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
            }
            break;
        }
        case TableWidgetDelegate::TYPE_COMBOBOX:
        {
            QComboBox* data1 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA).value<QComboBox*>();
            QComboBox* data2 = item->data(Qt::UserRole+TableWidgetDelegate::ROLE_DATA2).value<QComboBox*>();
            if(nullptr != data1 && data1 == data2)
            {
                if(data1->isEditable())
                {
                    data1->setCurrentText(value.toString());
                    item->setText(data1->currentText());
                    item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
                }
                else
                {
                    data1->setCurrentIndex(value.toInt());
                    item->setText(data1->currentText());
                    item->setData(Qt::UserRole+TableWidgetDelegate::ROLE_VALUE, value);
                }
            }
            break;
        }
#endif
        default:
            break;
        }
    }
}
