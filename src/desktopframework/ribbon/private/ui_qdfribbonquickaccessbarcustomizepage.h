/********************************************************************************
** Form generated from reading UI file 'qdfribbonquickaccessbarcustomizepage.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDFRIBBONQUICKACCESSBARCUSTOMIZEPAGE_H
#define UI_QDFRIBBONQUICKACCESSBARCUSTOMIZEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QdfRibbonQuickAccessBarCustomizePage
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelCommandsFrom;
    QComboBox *comboBoxSourceCategory;
    QLabel *labelCommands;
    QListWidget *commandsList;
    QCheckBox *checkBoxQATOn;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *addButton;
    QPushButton *removeButton;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *quickAccessBarList;
    QPushButton *pushButtonReset;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_3;
    QPushButton *upButton;
    QPushButton *downButton;
    QSpacerItem *verticalSpacer_4;

    void setupUi(QWidget *QdfRibbonQuickAccessBarCustomizePage)
    {
        if (QdfRibbonQuickAccessBarCustomizePage->objectName().isEmpty())
            QdfRibbonQuickAccessBarCustomizePage->setObjectName(QString::fromUtf8("QdfRibbonQuickAccessBarCustomizePage"));
        QdfRibbonQuickAccessBarCustomizePage->resize(629, 371);
        gridLayout = new QGridLayout(QdfRibbonQuickAccessBarCustomizePage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        labelCommandsFrom = new QLabel(QdfRibbonQuickAccessBarCustomizePage);
        labelCommandsFrom->setObjectName(QString::fromUtf8("labelCommandsFrom"));
        labelCommandsFrom->setTextFormat(Qt::AutoText);
        labelCommandsFrom->setWordWrap(false);
        labelCommandsFrom->setOpenExternalLinks(false);
        labelCommandsFrom->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

        verticalLayout_3->addWidget(labelCommandsFrom);

        comboBoxSourceCategory = new QComboBox(QdfRibbonQuickAccessBarCustomizePage);
        comboBoxSourceCategory->setObjectName(QString::fromUtf8("comboBoxSourceCategory"));

        verticalLayout_3->addWidget(comboBoxSourceCategory);

        labelCommands = new QLabel(QdfRibbonQuickAccessBarCustomizePage);
        labelCommands->setObjectName(QString::fromUtf8("labelCommands"));
        labelCommands->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

        verticalLayout_3->addWidget(labelCommands);

        commandsList = new QListWidget(QdfRibbonQuickAccessBarCustomizePage);
        commandsList->setObjectName(QString::fromUtf8("commandsList"));

        verticalLayout_3->addWidget(commandsList);

        checkBoxQATOn = new QCheckBox(QdfRibbonQuickAccessBarCustomizePage);
        checkBoxQATOn->setObjectName(QString::fromUtf8("checkBoxQATOn"));

        verticalLayout_3->addWidget(checkBoxQATOn);

        verticalLayout_3->setStretch(1, 1);

        gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

        verticalLayout->addItem(verticalSpacer);

        addButton = new QPushButton(QdfRibbonQuickAccessBarCustomizePage);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(addButton->sizePolicy().hasHeightForWidth());
        addButton->setSizePolicy(sizePolicy);
        addButton->setAutoDefault(false);

        verticalLayout->addWidget(addButton);

        removeButton = new QPushButton(QdfRibbonQuickAccessBarCustomizePage);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        sizePolicy.setHeightForWidth(removeButton->sizePolicy().hasHeightForWidth());
        removeButton->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(removeButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

        verticalLayout->addItem(verticalSpacer_2);

        verticalLayout->setStretch(2, 1);

        gridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        quickAccessBarList = new QListWidget(QdfRibbonQuickAccessBarCustomizePage);
        quickAccessBarList->setObjectName(QString::fromUtf8("quickAccessBarList"));

        verticalLayout_2->addWidget(quickAccessBarList);

        pushButtonReset = new QPushButton(QdfRibbonQuickAccessBarCustomizePage);
        pushButtonReset->setObjectName(QString::fromUtf8("pushButtonReset"));
        sizePolicy.setHeightForWidth(pushButtonReset->sizePolicy().hasHeightForWidth());
        pushButtonReset->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(pushButtonReset);


        gridLayout->addLayout(verticalLayout_2, 0, 2, 1, 1);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        upButton = new QPushButton(QdfRibbonQuickAccessBarCustomizePage);
        upButton->setObjectName(QString::fromUtf8("upButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(upButton->sizePolicy().hasHeightForWidth());
        upButton->setSizePolicy(sizePolicy1);
        upButton->setMaximumSize(QSize(30, 30));

        verticalLayout_4->addWidget(upButton);

        downButton = new QPushButton(QdfRibbonQuickAccessBarCustomizePage);
        downButton->setObjectName(QString::fromUtf8("downButton"));
        sizePolicy1.setHeightForWidth(downButton->sizePolicy().hasHeightForWidth());
        downButton->setSizePolicy(sizePolicy1);
        downButton->setMaximumSize(QSize(30, 30));
        downButton->setFlat(false);

        verticalLayout_4->addWidget(downButton);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);


        gridLayout->addLayout(verticalLayout_4, 0, 3, 1, 1);

#if QT_CONFIG(shortcut)
        labelCommandsFrom->setBuddy(comboBoxSourceCategory);
        labelCommands->setBuddy(commandsList);
#endif // QT_CONFIG(shortcut)

        retranslateUi(QdfRibbonQuickAccessBarCustomizePage);

        QMetaObject::connectSlotsByName(QdfRibbonQuickAccessBarCustomizePage);
    } // setupUi

    void retranslateUi(QWidget *QdfRibbonQuickAccessBarCustomizePage)
    {
        QdfRibbonQuickAccessBarCustomizePage->setWindowTitle(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "Customize the Quick Access Toolbar", nullptr));
        labelCommandsFrom->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "&Choose commands from:", nullptr));
        labelCommands->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "C&ommands:", nullptr));
        checkBoxQATOn->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "S&how Quick Access Toolbar below the Ribbon", nullptr));
        addButton->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "&Add > >", nullptr));
        removeButton->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "< < &Remove", nullptr));
        pushButtonReset->setText(QCoreApplication::translate("QdfRibbonQuickAccessBarCustomizePage", "Re&set", nullptr));
        upButton->setText(QString());
        downButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QdfRibbonQuickAccessBarCustomizePage: public Ui_QdfRibbonQuickAccessBarCustomizePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDFRIBBONQUICKACCESSBARCUSTOMIZEPAGE_H
