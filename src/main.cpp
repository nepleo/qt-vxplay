#include "xplay.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xplay w;
    w.show();
    // char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";
    return a.exec();
}
