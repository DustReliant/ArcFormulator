#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_P_H

#include "ui_qdfribbonbarcustomizepage.h"
#include "ui_qdfribbonquickaccessbarcustomizepage.h"
#include <QDialogButtonBox>
#include <QFrame>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <qdf_global.h>
#include <ribbon/qdfribboncustomizepage.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonCustomizePageWidget : public QWidget
{
public:
    explicit QdfRibbonCustomizePageWidget(QWidget *parent);
    virtual ~QdfRibbonCustomizePageWidget();

public:
    QWidget *content() const;
    void setContent(QWidget *content);

protected:
    QHBoxLayout *m_hBoxLayout;
    QVBoxLayout *m_vBoxLayout;
    QLabel m_labelIcon;
    QLabel m_labelTitle;
    QWidget *m_content;

private:
    friend class QdfRibbonCustomizeDialog;
    Q_DISABLE_COPY(QdfRibbonCustomizePageWidget)
};

class QdfRibbonCustomizePageItem : public QListWidgetItem
{
public:
    QdfRibbonCustomizePageWidget *m_page;

public:
    explicit QdfRibbonCustomizePageItem(const QString &text, QdfRibbonCustomizePageWidget *page)
        : QListWidgetItem(text), m_page(page)
    {
    }
};


class QdfRibbonCustomizeListWidget : public QListWidget
{
public:
    explicit QdfRibbonCustomizeListWidget(QWidget *parent = nullptr);
    virtual ~QdfRibbonCustomizeListWidget();

public:
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
};

class QdfRibbonCustomizeDialogPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonCustomizeDialog)
public:
    explicit QdfRibbonCustomizeDialogPrivate();
    virtual ~QdfRibbonCustomizeDialogPrivate();

public:
    void init();
    void setCustomizeMode(bool edit);
    void addPage(QdfRibbonCustomizePageWidget *page);
    void insertPage(int index, QdfRibbonCustomizePageWidget *page);

public Q_SLOTS:
    void switchToPage(int currentRow);

public:
    QList<QWidget *> m_listWidget;
    QVBoxLayout *m_verticalLayout;
    QHBoxLayout *m_horizontalLayout;
    QListWidget *m_listPage;
    QFrame *m_pageFrame;
    QDialogButtonBox *m_buttonBox;

    int m_currentPage;
    QVBoxLayout *m_pageVBoxLayout;

private:
    Q_DISABLE_COPY(QdfRibbonCustomizeDialogPrivate)
};


class QdfRibbonCustomizeStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit QdfRibbonCustomizeStyledItemDelegate(QListWidget *listWidget);
    virtual ~QdfRibbonCustomizeStyledItemDelegate();

public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

protected:
    QListWidget *m_listWidget;

private:
    Q_DISABLE_COPY(QdfRibbonCustomizeStyledItemDelegate)
};

class QdfRibbonBar;
class QdfRibbonPage;


class QdfRibbonQuickAccessBarCustomizePagePrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonQuickAccessBarCustomizePage)
    enum ColumnCommand
    {
        nIconCommand = 0,
        nNameCommand,
        nViewCommand
    };

public:
    explicit QdfRibbonQuickAccessBarCustomizePagePrivate();
    virtual ~QdfRibbonQuickAccessBarCustomizePagePrivate();

public:
    void init();
    void setupPage();
    void initListWidgetCommands();
    QIcon createIconStyleWidget(QWidget *widget);
    QPixmap createIconExStyleWidget(QWidget *widget, bool &iconView);

    void fillActions();
    void fillStateCommands();
    void fillListCategorieCommads(const QList<QAction *> &actions);

    void setButtons();

    void applyClicked();

public Q_SLOTS:
    void setCurrentCategoryText(const QString &);
    void currentCommandChanged(QListWidgetItem *current);
    void currentQTCommandChanged(QListWidgetItem *current);

    void addClicked();
    void removeClicked();
    void resetClicked();
    void upClicked();
    void downClicked();

