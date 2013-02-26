#include "headerdelegate.h"

HeaderDelegate::HeaderDelegate(Qt::Orientation orientation, QObject* parent) : QItemDelegate(parent), orientation(orientation)
{
}

QWidget* HeaderDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    if (!index.isValid())
        return 0;

    QWidget* editor = 0;
    HeaderView* header = qobject_cast<HeaderView*>(QObject::parent());
    if (header)
    {
        const int logicalIndex = header->logicalIndex(index.row());
        const QVariant data = index.model()->headerData(logicalIndex, orientation, Qt::EditRole);
        const QVariant::Type type = static_cast<QVariant::Type>(data.userType());
        editor = editorFactory()->createEditor(type, parent);
    }
    return editor;
}

void HeaderDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    HeaderView* header = qobject_cast<HeaderView*>(parent());
    if (header)
    {
        const int logicalIndex = header->logicalIndex(index.row());
        const QVariant value = index.model()->headerData(logicalIndex, orientation, Qt::EditRole);
        QByteArray name = editor->metaObject()->userProperty().name();
        if (name.isEmpty())
            name = editorFactory()->valuePropertyName(static_cast<QVariant::Type>(value.userType()));
        if (!name.isEmpty())
            editor->setProperty(name, value);
    }
}

void HeaderDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    HeaderView* header = qobject_cast<HeaderView*>(parent());
    if (header)
    {
        const int logicalIndex = header->logicalIndex(index.row());
        const QVariant value = index.model()->headerData(logicalIndex, orientation, Qt::EditRole);
        QByteArray name = editor->metaObject()->userProperty().name();
        if (name.isEmpty())
            name = editorFactory()->valuePropertyName(static_cast<QVariant::Type>(value.userType()));
        if (!name.isEmpty())
            model->setHeaderData(logicalIndex, orientation, editor->property(name), Qt::EditRole);
    }
}

void HeaderDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    HeaderView* header = qobject_cast<HeaderView*>(parent());
    if (header)
    {
        const int logicalIndex = header->logicalIndex(index.row());
        const QRect rect = header->sectionRect(logicalIndex);
        editor->setGeometry(rect);
    }
}

const QItemEditorFactory* HeaderDelegate::editorFactory() const
{
    const QItemEditorFactory* factory = itemEditorFactory();
    if (factory == 0)
        factory = QItemEditorFactory::defaultFactory();
    return factory;
}

