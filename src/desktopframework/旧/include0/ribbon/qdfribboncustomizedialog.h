#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonBar;
class QdfRibbonCustomizeDialogPrivate;
class QDF_EXPORT QdfRibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QdfRibbonCustomizeDialog(QWidget *parent = nullptr);
    virtual ~QdfRibbonCustomizeDialog();

public:
    void addPage(QWidget *page);
    void insertPage(int index, QWidget *page);

    int indexOf(QWidget *page) const;

    QWidget *currentPage() const;
    void setCurrentPage(QWidget *page);

    int currentPageIndex() const;
    void setCurrentPageIndex(int index);

    int pageCount() const;
    QWidget *pageByIndex(int index) const;

public:
    virtual void accept();
    virtual void reject();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonCustomizeDialog)
    Q_DISABLE_COPY(QdfRibbonCustomizeDialog)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEDIALOG_H
