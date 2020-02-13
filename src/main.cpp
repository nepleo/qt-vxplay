#include "xplay.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "xdemux.h"
#include "xdecode.h"
#include "xvideowidget.h"
#include "resample.h"
#include "xaudioplay.h"
#include "xaudiothread.h"
#include "xvideothread.h"
#include "xdemuxthread.h"
#include <QThread>
using namespace std;

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	xplay w;
	w.show();
	//char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";   //ºþÄÏTV
	return a.exec();
}
