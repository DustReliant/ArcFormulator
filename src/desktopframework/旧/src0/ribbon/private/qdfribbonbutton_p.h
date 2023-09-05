#ifndef DESKTOPFRAMEWORK_QDFRIBBONBUTTON_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONBUTTON_P_H

#include <QStyle>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonButton : public QToolButton
{
    Q_OBJECT
public:
    QdfRibbonButton(QWidget *parent = nullptr);
    virtual ~QdfRibbonButton();

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);

private:
    QSize m_sizeHint;
    Qt::ToolButtonStyle m_saveButtonStyle;
    Qt::ArrowType m_saveArrowType;

private:
    Q_DISABLE_COPY(QdfRibbonButton)
};

class QdfRibbonTitleButton : public QToolButton
{
    Q_OBJECT
public:
    QdfRibbonTitleButton(QWidget *parent, QStyle::SubControl subControl);
    virtual ~QdfRibbonTitleButton();

public:
    QStyle::SubControl subControl() const;

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QStyle::SubControl m_subControl;

private:
    Q_DISABLE_COPY(QdfRibbonTitleButton)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONBUTTON_P_H