#ifndef DESKTOPFRAMEWORK_QDFRIBBONGALLERY_H
#define DESKTOPFRAMEWORK_QDFRIBBONGALLERY_H

#include <QIcon>
#include <QMenu>
#include <QStyleOptionSlider>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonGallery;
class QdfRibbonGalleryGroup;
class QdfRibbonGalleryItemPrivate;

class QDF_EXPORT QdfRibbonGalleryItem
{
public:
    QdfRibbonGalleryItem();
    virtual ~QdfRibbonGalleryItem();

public:
    virtual QSize sizeHint() const;
    void setSizeHint(const QSize &size);
    QIcon icon() const;
    void setIcon(const QIcon &icon);
    QString caption() const;
    void setCaption(const QString &caption);
    QString toolTip() const;
    void setToolTip(const QString &toolTip);
    QString statusTip() const;
    void setStatusTip(const QString &statusTip);
    int getIndex() const;
    bool isSeparator() const;
    void setSeparator(bool b);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setVisible(bool visible);
    bool isVisible() const;
    virtual QVariant data(int role) const;
    virtual void setData(int role, const QVariant &value);
    virtual void draw(QPainter *p, QdfRibbonGallery *gallery, QRect rectItem, bool enabled, bool selected, bool pressed,
                      bool checked);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonGalleryItem)
    Q_DISABLE_COPY(QdfRibbonGalleryItem)
    friend class QdfRibbonGalleryGroup;
    friend class QdfRibbonGalleryGroupPrivate;
};

class QdfRibbonGalleryGroupPrivate;
class QdfOfficePopupMenu;
class QDF_EXPORT QdfRibbonGalleryGroup : public QObject
{
    Q_OBJECT
public:
    QdfRibbonGalleryGroup(QObject *parent = nullptr);
    virtual ~QdfRibbonGalleryGroup();

public:
    static QdfRibbonGalleryGroup *createGalleryGroup();

public:
    QdfRibbonGalleryItem *addItem(const QString &caption, const QPixmap &pixmap = QPixmap(),
                                  const QColor &transparentColor = QColor());
    QdfRibbonGalleryItem *addItemFromMap(const QString &caption, int mapIndex, const QPixmap &map,
                                         const QSize &mapItemSize, const QColor &transparentColor = QColor());

    void appendItem(QdfRibbonGalleryItem *item);
    void insertItem(int index, QdfRibbonGalleryItem *item);

    QdfRibbonGalleryItem *addSeparator(const QString &caption);

    void clear();
    void remove(int index);

    int itemCount() const;
    QdfRibbonGalleryItem *item(int index) const;
    QdfRibbonGalleryItem *takeItem(int index);

    QSize size() const;
    void setSize(const QSize &size);

public:
    void setClipItems(bool clipItems);

private:
    friend class QdfRibbonGalleryItem;
    friend class QdfRibbonGallery;
    friend class QdfRibbonGalleryPrivate;
    QDF_DECLARE_PRIVATE(QdfRibbonGalleryGroup)
    Q_DISABLE_COPY(QdfRibbonGalleryGroup)
};

class QdfRibbonGalleryPrivate;
class QDF_EXPORT QdfRibbonGallery : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isBorderVisible READ isBorderVisible WRITE setBorderVisible)
    Q_PROPERTY(int minimumColumnCount READ minimumColumnCount WRITE setMinimumColumnCount)
    Q_PROPERTY(int maximumColumnCount READ maximumColumnCount WRITE setMaximumColumnCount)
    Q_PROPERTY(Qt::ScrollBarPolicy scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy)
    Q_PROPERTY(bool isResizable READ isResizable)
    Q_PROPERTY(bool autoWidth READ autoWidth WRITE setAutoWidth)
public:
    QdfRibbonGallery(QWidget *parent = nullptr);
    virtual ~QdfRibbonGallery();

public:
    void setGalleryGroup(QdfRibbonGalleryGroup *items);
    QdfRibbonGalleryGroup *galleryGroup() const;

    void setBorderVisible(bool isBorderVisible);
    bool isBorderVisible() const;

    void setLabelsVisible(bool showLabels);
    bool isLabelsVisible() const;

    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    Qt::ScrollBarPolicy scrollBarPolicy() const;

    void ensureVisible(int index);

    QAction *setPopupMenu(QdfOfficePopupMenu *popupMenu);
    QdfOfficePopupMenu *popupMenu() const;

    void setMinimumColumnCount(int count);
    int minimumColumnCount() const;

    void setMaximumColumnCount(int count);
    int maximumColumnCount() const;

    void setColumnCount(int count);
    int columnCount() const;

public:
    int itemCount() const;

    QdfRibbonGalleryItem *item(int index) const;
    void setSelectedItem(int selected);
    int selectedItem() const;

    void setCheckedIndex(int index);
    int checkedIndex() const;

    void setCheckedItem(const QdfRibbonGalleryItem *item);
    QdfRibbonGalleryItem *checkedItem() const;

    bool isShowAsButton() const;
    bool isItemSelected() const;

public:
    virtual QRect borders() const;
    QRect getDrawItemRect(int index);
    bool isResizable() const;

    bool autoWidth() const;
    void setAutoWidth(bool width);

    int hitTestItem(QPoint point, QRect *rect = nullptr) const;

    QRect getItemsRect() const;
    void hideSelection();
    void updatelayout();
    void bestFit();

Q_SIGNALS:
    void itemPressed(QdfRibbonGalleryItem *item);
    void itemClicked(QdfRibbonGalleryItem *item);
    void itemClicking(QdfRibbonGalleryItem *item, bool &handled);
    void currentItemChanged(QdfRibbonGalleryItem *current, QdfRibbonGalleryItem *previous);
    void itemSelectionChanged();

public:
    virtual void selectedItemChanged();
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    friend class QdfRibbonGalleryGroupPrivate;
    QDF_DECLARE_PRIVATE(QdfRibbonGallery)
    Q_DISABLE_COPY(QdfRibbonGallery)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONGALLERY_H
