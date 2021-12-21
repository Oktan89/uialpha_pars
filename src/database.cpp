#include "database.h"
#include "logparser.h"
#include "mainwin.h"

std::string meterStatus_s(const ObjectPolling& meter);

void Database::setBox(hBox *box)
{
    _box = box;
}

void Database::setObject(ObjectAskue& askue)
{
    std::lock_guard<std::mutex> lg(_mx_map);
    auto it = _db.find(askue.getId());
    if (it != _db.end())
    {
        std::swap(it->second, askue); //�������� ����� ���������� � ������������ ������������
        pcout{} << "������ ��������\n";
        showAskue(it->second);
        _box->updateAskueObject(it->second.getId());
    }
    else
    {
        auto obj = _db[askue.getId()] = askue;
        pcout{} << "������ ��������\n";
        showAskue(obj);
        _box->addAskueObject(obj.getId());
    }
}

ObjectAskue &Database::getObject(int key)
{
    return _db[key];
}

void showAskue(const ObjectAskue& askue)
{
    pcout{} <<"[--------------------]\n";
    pcout{} <<"[ID  : "<< askue.getId() <<" ]\n";
    pcout{} <<"[NAME: "<< askue.getName()<<" ]\n";
    pcout{} <<"[POLL STATUS: "<< askue.getStatus_s()<<" ";
    auto meter = askue.getPollMeter();
    switch (askue.getStatus())
    {
        case STATUSOBJECT::START_POLL:
            pcout{} <<"[TIME: "<< askue.getStatusTime()<<" ]\n";
            pcout{} <<"[PORT: "<< askue.getInetrface_s()<<" ]\n";
            break;
        case STATUSOBJECT::STOP_POLL:
            pcout{} <<"[TIME: "<< askue.getStatusTime()<<" ]\n";
            pcout{} <<"[METER STATUS: "<< meterStatus_s(meter)<<" ]\n";
             for(const auto &m: meter.meter)
                {
                    if(!m.status_poll)
                    {
                        pcout{} << "    Meter: " << m.id <<" error, repit " << m.repit_poll <<"\n";
                    }
                }
            break;
        case STATUSOBJECT::WAIT_START_POLL:
            pcout{} <<"[TIME: "<< askue.getStatusTime()<<" ]\n";
            break;
        default:
            break;
    }
    pcout{} <<"[--------------------]\n";
}

std::string meterStatus_s(const ObjectPolling& meter)
{
    switch (meter.status)
    {
    case STATUSPOLL::POLL_OK :
        return "OK";
    case STATUSPOLL::POLL_ERROR :
        return "ERROR";
    default:

        return "UNKNOWN";
    }
}