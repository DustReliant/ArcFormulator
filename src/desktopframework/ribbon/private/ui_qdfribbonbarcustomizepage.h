/********************************************************************************
** Form generated from reading UI file 'qdfribbonbarcustomizepage.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDFRIBBONBARCUSTOMIZEPAGE_H
#define UI_QDFRIBBONBARCUSTOMIZEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QdfRibbonBarCustomizePage
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labelCommandsFrom;
    QComboBox *comboBoxSourceCategory;
    QTreeWidget *treeCommands;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer;
    QPushButton *addButton;
    QPushButton *removeButton;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelCustomizeRibbon;
    QComboBox *switchTabsBox;
    QTreeWidget *treeRibbon;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *newTabButton;
    QPushButton *newGroupButton;
    QPushButton *renameButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelCustomizations;
    QPushButton *resetButton;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_3;
    QPushButton *upButton;
    QPushButton *downButton;
    QSpacerItem *verticalSpacer_4;

    void setupUi(QWidget *QdfRibbonBarCustomizePage)
    {
        if (QdfRibbonBarCustomizePage->objectName().isEmpty())
            QdfRibbonBarCustomizePage->setObjectName(QString::fromUtf8("QdfRibbonBarCustomizePage"));
        QdfRibbonBarCustomizePage->resize(648, 421);
        gridLayout = new QGridLayout(QdfRibbonBarCustomizePage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelCommandsFrom = new QLabel(QdfRibbonBarCustomizePage);
        labelCommandsFrom->setObjectName(QString::fromUtf8("labelCommandsFrom"));

        verticalLayout->addWidget(labelCommandsFrom);

        comboBoxSourceCategory = new QComboBox(QdfRibbonBarCustomizePage);
        comboBoxSourceCategory->setObjectName(QString::fromUtf8("comboBoxSourceCategory"));

        verticalLayout->addWidget(comboBoxSourceCategory);

        treeCommands = new QTreeWidget(QdfRibbonBarCustomizePage);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeCommands->setHeaderItem(__qtreewidgetitem);
        treeCommands->setObjectName(QString::fromUtf8("treeCommands"));
        treeCommands->setHeaderHidden(true);

        verticalLayout->addWidget(treeCommands);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        addButton = new QPushButton(QdfRibbonBarCustomizePage);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(addButton->sizePolicy().hasHeightForWidth());
        addButton->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(addButton);

        removeButton = new QPushButton(QdfRibbonBarCustomizePage);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        sizePolicy.setHeightForWidth(removeButton->sizePolicy().hasHeightForWidth());
        removeButton->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(removeButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        verticalLayout_3->setStretch(2, 1);

        gridLayout->addLayout(verticalLayout_3, 0, 1, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        labelCustomizeRibbon = new QLabel(QdfRibbonBarCustomizePage);
        labelCustomizeRibbon->setObjectName(QString::fromUtf8("labelCustomizeRibbon"));

        verticalLayout_2->addWidget(labelCustomizeRibbon);

        switchTabsBox = new QComboBox(QdfRibbonBarCustomizePage);
        switchTabsBox->setObjectName(QString::fromUtf8("switchTabsBox"));

        verticalLayout_2->addWidget(switchTabsBox);

        treeRibbon = new QTreeWidget(QdfRibbonBarCustomizePage);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QString::fromUtf8("1"));
        treeRibbon->setHeaderItem(__qtreewidgetitem1);
        treeRibbon->setObjectName(QString::fromUtf8("treeRibbon"));
        treeRibbon->header()->setVisible(false);
        treeRibbon->header()->setStretchLastSection(true);

        verticalLayout_2->addWidget(treeRibbon);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        newTabButton = new QPushButton(QdfRibbonBarCustomizePage);
        newTabButton->setObjectName(QString::fromUtf8("newTabButton"));

        horizontalLayout_2->addWidget(newTabButton);

        newGroupButton = new QPushButton(QdfRibbonBarCustomizePage);
        newGroupButton->setObjectName(QString::fromUtf8("newGroupButton"));

        horizontalLayout_2->addWidget(newGroupButton);

        renameButton = new QPushButton(QdfRibbonBarCustomizePage);
        renameButton->setObjectName(QString::fromUtf8("renameButton"));

        horizontalLayout_2->addWidget(renameButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, -1, -1);
        labelCustomizations = new QLabel(QdfRibbonBarCustomizePage);
        labelCustomizations->setObjectName(QString::fromUtf8("labelCustomizations"));

        horizontalLayout_3->addWidget(labelCustomizations);

        resetButton = new QPushButton(QdfRibbonBarCustomizePage);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        horizontalLayout_3->addWidget(resetButton);

        horizontalLayout_3->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout_3);


        gridLayout->addLayout(verticalLayout_2, 0, 2, 1, 1);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, -1, -1, -1);
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        upButton = new QPushButton(QdfRibbonBarCustomizePage);
        upButton->setObjectName(QString::fromUtf8("upButton"));

        verticalLayout_4->addWidget(upButton);

        downButton = new QPushButton(QdfRibbonBarCustomizePage);
        downButton->setObjectName(QString::fromUtf8("downButton"));

        verticalLayout_4->addWidget(downButton);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);


        gridLayout->addLayout(verticalLayout_4, 0, 3, 1, 1);

#if QT_CONFIG(shortcut)
        labelCommandsFrom->setBuddy(comboBoxSourceCategory);
        labelCustomizeRibbon->setBuddy(treeRibbon);
#endif // QT_CONFIG(shortcut)

        retranslateUi(QdfRibbonBarCustomizePage);

        QMetaObject::connectSlotsByName(QdfRibbonBarCustomizePage);
    } // setupUi

    void retranslateUi(QWidget *QdfRibbonBarCustomizePage)
    {
        QdfRibbonBarCustomizePage->setWindowTitle(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Customize Ribbon", nullptr));
        labelCommandsFrom->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "&Choose commands from:", nullptr));
        addButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "&Add > >", nullptr));
        removeButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "< < &Remove", nullptr));
        labelCustomizeRibbon->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Customize the Ri&bbon:", nullptr));
        newTabButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Ne&w Tab", nullptr));
        newGroupButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "&New Group", nullptr));
        renameButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Rena&me...", nullptr));
        labelCustomizations->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Customizations:", nullptr));
        resetButton->setText(QCoreApplication::translate("QdfRibbonBarCustomizePage", "Re&set", nullptr));
        upButton->setText(QString());
        downButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QdfRibbonBarCustomizePage: public Ui_QdfRibbonBarCustomizePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDFRIBBONBARCUSTOMIZEPAGE_H
