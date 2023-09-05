#ifndef DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_H
#define DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_H

#include <QToolBar>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonQuickAccessBarPrivate;
class QDF_EXPORT QdfRibbonQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit QdfRibbonQuickAccessBar(QWidget *parent = nullptr);
    ~QdfRibbonQuickAccessBar() override;

public:
    QAction *actionCustomizeButton() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

public:
    virtual QSize sizeHint() const;

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);

private Q_SLOTS:
    void customizeAction(QAction *);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonQuickAccessBar)
    Q_DISABLE_COPY(QdfRibbonQuickAccessBar)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_H
