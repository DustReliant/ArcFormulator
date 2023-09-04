#ifndef DESKTOPFRAMEWORK_QDFRIBBONPAGE_H
#define DESKTOPFRAMEWORK_QDFRIBBONPAGE_H

#include "qdfribbonbar.h"
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonGroup;
class QdfRibbonPagePrivate;
class QDF_EXPORT QdfRibbonPage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(const QString &title READ title WRITE setTitle)
    Q_PROPERTY(const QString &contextTitle READ contextTitle WRITE setContextTitle)
    Q_PROPERTY(int groupCount READ groupCount)
    Q_PROPERTY(ContextColor contextColor READ contextColor WRITE setContextColor)
    Q_PROPERTY(int tabWidth READ tabWidth WRITE setTabWidth)

public:
    enum ContextColor
    {
        ContextColorNone,
        ContextColorGreen,
        ContextColorBlue,
        ContextColorRed,
        ContextColorYellow,
        ContextColorCyan,
        ContextColorPurple,
        ContextColorOrange
    };
    Q_ENUM(ContextColor)

public:
    explicit QdfRibbonPage(QWidget *parent = nullptr);
    explicit QdfRibbonPage(QdfRibbonBar *ribbonBar, const QString &title);
    virtual ~QdfRibbonPage();

public:
    bool isVisible() const;

public:
    void addGroup(QdfRibbonGroup *group);
    QdfRibbonGroup *addGroup(const QString &title);
    QdfRibbonGroup *addGroup(const QIcon &icon, const QString &title);

    void insertGroup(int index, QdfRibbonGroup *group);
    QdfRibbonGroup *insertGroup(int index, const QString &title);
    QdfRibbonGroup *insertGroup(int index, const QIcon &icon, const QString &title);

    void removeGroup(QdfRibbonGroup *group);
    void removeGroup(int index);

    void detachGroup(QdfRibbonGroup *group);
    void detachGroup(int index);

    void clearGroups();

    QAction *defaultAction() const;

    int groupCount() const;
    QdfRibbonGroup *getGroup(int index) const;
    int groupIndex(QdfRibbonGroup *group) const;
    QList<QdfRibbonGroup *> groups() const;

    void setContextColor(ContextColor color);
    ContextColor contextColor() const;

    const QString &title() const;
    const QString &contextTitle() const;
    void setContextGroupName(const QString &groupName);

    void setTabWidth(int width);
    int tabWidth() const;

    void updateLayout();

public Q_SLOTS:
    virtual void setVisible(bool visible);
    void setTitle(const QString &title);
    void setContextTitle(const QString &title);

Q_SIGNALS:
    void titleChanged(const QString &title);
    void activated();
    void activating(bool &allow);

protected Q_SLOTS:
    void released();
    void actionTriggered(QAction *action);

private:
    void setAssociativeTab(QWidget *widget);
    QWidget *associativeTab() const;
    void setPageVisible(bool visible);
    void popup();
    void setRibbonMinimized(bool minimized);

protected:
    void activatingPage(bool &allow);
    void setGroupsHeight(int height);

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

#ifdef Q_OS_WIN
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif// Q_OS_WIN

private:
    friend class QdfRibbonBar;
    friend class QdfRibbonBarPrivate;
    friend class QdfRibbonTabBar;
    friend class QdfRibbonGroup;
    friend class QdfRibbonGroupPrivate;
    friend class QdfRibbonTab;
    friend class QdfRibbonBarCustomizeEngine;

    QDF_DECLARE_PRIVATE(QdfRibbonPage)
    Q_DISABLE_COPY(QdfRibbonPage)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONPAGE_H
