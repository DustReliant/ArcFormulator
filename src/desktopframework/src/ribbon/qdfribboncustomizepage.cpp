#include "private/qdfribbon_p.h"
#include "private/qdfribbonbar_p.h"
#include "private/qdfribbonbutton_p.h"
#include "private/qdfribboncustomizedialog_p.h"
#include "private/qdfribboncustomizemanager_p.h"
#include "private/qdfribboncustomizepage_p.h"
#include "qdfribbon_def.h"
#include "style/qdfofficestyle_p.h"
#include "ui_qdfribbonrenamedialog.h"
#include <QBitmap>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QWidgetAction>
#include <ribbon/qdfofficestyle.h>
#include <ribbon/qdfribboncustomizemanager.h>
#include <ribbon/qdfribboncustomizepage.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

QDF_USE_NAMESPACE

static const QColor clrTransparent = QColor(255, 0, 255);


QdfRibbonCustomizeStyledItemDelegate::QdfRibbonCustomizeStyledItemDelegate(QListWidget *listWidget)
    : QStyledItemDelegate(listWidget), m_listWidget(listWidget)
{
}

QdfRibbonCustomizeStyledItemDelegate::~QdfRibbonCustomizeStyledItemDelegate() {}

void QdfRibbonCustomizeStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionViewItemV4 opt = option;
#else
    QStyleOptionViewItem opt = option;
#endif
    initStyleOption(&opt, index);

    const QWidget *widget = m_listWidget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    painter->save();
    painter->setClipRect(opt.rect);

    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    iconRect.setLeft(3);
    iconRect.setRight(19);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    int offset = iconRect.width() + 3;
    textRect.translate(offset - textRect.x(), 0);

    // draw the background
    style->proxy()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // draw the icon
    QIcon::Mode mode = QIcon::Normal;
    if (!(opt.state & QStyle::State_Enabled))
    {
        mode = QIcon::Disabled;
    }
    else if (opt.state & QStyle::State_Selected)
    {
        mode = QIcon::Selected;
    }
    QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
    opt.icon.paint(painter, iconRect, opt.decorationAlignment, mode, state);

    // draw the text
    if (!opt.text.isEmpty())
    {
        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        {
            cg = QPalette::Inactive;
        }

        if (opt.state & QStyle::State_Selected)
        {
            painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
        }
        else
        {
            painter->setPen(opt.palette.color(cg, QPalette::Text));
        }

        if (opt.state & QStyle::State_Editing)
        {
            painter->setPen(opt.palette.color(cg, QPalette::Text));
            painter->drawRect(textRect.adjusted(0, 0, -1, -1));
        }

        const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
        textRect = textRect.adjusted(textMargin, 0, -textMargin, 0);// remove width padding

        painter->setFont(opt.font);
        style->drawItemText(painter, textRect, QStyle::visualAlignment(opt.direction, opt.displayAlignment),
                            opt.palette, true /*enabled*/,
                            opt.text /*, QPalette::ColorRole textRole = QPalette::NoRole*/);
    }

    // draw the focus rect
    if (opt.state & QStyle::State_HasFocus)
    {
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(opt);
        o.rect = style->proxy()->subElementRect(QStyle::SE_ItemViewItemFocusRect, &opt, widget);
        o.state |= QStyle::State_KeyboardFocusChange;
        o.state |= QStyle::State_Item;
        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
        o.backgroundColor =
                opt.palette.color(cg, (opt.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Window);
        style->proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
    }

    painter->restore();
}

/* QdfRibbonQuickAccessBarCustomizePagePrivate */
QdfRibbonQuickAccessBarCustomizePagePrivate::QdfRibbonQuickAccessBarCustomizePagePrivate()
{
    m_ribbonBar = nullptr;
    m_separator = nullptr;
    m_currentAction = nullptr;
    m_separatorText = QdfRibbonBar::tr_compatible(RibbonSeparatorString);
    m_heightRowItem = 16;
    m_widthColIconItem = 20;
    m_widthColViewItem = 35;
    m_wasDisplayed = false;
}

QdfRibbonQuickAccessBarCustomizePagePrivate::~QdfRibbonQuickAccessBarCustomizePagePrivate() { delete m_separator; }

