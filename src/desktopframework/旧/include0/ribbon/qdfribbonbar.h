#ifndef DESKTOPFRAMEWORK_QDFRIBBONBAR_H
#define DESKTOPFRAMEWORK_QDFRIBBONBAR_H

#include <QMenu>
#include <QMenuBar>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonPage;
class QdfRibbonBarPrivate;
class QdfRibbonSystemButton;
class QdfRibbonQuickAccessBar;
class QdfRibbonCustomizeManager;
class QdfRibbonCustomizeDialog;

class QDF_EXPORT QdfRibbonBar : public QMenuBar
{
    Q_OBJECT
    Q_PROPERTY(QuickAccessBarPosition quickAccessBarPosition READ quickAccessBarPosition WRITE setQuickAccessBarPosition)
    Q_PROPERTY(TabBarPosition tabBarPosition READ tabBarPosition WRITE setTabBarPosition)
    Q_PROPERTY(int currentPageIndex READ currentPageIndex WRITE setCurrentPageIndex NOTIFY currentPageIndexChanged)
    Q_PROPERTY(bool frameThemeEnabled READ isFrameThemeEnabled WRITE setFrameThemeEnabled)
    Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized NOTIFY minimizationChanged)
    Q_PROPERTY(bool titleGroupsVisible READ isTitleGroupsVisible WRITE setTitleGroupsVisible)

public:
    enum QuickAccessBarPosition
    {
        QATopPosition QDF_ENUM_STRING_VALUE(QATopPosition) = 1,
        QABottomPosition QDF_ENUM_STRING_VALUE(QABottomPosition)
    };
    Q_ENUM(QuickAccessBarPosition)

    enum TabBarPosition
    {
        TabBarTopPosition = 1,
        TabBarBottomPosition
    };
    Q_ENUM(TabBarPosition)

public:
    explicit QdfRibbonBar(QWidget *parent = nullptr);
    ~QdfRibbonBar() override;

public:
    bool isVisible() const;

public:
    QdfRibbonPage *addPage(const QString &text);
    void addPage(QdfRibbonPage *page);

    QdfRibbonPage *insertPage(int index, const QString &text);
    void insertPage(int index, QdfRibbonPage *page);

    void movePage(QdfRibbonPage *page, int newIndex);
    void movePage(int index, int newIndex);

    void removePage(QdfRibbonPage *page);
    void removePage(int index);

    void detachPage(QdfRibbonPage *page);
    void detachPage(int index);

    void clearPages();

    bool isKeyTipsShowing() const;
    bool keyTipsEnabled() const;
    void setKeyTipsEnable(bool enable);
    bool isKeyTipsComplement() const;
    void setKeyTipsComplement(bool complement);
    void setKeyTip(QAction *action, const QString &keyTip);

    void setLogoPixmap(Qt::AlignmentFlag alignment, const QPixmap &logotype);
    QPixmap logoPixmap(Qt::AlignmentFlag &alignment) const;

    void setTitleBackground(const QPixmap &px);
    const QPixmap &titleBackground() const;

    QdfRibbonQuickAccessBar *quickAccessBar() const;

    void showQuickAccess(bool show = true);
    bool isQuickAccessVisible() const;

    void setQuickAccessBarPosition(QuickAccessBarPosition position);
    QuickAccessBarPosition quickAccessBarPosition() const;

    void setTabBarPosition(TabBarPosition position);
    TabBarPosition tabBarPosition() const;

    void minimize();
    bool isMinimized() const;
    void setMinimized(bool flag);

    void maximize();
    bool isMaximized() const;
    void setMaximized(bool flag);

    void setMinimizationEnabled(bool enabled);
    bool isMinimizationEnabled() const;

    QdfRibbonCustomizeManager *customizeManager();
    QdfRibbonCustomizeDialog *customizeDialog();

public Q_SLOTS:
    void setCurrentPageIndex(int index);
    void showCustomizeDialog();

public:
    int currentPageIndex() const;
    const QList<QdfRibbonPage *> &pages() const;
    QdfRibbonPage *getPage(int index) const;
    int getPageCount() const;

    static bool loadTranslation(const QString &country = QString());
    static QString tr_compatible(const char *s, const char *c = 0);

    QMenu *addMenu(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu = nullptr);
    QAction *addAction(QAction *action, Qt::ToolButtonStyle style);

    QAction *addSystemButton(const QString &text);
    QAction *addSystemButton(const QIcon &icon, const QString &text);

    QdfRibbonSystemButton *getSystemButton() const;

    bool isBackstageVisible() const;

    void setFrameThemeEnabled(bool enable = true);
    bool isFrameThemeEnabled() const;

    bool isTitleBarVisible() const;
    void setTitleBarVisible(bool visible);

    bool isTitleGroupsVisible() const;
    void setTitleGroupsVisible(bool visible);

    int rowItemHeight() const;
    int rowItemCount() const;
    int titleBarHeight() const;

    void updateLayout();

Q_SIGNALS:
    void minimizationChanged(bool minimized);
    void currentPageIndexChanged(int index);
    void currentPageChanged(QdfRibbonPage *page);
    void keyTipsShowed(bool showed);
    void showRibbonContextMenu(QMenu *menu, QContextMenuEvent *event);

protected:
    int backstageViewTop() const;
    int tabBarHeight(bool isVisible = false) const;
    void setMinimizedFlag(bool flag);
#ifdef Q_OS_WIN
    void updateWindowTitle();
#endif
public:
    virtual int heightForWidth(int) const;
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *object, QEvent *event);
    virtual void paintEvent(QPaintEvent *p);
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    #else
    virtual bool winEvent(MSG *message, long *result);
    #endif
#endif// Q_OS_WIN
private:
    friend class QdfRibbonPage;
    friend class QdfRibbonPagePrivate;
    friend class QdfRibbonTabBarPrivate;
    friend class QdfRibbonSystemPopupBar;
    friend class QdfRibbonBackstageViewPrivate;
    friend class QdfRibbonBackstageView;
    friend class QdfRibbonStyle;
    friend class QdfRibbonQuickAccessBar;
    friend class QdfRibbonSystemButton;
    friend class QdfRibbonTab;
    friend class QdfRibbonTabBar;
#ifdef Q_OS_WIN
    friend class QdfOfficeFrameHelperWin;
#endif// Q_OS_WIN
    QDF_DECLARE_PRIVATE(QdfRibbonBar)
    Q_DISABLE_COPY(QdfRibbonBar)
};

extern void qdf_set_font_to_ribbon_children(QWidget *widget, const QFont &font);

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONBAR_H
