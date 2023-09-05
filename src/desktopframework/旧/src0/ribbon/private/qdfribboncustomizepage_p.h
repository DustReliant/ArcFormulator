#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_P_H

#include <QList>
#include <QString>
#include <QStyledItemDelegate>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonCustomizeRibbonStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit QdfRibbonCustomizeRibbonStyledItemDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}
    virtual ~QdfRibbonCustomizeRibbonStyledItemDelegate() {}
    void addCaption(const QString &strCategory)
    {
        Q_ASSERT(!strCategory.isEmpty());
        m_lstCaption.append(strCategory);
    }
    void clearCaptions() { m_lstCaption.clear(); }

protected:
    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
    {
        QStyledItemDelegate::initStyleOption(option, index);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (m_lstCaption.contains(option->text))
        {
            option->rect.setLeft(0);
        }
#else
        if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
        {
            if (m_lstCaption.contains(vopt->text))
            {
                option->rect.setLeft(0);
            }
        }
#endif
    }

protected:
    QList<QString> m_lstCaption;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_P_H