void QdfRibbonQuickAccessBarCustomizePagePrivate::init()
{
    QDF_Q(QdfRibbonQuickAccessBarCustomizePage)
    Q_ASSERT(m_ribbonBar != nullptr);
    ui->setupUi(q);

    QAbstractItemDelegate *itemDelegate = ui->commandsList->itemDelegate();
    ui->commandsList->setItemDelegate(new QdfRibbonCustomizeStyledItemDelegate(ui->commandsList));
    delete itemDelegate;

    QPixmap imagesSrc;
    imagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = imagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    imagesSrc = QPixmap::fromImage(img);
#else
    imagesSrc.setAlphaChannel(imagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif
    QSize sz = QSize(9, 9);
    QPixmap m_imagesUpSrc =
            imagesSrc.copy(QRect(QPoint((int) QdfOfficePaintManager::Icon_ArowUpLarge * sz.width(), 0), sz));
    QPixmap m_imagesDownSrc =
            imagesSrc.copy(QRect(QPoint((int) QdfOfficePaintManager::Icon_ArowDownLarge * sz.width(), 0), sz));

    ui->upButton->setText("");
    ui->upButton->setIcon(m_imagesUpSrc);
    ui->downButton->setText("");
    ui->downButton->setIcon(m_imagesDownSrc);

    m_separator = new QAction(m_separatorText, nullptr);
    m_separator->setSeparator(true);

    initListWidgetCommands();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(ui->comboBoxSourceCategory, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#else
    connect(ui->comboBoxSourceCategory, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#endif

    connect(ui->addButton, SIGNAL(released()), this, SLOT(addClicked()));
    connect(ui->removeButton, SIGNAL(released()), this, SLOT(removeClicked()));
    connect(ui->pushButtonReset, SIGNAL(released()), this, SLOT(resetClicked()));
    connect(ui->upButton, SIGNAL(released()), this, SLOT(upClicked()));
    connect(ui->downButton, SIGNAL(released()), this, SLOT(downClicked()));

    connect(ui->commandsList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
            SLOT(currentCommandChanged(QListWidgetItem *)));
    connect(ui->quickAccessBarList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
            SLOT(currentQTCommandChanged(QListWidgetItem *)));

    ui->checkBoxQATOn->setChecked(m_ribbonBar->quickAccessBarPosition() != QdfRibbonBar::QATopPosition);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::setupPage()
{
    Q_ASSERT(m_ribbonBar != nullptr);

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    manager->setEditMode(true);

    fillActions();
    fillStateCommands();
    setButtons();
}

QIcon QdfRibbonQuickAccessBarCustomizePagePrivate::createIconStyleWidget(QWidget *widget)
{
    return QdfDrawHelpers::createIconStyleWidget(widget);
}

QPixmap QdfRibbonQuickAccessBarCustomizePagePrivate::createIconExStyleWidget(QWidget *widget, bool &iconView)
{
    return QdfDrawHelpers::createIconExStyleWidget(widget, m_widthColViewItem, m_heightRowItem, iconView);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::initListWidgetCommands()
{
    QStyleOption opt;
    opt.init(ui->commandsList);

    QString str = QString("XX");
    int width = opt.fontMetrics.boundingRect(str).width();
    m_heightRowItem = qMax(m_heightRowItem, width);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::fillActions()
{
    ui->comboBoxSourceCategory->clear();
    ui->comboBoxSourceCategory->addItems(m_sourceCategories);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::fillStateCommands()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    ui->quickAccessBarList->clear();

    setButtons();
    QListIterator<QAction *> itAction(manager->actions(m_ribbonBar->quickAccessBar()));
    QListWidgetItem *first = nullptr;

    while (itAction.hasNext())
    {
        QAction *action = itAction.next();

        QString actionName = m_separatorText;
        if (action)
        {
            actionName = action->text();
        }

        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(actionName, ui->quickAccessBarList);
            if (action)
            {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                QIcon icon = action->property("icon").value<QIcon>();
                item->setIcon(icon);
            }
            if (!first)
            {
                first = item;
            }
        }
    }
    if (first)
    {
        ui->quickAccessBarList->setCurrentItem(first);
    }
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::fillListCategorieCommads(const QList<QAction *> &actions)
{
    ui->commandsList->clear();
    m_actionToItem.clear();
    m_itemToAction.clear();

    QListWidgetItem *item = new QListWidgetItem(m_separatorText, ui->commandsList);
    ui->commandsList->setCurrentItem(item);
    m_currentAction = item;
    m_actionToItem.insert(0, item);
    m_itemToAction.insert(item, 0);

    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext())
    {
        if (QAction *action = itAction.next())
        {
            QString actionName = action->text();
            actionName.remove(QChar::fromLatin1('&'));

            item = new QListWidgetItem(actionName, ui->commandsList);
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            m_actionToItem.insert(action, item);
            m_itemToAction.insert(item, action);
        }
    }
    ui->commandsList->setCurrentItem(m_currentAction);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::setButtons()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    bool addEnabled = false;
    bool removeEnabled = false;

    QListWidgetItem *itemCommand = ui->commandsList->currentItem();
    QListWidgetItem *itemCommandQT = ui->quickAccessBarList->currentItem();
    if (itemCommand && (itemCommandQT || ui->quickAccessBarList->count() == 0))
    {
        if (itemCommand->text() != m_separatorText)
        {
            QAction *action = m_itemToAction.value(itemCommand);
            if (action && !manager->containsAction(m_ribbonBar->quickAccessBar(), action))
            {
                addEnabled = true;
            }
        }
        else
        {
            addEnabled = true;
        }
        removeEnabled = true;
    }
    ui->addButton->setEnabled(addEnabled);
    ui->removeButton->setEnabled(removeEnabled);

    int indexRow = ui->quickAccessBarList->currentRow();
    ui->upButton->setEnabled(indexRow != 0);
    ui->downButton->setEnabled(indexRow < ui->quickAccessBarList->count() - 1);
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::applyClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);
    manager->commit();

    if (m_ribbonBar->isQuickAccessVisible())
    {
        m_ribbonBar->setQuickAccessBarPosition(ui->checkBoxQATOn->isChecked() ? QdfRibbonBar::QABottomPosition
                                                                              : QdfRibbonBar::QATopPosition);
    }
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::setCurrentCategoryText(const QString &strCategory)
{
    if (strCategory.isEmpty())
    {
        return;
    }

    if (m_strSeparator == strCategory)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        ui->comboBoxSourceCategory->setCurrentText(m_currentSourceCategory);
#else
        int index = ui->comboBoxSourceCategory->findText(m_currentSourceCategory);
        if (index != -1)
        {
            ui->comboBoxSourceCategory->setCurrentIndex(index);
        }
#endif
        return;
    }
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QList<QAction *> actions = manager->actionsByCategory(strCategory);
    fillListCategorieCommads(actions);
    m_currentSourceCategory = strCategory;
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::currentCommandChanged(QListWidgetItem *current)
{
    if (m_itemToAction.contains(current))
    {
        m_currentAction = current;
    }
    else
    {
        m_currentAction = nullptr;
    }
    setButtons();
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::currentQTCommandChanged(QListWidgetItem *current)
{
    Q_UNUSED(current);
    setButtons();
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::addClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (!m_currentAction)
    {
        return;
    }

    QListWidgetItem *currentToolBarAction = ui->quickAccessBarList->currentItem();

    QAction *action = m_itemToAction.value(m_currentAction);
    QListWidgetItem *item = nullptr;
    if (action)
    {
        if (manager->containsAction(m_ribbonBar->quickAccessBar(), action))
        {
            item = m_actionToCurrentItem.value(action);

            if (item == currentToolBarAction)
            {
                return;
            }

            int row = ui->quickAccessBarList->row(item);
            ui->quickAccessBarList->takeItem(row);
            manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
        }
        else
        {
            QString actionName = action->text();
            actionName.remove(QChar::fromLatin1('&'));
            item = new QListWidgetItem(actionName);
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_currentItemToAction.insert(item, action);
            m_actionToCurrentItem.insert(action, item);
        }
    }
    else
    {
        item = new QListWidgetItem(m_separatorText);
        m_currentItemToAction.insert(item, 0);
    }

    int row = ui->quickAccessBarList->count();
    if (currentToolBarAction)
    {
        row = ui->quickAccessBarList->row(currentToolBarAction) + 1;
    }

    ui->quickAccessBarList->insertItem(row, item);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, row);

    QListWidgetItem *itemCommand = ui->commandsList->currentItem();
    int index = ui->commandsList->row(itemCommand) + 1;
    ui->commandsList->setCurrentRow(index);

    ui->quickAccessBarList->setCurrentItem(item);

    setButtons();
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::removeClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListWidgetItem *currentToolBarAction = ui->quickAccessBarList->currentItem();
    if (!currentToolBarAction)
    {
        return;
    }

    int row = ui->quickAccessBarList->row(currentToolBarAction);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);

    QAction *action = m_currentItemToAction.value(currentToolBarAction);

    if (action)
    {
        m_actionToCurrentItem.remove(action);
    }

    m_currentItemToAction.remove(currentToolBarAction);
    delete currentToolBarAction;

    if (row == ui->quickAccessBarList->count())
    {
        row--;
    }

    if (row >= 0)
    {
        QListWidgetItem *item = ui->quickAccessBarList->item(row);
        ui->quickAccessBarList->setCurrentItem(item);
    }
    setButtons();
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::resetClicked()
{
    if (m_ribbonBar->quickAccessBar())
    {
        QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != nullptr);
        manager->reset(m_ribbonBar->quickAccessBar());
        fillStateCommands();
    }
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::upClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QListWidgetItem *currentToolBarAction = ui->quickAccessBarList->currentItem();
    if (!currentToolBarAction)
    {
        return;
    }
    int row = ui->quickAccessBarList->row(currentToolBarAction);
    if (row == 0)
    {
        return;
    }
    ui->quickAccessBarList->takeItem(row);
    int newRow = row - 1;
    ui->quickAccessBarList->insertItem(newRow, currentToolBarAction);

    QAction *action = manager->actions(m_ribbonBar->quickAccessBar()).at(row);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, newRow);
    ui->quickAccessBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

void QdfRibbonQuickAccessBarCustomizePagePrivate::downClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QListWidgetItem *currentToolBarAction = ui->quickAccessBarList->currentItem();
    if (!currentToolBarAction)
    {
        return;
    }
    int row = ui->quickAccessBarList->row(currentToolBarAction);
    if (row == ui->quickAccessBarList->count() - 1)
    {
        return;
    }
    ui->quickAccessBarList->takeItem(row);
    int newRow = row + 1;
    ui->quickAccessBarList->insertItem(newRow, currentToolBarAction);
    QAction *action = manager->actions(m_ribbonBar->quickAccessBar()).at(row);
    manager->removeActionAt(m_ribbonBar->quickAccessBar(), row);
    manager->insertAction(m_ribbonBar->quickAccessBar(), action, newRow);
    ui->quickAccessBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

/*QdfRibbonQuickAccessBarCustomizePage*/
QdfRibbonQuickAccessBarCustomizePage::QdfRibbonQuickAccessBarCustomizePage(QdfRibbonBar *ribbonBar) : QWidget(nullptr)
{
    QDF_INIT_PRIVATE(QdfRibbonQuickAccessBarCustomizePage);
    QDF_D(QdfRibbonQuickAccessBarCustomizePage)
    d->m_ribbonBar = ribbonBar;
    d->init();
}

QdfRibbonQuickAccessBarCustomizePage::~QdfRibbonQuickAccessBarCustomizePage() { QDF_FINI_PRIVATE(); }

QdfRibbonBar *QdfRibbonQuickAccessBarCustomizePage::ribbonBar() const
{
    QDF_D(const QdfRibbonQuickAccessBarCustomizePage)
    return d->m_ribbonBar;
}

void QdfRibbonQuickAccessBarCustomizePage::addCustomCategory(const QString &strCategory)
{
    QDF_D(QdfRibbonQuickAccessBarCustomizePage)
    QString str = strCategory;
    str.remove(QChar::fromLatin1('&'));
    d->m_sourceCategories.append(str);
}

void QdfRibbonQuickAccessBarCustomizePage::addSeparatorCategory(const QString &strCategory)
{
    QDF_D(QdfRibbonQuickAccessBarCustomizePage)

    QString separator = strCategory;
    if (separator.isEmpty())
    {
        separator = QString("----------");
    }

    d->m_strSeparator = separator;
    d->m_sourceCategories.append(separator);
}

void QdfRibbonQuickAccessBarCustomizePage::accepted()
{
    QDF_D(QdfRibbonQuickAccessBarCustomizePage)
    if (d->m_wasDisplayed)
    {
        d->applyClicked();
    }
}

void QdfRibbonQuickAccessBarCustomizePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QDF_D(QdfRibbonQuickAccessBarCustomizePage)
    d->m_wasDisplayed = true;
    d->setupPage();
}

///////////////////////////////////////////////////////////////////////////////
/* QdfRibbonBarCustomizePagePrivate */
QdfRibbonBarCustomizePagePrivate::QdfRibbonBarCustomizePagePrivate()
{
    m_ribbonBar = nullptr;
    m_currentSourceItemPage = nullptr;
    m_currentSourceItemGroup = nullptr;
    m_currentSourceAction = nullptr;
    m_currentItemPage = nullptr;
    m_currentItemGroup = nullptr;
    m_currentItemAction = nullptr;
    m_sufNameCustom = QString(" (Custom)");
    m_wasDisplayed = false;
}

QdfRibbonBarCustomizePagePrivate::~QdfRibbonBarCustomizePagePrivate() {}

/* QdfRibbonCustomizeRibbonStyledItemDelegate */
class QdfRibbonCustomizeRibbonStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit QdfRibbonCustomizeRibbonStyledItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
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
#endif// QT_VERSION_CHECK(4, 0, 0)
    }

protected:
    QList<QString> m_lstCaption;
};

void QdfRibbonBarCustomizePagePrivate::init()
{
    QDF_Q(QdfRibbonBarCustomizePage)
    Q_ASSERT(m_ribbonBar != nullptr);
    ui->setupUi(q);

    QPixmap imagesSrc;
    imagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = imagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    imagesSrc = QPixmap::fromImage(img);
#else
    imagesSrc.setAlphaChannel(imagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif// QT_VERSION_CHECK(4, 0, 0)
    ui->treeCommands->setItemDelegate(new QdfRibbonCustomizeRibbonStyledItemDelegate(ui->treeCommands));
    ui->treeRibbon->setItemDelegate(new QdfRibbonCustomizeRibbonStyledItemDelegate(ui->treeRibbon));
    ui->switchTabsBox->setVisible(false);

    QSize sz = QSize(9, 9);
    QPixmap m_imagesUpSrc =
            imagesSrc.copy(QRect(QPoint((int) QdfOfficePaintManager::Icon_ArowUpLarge * sz.width(), 0), sz));
    QPixmap m_imagesDownSrc =
            imagesSrc.copy(QRect(QPoint((int) QdfOfficePaintManager::Icon_ArowDownLarge * sz.width(), 0), sz));

    ui->upButton->setText("");
    ui->upButton->setIcon(m_imagesUpSrc);
    ui->downButton->setText("");
    ui->downButton->setIcon(m_imagesDownSrc);

    connect(ui->addButton, SIGNAL(released()), this, SLOT(addClicked()));
    connect(ui->newTabButton, SIGNAL(released()), this, SLOT(newTabClicked()));
    connect(ui->newGroupButton, SIGNAL(released()), this, SLOT(newGroupClicked()));
    connect(ui->removeButton, SIGNAL(released()), this, SLOT(removeClicked()));
    connect(ui->resetButton, SIGNAL(released()), this, SLOT(resetClicked()));
    connect(ui->upButton, SIGNAL(released()), this, SLOT(upClicked()));
    connect(ui->downButton, SIGNAL(released()), this, SLOT(downClicked()));
    connect(ui->renameButton, SIGNAL(released()), this, SLOT(renameClicked()));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(ui->comboBoxSourceCategory, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#else
    connect(ui->comboBoxSourceCategory, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(setCurrentCategoryText(const QString &)));
#endif

    connect(ui->treeCommands, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(currentSourceChanged(QTreeWidgetItem *)));

    connect(ui->treeRibbon, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(currentPageChanged(QTreeWidgetItem *)));
    connect(ui->treeRibbon, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(itemPageChanged(QTreeWidgetItem *, int)));
}

void QdfRibbonBarCustomizePagePrivate::setupPage()
{
    Q_ASSERT(m_ribbonBar != nullptr);

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    if (!manager->isEditMode())
    {
        manager->setEditMode();
    }
    fillSourceCategories();
    setCurrentTabsText(QString());
    fillStateCategories();
}

void QdfRibbonBarCustomizePagePrivate::setCategoryCaptionTree(QTreeWidget *tree, const QString &strCategory)
{
    QDF_Q(QdfRibbonBarCustomizePage)
    if (!strCategory.isEmpty())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree, QStringList(strCategory));
        item->setDisabled(true);
        QFont font = q->font();
        font.setBold(true);
        item->setFont(0, font);
        item->setBackground(0, QColor(238, 238, 238));
        QdfRibbonCustomizeRibbonStyledItemDelegate *ribbonStyledItemDelegate =
                dynamic_cast<QdfRibbonCustomizeRibbonStyledItemDelegate *>(tree->itemDelegate());
        Q_ASSERT(ribbonStyledItemDelegate != nullptr);
        ribbonStyledItemDelegate->addCaption(strCategory);
    }
}

void QdfRibbonBarCustomizePagePrivate::clearCategoryCaptionTree(QTreeWidget *tree)
{
    QdfRibbonCustomizeRibbonStyledItemDelegate *ribbonStyledItemDelegate =
            dynamic_cast<QdfRibbonCustomizeRibbonStyledItemDelegate *>(tree->itemDelegate());
    Q_ASSERT(ribbonStyledItemDelegate != nullptr);
    ribbonStyledItemDelegate->clearCaptions();
}

void QdfRibbonBarCustomizePagePrivate::fillSourceCategories()
{
    ui->comboBoxSourceCategory->clear();
    ui->comboBoxSourceCategory->addItems(m_sourceCategories);
    if (m_sourceCategories.size() > 0)
    {
        ui->comboBoxSourceCategory->setCurrentIndex(0);
    }
}

void QdfRibbonBarCustomizePagePrivate::fillSourceActions(QList<QAction *> &actions, QTreeWidgetItem *parentItem)
{
    QListIterator<QAction *> itAction(actions);
    QTreeWidgetItem *first = nullptr;

    while (itAction.hasNext())
    {
        QAction *action = itAction.next();
        if (action == nullptr)
        {
            continue;
        }

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty())
        {
            QTreeWidgetItem *item = nullptr;
            if (parentItem)
            {
                item = new QTreeWidgetItem(parentItem, QStringList(actionName));
            }
            else
            {
                item = new QTreeWidgetItem(ui->treeCommands, QStringList(actionName));
            }

            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            item->setIcon(0, icon);
            m_currentSourceItemToAction.insert(item, action);

            if (!first)
            {
                first = item;
            }
        }
    }

    if (first)
    {
        ui->treeCommands->setCurrentItem(first);
    }
}

