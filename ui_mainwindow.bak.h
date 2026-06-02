/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QxtSpanSlider.h"
#include "myglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_open;
    QAction *actionRect;
    QAction *actionPoint;
    QAction *action_save;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_emiss;
    QLineEdit *lineEdit_emiss;
    QLabel *label_relHum;
    QLineEdit *lineEdit_relHum;
    QLabel *label;
    QLineEdit *lineEdit_distance;
    QLabel *label_2;
    QLineEdit *lineEdit_reflectedT;
    QLabel *label_3;
    QLineEdit *lineEdit_atmospherT;
    QLabel *label_4;
    QLineEdit *lineEdit_K;
    QLabel *label_5;
    QLineEdit *lineEdit_B;
    QHBoxLayout *horizontalLayout_2;
    QListView *listView;
    myGLwidget *openGLwidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit_TH;
    QxtSpanSlider *verticalSlider;
    QLineEdit *lineEdit_TL;
    QToolButton *toolButton;
    QComboBox *comboBox;
    QMenuBar *menubar;
    QMenu *menu_file;
    QMenu *menu_tool;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(849, 584);
        MainWindow->setStyleSheet(QString::fromUtf8("QMenuBar{background-color:rgba(205,201,201,0.8);}\n"
"QMenuBar::selected{background-color:transparent;}\n"
"QMenuBar::item{font-size:12px;font-family:Microsoft YaHei;color:rgba(0,0,0,1);}\n"
"QMainWindow{background-color: rgb(238,233,233);color: rgb(0,0,0);}\n"
""));
        MainWindow->setUnifiedTitleAndToolBarOnMac(false);
        action_open = new QAction(MainWindow);
        action_open->setObjectName(QString::fromUtf8("action_open"));
        action_open->setIconVisibleInMenu(true);
        actionRect = new QAction(MainWindow);
        actionRect->setObjectName(QString::fromUtf8("actionRect"));
        actionRect->setCheckable(true);
        actionPoint = new QAction(MainWindow);
        actionPoint->setObjectName(QString::fromUtf8("actionPoint"));
        actionPoint->setCheckable(true);
        action_save = new QAction(MainWindow);
        action_save->setObjectName(QString::fromUtf8("action_save"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 2, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_emiss = new QLabel(centralwidget);
        label_emiss->setObjectName(QString::fromUtf8("label_emiss"));

        horizontalLayout->addWidget(label_emiss);

        lineEdit_emiss = new QLineEdit(centralwidget);
        lineEdit_emiss->setObjectName(QString::fromUtf8("lineEdit_emiss"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit_emiss->sizePolicy().hasHeightForWidth());
        lineEdit_emiss->setSizePolicy(sizePolicy);
        lineEdit_emiss->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(lineEdit_emiss);

        label_relHum = new QLabel(centralwidget);
        label_relHum->setObjectName(QString::fromUtf8("label_relHum"));

        horizontalLayout->addWidget(label_relHum);

        lineEdit_relHum = new QLineEdit(centralwidget);
        lineEdit_relHum->setObjectName(QString::fromUtf8("lineEdit_relHum"));
        sizePolicy.setHeightForWidth(lineEdit_relHum->sizePolicy().hasHeightForWidth());
        lineEdit_relHum->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_relHum);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit_distance = new QLineEdit(centralwidget);
        lineEdit_distance->setObjectName(QString::fromUtf8("lineEdit_distance"));
        sizePolicy.setHeightForWidth(lineEdit_distance->sizePolicy().hasHeightForWidth());
        lineEdit_distance->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_distance);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        lineEdit_reflectedT = new QLineEdit(centralwidget);
        lineEdit_reflectedT->setObjectName(QString::fromUtf8("lineEdit_reflectedT"));
        sizePolicy.setHeightForWidth(lineEdit_reflectedT->sizePolicy().hasHeightForWidth());
        lineEdit_reflectedT->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_reflectedT);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);

        lineEdit_atmospherT = new QLineEdit(centralwidget);
        lineEdit_atmospherT->setObjectName(QString::fromUtf8("lineEdit_atmospherT"));
        sizePolicy.setHeightForWidth(lineEdit_atmospherT->sizePolicy().hasHeightForWidth());
        lineEdit_atmospherT->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_atmospherT);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        lineEdit_K = new QLineEdit(centralwidget);
        lineEdit_K->setObjectName(QString::fromUtf8("lineEdit_K"));
        sizePolicy.setHeightForWidth(lineEdit_K->sizePolicy().hasHeightForWidth());
        lineEdit_K->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_K);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        lineEdit_B = new QLineEdit(centralwidget);
        lineEdit_B->setObjectName(QString::fromUtf8("lineEdit_B"));
        sizePolicy.setHeightForWidth(lineEdit_B->sizePolicy().hasHeightForWidth());
        lineEdit_B->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(lineEdit_B);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        listView = new QListView(centralwidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy1);
        listView->setMinimumSize(QSize(135, 0));
        listView->setMaximumSize(QSize(16777215, 512));
        listView->setStyleSheet(QString::fromUtf8("QHeaderView::section{background:white;color: black;}"));
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setAlternatingRowColors(false);

        horizontalLayout_2->addWidget(listView);

        openGLwidget = new myGLwidget(centralwidget);
        openGLwidget->setObjectName(QString::fromUtf8("openGLwidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(openGLwidget->sizePolicy().hasHeightForWidth());
        openGLwidget->setSizePolicy(sizePolicy2);
        openGLwidget->setMinimumSize(QSize(640, 512));
        openGLwidget->setMaximumSize(QSize(640, 512));
        openGLwidget->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(openGLwidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEdit_TH = new QLineEdit(centralwidget);
        lineEdit_TH->setObjectName(QString::fromUtf8("lineEdit_TH"));
        sizePolicy.setHeightForWidth(lineEdit_TH->sizePolicy().hasHeightForWidth());
        lineEdit_TH->setSizePolicy(sizePolicy);
        lineEdit_TH->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lineEdit_TH);

        verticalSlider = new QxtSpanSlider(centralwidget);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(verticalSlider->sizePolicy().hasHeightForWidth());
        verticalSlider->setSizePolicy(sizePolicy3);
        verticalSlider->setMinimumSize(QSize(20, 280));
        verticalSlider->setStyleSheet(QString::fromUtf8("QSlider::groove:vertical\n"
"{\n"
"	width: 20px;\n"
"	background: url(:/colorBar/index2.png)\n"
"}\n"
"\n"
"QSlider::handle:vertical\n"
"{\n"
"        width: 20px;\n"
"        height: 8px;\n"
"		background:rgba(32,178,170,0.8);\n"
"        margin: -5px 0px -5px 0px;\n"
"        border-radius: 2px;\n"
"}\n"
""));
        verticalSlider->setValue(0);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setInvertedAppearance(false);
        verticalSlider->setInvertedControls(false);
        verticalSlider->setTickPosition(QSlider::NoTicks);
        verticalSlider->setTickInterval(1);

        verticalLayout->addWidget(verticalSlider);

        lineEdit_TL = new QLineEdit(centralwidget);
        lineEdit_TL->setObjectName(QString::fromUtf8("lineEdit_TL"));
        sizePolicy.setHeightForWidth(lineEdit_TL->sizePolicy().hasHeightForWidth());
        lineEdit_TL->setSizePolicy(sizePolicy);
        lineEdit_TL->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lineEdit_TL);

        toolButton = new QToolButton(centralwidget);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        sizePolicy.setHeightForWidth(toolButton->sizePolicy().hasHeightForWidth());
        toolButton->setSizePolicy(sizePolicy);
        toolButton->setCheckable(true);
        toolButton->setChecked(true);

        verticalLayout->addWidget(toolButton);

        comboBox = new QComboBox(centralwidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        verticalLayout->addWidget(comboBox);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 849, 21));
        menubar->setMouseTracking(true);
        menubar->setNativeMenuBar(false);
        menu_file = new QMenu(menubar);
        menu_file->setObjectName(QString::fromUtf8("menu_file"));
        menu_file->setAcceptDrops(false);
        menu_file->setToolTipDuration(-1);
        menu_file->setToolTipsVisible(true);
        menu_tool = new QMenu(menubar);
        menu_tool->setObjectName(QString::fromUtf8("menu_tool"));
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        QWidget::setTabOrder(toolButton, listView);
        QWidget::setTabOrder(listView, verticalSlider);
        QWidget::setTabOrder(verticalSlider, lineEdit_reflectedT);
        QWidget::setTabOrder(lineEdit_reflectedT, lineEdit_atmospherT);
        QWidget::setTabOrder(lineEdit_atmospherT, lineEdit_K);
        QWidget::setTabOrder(lineEdit_K, lineEdit_B);
        QWidget::setTabOrder(lineEdit_B, lineEdit_TH);
        QWidget::setTabOrder(lineEdit_TH, lineEdit_relHum);
        QWidget::setTabOrder(lineEdit_relHum, lineEdit_distance);
        QWidget::setTabOrder(lineEdit_distance, lineEdit_TL);
        QWidget::setTabOrder(lineEdit_TL, comboBox);

        menubar->addAction(menu_file->menuAction());
        menubar->addAction(menu_tool->menuAction());
        menu_file->addAction(action_open);
        menu_file->addAction(action_save);
        menu_tool->addAction(actionRect);
        menu_tool->addAction(actionPoint);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "IR Analyst", nullptr));
        action_open->setText(QApplication::translate("MainWindow", "open file", nullptr));
