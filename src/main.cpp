#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <iostream>
#include <string>
#include <filesystem>
#ifdef _WIN32
#include "consolecp.h"
#endif
#include "threadsafe_queue.h"
#include "logreader.h"
#include "logparser.h"
#include "database.h"
#include "pcout.h" // thread safe cout
#include "mainwin.h"

int main(int argc, char** argv)
{
    #ifdef _WIN32
        ConsoleCP console(1251);
    #elif __linux__
       
    #endif

    std::shared_ptr<Database> data = std::make_shared<Database>();  
    std::shared_ptr<IBaseParser> pars = std::make_shared<ParseLogSrv>(data);

    QApplication app(argc, argv);
    auto  window = new alphaWindow(pars);
    auto hboxlayout = new hBox(data, window);
    data->setBox(hboxlayout);
    auto buttonStart = new QPushButton("start");
    auto buttonStop = new QPushButton("stop");
    hboxlayout->addWidget(buttonStart);
    hboxlayout->addWidget(buttonStop);
   
    QWidget::connect(buttonStart, &QPushButton::clicked, window, &alphaWindow::pushStart);
    QWidget::connect(buttonStop, &QPushButton::clicked, window, &alphaWindow::pushStop);
   
    window->show();   
    
    return app.exec();
}