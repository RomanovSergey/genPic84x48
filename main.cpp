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
                                 "input file name (*.png) <name>.",
                                 "name");
    parser.addOption( optFileName );

    QCommandLineOption optOutName( QStringList() << "o" << "output",
                                 "output file name (*.c) <name>.",
                                 "name");
    parser.addOption( optOutName );

    // Process the actual command line arguments given by the user
    parser.process( a );

    const QStringList args = parser.positionalArguments();
    cout << "args size = " << args.size() << endl;
    for ( int i = 0; i < args.size(); i++ ) { // for debug purpose
        cout << "args[" << i << "]=" << args.at(i).toStdString() << endl;
    }

    QString picName = parser.value( optFileName );
    cout << "picture name is: " << picName.toStdString() << endl;
    QString outName = parser.value( optOutName );
    cout << "output name is: " << outName.toStdString() << endl;


    cout << endl;
    cout << "Exit" << endl;  //a.quit(); // х.з. как выйти
    return 0;                //a.exec();
}
