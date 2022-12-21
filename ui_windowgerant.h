/********************************************************************************
** Form generated from reading UI file 'windowgerant.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWGERANT_H
#define UI_WINDOWGERANT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WindowGerant
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QFrame *frame;
    QTableWidget *tableWidgetStock;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEditIntitule;
    QLineEdit *lineEditPrix;
    QLineEdit *lineEditStock;
    QPushButton *pushButtonModifier;
    QLabel *label_5;
    QFrame *frame_2;
    QLabel *label_6;
    QLineEdit *lineEditPublicite;
    QPushButton *pushButtonPublicite;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *WindowGerant)
    {
        if (WindowGerant->objectName().isEmpty())
            WindowGerant->setObjectName(QString::fromUtf8("WindowGerant"));
        WindowGerant->resize(544, 489);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WindowGerant->sizePolicy().hasHeightForWidth());
        WindowGerant->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8("images/poivron.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        WindowGerant->setWindowIcon(icon);
        centralwidget = new QWidget(WindowGerant);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 64, 17));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(12);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8(""));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 30, 521, 301));
        frame->setStyleSheet(QString::fromUtf8("background:rgb(182, 250, 217)"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        tableWidgetStock = new QTableWidget(frame);
        tableWidgetStock->setObjectName(QString::fromUtf8("tableWidgetStock"));
        tableWidgetStock->setGeometry(QRect(10, 10, 501, 192));
        tableWidgetStock->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255)"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 210, 71, 21));
        label_2->setFont(font);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 240, 64, 21));
        label_3->setFont(font);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 270, 71, 21));
        label_4->setFont(font);
        lineEditIntitule = new QLineEdit(frame);
        lineEditIntitule->setObjectName(QString::fromUtf8("lineEditIntitule"));
        lineEditIntitule->setGeometry(QRect(90, 210, 191, 25));
        lineEditIntitule->setFont(font);
        lineEditIntitule->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255)"));
        lineEditIntitule->setAlignment(Qt::AlignCenter);
        lineEditIntitule->setReadOnly(true);
        lineEditPrix = new QLineEdit(frame);
        lineEditPrix->setObjectName(QString::fromUtf8("lineEditPrix"));
        lineEditPrix->setGeometry(QRect(90, 240, 191, 25));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu Sans"));
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        lineEditPrix->setFont(font1);
        lineEditPrix->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255);\n"
"color:rgb(239, 41, 41)\n"
""));
        lineEditPrix->setAlignment(Qt::AlignCenter);
        lineEditStock = new QLineEdit(frame);
        lineEditStock->setObjectName(QString::fromUtf8("lineEditStock"));
        lineEditStock->setGeometry(QRect(90, 270, 191, 25));
        lineEditStock->setFont(font1);
        lineEditStock->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255);\n"
"color:rgb(239, 41, 41)"));
        lineEditStock->setAlignment(Qt::AlignCenter);
        pushButtonModifier = new QPushButton(frame);
        pushButtonModifier->setObjectName(QString::fromUtf8("pushButtonModifier"));
        pushButtonModifier->setGeometry(QRect(300, 210, 211, 81));
        pushButtonModifier->setFont(font);
        pushButtonModifier->setCursor(QCursor(Qt::PointingHandCursor));
        pushButtonModifier->setStyleSheet(QString::fromUtf8("background-color: lightblue"));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 350, 91, 17));
        label_5->setFont(font);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 370, 521, 91));
        frame_2->setStyleSheet(QString::fromUtf8("background:rgb(182, 250, 217)"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 10, 71, 21));
        label_6->setFont(font);
        lineEditPublicite = new QLineEdit(frame_2);
        lineEditPublicite->setObjectName(QString::fromUtf8("lineEditPublicite"));
        lineEditPublicite->setGeometry(QRect(80, 10, 431, 25));
        lineEditPublicite->setFont(font1);
        lineEditPublicite->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255);\n"
"color:rgb(239, 41, 41)"));
        lineEditPublicite->setMaxLength(50);
        pushButtonPublicite = new QPushButton(frame_2);
        pushButtonPublicite->setObjectName(QString::fromUtf8("pushButtonPublicite"));
        pushButtonPublicite->setGeometry(QRect(10, 50, 501, 25));
        pushButtonPublicite->setFont(font);
        pushButtonPublicite->setCursor(QCursor(Qt::PointingHandCursor));
        pushButtonPublicite->setStyleSheet(QString::fromUtf8("background-color: lightblue"));
        WindowGerant->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WindowGerant);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 544, 22));
        WindowGerant->setMenuBar(menubar);
        statusbar = new QStatusBar(WindowGerant);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WindowGerant->setStatusBar(statusbar);

        retranslateUi(WindowGerant);

        QMetaObject::connectSlotsByName(WindowGerant);
    } // setupUi

    void retranslateUi(QMainWindow *WindowGerant)
    {
        WindowGerant->setWindowTitle(QApplication::translate("WindowGerant", "G\303\251rant du Maraicher en ligne", nullptr));
        label->setText(QApplication::translate("WindowGerant", "<html><head/><body><p><span style=\" font-weight:600; color:#1205fd;\">Stock :</span></p></body></html>", nullptr));
        label_2->setText(QApplication::translate("WindowGerant", "Intitul\303\251 :", nullptr));
        label_3->setText(QApplication::translate("WindowGerant", "Prix :", nullptr));
        label_4->setText(QApplication::translate("WindowGerant", "Stock :", nullptr));
        pushButtonModifier->setText(QApplication::translate("WindowGerant", "Modifier", nullptr));
        label_5->setText(QApplication::translate("WindowGerant", "<html><head/><body><p><span style=\" font-weight:600; color:#1205fd;\">Publicit\303\251 :</span></p></body></html>", nullptr));
        label_6->setText(QApplication::translate("WindowGerant", "Texte :", nullptr));
        pushButtonPublicite->setText(QApplication::translate("WindowGerant", "Mettre \303\240 jour", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WindowGerant: public Ui_WindowGerant {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWGERANT_H
