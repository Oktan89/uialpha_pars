#include "mainwin.h"
#include <thread>

void alphaWindow::startLoop()
{
    loopaskue = std::make_shared<threadsafe_queue<std::string>>();
    logreader.intit();
    std::string dataparse;
    if(std::filesystem::exists(logreader.getPatch()))
    {
        for(auto run = logreader.start(loopaskue, 500); run ; run = logreader.status())
        {
           loopaskue->wait_and_pop(dataparse);
           pars->parse(dataparse);
        }
        pcout{} << "Log reader " << logreader.status_s() << "\n";
    }
    else
    {
        pcout{} << "Log reader " << logreader.status_s() << "\n";
    }
}

void alphaWindow::stopLoop()
{
    logreader.stop();
    pcout{} << "Log reader " << logreader.status_s() << "\n";
}

void alphaWindow::thredParseStart()
{
    std::thread run(&alphaWindow::startLoop, this);
    run.detach();///Косяк !!!! удалить процесс
}

void alphaWindow::pushStart()
{
    thredParseStart();
}

void alphaWindow::pushStop()
{
    stopLoop();
}