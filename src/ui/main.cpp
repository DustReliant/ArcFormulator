//#include "QArcMainWindow.h"
#include "mainwindow.h"
#include <ribbon/qdfribbonstyle.h>
#include <QApplication>
int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    //app.setStyle(new QdfRibbonStyle());
    //app.setApplicationName("RibbonDemo");
    //app.setOrganizationName("qtcanpool");
    //QArcMainWindow w;
    MainWindow w;
    w.show();
    return app.exec();
}
