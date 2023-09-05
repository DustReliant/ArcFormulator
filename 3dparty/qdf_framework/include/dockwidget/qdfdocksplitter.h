#ifndef DESKTOPFRAMEWORK_QDFDOCKSPLITTER_H
#define DESKTOPFRAMEWORK_QDFDOCKSPLITTER_H

#include <QSplitter>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockSplitterPrivate;
class QDF_EXPORT QdfDockSplitter : public QSplitter
{
    Q_OBJECT
public:
    QdfDockSplitter(QWidget *parent = nullptr);
    QdfDockSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~QdfDockSplitter() override;

    bool hasVisibleContent() const;
    QWidget *firstWidget() const;
    QWidget *lastWidget() const;
    bool isResizingWithContainer() const;

private:
    QDF_DECLARE_PRIVATE(QdfDockSplitter)
    Q_DISABLE_COPY(QdfDockSplitter)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKSPLITTER_H