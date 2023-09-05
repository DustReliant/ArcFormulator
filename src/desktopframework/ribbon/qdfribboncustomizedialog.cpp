#include "private/qdfribbonbar_p.h"
#include "private/qdfribboncustomizedialog_p.h"
#include "qdfribbon_def.h"
#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>
#include <ribbon/qdfribboncustomizedialog.h>
#include <ribbon/qdfribboncustomizemanager.h>
#include <ribbon/qdfribboncustomizepage.h>

QDF_USE_NAMESPACE

static QWidget *qdf_WantTheFocus(QWidget *ancestor)
{
    const int MaxIterations = 100;

    QWidget *candidate = ancestor;
    for (int i = 0; i < MaxIterations; ++i)
    {
        candidate = candidate->nextInFocusChain();
        if (!candidate)
        {
            break;
        }

        if (candidate->focusPolicy() & Qt::TabFocus)
        {
            if (candidate != ancestor && ancestor->isAncestorOf(candidate))
            {
                return candidate;
            }
        }
    }
    return 0;
}


QdfRibbonCustomizePageWidget::QdfRibbonCustomizePageWidget(QWidget *parent) : QWidget(parent)
{
    m_content = nullptr;
    QFont font = m_labelTitle.font();
    font.setBold(true);
    m_labelTitle.setFont(font);
    m_hBoxLayout = new QHBoxLayout();
    m_hBoxLayout->addWidget(&m_labelIcon);
    m_hBoxLayout->addWidget(&m_labelTitle, 1);
    m_vBoxLayout = new QVBoxLayout(this);
    m_vBoxLayout->addLayout(m_hBoxLayout);
}

QdfRibbonCustomizePageWidget::~QdfRibbonCustomizePageWidget()
{
}

QWidget *QdfRibbonCustomizePageWidget::content() const
{
    return m_content;
}

void QdfRibbonCustomizePageWidget::setContent(QWidget *content)
{
    if (content == nullptr)
    {
        Q_ASSERT(false);
        return;
    }
    QIcon icon = content->windowIcon();
    m_labelIcon.setPixmap(icon.isNull() ? QPixmap(":/res/logo.png") : icon.pixmap(QSize(32, 32)));
    m_labelTitle.setText(content->statusTip());
    m_vBoxLayout->addWidget(content);
    m_content = content;
}

QdfRibbonCustomizeListWidget::QdfRibbonCustomizeListWidget(QWidget *parent) : QListWidget(parent)
{
}

QdfRibbonCustomizeListWidget::~QdfRibbonCustomizeListWidget()
{
}

QSize QdfRibbonCustomizeListWidget::sizeHint() const
{
    QSize sz = QListWidget::sizeHint();
    QStyleOption opt;
    opt.init(this);

    int maxWidth = 0;
    for (int row = 0, countRow = count(); countRow > row; row++)
    {
        if (QListWidgetItem *rbItem = item(row))
        {
            QString str = rbItem->text() + QString("XX");
            int width = opt.fontMetrics.boundingRect(str).width();
            maxWidth = qMax(maxWidth, width);
        }
    }
    return QSize(maxWidth, sz.height());
}

QSize QdfRibbonCustomizeListWidget::minimumSizeHint() const
{
    QSize sz = QListWidget::minimumSizeHint();
    sz.setWidth(sizeHint().width());
    return sz;
}

QdfRibbonCustomizeDialogPrivate::QdfRibbonCustomizeDialogPrivate()
{
    m_currentPage = 0;
    m_pageVBoxLayout = nullptr;
}

QdfRibbonCustomizeDialogPrivate::~QdfRibbonCustomizeDialogPrivate()
{
}

void QdfRibbonCustomizeDialogPrivate::init()
{
    QDF_Q(QdfRibbonCustomizeDialog);
    q->setWindowTitle(QdfRibbonBar::tr_compatible(RibbonCustomizeDialogOptionsString));

    q->resize(734, 465);
    m_verticalLayout = new QVBoxLayout(q);
    m_horizontalLayout = new QHBoxLayout();
    m_horizontalLayout->setSpacing(1);
    m_listPage = new QdfRibbonCustomizeListWidget(q);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_listPage->sizePolicy().hasHeightForWidth());
    m_listPage->setSizePolicy(sizePolicy);
    m_listPage->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_horizontalLayout->addWidget(m_listPage, 0, Qt::AlignLeft);

    m_pageFrame = new QFrame(q);
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_pageFrame->sizePolicy().hasHeightForWidth());
    m_pageFrame->setSizePolicy(sizePolicy1);
    m_pageFrame->setAutoFillBackground(false);
    m_pageFrame->setFrameShape(QFrame::StyledPanel);
    m_pageFrame->setFrameShadow(QFrame::Sunken);

    m_horizontalLayout->addWidget(m_pageFrame);

    m_horizontalLayout->setStretch(1, 1);

    m_verticalLayout->addLayout(m_horizontalLayout);

    m_buttonBox = new QDialogButtonBox(q);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    m_verticalLayout->addWidget(m_buttonBox);

    QObject::connect(m_buttonBox, SIGNAL(accepted()), q, SLOT(accept()), Qt::DirectConnection);
    QObject::connect(m_buttonBox, SIGNAL(rejected()), q, SLOT(reject()), Qt::DirectConnection);

    m_verticalLayout->setContentsMargins(2, 2, 2, 6);

    m_pageVBoxLayout = new QVBoxLayout(m_pageFrame);
    m_pageVBoxLayout->setSpacing(0);
    m_pageVBoxLayout->addSpacing(0);
    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
    m_pageVBoxLayout->addItem(spacerItem);
    connect(m_listPage, SIGNAL(currentRowChanged(int)), this, SLOT(switchToPage(int)));
}

