#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include "logreader.h"
#include "threadsafe_queue.h"
#include "logparser.h"
#include "database.h"


class alphaWindow : public QWidget
{
    Q_OBJECT
    Logreader logreader;
    std::thread run;
    std::shared_ptr<threadsafe_queue<std::string>> loopaskue;
    std::shared_ptr<IBaseParser> pars;
    void thredParseStart();
    void startLoop();
    void stopLoop();

public:
    alphaWindow(std::shared_ptr<IBaseParser> pars) : pars(pars){}
    ~alphaWindow();
public slots:
    void pushStart();
    void pushStop();

};

class hBox : public QHBoxLayout
{
    Q_OBJECT
 std::shared_ptr<Database> _data;
public:
    hBox(std::shared_ptr<Database> data, QWidget *parent = nullptr);

   void pushAskueObject(int key);

};