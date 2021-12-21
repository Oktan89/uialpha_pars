#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <map>
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

    QMenuBar *menubar;
    QMenu *menu;

    void thredParseStart();
    void startLoop();
    void stopLoop();

public:
    alphaWindow(std::shared_ptr<IBaseParser> pars);
    ~alphaWindow();
public slots:
    void pushStart();
    void pushStop();

};

class hBox : public QGridLayout
{
    Q_OBJECT
 std::shared_ptr<Database> _data;
    std::map<int, QPushButton *> _databutton;
public:
    hBox(std::shared_ptr<Database> data, QWidget *parent = nullptr);

   void addAskueObject(int key);
   void updateAskueObject(int key);
   
public slots:
    void setNewObject(int key);
    void updateObject(int key);
signals:
    void signalSetObject(int key);
    void signalUpdateObject(int key);
private:
    QPushButton* setStatusPollAskueObject(QPushButton *button, const ObjectAskue& object);
    int x = 2;
    int y = 0;
};