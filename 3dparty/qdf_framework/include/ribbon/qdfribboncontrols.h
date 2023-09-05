#ifndef DESKTOPFRAMEWORK_QDFRIBBONCONTROL_H
#define DESKTOPFRAMEWORK_QDFRIBBONCONTROL_H

#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonControl;
class QdfRibbonControlSizeDefinitionPrivate;
class QDF_EXPORT QdfRibbonControlSizeDefinition : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QdfRibbonControlSizeDefinition)
    QDF_DECLARE_PRIVATE(QdfRibbonControlSizeDefinition)

public:
    enum GroupSize
    {
        GroupLarge = 0,
        GroupMedium = 1,
        GroupSmall = 2,
        GroupPopup = 3
    };
    Q_ENUM(GroupSize)

    enum ControlImageSize
    {
        ImageNone,
        ImageLarge,
        ImageSmall
    };
    Q_ENUM(ControlImageSize)

public:
    explicit QdfRibbonControlSizeDefinition(QdfRibbonControl *parentControl, GroupSize size);
    virtual ~QdfRibbonControlSizeDefinition() override;

public:
    ControlImageSize imageSize() const;
    void setImageSize(ControlImageSize size);

    bool isLabelVisible() const;
    void setLabelVisible(bool visible);

    bool isPopup() const;
    void setPopup(bool popup);

    int visualIndex() const;
    void setVisualIndex(int index);

    bool showSeparator() const;
    void setShowSeparator(bool showSeparator);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isStretchable() const;
    void setStretchable(bool stretchable);

    int minimumItemCount() const;
    void setMinimumItemCount(int count);

    int maximumItemCount() const;
    void setMaximumItemCount(int count);
};


class QdfRibbonGroup;
class QdfRibbonControlPrivate;
class QDF_EXPORT QdfRibbonControl : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonControl(QdfRibbonGroup *parent = nullptr);
    ~QdfRibbonControl() override;

public:
    QdfRibbonGroup *parentGroup() const;
    QAction *defaultAction() const;
    void setDefaultAction(QAction *action);

    virtual void adjustCurrentSize(bool expand);
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);

    virtual void actionChanged();
    virtual void updateLayout();

    QdfRibbonControlSizeDefinition::GroupSize currentSize() const;
    QdfRibbonControlSizeDefinition *sizeDefinition(QdfRibbonControlSizeDefinition::GroupSize size) const;

private:
    friend class QdfRibbonGroup;
    QDF_DECLARE_PRIVATE(QdfRibbonControl)
    Q_DISABLE_COPY(QdfRibbonControl)
};


class QdfRibbonWidgetControlPrivate;
class QDF_EXPORT QdfRibbonWidgetControl : public QdfRibbonControl
{
    Q_OBJECT
public:
    explicit QdfRibbonWidgetControl(QdfRibbonGroup *parentGroup, bool ignoreActionSettings);
    virtual ~QdfRibbonWidgetControl();

public:
    int margin() const;
    void setMargin(int margin);

public:
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);
    virtual QSize sizeHint() const;

protected:
    QWidget *contentWidget() const;
    void setContentWidget(QWidget *widget);

protected:
    virtual void childEvent(QChildEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    friend class QdfRibbonGroup;
    friend class QdfRibbonGroupPrivate;
    QDF_DECLARE_PRIVATE(QdfRibbonWidgetControl)
    Q_DISABLE_COPY(QdfRibbonWidgetControl)
};


class QdfRibbonColumnBreakControlPrivate;
class QDF_EXPORT QdfRibbonColumnBreakControl : public QdfRibbonWidgetControl
{
    Q_OBJECT
public:
    explicit QdfRibbonColumnBreakControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonColumnBreakControl();

public:
    Qt::Alignment alignmentText() const;
    void setAlignmentText(Qt::Alignment alignment);

public:
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonColumnBreakControl)
    Q_DISABLE_COPY(QdfRibbonColumnBreakControl)
};


class QDF_EXPORT QdfRibbonLabelControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonLabelControl(QdfRibbonGroup *parentGroup = nullptr);
    explicit QdfRibbonLabelControl(const QString &text, QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonLabelControl();

public:
    QLabel *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonLabelControl)
};

class QdfRibbonToolBarControlPrivate;
class QDF_EXPORT QdfRibbonToolBarControl : public QdfRibbonControl
{
    Q_OBJECT
public:
    explicit QdfRibbonToolBarControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonToolBarControl();

public:
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);
    virtual void updateLayout();
    void clear();
    QAction *addWidget(QWidget *widget);
    QAction *addWidget(const QIcon &icon, const QString &text, QWidget *widget);
    QAction *addMenu(const QIcon &icon, const QString &text, QMenu *menu,
                     QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu = nullptr,
                       QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style,
                          QMenu *menu = nullptr,
                          QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addAction(const QString &text, const QObject *receiver, const char *member);
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver,
                       const char *member);
    QAction *addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style,
                       QMenu *menu = nullptr,
                       QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
    QAction *addSeparator();
    virtual QSize sizeHint() const;
    void setMargin(int margin);
    int margin() const;
    QWidget *widgetByAction(QAction *action);
    using QWidget::addAction;
    using QWidget::insertAction;

protected:
    int rowsCount();
    void setRowsCount(int count);

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void changeEvent(QEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonToolBarControl)
    Q_DISABLE_COPY(QdfRibbonToolBarControl)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONCONTROL_H
