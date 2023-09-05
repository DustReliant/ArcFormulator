#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_H

#include "qdfribboncustomizedialog.h"
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonBar;
class QdfRibbonQuickAccessBarCustomizePagePrivate;
class QDF_EXPORT QdfRibbonQuickAccessBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonQuickAccessBarCustomizePage(QdfRibbonBar *ribbonBar);
    virtual ~QdfRibbonQuickAccessBarCustomizePage();

public:
    QdfRibbonBar *ribbonBar() const;
    void addCustomCategory(const QString &strCategory);
    void addSeparatorCategory(const QString &strCategory = QString());

public Q_SLOTS:
    void accepted();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonQuickAccessBarCustomizePage)
    Q_DISABLE_COPY(QdfRibbonQuickAccessBarCustomizePage)
};

class QdfRibbonBarCustomizePagePrivate;
class QDF_EXPORT QdfRibbonBarCustomizePage : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonBarCustomizePage(QdfRibbonBar *ribbonBar);
    virtual ~QdfRibbonBarCustomizePage();

public:
    QdfRibbonBar *ribbonBar() const;
    void addCustomCategory(const QString &strCategory);
    void addSeparatorCategory(const QString &strCategory = QString());

public Q_SLOTS:
    void accepted();
    void rejected();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonBarCustomizePage)
    Q_DISABLE_COPY(QdfRibbonBarCustomizePage)
};


QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMPAGE_H
