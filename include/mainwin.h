#pragma once
#include <QWidget>
#include "logreader.h"
#include "threadsafe_queue.h"
#include "logparser.h"
#include "database.h"
#include "pcout.h" // thread safe cout

class alphaWindow : public QWidget
{
    Q_OBJECT
    Logreader logreader;
    std::shared_ptr<threadsafe_queue<std::string>> loopaskue;
    std::shared_ptr<IBaseParser> pars;
    void thredParseStart();
    void startLoop();
    void stopLoop();

public:
    alphaWindow(std::shared_ptr<IBaseParser> pars) : pars(pars){}
public slots:
    void pushStart();
    void pushStop();

};
