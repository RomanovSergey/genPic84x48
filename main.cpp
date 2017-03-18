/*
 * Программа genPic84x48
 *   Предназначена для генерации массива с изображением
 * для микроконтроллера, котороый рисует на LCD
 * дисплее от Nokia5110.
 *   Параметры дисплея: разрешение 84х48,
 * монохромный - 1 бит на пиксель,
 * развертка вертикальная -> сначало идут байты по оси Y
 * 6 байт формируют 48 бит по вертикали, далее координата X
 * смещается на 1 пиксель и снова 6 байт (6х8=48),
 * младший значащий бит в байте имеет меньшее значение по
 * координате Y (т.е. располагается выше)
 *
 *    0\0 --------------------> 84 px (X coordinate)
 *     |       .......
 *     |       (.) (.)
 *     |     q    x    p
 *     |        <--->
 *     |
 *     V
 *    48 px
 *  (Y coordinate)
 *
 *    На вход программы подается изображение, формата *.png
 * с разрешением 84х48, монохромное.
 *   В результате работы генерируется файл в стиле языка Си
 * с кодом для вставки в программу на микроконтроллере.
 */

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QImage>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
      QByteArray localMsg = msg.toLocal8Bit();
      switch (type) {
      case QtDebugMsg:
          fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtInfoMsg:
          fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtWarningMsg:
          fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtCriticalMsg:
          fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtFatalMsg:
          fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          abort();
      }
  }

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("genPic84x48");
    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser parser;

    parser.setApplicationDescription("program for convert *.png image to C code for Microcontroller");
    parser.addHelpOption();
    parser.addVersionOption();

    // An option with a value
    QCommandLineOption optFileName( QStringList() << "i" << "input",
                                 "input file name (*.png or *.bmp or *.jpeg ...) <name>.",
                                 "name");
    parser.addOption( optFileName );

    QCommandLineOption optOutName( QStringList() << "o" << "output",
                                 "output file name (*.c) <name>.",
                                 "name");
    optOutName.setDefaultValue("default.c");
    parser.addOption( optOutName );

    parser.process( a ); // Process the actual command line arguments given by the user

    QString picName = parser.value( optFileName );
    if ( picName.isEmpty() ) {
        cout << "Please set picture's file name (-i FileName)" << endl;
        return 0;
    }
    cout << "Picture name is: " << endl << "\t" << picName.toStdString() << endl;
    QString outName = parser.value( optOutName );
    cout << "Output name is: " << endl << "\t" << outName.toStdString() << endl;

    QImage img( picName );
    if ( img.isNull() ) {
        cout << "Error: Can't load image file: " << picName.toStdString() << endl;
        cout << "Exit" << endl;
        return 0;
    }
    cout << "Format image: " << endl << "\t";
    switch ( img.format() ) {
    case QImage::Format_Invalid: // 0
        cout << "Error: Format_Invalid" << endl;
        return 0;
    case QImage::Format_Mono: // 1
        cout << "Format_Mono" << endl;
        break;
    case QImage::Format_MonoLSB: // 2
        cout << "Format_MonoLSB" << endl;
        break;
    case QImage::Format_Indexed8: // 3
        cout << "Format_Indexed8" << endl;
        break;
    case QImage::Format_RGB32: // 4
        cout << "Format_RGB32" << endl;
        break;
    case QImage::Format_ARGB32: // 5
        cout << "Format_ARGB32" << endl;
        break;
    default:
        cout << img.format() << endl;
    }

    QSize siz = img.size();
    cout << "Image width  = " << siz.width() << endl;
    cout << "Image height = " << siz.height() << endl;

    return 0;                //a.exec();
}
