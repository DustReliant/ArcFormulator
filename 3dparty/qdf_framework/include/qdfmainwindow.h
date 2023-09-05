#ifndef DESKTOPFRAMEWORK_QDFMAINWINDOW_H
#define DESKTOPFRAMEWORK_QDFMAINWINDOW_H

#include <QMainWindow>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonBar;
class QdfStatusBar;
class QdfOfficeFrameHelper;
class QdfDockManager;

class QdfMainWindowPrivate;
class QDF_EXPORT QdfMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit QdfMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~QdfMainWindow() override;

    QdfRibbonBar *ribbonBar() const;
    void setRibbonBar(QdfRibbonBar *ribbonBar);

    QdfDockManager *dockManager() const;
    void setDockManager(QdfDockManager *manager);

    using QMainWindow::setCentralWidget;
    void setCentralWidget(QStyle *style);

protected:
    void setFrameHelper(QdfOfficeFrameHelper *helper);

#ifdef Q_OS_WIN
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif// Q_OS_WIN

private:
    friend class QdfOfficeFrameHelperWin;
    QDF_DECLARE_PRIVATE(QdfMainWindow)
    Q_DISABLE_COPY(QdfMainWindow)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFMAINWINDOW_H