void QdfRibbonCustomizeDialogPrivate::setCustomizeMode(bool edit)
{
    for (int index = 0, count = m_listWidget.count(); count > index; ++index)
    {
        if (QdfRibbonBarCustomizePage *page =
                    qobject_cast<QdfRibbonBarCustomizePage *>(m_listWidget.at(index)))
        {
            page->ribbonBar()->customizeManager()->setEditMode(edit);
            break;
        }
        else if (QdfRibbonQuickAccessBarCustomizePage *page =
                         qobject_cast<QdfRibbonQuickAccessBarCustomizePage *>(m_listWidget.at(index)))
        {
            page->ribbonBar()->customizeManager()->setEditMode(edit);
            break;
        }
    }
}

void QdfRibbonCustomizeDialogPrivate::addPage(QdfRibbonCustomizePageWidget *page)
{
    if (!page)
    {
        qWarning("QdfRibbonCustomizeDialogPrivate::addPage: Cannot insert null page");
        return;
    }
    page->setParent(m_pageFrame);

    int n = m_pageVBoxLayout->count();
    bool pageVBoxLayoutEnabled = m_pageVBoxLayout->isEnabled();
    m_pageVBoxLayout->setEnabled(false);

    m_pageVBoxLayout->insertWidget(n - 1, page);

    page->hide();
    m_pageVBoxLayout->setEnabled(pageVBoxLayoutEnabled);
    QdfRibbonCustomizePageItem *item = new QdfRibbonCustomizePageItem(page->windowTitle(), page);
    m_listPage->addItem(item);
}

void QdfRibbonCustomizeDialogPrivate::insertPage(int index, QdfRibbonCustomizePageWidget *page)
{
    if (!page)
    {
        qWarning("QdfRibbonCustomizeDialogPrivate::insertPage: Cannot insert null page");
        return;
    }

    QdfRibbonCustomizePageItem *posItem =
            dynamic_cast<QdfRibbonCustomizePageItem *>(m_listPage->item(index));
    int indexItem = m_listPage->row(posItem);
    if (indexItem == -1)
    {
        return;
    }

    page->setParent(m_pageFrame);

    int n = m_pageVBoxLayout->count();
    bool pageVBoxLayoutEnabled = m_pageVBoxLayout->isEnabled();
    m_pageVBoxLayout->setEnabled(false);

    m_pageVBoxLayout->insertWidget(n - 1, page);

    page->hide();
    m_pageVBoxLayout->setEnabled(pageVBoxLayoutEnabled);

    QdfRibbonCustomizePageItem *item = new QdfRibbonCustomizePageItem(page->windowTitle(), page);
    m_listPage->insertItem(indexItem, item);
}

void QdfRibbonCustomizeDialogPrivate::switchToPage(int currentRow)
{
    if (m_currentPage != currentRow)
    {
        if (QdfRibbonCustomizePageItem *item =
                    dynamic_cast<QdfRibbonCustomizePageItem *>(m_listPage->item(m_currentPage)))
        {
            QdfRibbonCustomizePageWidget *page = item->m_page;
            page->setUpdatesEnabled(false);
            page->hide();
            page->parentWidget()->repaint();
        }
    }

    m_currentPage = currentRow;

    if (QdfRibbonCustomizePageItem *item =
                dynamic_cast<QdfRibbonCustomizePageItem *>(m_listPage->item(m_currentPage)))
    {
        QdfRibbonCustomizePageWidget *page = item->m_page;
        page->setUpdatesEnabled(true);
        page->show();

        if (QWidget *candidate = qdf_WantTheFocus(page))
        {
            candidate->setFocus();
        }

        bool expandPage = !page->layout();
        if (!expandPage)
        {
            const QLayoutItem *pageItem = m_pageVBoxLayout->itemAt(m_pageVBoxLayout->indexOf(page));
            expandPage = pageItem->expandingDirections() & Qt::Vertical;
        }
        QSpacerItem *bottomSpacer =
                m_pageVBoxLayout->itemAt(m_pageVBoxLayout->count() - 1)->spacerItem();
        Q_ASSERT(bottomSpacer);
        bottomSpacer->changeSize(0, 0, QSizePolicy::Ignored,
                                 expandPage ? QSizePolicy::Ignored : QSizePolicy::MinimumExpanding);
        m_pageVBoxLayout->invalidate();
        page->parentWidget()->repaint();
    }
}