#ifndef QT_NO_TOOLTIP
        action_open->setToolTip(QApplication::translate("MainWindow", "open a raw file", nullptr));
#endif // QT_NO_TOOLTIP
        actionRect->setText(QApplication::translate("MainWindow", "Rect", nullptr));
        actionPoint->setText(QApplication::translate("MainWindow", "Point", nullptr));
        action_save->setText(QApplication::translate("MainWindow", "save file", nullptr));
        label_emiss->setText(QApplication::translate("MainWindow", "Emiss:", nullptr));
        label_relHum->setText(QApplication::translate("MainWindow", "RelHum:", nullptr));
        label->setText(QApplication::translate("MainWindow", "Distance:", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "ReflectedTemper:", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "AtmosphericTemper:", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "K-modify:", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "B-modify:", nullptr));
        toolButton->setText(QApplication::translate("MainWindow", "Auto", nullptr));
        comboBox->setItemText(0, QApplication::translate("MainWindow", "\347\231\275\347\203\255", nullptr));
        comboBox->setItemText(1, QApplication::translate("MainWindow", "\347\206\224\345\262\251", nullptr));
        comboBox->setItemText(2, QApplication::translate("MainWindow", "\351\223\201\347\272\242", nullptr));
        comboBox->setItemText(3, QApplication::translate("MainWindow", "\347\203\255\351\223\201", nullptr));
        comboBox->setItemText(4, QApplication::translate("MainWindow", "\345\214\273\347\226\227", nullptr));
        comboBox->setItemText(5, QApplication::translate("MainWindow", "\345\214\227\346\236\201", nullptr));
        comboBox->setItemText(6, QApplication::translate("MainWindow", "\345\275\251\350\231\271", nullptr));
        comboBox->setItemText(7, QApplication::translate("MainWindow", "\346\236\201\345\205\211", nullptr));
        comboBox->setItemText(8, QApplication::translate("MainWindow", "\346\217\217\347\272\242", nullptr));
        comboBox->setItemText(9, QApplication::translate("MainWindow", "\351\273\221\347\203\255", nullptr));

        menu_file->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menu_tool->setTitle(QApplication::translate("MainWindow", "Tool", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
