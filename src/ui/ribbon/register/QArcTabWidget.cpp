#include "QArcTabWidget.h"
#include <QCheckBox>
#include <QVBoxLayout>

QArcTabWidget::QArcTabWidget(QWidget *parent) 
    : QWidget(parent)
    , tabBar_(nullptr)
    , stackedWidget_(nullptr)
    , tabPosition_(North)
    , tabsClosable_(false)
    , movable_(false)
    , currentTabIndex_(-1)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tabBar_ = new QTabBar(this);
    layout->addWidget(tabBar_);

    stackedWidget_ = new QStackedWidget(this);
    layout->addWidget(stackedWidget_);

    connect(tabBar_, &QTabBar::currentChanged, this, &QArcTabWidget::currentChanged);
    connect(tabBar_, &QTabBar::tabCloseRequested, this, &QArcTabWidget::tabCloseRequested);

    setLayout(layout);
    // 设置默认选项卡位置为左侧
    setTabPosition(West);
}

QArcTabWidget::~QArcTabWidget()
{
}

int QArcTabWidget::count() const
{
    return tabBar_->count();
}


int QArcTabWidget::addTab(QWidget *widget, const QString &label)
{
    return insertTab(count(), widget, label);
}

int QArcTabWidget::insertTab(int index, QWidget *widget, const QString &label)
{
    if (index < 0 || index > count() || !widget)
        return -1;

    QCheckBox *checkBox = new QCheckBox(label, tabBar_);
    tabBar_->insertTab(index, label);
    tabBar_->setTabButton(index, QTabBar::LeftSide, checkBox);

    stackedWidget_->insertWidget(index, widget);

    connect(checkBox, &QCheckBox::clicked, this, &QArcTabWidget::checkBoxClicked);

    if (currentIndex() == -1)
        setCurrentIndex(index);

    return index;
}

void QArcTabWidget::removeTab(int index)
{
    if (index < 0 || index >= count())
        return;

    tabBar_->removeTab(index);
    stackedWidget_->removeWidget(stackedWidget_->widget(index));

    if (currentIndex() == index)
        setCurrentIndex(-1);
}

bool QArcTabWidget::isTabEnabled(int index) const
{
    return index >= 0 && index < count() && tabBar_->isTabEnabled(index);
}

void QArcTabWidget::setTabEnabled(int index, bool enabled)
{
    if (index >= 0 && index < count())
        tabBar_->setTabEnabled(index, enabled);
}

QString QArcTabWidget::tabText(int index) const
{
    return index >= 0 && index < count() ? tabBar_->tabText(index) : QString();
}

void QArcTabWidget::setTabText(int index, const QString &text)
{
    if (index >= 0 && index < count())
        tabBar_->setTabText(index, text);
}

bool QArcTabWidget::isTabCheck(int index) const
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(tabBar_->tabButton(index, QTabBar::LeftSide));
    return checkBox && checkBox->isChecked();
}

void QArcTabWidget::setTabCheck(int index, bool check)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(tabBar_->tabButton(index, QTabBar::LeftSide));
    if (checkBox)
        checkBox->setChecked(check);
}

QString QArcTabWidget::tabToolTip(int index) const
{
    return index >= 0 && index < count() ? tabBar_->tabToolTip(index) : QString();
}

void QArcTabWidget::setTabToolTip(int index, const QString &tip)
{
    if (index >= 0 && index < count())
        tabBar_->setTabToolTip(index, tip);
}

int QArcTabWidget::currentIndex() const
{
    return currentTabIndex_;
}

void QArcTabWidget::setCurrentIndex(int index)
{
    if (index >= 0 && index < count())
    {
        stackedWidget_->setCurrentIndex(index);
        tabBar_->setCurrentIndex(index);
        currentTabIndex_ = index;
        emit currentChanged(index);
    }
}

QArcTabWidget::emArcTabPosition QArcTabWidget::tabPosition() const
{
    return tabPosition_;
}

void QArcTabWidget::setTabPosition(QArcTabWidget::emArcTabPosition position)
{
    if (tabPosition_ != position)
    {
        tabPosition_ = position;
        update();
    }
}

bool QArcTabWidget::tabsClosable() const
{
    return tabsClosable_;
}

void QArcTabWidget::setTabsClosable(bool closeable)
{
    if (tabsClosable_ != closeable)
    {
        tabsClosable_ = closeable;
        update();
    }
}

bool QArcTabWidget::isMovable() const
{
    return movable_;
}

void QArcTabWidget::setMovable(bool movable)
{
    if (movable_ != movable)
    {
        movable_ = movable;
        update();
    }
}

void QArcTabWidget::tabInserted(int index)
{
    Q_UNUSED(index);
}

void QArcTabWidget::tabRemoved(int index)
{
    Q_UNUSED(index);
}

void QArcTabWidget::checkBoxClicked()
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if (checkBox)
    {
        int index = tabBar_->tabAt(checkBox->pos());
        if (index >= 0 && index < count())
        {
            emit checkBoxStateChanged(index, checkBox->isChecked());
        }
    }
}

QSize QArcTabWidget::sizeHint() const
{
    return QSize(200, 150);// Modify the default size hint as needed
}
