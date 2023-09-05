#ifndef DESKTOPFRAMEWORK_QDFAPPLICATION_H
#define DESKTOPFRAMEWORK_QDFAPPLICATION_H

#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfEvent;
class QdfPlugin;
class QdfApplicationPrivate;
class QDF_EXPORT QdfApplication
{
public:
    QdfApplication();
    virtual ~QdfApplication();
    static QdfApplication *instance();


private:
    QdfApplication(const QdfApplication &) = delete;
    QdfApplication &operator=(const QdfApplication &) = delete;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAPPLICATION_H