void QdfRibbonBarCustomizePagePrivate::fillSourcePages(QList<QdfRibbonPage *> &pages, const QString &strCategory)
{
    Q_UNUSED(strCategory);
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListIterator<QdfRibbonPage *> itPage(pages);
    while (itPage.hasNext())
    {
        QdfRibbonPage *page = itPage.next();

        if (!manager->qdf_d()->m_ribbonManager->pageDefaultName(page).isEmpty())
        {
            QString title = manager->qdf_d()->m_ribbonManager->pageDefaultName(page);
            title.remove(QChar::fromLatin1('&'));

            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeCommands, QStringList(title));
            fillSourceGroups(page, item);

            m_currentSourceItemToPage.insert(item, page);
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::fillSourceGroups(QdfRibbonPage *page, QTreeWidgetItem *parentItem)
{
    Q_ASSERT(page != nullptr);
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QList<QdfRibbonGroup *> listGroup = manager->qdf_d()->m_ribbonManager->pageDefaultGroups(page);
    for (int i = 0, count = listGroup.count(); count > i; ++i)
    {
        QString groupTitle = manager->qdf_d()->m_ribbonManager->groupDefaultName(listGroup[i]);
        groupTitle.remove(QChar::fromLatin1('&'));
        if (!groupTitle.isEmpty())
        {
            QTreeWidgetItem *newItem = new QTreeWidgetItem(parentItem, QStringList(groupTitle));
            fillSourceGroupActions(listGroup[i], newItem);
            m_currentSourceItemToGroup.insert(newItem, listGroup[i]);
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::fillSourceGroupActions(QdfRibbonGroup *group, QTreeWidgetItem *parentItem)
{
    Q_ASSERT(group != nullptr);
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    QListIterator<QAction *> itAction(manager->actionsGroup(group));
    while (itAction.hasNext())
    {
        QAction *action = itAction.next();

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty())
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(parentItem, QStringList(actionName));
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            item->setIcon(0, icon);
            m_currentSourceItemToAction.insert(item, action);
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::fillStateCategories()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    QStringList listCommands = manager->categories();

    QListIterator<QString> itAction(listCommands);
    QStringList list;
    while (itAction.hasNext())
    {
        QString str = itAction.next();
        str.remove(QChar::fromLatin1('&'));
        list << str;
    }

    ui->switchTabsBox->addItems(list);
    if (listCommands.size() > 0)
    {
        ui->switchTabsBox->setCurrentIndex(0);
    }
}

void QdfRibbonBarCustomizePagePrivate::fillStateRibbon(const QString &strCategory)
{
    Q_UNUSED(strCategory);
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    ui->treeRibbon->clear();
    m_pageToCurrentItem.clear();
    m_currentItemToPage.clear();
    m_currentItemToGroup.clear();

    QList<QdfRibbonPage *> pages = manager->pages();
    QListIterator<QdfRibbonPage *> itPages(pages);
    QTreeWidgetItem *first = nullptr;

    while (itPages.hasNext())
    {
        if (QdfRibbonPage *page = itPages.next())
        {
            QString pageTitle = !manager->pageName(page).isEmpty() ? manager->pageName(page) : page->title();
            pageTitle.remove(QChar::fromLatin1('&'));

            if (!pageTitle.isEmpty())
            {
                if (page->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                {
                    pageTitle += m_sufNameCustom;
                }

                QStringList stringsPage;
                stringsPage.append(pageTitle);
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeRibbon, stringsPage);

                if (manager->isPageHidden(page))
                {
                    item->setCheckState(0, Qt::Unchecked);
                }
                else
                {
                    item->setCheckState(0, Qt::Checked);
                }

                m_pageToCurrentItem.insert(page, item);
                m_currentItemToPage.insert(item, page);

                fillStateGroups(page, item);

                if (!first)
                {
                    first = item;
                }
            }
        }
    }

    if (first)
    {
        ui->treeRibbon->setCurrentItem(first);
        ui->treeRibbon->expandItem(first);
    }
}

void QdfRibbonBarCustomizePagePrivate::fillStateGroups(QdfRibbonPage *page, QTreeWidgetItem *itemParent)
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QList<QdfRibbonGroup *> groups = manager->pageGroups(page);
    QListIterator<QdfRibbonGroup *> itGroups(groups);

    while (itGroups.hasNext())
    {
        QdfRibbonGroup *group = itGroups.next();
        QString groupTitle = !manager->groupName(group).isEmpty() ? manager->groupName(group) : group->title();

        groupTitle.remove(QChar::fromLatin1('&'));

        if (!groupTitle.isEmpty())
        {
            if (group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
            {
                groupTitle += m_sufNameCustom;
            }

            QStringList stringsGroup;
            stringsGroup.append(groupTitle);
            QTreeWidgetItem *item = new QTreeWidgetItem(itemParent, stringsGroup);
            m_currentItemToGroup.insert(item, group);
            fillStateActions(group, item, false);
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::fillStateActions(QdfRibbonGroup *group, QTreeWidgetItem *itemParent, bool newGroup)
{
    Q_ASSERT(group != nullptr);
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    for (int i = itemParent->childCount() - 1; i >= 0; --i)
    {
        QTreeWidgetItem *item = itemParent->takeChild(i);
        QAction *action = m_currentItemToAction.value(item);
        m_currentItemToAction.remove(item);
        m_actionToCurrentItem.remove(action);
        delete item;
    }

    bool isCustomGroup = group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom");
    QListIterator<QAction *> itAction(newGroup ? group->actions() : manager->actionsGroup(group));
    while (itAction.hasNext())
    {
        QAction *action = itAction.next();

        QString actionName = action->text();
        actionName.remove(QChar::fromLatin1('&'));

        if (!actionName.isEmpty())
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(itemParent, QStringList(actionName));
            if (!isCustomGroup)
            {
                item->setForeground(0, QColor(Qt::gray));
            }

            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            QIcon icon = action->property("icon").value<QIcon>();
            QPixmap px = icon.pixmap(QSize(16, 16), isCustomGroup ? QIcon::Normal : QIcon::Disabled);
            item->setIcon(0, QIcon(px));

            if (isCustomGroup)
            {
                m_currentItemToAction.insert(item, action);
                m_actionToCurrentItem.insert(action, item);
            }
        }
    }
    if (isCustomGroup)
    {
        manager->appendActions(group, manager->actionsGroup(group));
    }
}

QdfRibbonPage *QdfRibbonBarCustomizePagePrivate::addPageCustom(QdfRibbonPage *srcPage)
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QString strNewPage(QdfRibbonBar::tr_compatible(RibbonNewPageString));
    if (srcPage)
    {
        strNewPage = srcPage->title();
        strNewPage.remove('&');
    }

    QdfRibbonPage *findPage = m_currentItemToPage.value(m_currentItemPage);
    int index = manager->pageIndex(findPage) + 1;
    QdfRibbonPage *newPage = manager->createPage(strNewPage, index);

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeRibbon, m_currentItemPage);
    item->setCheckState(0, Qt::Checked);
    strNewPage += m_sufNameCustom;
    item->setText(0, strNewPage);

    m_pageToCurrentItem.insert(newPage, item);
    m_currentItemToPage.insert(item, newPage);
    ui->treeRibbon->setCurrentItem(item);
    return newPage;
}

void QdfRibbonBarCustomizePagePrivate::addGroupCopy(QdfRibbonPage *srcPage, QdfRibbonPage *copyPage)
{
    if (!m_currentItemPage)
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QList<QdfRibbonGroup *> srcGroups = srcPage->groups();
    for (int i = 0, count = srcGroups.count(); count > i; ++i)
    {
        QdfRibbonGroup *srcGroup = srcGroups.at(i);
        if (srcGroup->property(__qdf_Widget_Copy).toString() != QLatin1String("__qtc_Group_Copy"))
        {
            QString titleGroup(srcGroup->title());
            QdfRibbonGroup *newGroup = manager->createGroup(copyPage, titleGroup, i);
            newGroup->setIcon(srcGroup->icon());
            newGroup->setProperty(__qdf_Widget_Copy, QLatin1String("__qtc_Group_Copy"));

            titleGroup.remove(QChar::fromLatin1('&'));
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(titleGroup));
            m_currentItemPage->insertChild(i, item);

            m_currentItemToGroup.insert(item, newGroup);

            QListIterator<QAction *> itAction(srcGroup->actions());
            while (itAction.hasNext())
            {
                QAction *action = itAction.next();
                newGroup->addAction(action);
            }
            fillStateActions(newGroup, item, true);
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::addActionGroupCustom(QdfRibbonGroup *group, QAction *action, int index)
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QString actionName = action->text();
    actionName.remove(QChar::fromLatin1('&'));

    if (!actionName.isEmpty())
    {
        QTreeWidgetItem *item = nullptr;
        if (index == -1)
        {
            item = new QTreeWidgetItem(m_currentItemGroup, QStringList(actionName));
        }
        else
        {
            item = new QTreeWidgetItem(QStringList(actionName));
            m_currentItemGroup->insertChild(index, item);
        }
        item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        QIcon icon = action->property("icon").value<QIcon>();
        QPixmap px = icon.pixmap(QSize(16, 16), QIcon::Normal);
        item->setIcon(0, QIcon(px));

        m_currentItemToAction.insert(item, action);
        m_actionToCurrentItem.insert(action, item);
        manager->insertAction(group, action, index);
    }
}

void QdfRibbonBarCustomizePagePrivate::setButtons()
{
    bool addEnabled = false;
    bool removeEnabled = false;
    bool renameEnabled = false;

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    if (m_currentSourceAction)
    {
        if (QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup))
        {
            if (QAction *action = m_currentSourceItemToAction.value(m_currentSourceAction))
            {
                addEnabled = !manager->containsAction(group, action);
            }
        }
    }
    else if (!m_currentSourceItemGroup)
    {
        addEnabled = true;
    }
    else if (m_currentSourceItemGroup)
    {
        QdfRibbonPage *currentSourcePage = m_currentSourceItemToPage.value(m_currentSourceItemPage);
        QdfRibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        if (currentSourcePage && currentPage)
        {
            if (currentSourcePage != currentPage)
            {
                QdfRibbonGroup *group = m_currentSourceItemToGroup.value(m_currentSourceItemGroup);
                QList<QAction *> listActions = manager->actionsGroup(group);// group->actions();

                QList<QdfRibbonGroup *> listGroup = manager->pageGroups(currentPage);
                QListIterator<QdfRibbonGroup *> itGroup(listGroup);
                bool find = false;
                while (itGroup.hasNext())
                {
                    QdfRibbonGroup *currentGroup = itGroup.next();
                    QList<QAction *> currentListActions = currentGroup->actions();

                    if (listActions == currentListActions)
                    {
                        find = true;
                        break;
                    }
                }
                if (!find)
                {
                    addEnabled = true;
                }
            }
        }
    }

    if (m_currentItemAction && m_currentItemGroup)
    {
        QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
        if (group && group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
        {
            removeEnabled = true;
        }
    }
    else if (m_currentItemGroup)
    {
        renameEnabled = true;
        removeEnabled = true;
    }
    else if (m_currentItemPage)
    {
        renameEnabled = true;
        QdfRibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        if (currentPage && currentPage->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
        {
            removeEnabled = true;
        }
    }

    ui->addButton->setEnabled(addEnabled);
    ui->removeButton->setEnabled(removeEnabled);
    ui->renameButton->setEnabled(renameEnabled);

    bool upEnabled = false;
    bool downEnabled = false;

    if (m_currentItemPage && m_currentItemGroup)
    {
        QdfRibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
        Q_ASSERT(currentPage != nullptr);
        QdfRibbonGroup *currentGroup = m_currentItemToGroup.value(m_currentItemGroup);
        Q_ASSERT(currentGroup != nullptr);

        QList<QdfRibbonGroup *> groups = manager->pageGroups(currentPage);

        int index = groups.indexOf(currentGroup);
        upEnabled = index != 0;
        downEnabled = index < (groups.size() - 1);

        if (m_currentItemAction)
        {
            if (QAction *action = m_currentItemToAction.value(m_currentItemAction))
            {
                QList<QAction *> actions = manager->actionsGroup(currentGroup);
                index = actions.indexOf(action);
                upEnabled = index != 0;
                downEnabled = index < (actions.size() - 1);
            }
        }
    }
    else if (m_currentItemPage && !m_currentItemGroup)
    {
        int index = ui->treeRibbon->indexOfTopLevelItem(ui->treeRibbon->currentItem());
        upEnabled = index != 0;
        downEnabled = index < ui->treeRibbon->topLevelItemCount() - 1;
    }

    ui->upButton->setEnabled(upEnabled);
    ui->downButton->setEnabled(downEnabled);
}

void QdfRibbonBarCustomizePagePrivate::applyClicked()
{
    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);
    manager->commit();
}

void QdfRibbonBarCustomizePagePrivate::cancelClicked() { m_ribbonBar->customizeManager()->cancel(); }

void QdfRibbonBarCustomizePagePrivate::addClicked()
{
    QDF_Q(QdfRibbonBarCustomizePage)

    if (m_currentSourceAction && m_currentItemGroup)
    {
        QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
        if (group && group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
        {
            if (QAction *action = m_currentSourceItemToAction.value(m_currentSourceAction))
            {
                int index = m_currentItemGroup->indexOfChild(m_currentItemAction);
                addActionGroupCustom(group, action, index + 1);

                m_currentItemAction = m_currentItemGroup->child(index + 1);

                if (m_currentItemAction)
                {
                    ui->treeRibbon->setCurrentItem(m_currentItemAction);
                }
                else
                {
                    QTreeWidgetItem *currentItem = m_currentItemGroup->child(m_currentItemGroup->childCount() - 1);
                    ui->treeRibbon->setCurrentItem(currentItem);
                }

                if (m_currentSourceItemGroup && m_currentSourceAction)
                {
                    int indexNext = m_currentSourceItemGroup->indexOfChild(m_currentSourceAction) + 1;
                    QTreeWidgetItem *nextItem = m_currentSourceItemGroup->child(indexNext);
                    nextItem = nextItem ? nextItem : m_currentSourceAction;
                    ui->treeCommands->setCurrentItem(nextItem);
                }
                else if (m_currentSourceAction)
                {
                    int index = ui->treeCommands->indexOfTopLevelItem(m_currentSourceAction);
                    QTreeWidgetItem *item = ui->treeCommands->topLevelItem(index + 1);
                    item = item ? item : m_currentSourceAction;
                    ui->treeCommands->setCurrentItem(item);
                }
                ui->treeRibbon->expandItem(m_currentItemGroup);
            }
        }
        else
        {
            QMessageBox::warning(q, q->windowTitle(), QdfRibbonBar::tr_compatible(RibbonAddCommandWarningString));
        }
    }
    else if (!m_currentSourceItemGroup)
    {
        if (QdfRibbonPage *page = m_currentSourceItemToPage.value(m_currentSourceItemPage))
        {
            int index = ui->treeCommands->indexOfTopLevelItem(m_currentSourceItemPage);
            QTreeWidgetItem *item = ui->treeCommands->topLevelItem(index + 1);
            item = item ? item : m_currentSourceItemPage;
            ui->treeCommands->setCurrentItem(item);

            QdfRibbonPage *newPage = addPageCustom(page);
            if (newPage)
            {
                addGroupCopy(page, newPage);
            }
        }
    }
    else if (m_currentSourceItemGroup)
    {
        QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != nullptr);

        QdfRibbonGroup *group = m_currentSourceItemToGroup.value(m_currentSourceItemGroup);
        QdfRibbonPage *page = m_currentItemToPage.value(m_currentItemPage);

        if (group != nullptr && page != nullptr)
        {
            QTreeWidgetItem *itemParent = m_currentItemPage;
            int index = itemParent->childCount();
            QString titleGroup(group->title());
            QdfRibbonGroup *newGroup = manager->createGroup(page, titleGroup, index);
            newGroup->setIcon(group->icon());
            newGroup->setProperty(__qdf_Widget_Copy, QLatin1String("__qtc_Group_Copy"));

            titleGroup.remove(QChar::fromLatin1('&'));
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(titleGroup));
            m_currentItemPage->insertChild(index, item);

            m_currentItemToGroup.insert(item, newGroup);

            QListIterator<QAction *> itAction(manager->actionsGroup(group));
            while (itAction.hasNext())
            {
                QAction *action = itAction.next();
                newGroup->addAction(action);
            }
            fillStateActions(newGroup, item, true);

            int indexNext = m_currentSourceItemPage->indexOfChild(m_currentSourceItemGroup) + 1;
            QTreeWidgetItem *nextItem = m_currentSourceItemPage->child(indexNext);
            nextItem = nextItem ? nextItem : m_currentSourceItemGroup;
            ui->treeCommands->setCurrentItem(nextItem);
            ui->treeRibbon->setCurrentItem(item);
        }
    }
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::newTabClicked()
{
    if (!m_currentItemPage)
    {
        return;
    }

    addPageCustom(nullptr);
    newGroupClicked();
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::newGroupClicked()
{
    if (!m_currentItemPage)
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QString strNewGroup(QdfRibbonBar::tr_compatible(RibbonNewGroupString));
    QdfRibbonPage *currentPage = m_currentItemToPage.value(m_currentItemPage);
    Q_ASSERT(currentPage != nullptr);

    int index = 0;
    if (m_currentItemGroup)
    {
        QdfRibbonGroup *currentGroup = m_currentItemToGroup.value(m_currentItemGroup);
        Q_ASSERT(currentGroup != nullptr);

        QList<QdfRibbonGroup *> groups = manager->pageGroups(currentPage);
        index = groups.indexOf(currentGroup) + 1;
    }

    QdfRibbonGroup *newGroup = manager->createGroup(currentPage, strNewGroup, index);

    QStringList stringsGroup;
    strNewGroup += m_sufNameCustom;
    stringsGroup.append(strNewGroup);
    QTreeWidgetItem *item = new QTreeWidgetItem(stringsGroup);
    m_currentItemPage->insertChild(index, item);
    newGroup->setProperty(__qdf_Widget_Custom, QLatin1String("__qtc_Group_Custom"));
    m_currentItemToGroup.insert(item, newGroup);
    ui->treeRibbon->setCurrentItem(item);
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::removeClicked()
{
    QTreeWidgetItem *currentItem = ui->treeRibbon->currentItem();
    if (!currentItem)
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    if (m_currentItemGroup && m_currentItemAction)
    {
        if (QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup))
        {
            if (group && group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
            {
                QTreeWidgetItem *currentParent = currentItem->parent();
                int index = currentParent->indexOfChild(currentItem);
                if (index == -1)
                {
                    return;
                }

                if (QAction *action = m_currentItemToAction.value(m_currentItemAction))
                {
                    m_actionToCurrentItem.remove(action);
                    m_currentItemToAction.remove(currentItem);
                    manager->removeActionAt(group, index);
                }
                delete currentItem;
            }
        }
    }
    else if (QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup))
    {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index == -1)
        {
            return;
        }
        QdfRibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
        Q_ASSERT(page != nullptr);

        manager->clearActions(group);
        manager->deleteGroup(page, index);
        m_currentItemToGroup.remove(currentItem);
        delete currentItem;
    }
    else if (QdfRibbonPage *page = m_currentItemToPage.value(m_currentItemPage))
    {
        int index = ui->treeRibbon->indexOfTopLevelItem(currentItem);
        //        manager->removePage(page);
        manager->deletePage(page);
        m_pageToCurrentItem.remove(page);
        QListIterator<QdfRibbonGroup *> itGroup(manager->pageGroups(page));
        while (itGroup.hasNext())
        {
            manager->clearActions(itGroup.next());
        }

        m_currentItemToPage.remove(currentItem);
        delete currentItem;

        if (index == ui->treeRibbon->topLevelItemCount())
        {
            index--;
        }

        if (index >= 0)
        {
            QTreeWidgetItem *item = ui->treeRibbon->topLevelItem(index);
            ui->treeRibbon->setCurrentItem(item);
        }
    }
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::resetClicked()
{
    if (m_ribbonBar)
    {
        QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
        Q_ASSERT(manager != NULL);
        manager->reset();
        fillStateRibbon(ui->switchTabsBox->currentText());
    }
}

template<class T>
QTreeWidgetItem *currentItem(const QMap<QTreeWidgetItem *, T *> &currentItemToObject, QTreeWidgetItem *current)
{
    if (current == nullptr)
    {
        return nullptr;
    }

    if (currentItemToObject.contains(current))
    {
        return current;
    }

    return currentItem(currentItemToObject, current->parent());
}

void QdfRibbonBarCustomizePagePrivate::upClicked()
{
    QTreeWidgetItem *currentItem = ui->treeRibbon->currentItem();
    if (!currentItem)
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (m_currentItemGroup && m_currentItemAction)
    {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (!(index == -1 || index == 0))
        {
            currentParent->takeChild(index);
            int newIndex = index - 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != nullptr);
            QAction *action = m_currentItemToAction.value(currentItem);
            Q_ASSERT(action != nullptr);

            QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
            Q_ASSERT(group != nullptr);
            manager->removeActionAt(group, index);
            manager->insertAction(group, action, newIndex);

            ui->treeRibbon->setCurrentItem(currentItem);
        }
    }
    else if (m_currentItemToPage.contains(currentItem))
    {
        int index = ui->treeRibbon->indexOfTopLevelItem(currentItem);
        if (index != -1)
        {
            ui->treeRibbon->takeTopLevelItem(index);
            int newIndex = index - 1;
            ui->treeRibbon->insertTopLevelItem(newIndex, currentItem);
            manager->movePage(index, newIndex);
            ui->treeRibbon->setCurrentItem(currentItem);
        }
    }
    else if (m_currentItemToGroup.contains(currentItem))
    {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (!(index == -1 || index == 0))
        {
            currentParent->takeChild(index);
            int newIndex = index - 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != nullptr);
            QdfRibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
            Q_ASSERT(page != nullptr);
            manager->moveGroup(page, index, newIndex);
            ui->treeRibbon->setCurrentItem(currentItem);
        }
    }

    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::downClicked()
{
    QTreeWidgetItem *currentItem = ui->treeRibbon->currentItem();
    if (!currentItem)
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    if (m_currentItemGroup && m_currentItemAction)
    {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index != currentParent->childCount() - 1)
        {
            currentParent->takeChild(index);
            int newIndex = index + 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != nullptr);
            QAction *action = m_currentItemToAction.value(currentItem);
            Q_ASSERT(action != nullptr);

            QdfRibbonGroup *group = m_currentItemToGroup.value(m_currentItemGroup);
            Q_ASSERT(group != nullptr);
            manager->removeActionAt(group, index);
            manager->insertAction(group, action, newIndex);
        }
    }
    else if (m_currentItemToPage.contains(currentItem))
    {
        int index = ui->treeRibbon->indexOfTopLevelItem(currentItem);
        if (index != ui->treeRibbon->topLevelItemCount() - 1)
        {
            ui->treeRibbon->takeTopLevelItem(index);
            int newIndex = index + 1;
            ui->treeRibbon->insertTopLevelItem(newIndex, currentItem);
            manager->movePage(index, newIndex);
        }
    }
    else if (m_currentItemToGroup.contains(currentItem))
    {
        QTreeWidgetItem *currentParent = currentItem->parent();
        int index = currentParent->indexOfChild(currentItem);
        if (index != currentParent->childCount() - 1)
        {
            currentParent->takeChild(index);
            int newIndex = index + 1;
            currentParent->insertChild(newIndex, currentItem);
            Q_ASSERT(m_currentItemPage != nullptr);
            QdfRibbonPage *page = m_currentItemToPage.value(m_currentItemPage);
            Q_ASSERT(page != nullptr);
            manager->moveGroup(page, index, newIndex);
        }
    }
    ui->treeRibbon->setCurrentItem(currentItem);
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::renameClicked()
{
    QDF_Q(QdfRibbonBarCustomizePage);
    class RenameDialog : public QDialog
    {
    public:
        RenameDialog(QWidget *parent, const QString &currentName) : QDialog(parent)
        {
            setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
            ui.setupUi(this);
            ui.lineEditName->setText(currentName);
        }
        QString currentName() const { return ui.lineEditName->text(); }

    protected:
        Ui::QdfRibbonRenameDialog ui;
    };

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QTreeWidgetItem *currentItem = ui->treeRibbon->currentItem();
    if (!currentItem)
    {
        return;
    }

    QString currentName;
    QdfRibbonPage *page = m_currentItemToPage.value(currentItem);
    QdfRibbonGroup *group = m_currentItemToGroup.value(currentItem);
    if (page)
    {
        if (!manager->pageName(page).isEmpty())
        {
            currentName = manager->pageName(page);
        }
        else
        {
            currentName = page->title();
        }
    }
    else if (group)
    {
        if (!manager->groupName(group).isEmpty())
        {
            currentName = manager->groupName(group);
        }
        else
        {
            currentName = group->title();
        }
    }

    if (!currentName.isEmpty())
    {
        RenameDialog renameDialog(q, currentName);
        if (renameDialog.exec() == QDialog::Accepted)
        {
            QString currentText = renameDialog.currentName();
            if (page != nullptr)
            {
                manager->setPageName(page, currentText);
                currentText.remove(QChar::fromLatin1('&'));
                if (page->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                {
                    currentText += m_sufNameCustom;
                }
                currentItem->setText(0, currentText);
            }
            else if (group != nullptr)
            {
                manager->setGroupName(group, currentText);
                currentText.remove(QChar::fromLatin1('&'));
                if (group->property(__qdf_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom"))
                {
                    currentText += m_sufNameCustom;
                }
                currentItem->setText(0, currentText);
            }
        }
    }
}

void QdfRibbonBarCustomizePagePrivate::setCurrentCategoryText(const QString &strCategory)
{
    if (strCategory.isEmpty())
    {
        return;
    }

    if (m_strSeparator == strCategory)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        ui->comboBoxSourceCategory->setCurrentText(m_currentSourceCategory);
#else
        int index = ui->comboBoxSourceCategory->findText(m_currentSourceCategory);
        if (index != -1)
        {
            ui->comboBoxSourceCategory->setCurrentIndex(index);
        }
#endif
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != NULL);

    m_currentSourceItemToPage.clear();
    m_currentSourceItemToGroup.clear();
    m_currentSourceItemToAction.clear();

    ui->treeCommands->clear();
    clearCategoryCaptionTree(ui->treeCommands);

    QList<QAction *> actions = manager->actionsByCategory(strCategory);
    if (actions.size() > 0)
    {
        ui->treeCommands->setRootIsDecorated(false);
        fillSourceActions(actions);
    }
    else
    {
        QList<QdfRibbonPage *> pages = manager->pagesByCategory(strCategory);
        if (pages.size() > 0)
        {
            ui->treeCommands->setRootIsDecorated(true);
            fillSourcePages(pages, strCategory);
        }
    }
    m_currentSourceCategory = strCategory;
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::setCurrentTabsText(const QString &strCategory)
{
    // if (strCategory.isEmpty())
    //     return;
    fillStateRibbon(strCategory);
}

void QdfRibbonBarCustomizePagePrivate::currentSourceChanged(QTreeWidgetItem *current)
{
    m_currentSourceItemPage = ::currentItem(m_currentSourceItemToPage, current);
    m_currentSourceItemGroup = ::currentItem(m_currentSourceItemToGroup, current);
    m_currentSourceAction = ::currentItem(m_currentSourceItemToAction, current);
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::currentPageChanged(QTreeWidgetItem *current)
{
    m_currentItemPage = ::currentItem(m_currentItemToPage, current);
    m_currentItemGroup = ::currentItem(m_currentItemToGroup, current);
    m_currentItemAction = (m_currentItemGroup && m_currentItemGroup != current) ? current : nullptr;
    setButtons();
}

void QdfRibbonBarCustomizePagePrivate::itemPageChanged(QTreeWidgetItem *itemPage, int column)
{
    if (m_currentItemToPage.empty())
    {
        return;
    }

    QdfRibbonCustomizeManager *manager = m_ribbonBar->customizeManager();
    Q_ASSERT(manager != nullptr);

    QdfRibbonPage *page = m_currentItemToPage.value(itemPage);
    if (page == nullptr)
    {
        return;
    }

    if (itemPage->checkState(column) == Qt::Checked)
    {
        if (manager->isPageHidden(page))
        {
            m_ribbonBar->customizeManager()->setPageHidden(page, false);
        }
    }
    else if (itemPage->checkState(column) == Qt::Unchecked)
    {
        if (!manager->isPageHidden(page))
        {
            m_ribbonBar->customizeManager()->setPageHidden(page, true);
        }
    }
}

/* QdfRibbonBarCustomizePage */
QdfRibbonBarCustomizePage::QdfRibbonBarCustomizePage(QdfRibbonBar *ribbonBar) : QWidget(nullptr)
{
    QDF_INIT_PRIVATE(QdfRibbonBarCustomizePage);
    QDF_D(QdfRibbonBarCustomizePage)
    d->m_ribbonBar = ribbonBar;
    d->init();
}

QdfRibbonBarCustomizePage::~QdfRibbonBarCustomizePage() { QDF_FINI_PRIVATE(); }

QdfRibbonBar *QdfRibbonBarCustomizePage::ribbonBar() const
{
    QDF_D(const QdfRibbonBarCustomizePage)
    return d->m_ribbonBar;
}

void QdfRibbonBarCustomizePage::addCustomCategory(const QString &strCategory)
{
    QDF_D(QdfRibbonBarCustomizePage)
    d->m_sourceCategories.append(strCategory);
}

void QdfRibbonBarCustomizePage::addSeparatorCategory(const QString &strCategory)
{
    QDF_D(QdfRibbonBarCustomizePage)

    QString separator = strCategory;
    if (separator.isEmpty())
    {
        separator = QString("----------");
    }

    d->m_strSeparator = separator;
    d->m_sourceCategories.append(separator);
}

void QdfRibbonBarCustomizePage::accepted()
{
    QDF_D(QdfRibbonBarCustomizePage)
    if (d->m_wasDisplayed)
    {
        d->applyClicked();
    }
}

void QdfRibbonBarCustomizePage::rejected()
{
    QDF_D(QdfRibbonBarCustomizePage)
    d->cancelClicked();
}

void QdfRibbonBarCustomizePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QDF_D(QdfRibbonBarCustomizePage)
    d->m_wasDisplayed = true;
    d->setupPage();
}

void QdfRibbonBarCustomizePage::hideEvent(QHideEvent *event) { QWidget::hideEvent(event); }
