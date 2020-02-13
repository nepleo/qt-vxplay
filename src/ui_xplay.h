/********************************************************************************
** Form generated from reading UI file 'xplay.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "xvideowidget.h"

QT_BEGIN_NAMESPACE

class Ui_xplayClass
{
public:
    xvideowidget *video;
    QPushButton *openFile;
    QSlider *playpos;
    QPushButton *isplay;

    void setupUi(QWidget *xplayClass)
    {
        if (xplayClass->objectName().isEmpty())
            xplayClass->setObjectName(QString::fromUtf8("xplayClass"));
        xplayClass->resize(1280, 720);
        video = new xvideowidget(xplayClass);
        video->setObjectName(QString::fromUtf8("video"));
        video->setGeometry(QRect(0, 0, 1280, 720));
        openFile = new QPushButton(xplayClass);
        openFile->setObjectName(QString::fromUtf8("openFile"));
        openFile->setGeometry(QRect(80, 640, 61, 31));
        playpos = new QSlider(xplayClass);
        playpos->setObjectName(QString::fromUtf8("playpos"));
        playpos->setGeometry(QRect(40, 690, 1211, 22));
        playpos->setMaximum(999);
        playpos->setOrientation(Qt::Horizontal);
        isplay = new QPushButton(xplayClass);
        isplay->setObjectName(QString::fromUtf8("isplay"));
        isplay->setGeometry(QRect(180, 640, 61, 31));

        retranslateUi(xplayClass);
        QObject::connect(openFile, SIGNAL(clicked()), xplayClass, SLOT(openFile()));
        QObject::connect(isplay, SIGNAL(clicked()), xplayClass, SLOT(PlayOrPause()));
        QObject::connect(playpos, SIGNAL(sliderPressed()), xplayClass, SLOT(SliderPress()));
        QObject::connect(playpos, SIGNAL(sliderReleased()), xplayClass, SLOT(SliderRelease()));

        QMetaObject::connectSlotsByName(xplayClass);
    } // setupUi

    void retranslateUi(QWidget *xplayClass)
    {
        xplayClass->setWindowTitle(QCoreApplication::translate("xplayClass", "xplay", nullptr));
        openFile->setText(QCoreApplication::translate("xplayClass", "\346\226\207\344\273\266", nullptr));
        isplay->setText(QCoreApplication::translate("xplayClass", "\346\222\255\346\224\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class xplayClass: public Ui_xplayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
