#ifndef DESKTOPFRAMEWORK_QDFRIBBONGROUP_H
#define DESKTOPFRAMEWORK_QDFRIBBONGROUP_H

#include "qdfribboncontrols.h"
#include "qdfribbonpage.h"
#include <QMenu>
#include <QToolButton>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfRibbonGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool reduced READ isReduced)
    Q_PROPERTY(const QString &title READ title WRITE setTitle)
    Q_PROPERTY(const QFont &titleFont READ titleFont WRITE setTitleFont)
    Q_PROPERTY(const QColor &titleColor READ titleColor WRITE setTitleColor)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool isOptionButtonVisible READ isOptionButtonVisible WRITE setOptionButtonVisible
                       DESIGNABLE false)
    Q_PROPERTY(Qt::Alignment contentAlignment READ contentAlignment WRITE setContentAlignment)
    Q_PROPERTY(Qt::TextElideMode titleElideMode READ titleElideMode WRITE setTitleElideMode)

public:
    explicit QdfRibbonGroup(QWidget *parent = nullptr);
    explicit QdfRibbonGroup(QdfRibbonPage *page, const QString &title);
    ~QdfRibbonGroup() override;

public:
    QdfRibbonBar *ribbonBar() const;

    bool isReduced() const;

    const QString &title() const;
    void setTitle(const QString &title);

    const QFont &titleFont() const;
    void setTitleFont(const QFont &font);

    const QColor &titleColor() const;
    void setTitleColor(const QColor &color);

    const QIcon &icon() const;
    void setIcon(const QIcon &icon);

    bool isOptionButtonVisible() const;
    void setOptionButtonVisible(bool visible = true);

    QAction *optionButtonAction() const;
    void setOptionButtonAction(QAction *action);

    Qt::Alignment contentAlignment() const;
    void setContentAlignment(Qt::Alignment alignment);

    Qt::Alignment controlsAlignment() const;
    void setControlsAlignment(Qt::Alignment alignment);

    int controlCount() const;
    QdfRibbonControl *controlByIndex(int index) const;
    QdfRibbonControl *controlByAction(QAction *action) const;
    QdfRibbonWidgetControl *controlByWidget(QWidget *widget) const;
    QdfRibbonControlSizeDefinition::GroupSize currentSize() const;

    void addControl(QdfRibbonControl *control);
    void removeControl(QdfRibbonControl *control);

    QAction *addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style,
                       QMenu *menu = nullptr,
                       QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu = nullptr,
                       QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style,
                          QMenu *menu = nullptr,
                          QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);

    QAction *addWidget(QWidget *widget);
    QAction *addWidget(const QIcon &icon, const QString &text, QWidget *widget);
    QAction *addWidget(const QIcon &icon, const QString &text, bool stretch, QWidget *widget);
    QMenu *addMenu(const QIcon &icon, const QString &text,
                   Qt::ToolButtonStyle style = Qt::ToolButtonFollowStyle);
    QAction *addSeparator();

    void remove(QWidget *widget);
    void clear();

    Qt::TextElideMode titleElideMode() const;
    void setTitleElideMode(Qt::TextElideMode mode);

public:
    void setControlsCentering(bool controlsCentering = true);
    bool isControlsCentering() const;
    void setControlsGrouping(bool controlsGrouping = true);
    bool isControlsGrouping() const;
    virtual QSize sizeHint() const;

public:
    using QWidget::addAction;
    using QWidget::insertAction;

Q_SIGNALS:
    void released();
    void actionTriggered(QAction *);
    void hidePopup();

Q_SIGNALS:
    void titleChanged(const QString &title);
    void titleFontChanged(const QFont &font);

protected Q_SLOTS:
    void currentIndexChanged(int index);

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    friend class QdfRibbonPagePrivate;
    friend class QdfRibbonGroupPopup;
    friend class QdfRibbonGroupPopupButton;

    QDF_DECLARE_PRIVATE(QdfRibbonGroup)
    Q_DISABLE_COPY(QdfRibbonGroup)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONGROUP_H
