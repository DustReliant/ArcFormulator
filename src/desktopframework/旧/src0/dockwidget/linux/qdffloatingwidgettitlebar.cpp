#include "qdffloatingwidgettitlebar.h"
#include "../qdfdockwidgethelper.h"
#include "ElidingLabel.h"
#include "FloatingDockContainer.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <iostream>

QDF_USE_NAMESPACE

using QdfElidingLabel = QdfElidingLabel;
using tCloseButton = QToolButton;
using tMaximizeButton = QToolButton;

/**
 * @brief Private data class of public interface QdfFloatingWidgetTitleBar
 */
struct QdfFloatingWidgetTitleBarPrivate
{
    QdfFloatingWidgetTitleBar *q;///< public interface class
    QLabel *IconLabel = nullptr;
    QdfElidingLabel *TitleLabel;
    tCloseButton *CloseButton = nullptr;
    tMaximizeButton *MaximizeButton = nullptr;
    QdfFloatingDockContainer *FloatingWidget = nullptr;
    DragState DragState = DragState::DS_Inactive;
    QIcon MaximizeIcon;
    QIcon NormalIcon;
    bool Maximized = false;

    QdfFloatingWidgetTitleBarPrivate(QdfFloatingWidgetTitleBar *_public) : q(_public)
    {
    }

    /**
	 * Creates the complete layout including all controls
	 */
    void createLayout();
};


void QdfFloatingWidgetTitleBarPrivate::createLayout()
{
    TitleLabel = new QdfElidingLabel();
    TitleLabel->setElideMode(Qt::ElideRight);
    TitleLabel->setText("DockWidget->windowTitle()");
    TitleLabel->setObjectName("floatingTitleLabel");
    TitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    CloseButton = new tCloseButton();
    CloseButton->setObjectName("floatingTitleCloseButton");
    CloseButton->setAutoRaise(true);

    MaximizeButton = new tMaximizeButton();
    MaximizeButton->setObjectName("floatingTitleMaximizeButton");
    MaximizeButton->setAutoRaise(true);

    // The standard icons do does not look good on high DPI screens
    QIcon CloseIcon;
    QPixmap normalPixmap = q->style()->standardPixmap(
            QStyle::SP_TitleBarCloseButton, 0, CloseButton);
    CloseIcon.addPixmap(normalPixmap, QIcon::Normal);
    CloseIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25),
                        QIcon::Disabled);
    CloseButton->setIcon(
            q->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    CloseButton->setVisible(true);
    CloseButton->setFocusPolicy(Qt::NoFocus);
    q->connect(CloseButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

    q->setMaximizedIcon(false);
    MaximizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    MaximizeButton->setVisible(true);
    MaximizeButton->setFocusPolicy(Qt::NoFocus);
    q->connect(MaximizeButton, &QPushButton::clicked, q, &QdfFloatingWidgetTitleBar::maximizeRequested);

    QFontMetrics fm(TitleLabel->font());
    int Spacing = qRound(fm.height() / 4.0);

    // Fill the layout
    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::LeftToRight);
    Layout->setContentsMargins(6, 0, 0, 0);
    Layout->setSpacing(0);
    q->setLayout(Layout);
    Layout->addWidget(TitleLabel, 1);
    Layout->addSpacing(Spacing);
    Layout->addWidget(MaximizeButton);
    Layout->addWidget(CloseButton);
    Layout->setAlignment(Qt::AlignCenter);

    TitleLabel->setVisible(true);
}


QdfFloatingWidgetTitleBar::QdfFloatingWidgetTitleBar(QdfFloatingDockContainer *parent) : QFrame(parent),
                                                                                         d(new QdfFloatingWidgetTitleBarPrivate(this))
{
    d->FloatingWidget = parent;
    d->createLayout();

    auto normalPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarNormalButton, 0, d->MaximizeButton);
    d->NormalIcon.addPixmap(normalPixmap, QIcon::Normal);
    d->NormalIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);

    auto maxPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMaxButton, 0, d->MaximizeButton);
    d->MaximizeIcon.addPixmap(maxPixmap, QIcon::Normal);
    d->MaximizeIcon.addPixmap(internal::createTransparentPixmap(maxPixmap, 0.25), QIcon::Disabled);
    setMaximizedIcon(d->Maximized);
}


QdfFloatingWidgetTitleBar::~QdfFloatingWidgetTitleBar()
{
    delete d;
}


void QdfFloatingWidgetTitleBar::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        d->DragState = DragState::DS_FloatingWidget;
        d->FloatingWidget->startDragging(ev->pos(), d->FloatingWidget->size(),
                                         this);
        return;
    }
    Super::mousePressEvent(ev);
}


void QdfFloatingWidgetTitleBar::mouseReleaseEvent(QMouseEvent *ev)
{
    d->DragState = DragState::DS_Inactive;
    if (d->FloatingWidget)
    {
        d->FloatingWidget->finishDragging();
    }
    Super::mouseReleaseEvent(ev);
}


void QdfFloatingWidgetTitleBar::mouseMoveEvent(QMouseEvent *ev)
{
    if (!(ev->buttons() & Qt::LeftButton) || DragState::DS_Inactive == d->DragState)
    {
        d->DragState = DragState::DS_Inactive;
        Super::mouseMoveEvent(ev);
        return;
    }

    // move floating window
    if (DragState::DS_FloatingWidget == d->DragState)
    {
        if (d->FloatingWidget->isMaximized())
        {
            d->FloatingWidget->showNormal(true);
        }
        d->FloatingWidget->moveFloating();
        Super::mouseMoveEvent(ev);
        return;
    }
    Super::mouseMoveEvent(ev);
}


void QdfFloatingWidgetTitleBar::enableCloseButton(bool Enable)
{
    d->CloseButton->setEnabled(Enable);
}


void QdfFloatingWidgetTitleBar::setTitle(const QString &Text)
{
    d->TitleLabel->setText(Text);
}


void QdfFloatingWidgetTitleBar::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}


void QdfFloatingWidgetTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        emit maximizeRequested();
        event->accept();
    }
    else
    {
        QWidget::mouseDoubleClickEvent(event);
    }
}


void QdfFloatingWidgetTitleBar::setMaximizedIcon(bool maximized)
{
    d->Maximized = maximized;
    if (maximized)
    {
        d->MaximizeButton->setIcon(d->NormalIcon);
    }
    else
    {
        d->MaximizeButton->setIcon(d->MaximizeIcon);
    }
}


void QdfFloatingWidgetTitleBar::setMaximizeIcon(const QIcon &Icon)
{
    d->MaximizeIcon = Icon;
    if (d->Maximized)
    {
        setMaximizedIcon(d->Maximized);
    }
}


void QdfFloatingWidgetTitleBar::setNormalIcon(const QIcon &Icon)
{
    d->NormalIcon = Icon;
    if (!d->Maximized)
    {
        setMaximizedIcon(d->Maximized);
    }
}


QIcon QdfFloatingWidgetTitleBar::maximizeIcon() const
{
    return d->MaximizeIcon;
}


QIcon QdfFloatingWidgetTitleBar::normalIcon() const
{
    return d->NormalIcon;
}


}// namespace ads
