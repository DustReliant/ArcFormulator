#ifndef DESKTOPFRAMEWORK_QDFELIDINGLABEL_P_H
#define DESKTOPFRAMEWORK_QDFELIDINGLABEL_P_H

#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfElidingLabel;
class QdfElidingLabelPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfElidingLabel)

    QdfElidingLabelPrivate() = default;
    void elideText(int width);
    bool isModeElideNone() const
    {
        return Qt::ElideNone == elideMode;
    }

private:
    Qt::TextElideMode elideMode = Qt::ElideNone;
    QString text;
    bool isElided = false;

    Q_DISABLE_COPY(QdfElidingLabelPrivate)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFELIDINGLABEL_P_H