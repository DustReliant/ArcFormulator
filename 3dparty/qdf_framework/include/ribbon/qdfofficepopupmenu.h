#ifndef DESKTOPFRAMEWORK_QDFOFFICEPOPUPMENU_H
#define DESKTOPFRAMEWORK_QDFOFFICEPOPUPMENU_H

#include <QMenu>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfOfficePopupMenuPrivate;
class QDF_EXPORT QdfOfficePopupMenu : public QMenu
{
    Q_OBJECT
    Q_PROPERTY(bool gripVisible READ isGripVisible WRITE setGripVisible)
public:
    QdfOfficePopupMenu(QWidget *parent = nullptr);
    virtual ~QdfOfficePopupMenu();

    static QdfOfficePopupMenu *createPopupMenu(QWidget *parent = nullptr);

public:
    QAction *addWidget(QWidget *widget);

    void setGripVisible(bool visible);
    bool isGripVisible() const;

public:
    virtual QSize sizeHint() const;

protected:
    void setWidgetBar(QWidget *widget);

private Q_SLOTS:
    void aboutToShowBar();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void moveEvent(QMoveEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    friend class QdfRibbonGalleryPrivate;
    QDF_DECLARE_PRIVATE(QdfOfficePopupMenu)
    Q_DISABLE_COPY(QdfOfficePopupMenu)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFOFFICEPOPUPMENU_H