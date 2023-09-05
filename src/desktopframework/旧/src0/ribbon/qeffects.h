#ifndef DESKTOPFRAMEWORK_QEFFECTS_H
#define DESKTOPFRAMEWORK_QEFFECTS_H

#include <QElapsedTimer>
#include <QPixmap>
#include <QPointer>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE

struct QEffects {
    enum Direction
    {
        LeftScroll = 0x0001,
        RightScroll = 0x0002,
        UpScroll = 0x0004,
        DownScroll = 0x0008
    };

    typedef uint DirFlags;
};

void qScrollEffect(QWidget *, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);
void qFadeEffect(QWidget *, int time = -1);


class QAlphaWidget : public QWidget, private QEffects
{
    Q_OBJECT
public:
    QAlphaWidget(QWidget *w, Qt::WindowFlags f = {});
    ~QAlphaWidget();

    void run(int time);

protected:
    void paintEvent(QPaintEvent *e) override;
    void closeEvent(QCloseEvent *) override;
    void alphaBlend();
    bool eventFilter(QObject *, QEvent *) override;

protected slots:
    void render();

private:
    QPixmap pm;
    double alpha;
    QImage backImage;
    QImage frontImage;
    QImage mixedImage;
    QPointer<QWidget> widget;
    int duration;
    int elapsed;
    bool showWidget;
    QTimer anim;
    QElapsedTimer checkTime;
};

QT_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QEFFECTS_H