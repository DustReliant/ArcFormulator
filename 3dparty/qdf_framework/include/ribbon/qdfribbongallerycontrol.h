#ifndef DESKTOPFRAMEWORK_QDFRIBBONGALLERYCONTROL_H
#define DESKTOPFRAMEWORK_QDFRIBBONGALLERYCONTROL_H

#include "qdfribboncontrols.h"
#include "qdfribbongallery.h"
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfRibbonGalleryControl : public QdfRibbonWidgetControl
{
    Q_OBJECT
public:
    explicit QdfRibbonGalleryControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonGalleryControl();

public:
    QdfRibbonGallery *widget() const;
    void setContentsMargins(int top, int bottom);

public:
    virtual QSize sizeHint() const;
    virtual void adjustCurrentSize(bool expand);
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    int m_marginTop;
    int m_marginBottom;

private:
    Q_DISABLE_COPY(QdfRibbonGalleryControl)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONGALLERYCONTROL_H