QdfRibbonCustomizeDialog::QdfRibbonCustomizeDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QDF_INIT_PRIVATE(QdfRibbonCustomizeDialog);
    QDF_D(QdfRibbonCustomizeDialog);
    d->init();
}

QdfRibbonCustomizeDialog::~QdfRibbonCustomizeDialog()
{
    QDF_FINI_PRIVATE();
}

void QdfRibbonCustomizeDialog::addPage(QWidget *page)
{
    QDF_D(QdfRibbonCustomizeDialog);
    Q_ASSERT(page != nullptr);
    QdfRibbonCustomizePageWidget *pageWidget = new QdfRibbonCustomizePageWidget(this);
    pageWidget->setContent(page);
    pageWidget->setWindowTitle(page->windowTitle());

    if (const QMetaObject *mo = page->metaObject())
    {
        if (mo->indexOfSlot("accepted()") != -1)
        {
            connect(this, SIGNAL(accepted()), page, SLOT(accepted()), Qt::DirectConnection);
        }
        if (mo->indexOfSlot("rejected()") != -1)
        {
            connect(this, SIGNAL(rejected()), page, SLOT(rejected()), Qt::DirectConnection);
        }
    }

    d->m_listWidget.append(page);
    d->addPage(pageWidget);
}

void QdfRibbonCustomizeDialog::insertPage(int index, QWidget *page)
{
    QDF_D(QdfRibbonCustomizeDialog);
    Q_ASSERT(page != nullptr);
    QdfRibbonCustomizePageWidget *pageWidget = new QdfRibbonCustomizePageWidget(this);
    pageWidget->setContent(page);
    pageWidget->setWindowTitle(page->windowTitle());
    connect(this, SIGNAL(accepted()), page, SLOT(accepted()));

    d->m_listWidget.insert(index, page);
    d->insertPage(index, pageWidget);
}

int QdfRibbonCustomizeDialog::indexOf(QWidget *page) const
{
    QDF_D(const QdfRibbonCustomizeDialog);
    return d->m_listWidget.indexOf(page);
}

QWidget *QdfRibbonCustomizeDialog::currentPage() const
{
    QDF_D(const QdfRibbonCustomizeDialog);
    return d->m_listWidget.at(d->m_currentPage);
}

void QdfRibbonCustomizeDialog::setCurrentPage(QWidget *page)
{
    int index = indexOf(page);
    if (index != -1)
    {
        setCurrentPageIndex(index);
    }
}

int QdfRibbonCustomizeDialog::currentPageIndex() const
{
    QDF_D(const QdfRibbonCustomizeDialog);
    return d->m_currentPage;
}

void QdfRibbonCustomizeDialog::setCurrentPageIndex(int index)
{
    QDF_D(QdfRibbonCustomizeDialog);

    if (index == -1)
    {
        Q_ASSERT(false);
        return;
    }
    d->m_currentPage = index;
    d->m_listPage->setCurrentRow(index);

    if (QdfRibbonCustomizePageItem *posItem =
                dynamic_cast<QdfRibbonCustomizePageItem *>(d->m_listPage->item(index)))
    {
        if (posItem->m_page)
        {
            posItem->m_page->setVisible(true);
        }
    }
}

int QdfRibbonCustomizeDialog::pageCount() const
{
    QDF_D(const QdfRibbonCustomizeDialog);
    return d->m_listWidget.count();
}

QWidget *QdfRibbonCustomizeDialog::pageByIndex(int index) const
{
    QDF_D(const QdfRibbonCustomizeDialog);
    return d->m_listWidget[index];
}

void QdfRibbonCustomizeDialog::accept()
{
    QDF_D(QdfRibbonCustomizeDialog);
    QDialog::accept();
    d->setCustomizeMode(false);
}

void QdfRibbonCustomizeDialog::reject()
{
    QDF_D(QdfRibbonCustomizeDialog);
    QDialog::reject();
    d->setCustomizeMode(false);
}

void QdfRibbonCustomizeDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QDF_D(QdfRibbonCustomizeDialog);
    d->setCustomizeMode(true);
    if (d->m_currentPage != -1)
    {
        setCurrentPageIndex(d->m_currentPage);
    }
}

void QdfRibbonCustomizeDialog::hideEvent(QHideEvent *event)
{
    QDF_D(QdfRibbonCustomizeDialog);
    QDialog::hideEvent(event);
    for (int index = 0, count = d->m_listPage->count(); count > index; ++index)
    {
        if (QdfRibbonCustomizePageItem *posItem =
                    dynamic_cast<QdfRibbonCustomizePageItem *>(d->m_listPage->item(index)))
        {
            if (posItem->m_page)
            {
                posItem->m_page->setVisible(false);
            }
        }
    }
}
