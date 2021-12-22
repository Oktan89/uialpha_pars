#include <QPushButton>
#include <QTextCodec>
#include <algorithm>
#include "mainwin.h"
#include <thread>

alphaWindow::alphaWindow(std::shared_ptr<IBaseParser> pars) : pars(pars)
{
   start = new QAction("Start", this);
   stop = new QAction("Stop", this);
   menu = menuBar()->addMenu("Menu");
   menu->addAction(start);
   menu->addAction(stop);
   connect(start, &QAction::triggered, this, &alphaWindow::pushStart);
   connect(stop, &QAction::triggered, this, &alphaWindow::pushStop);

}

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

hBox::hBox(std::shared_ptr<Database> data, QWidget *parent) :  QGridLayout(parent), _data(data)
{
    QObject::connect(this, &hBox::signalSetObject, this, &hBox::setNewObject);
    QObject::connect(this, &hBox::signalUpdateObject, this, &hBox::updateObject);
    //_data->setBox(this);
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
    auto object = _data->getObject(key);
    auto button = new QPushButton;//(encodeWin1251ToUTF(object.getName()));

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    button->setSizePolicy(sizePolicy);

    addWidget(setStatusPollAskueObject(button, object), y, x);
    _databutton[object.getId()] = button;
    (x < 10 )? (++x) : (++y, x = 0); 
}

void hBox::updateObject(int key)
{
    auto itbut = _databutton.find(key);
    auto itobj = _data->getObject(key);
    _databutton[key] = setStatusPollAskueObject(itbut->second, itobj);
}

QPushButton* hBox::setStatusPollAskueObject(QPushButton *button, const ObjectAskue& object)
{
    button->setText(encodeWin1251ToUTF(object.getName()));
  
    auto meter = object.getPollMeter();
    switch (meter.status)
    {
    case STATUSPOLL::POLL_OK :
        button->setStyleSheet("QPushButton { background-color: green;}\n");
        break;
    case STATUSPOLL::POLL_ERROR :
        button->setStyleSheet("QPushButton { background-color: red;}\n");
        break;
    default:
        button->setStyleSheet("QPushButton { background-color: gray;}\n");
        break;
    }
   /* switch(object.getStatus())
    {
        case STATUSOBJECT::START_POLL:
        button->setStyleSheet("QPushButton { background-color: green;}\n");
        break;
        case STATUSOBJECT::STOP_POLL:
        button->setStyleSheet("QPushButton { background-color: red;}\n");
        break;
        case STATUSOBJECT::WAIT_START_POLL:
        button->setStyleSheet("QPushButton { background-color: yellow;}\n");
        break;
        default:
        button->setStyleSheet("QPushButton { background-color: gray;}\n");
        break;
    }*/
    return  button;
}

QString hBox::encodeWin1251ToUTF(const std::string& string)
{
    QByteArray encodedString = string.c_str();
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    return codec->toUnicode(encodedString);
}