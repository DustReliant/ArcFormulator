#ifndef __RIBBONWINDOW_H
#define __RIBBONWINDOW_H

#include "ribbonimages.h"
#include <qdfmainwindow.h>
#include <ribbon/qdfribbonstyle.h>

QDF_USE_NAMESPACE

class RibbonWindow : public QdfMainWindow
{
    Q_OBJECT
public:
    explicit RibbonWindow(QWidget *parent = nullptr);

private:
    void createOptions();

protected Q_SLOTS:
    virtual void about();
    virtual void optionsTheme(QAction *);
    void optionsDialog();
    void includingAnimation(bool);
    void setTitleGroupsVisible(bool);
    void maximizeToggle();
    void minimizationChangedIcon(bool minimized);
    void optionsFont(QAction *act);
    void setDPIToggled(bool);
    void setNativeScrollBarsToggled(bool);
    void setNativeDialogsToggled(bool);
    void showQuickAccessCustomizeMenu(QMenu *menu);
    void switchQuickAccessBarPosition();
    virtual void showRibbonContextMenu(QMenu *menu, QContextMenuEvent *event);

#ifdef Q_OS_WIN
    void frameTheme(bool);
#endif// Q_OS_WIN
protected:
    QdfRibbonStyle::Theme styleTheme() const;
    void setStyleTheme(QdfRibbonStyle::Theme themeId);
    void setButtonStyleTheme(QdfRibbonStyle::Theme themeId);
    virtual void updateActionsTheme();

protected:
    QMenu *m_menuOptions;
    QActionGroup *m_styleActions;
    QAction *m_actionRibbonMinimize;
    QAction *m_actionRibbonMinimizeMenu;
    QAction *m_actionDefault;
    QAction *m_actionNormal;
    QAction *m_actionLarge;
    QAction *m_actionExLarge;
    QAction *m_actionShowBelowRibbon;
    QAction *m_actionShowAboveRibbon;

    QdfRibbonStyle *m_ribbonStyle;

private:
    Q_DISABLE_COPY(RibbonWindow)
};

#endif// __RIBBONWINDOW_H
