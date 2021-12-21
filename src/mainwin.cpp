#include <QPushButton>
#include "mainwin.h"
#include <thread>

void alphaWindow::startLoop()
{
    loopaskue = std::make_shared<threadsafe_queue<std::string>>();
    logreader.intit(false);
    std::string dataparse;
    if(std::filesystem::exists(logreader.getPatch()))
    {
        for(auto run = logreader.start(loopaskue, 500); run ; run = logreader.status())
        {
           loopaskue->wait_and_pop(dataparse);
           if(dataparse != "[exit logreader]")
           {
               pars->parse(dataparse);
           }
        }
    }
    else
    {
        pcout{} << "File not found" << logreader.getPatch() <<"\n";
    }
    pcout{} << "[threadParserUI] stop... OK\n";
}

void alphaWindow::stopLoop()
{
    logreader.stop();
}

void alphaWindow::thredParseStart()
{
    run = std::thread(&alphaWindow::startLoop, this);
    pcout{} << "[threadParserUI" << run.get_id() << "] start... OK\n";
    run.detach();
}

void alphaWindow::pushStart()
{
    thredParseStart();
}

void alphaWindow::pushStop()
{
    stopLoop();
}

alphaWindow::~alphaWindow()
{

}

hBox::hBox(std::shared_ptr<Database> data, QWidget *parent) :  QHBoxLayout(parent), _data(data)
{
    QObject::connect(this, &hBox::signalSetObject, this, &hBox::setNewObject);
    QObject::connect(this, &hBox::signalUpdateObject, this, &hBox::updateObject);
}

void hBox::addAskueObject(int key)
{
    emit signalSetObject(key);
}

void hBox::updateAskueObject(int key)
{
    emit signalUpdateObject(key);
}

void hBox::setNewObject(int key)
{
    auto object(_data->getObject(key));
    auto button = new QPushButton(object.getName().c_str());
    addWidget(button);
}

void hBox::updateObject(int key)
{
    pcout{} << "update" << key << "\n";
}