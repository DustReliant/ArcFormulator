#ifndef QARCTABWIDGET_H
#define QARCTABWIDGET_H


#include <QCheckBox>
#include <QStackedWidget>
#include <QTabBar>
#include <QWidget>

class QArcTabWidget : public QWidget
{
    Q_OBJECT

public:
    enum emArcTabPosition
    {
        North,
        South,
        West,
        East
    };

    explicit QArcTabWidget(QWidget *parent = nullptr);
    ~QArcTabWidget();
    int count() const;

    int addTab(QWidget *widget, const QString &label);
    int insertTab(int index, QWidget *widget, const QString &label);
    void removeTab(int index);

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool enabled);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    bool isTabCheck(int index) const;
    void setTabCheck(int index, bool check);

    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;

    int currentIndex() const;

    Q_ENUM(emArcTabPosition)
    emArcTabPosition tabPosition() const;
    void setTabPosition(emArcTabPosition position);

    bool tabsClosable() const;
    void setTabsClosable(bool closeable);

    bool isMovable() const;
    void setMovable(bool movable);

    QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);

Q_SIGNALS:
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void checkBoxStateChanged(int index, bool checked);

protected:
    void tabInserted(int index);
    void tabRemoved(int index);

private slots:
    void checkBoxClicked();

private:
    QTabBar *tabBar_;
    QStackedWidget *stackedWidget_;
    emArcTabPosition tabPosition_;
    bool tabsClosable_;
    bool movable_;
    int currentTabIndex_;

};
#endif // !QARCTABWIDGET_H