public:
    Ui::QdfRibbonQuickAccessBarCustomizePage *ui;
    QdfRibbonBar *m_ribbonBar;

    QStringList m_sourceCategories;
    QAction *m_separator;
    QString m_separatorText;
    QString m_strSeparator;
    QString m_currentSourceCategory;

    QListWidgetItem *m_currentAction;
    QMap<QAction *, QListWidgetItem *> m_actionToItem;
    QMap<QListWidgetItem *, QAction *> m_itemToAction;

    QMap<QAction *, QListWidgetItem *> m_actionToCurrentItem;
    QMap<QListWidgetItem *, QAction *> m_currentItemToAction;

    int m_heightRowItem;
    int m_widthColIconItem;
    int m_widthColViewItem;
    bool m_wasDisplayed;

private:
    Q_DISABLE_COPY(QdfRibbonQuickAccessBarCustomizePagePrivate)
};

class QdfRibbonGroup;
class QdfRibbonBarCustomizePagePrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonBarCustomizePage)
public:
    explicit QdfRibbonBarCustomizePagePrivate();
    virtual ~QdfRibbonBarCustomizePagePrivate();

public:
    void init();
    void setupPage();

    void setCategoryCaptionTree(QTreeWidget *tree, const QString &strCategory);
    void clearCategoryCaptionTree(QTreeWidget *tree);
    void fillSourceCategories();
    void fillSourceActions(QList<QAction *> &actions, QTreeWidgetItem *itemParent = nullptr);
    void fillSourcePages(QList<QdfRibbonPage *> &pages, const QString &strCategory);
    void fillSourceGroups(QdfRibbonPage *page, QTreeWidgetItem *itemParent);
    void fillSourceGroupActions(QdfRibbonGroup *group, QTreeWidgetItem *itemParent);

    void fillStateCategories();
    void fillStateRibbon(const QString &strCategory);
    void fillStateGroups(QdfRibbonPage *page, QTreeWidgetItem *itemParent);
    void fillStateActions(QdfRibbonGroup *group, QTreeWidgetItem *itemParent, bool newGroup);

    QdfRibbonPage *addPageCustom(QdfRibbonPage *srcPage);
    void addGroupCopy(QdfRibbonPage *srcPage, QdfRibbonPage *copyPage);
    void addActionGroupCustom(QdfRibbonGroup *group, QAction *action, int index);
    void setButtons();

    void applyClicked();
    void cancelClicked();

public Q_SLOTS:
    void addClicked();
    void newTabClicked();
    void newGroupClicked();
    void removeClicked();
    void resetClicked();
    void upClicked();
    void downClicked();
    void renameClicked();
    void setCurrentCategoryText(const QString &);
    void setCurrentTabsText(const QString &);

    void currentSourceChanged(QTreeWidgetItem *itemPage);

    void currentPageChanged(QTreeWidgetItem *itemPage);
    void itemPageChanged(QTreeWidgetItem *itemPage, int);

public:
    QdfRibbonBar *m_ribbonBar;
    Ui::QdfRibbonBarCustomizePage *ui;

    QString m_strSeparator;
    QString m_currentSourceCategory;

    QStringList m_sourceCategories;
    QTreeWidgetItem *m_currentSourceItemPage;
    QTreeWidgetItem *m_currentSourceItemGroup;
    QTreeWidgetItem *m_currentSourceAction;
    QMap<QTreeWidgetItem *, QdfRibbonPage *> m_currentSourceItemToPage;
    QMap<QTreeWidgetItem *, QdfRibbonGroup *> m_currentSourceItemToGroup;
    QMap<QTreeWidgetItem *, QAction *> m_currentSourceItemToAction;

    QTreeWidgetItem *m_currentItemPage;
    QTreeWidgetItem *m_currentItemGroup;
    QTreeWidgetItem *m_currentItemAction;
    QMap<QdfRibbonPage *, QTreeWidgetItem *> m_pageToCurrentItem;
    QMap<QTreeWidgetItem *, QdfRibbonPage *> m_currentItemToPage;

    QMap<QAction *, QTreeWidgetItem *> m_actionToCurrentItem;
    QMap<QTreeWidgetItem *, QAction *> m_currentItemToAction;
    QMap<QTreeWidgetItem *, QdfRibbonGroup *> m_currentItemToGroup;

    QString m_sufNameCustom;
    bool m_wasDisplayed;

private:
    Q_DISABLE_COPY(QdfRibbonBarCustomizePagePrivate)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_P_H