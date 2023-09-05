#ifndef DESKTOPFRAMEWORK_QDFPLUGIN_H
#define DESKTOPFRAMEWORK_QDFPLUGIN_H

#include <QObject>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfEvent;
class QDF_EXPORT QdfPlugin : public QObject
{
public:
    virtual ~QdfPlugin() = default;
    virtual QString name() const = 0;
    virtual QString alias() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    virtual QString organization() const = 0;

    virtual bool event(QdfEvent *event) = 0;
    virtual bool beforeInitialize() = 0;
    virtual bool afterInitialize() = 0;
    virtual void finishStartUp() = 0;
    virtual bool unload() = 0;
    virtual bool destory() = 0;

    /**
     * @brief The ribbon event loop is emitted by QApplication, 
     * and the plugin inherits this method to listen for event messages.
     * @param  event            
     */
    virtual void customEvent(QEvent *event) override;

protected:
    QdfPlugin() = default;

private:
    QdfPlugin(const QdfPlugin &) = delete;
    QdfPlugin &operator=(const QdfPlugin &) = delete;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFPLUGIN_H