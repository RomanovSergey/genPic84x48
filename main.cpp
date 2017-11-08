/*
 * Программа genPic84x48, реализовано на Qt 5.6.2
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
 *     |      |||||||||
 *     |       (.) (.)
 *     |     q    x    p
 *     |        <mmm>
 *     |          V
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
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "compressor.h"

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

    const int DISP_X = 84;
    const int DISP_Y = 48;
    uint8_t coor[DISP_X][DISP_Y / 8]; // буфер дисплея 84x48 пикселей (1 бит на пиксель)
    QString picName; // имя файла с изображением
    QString outName; // имя выходного файла - результата работы программы

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
    picName = parser.value( optFileName );
    if ( picName.isEmpty() ) {
        cout << "Please set picture's file name (-i FileName)" << endl;
        return -1;
    }
    cout << "Picture name is: " << endl << "\t" << picName.toStdString() << endl;
    outName = parser.value( optOutName );
    cout << "Output name is: " << endl << "\t" << outName.toStdString() << endl;


    QImage img( picName );
    if ( img.isNull() ) {
        cout << "Error: Can't load image file: " << picName.toStdString() << endl;
        cout << "Exit" << endl;
        return -1;
    }
    cout << "Format image: " << endl << "\t";
    switch ( img.format() ) {
    case QImage::Format_Invalid: // 0
        cout << "Error: Format_Invalid" << endl;
        return -1;
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
    if ( siz.width() != DISP_X ) {
        cout << "Error: image width must be 84 pixels" << endl;
        return -1;
    }
    if ( siz.height() != DISP_Y ) {
        cout << "Error: image height must be 48 pixels" << endl;
        return -1;
    }
    // Сформируем массив coor[x][yb]
    QRgb pix;
    uint8_t yByte = 0;
    for ( int x = 0; x < DISP_X; x++ ) {
        for ( int yb = 0; yb < DISP_Y / 8; yb++ ) {
            for ( int bit = 0; bit < 8; bit++ ) {
                pix = img.pixel( x, yb*8 + bit );
                if ( qGray( pix ) < 127 ) {
                    yByte |= (1 << bit);
                }
            }
            coor[x][yb] = yByte;
            yByte = 0;
        }
    }

    // Выведим картинку в косноль в текстовом режиме
    for ( int yb = 0; yb < DISP_Y / 8; yb++ ) {
        for ( int bit = 0; bit < 8; bit++ ) {
            for ( int x = 0; x < DISP_X; x++ ) {
                if ( coor[x][yb] & (1<<bit) ) {
                    cout << "##";
                } else {
                    cout << "  ";
                }
            }
            cout << endl;
        }
    }
    cout << endl;

    // Сформируем текстовое представление массива coor[][] на языке Си
    QString code = QString("\nconst uint8_t img[%1][%2] = {\n").arg(DISP_X).arg(DISP_Y / 8);
    for ( int x = 0; x < DISP_X; x++ ) {
        code += QString("  { ");
        for ( int y = 0; y < DISP_Y / 8; y++ ) {
            code += QString("0x%1, ").arg( coor[x][y], 2, 16, QLatin1Char( '0' ) );
        }
        code += QString("},\n");
    }
    code += QString("};\n\n");
    //cout << code.toStdString() << endl;

    // COMPRESSSSORRRR
    uint8_t opic[84*6 + 10];
    int ret;
    int owrited;
    ret = compressImg84x48( (uint8_t*)coor, opic, &owrited, sizeof(opic) );
    if ( ret < 0 ) {
        cout << "Error compress image, ret = " << ret << endl;
    }
    code += QString("const uint8_t imgCompressed[%1] = {\n    ").arg(owrited);
    for ( int n = 0; n < owrited; n++ ) {
        code += QString("0x%1, ").arg( opic[n], 2, 16, QLatin1Char( '0' ) );
        if ( n%6 == 0 ) {
            code += QString("\n    ");
        }
    }
    code += QString("};\n");
    cout << code.toStdString() << endl;

    // Сохраним текстовое содержимое code в файл
    QFile file( outName );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Error: Can't open file (" << outName.toStdString() << ")" << endl;
        return -1;
    }
    QTextStream outputStream(&file);
    //code.replace("\n","\r\n");
    outputStream << code;
    file.close();

    cout << "Done" << endl;
    return 0;
}
