/********************************************************************************
** Form generated from reading UI file 'qdfribbonrenamedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDFRIBBONRENAMEDIALOG_H
#define UI_QDFRIBBONRENAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QdfRibbonRenameDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayoutTop;
    QLabel *label;
    QLineEdit *lineEditName;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayoutBottom;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QdfRibbonRenameDialog)
    {
        if (QdfRibbonRenameDialog->objectName().isEmpty())
            QdfRibbonRenameDialog->setObjectName(QString::fromUtf8("QdfRibbonRenameDialog"));
        QdfRibbonRenameDialog->resize(303, 95);
        gridLayout = new QGridLayout(QdfRibbonRenameDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, -1, 0);
        horizontalLayoutTop = new QHBoxLayout();
        horizontalLayoutTop->setObjectName(QString::fromUtf8("horizontalLayoutTop"));
        horizontalLayoutTop->setSizeConstraint(QLayout::SetNoConstraint);
        label = new QLabel(QdfRibbonRenameDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayoutTop->addWidget(label);

        lineEditName = new QLineEdit(QdfRibbonRenameDialog);
        lineEditName->setObjectName(QString::fromUtf8("lineEditName"));
        lineEditName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayoutTop->addWidget(lineEditName);

        horizontalLayoutTop->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayoutTop);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayoutBottom = new QHBoxLayout();
        horizontalLayoutBottom->setObjectName(QString::fromUtf8("horizontalLayoutBottom"));
        horizontalLayoutBottom->setSizeConstraint(QLayout::SetDefaultConstraint);
        buttonBox = new QDialogButtonBox(QdfRibbonRenameDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayoutBottom->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayoutBottom);

        verticalLayout->setStretch(2, 1);

        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

#if QT_CONFIG(shortcut)
        label->setBuddy(lineEditName);
#endif // QT_CONFIG(shortcut)

        retranslateUi(QdfRibbonRenameDialog);

        QMetaObject::connectSlotsByName(QdfRibbonRenameDialog);
    } // setupUi

    void retranslateUi(QDialog *QdfRibbonRenameDialog)
    {
        QdfRibbonRenameDialog->setWindowTitle(QCoreApplication::translate("QdfRibbonRenameDialog", "Rename", nullptr));
        label->setText(QCoreApplication::translate("QdfRibbonRenameDialog", "&Display Name:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QdfRibbonRenameDialog: public Ui_QdfRibbonRenameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDFRIBBONRENAMEDIALOG_H
