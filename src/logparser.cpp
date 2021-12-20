#include <vector>
#include <cstring>
#include "logparser.h"


std::ostream& operator<<(std::ostream& out, const Time_stamp& time)
{
    out << time.day<<"/"<<time.mon<<"/"<<time.year<<" "<<time.hour<<":"<<time.min<<":"<<time.sec; 
    return out;
}


void ParseLogSrv::parse(const std::string& log)
{
    if(log.empty())
        return;
    if (splitRecord(log, _record, protocol.head))
    {
        for (const auto &br : _record)
        {
            ObjectAskue askue;
           
            if(auto [ok, size] = is_pollingPoints(br); ok)
            {
                askue.setId(getId(br, size));
                
                if(auto [ok, name_s] = getName(br); ok)
                {
                     askue.setName(name_s);
                } else {pcout{} << "[ParserLogSvr] Error get name\n"; break;}

                Time_stamp timestamp;
                if(auto [ok, time_f] = findTime(br); ok)
                {
                    timestamp = convertFindTime(time_f);
                }else {pcout{} << "[ParserLogSvr] Error find time start-stop\n"; break;}
                
                if(auto [ok, status_poll] = pollingStatusStartStop(br); ok)
                {
                    if(status_poll == STATUSOBJECT::START_POLL)
                    {
                        askue.setTime(status_poll, timestamp);
                        
                        if(auto [ok, port] = getPort(br); ok)
                        {
                            askue.setInterface(port);
                        }else {pcout{} << "[ParserLogSvr] Error get port\n"; break;}
                    }
                    else if(status_poll == STATUSOBJECT::STOP_POLL)
                    {
                        askue.setTime(status_poll, timestamp);
                        std::vector<std::string> err;
                        if(splitRecord(br, err, protocol.poll))
                        {
                            ObjectPolling meter;
                            for(const auto &e : err)
                            {
                                if(auto [ok, pollmeter] = is_PollOKError(e); ok)
                                {
                                   meter.meter.push_back(pollmeter);
                                   
                                }else {pcout{} << "[ParserLogSvr] Error parse status poll meter\n"; break;}
                            }
                            getStatusPollMeter(meter);
                            askue.setPollMeter(meter);                      
                        }else {pcout{} << "[ParserLogSvr] Error Not find info status poll\n";}
                    }
                    else if(status_poll == STATUSOBJECT::UNKNOWN)
                    {
                        askue.setTime(status_poll, timestamp);
                    }
                }else {pcout{} << "[ParserLogSvr] Error status start-stop\n"; break;}
                _data->setObject(askue);
            }
            else if(auto [ok, size] = is_pointsPolling(br); ok)
            {
                askue.setId(getId(br, size));
                if(auto [ok, s_time] = findTime(br); ok)
                {
                    auto timestamp = convertFindTime(s_time);
                    askue.setTime(STATUSOBJECT::WAIT_START_POLL, timestamp);
                }else {pcout{} << "[ParserLogSvr] Error find time next poll\n"; break;}
                _data->setObject(askue);
            }
            else
            {
                pcout{} << "[ParserLogSvr] String not parse... start\n";
                pcout{} << br <<"\n";
                pcout{} << "[ParserLogSvr] String not parse... end\n";
                //�������� �� ������ ��������� �� ����� � �����
            }
            
        }
    }else {pcout{} << "[ParserLogSvr] Error Not find head log string\n";}
    _record.clear();
}

//��������� ��� �������� �������� ������ � ���� ���� ���� ���� �� ���������� ����� ������ ������� ERROR
STATUSPOLL getStatusPollMeter(ObjectPolling& out_meter)
{
    STATUSPOLL status(STATUSPOLL::POLL_OK);
    for(const auto &m : out_meter.meter)
    {
        if(!m.status_poll)
            status = STATUSPOLL::POLL_ERROR;
    }
    out_meter.status = status;
    return status;
}
// ������ ����� ����� ��������� �������
std::pair<bool, Meter> ParseLogSrv::is_PollOKError(const std::string& log) const
{
    const char s_ok[] = "�������";
    const char s_err[] = "� ��������";
    Meter meter;
    std::size_t start_index = 0;
    std::size_t end_index = 0;
    int countdig = 0;
    bool status = false;
    for(std::size_t i = 0, ok = 0, err = 0; i < log.size(); ++i)
    {
        if(s_ok[ok] == log[i])
        {
           ++ok;
        }
        else
        {
            if(ok == std::strlen(s_ok))
            {
                meter.status_poll = true;
                status = true;
            }
            ok = 0;
        }
        if(s_err[err] == log[i])
        {
            ++err;
        }
        else
        {
            if(err == std::strlen(s_err))
            {   
                meter.status_poll =false;
                status = true;
            }
            err = 0;
        }
        if(isdigit(log[i]))
        {
            if(start_index == 0)
            {
                start_index = i;
                ++countdig;
                 if (countdig == 2)
                        meter.repit_poll = std::stoi(log.substr(start_index));
                status = true;
            }
        }
        else
        {
            if(start_index != 0)
            {
                try
                {
                    end_index = i;
                    if (countdig == 1)
                        meter.id = std::stoi(log.substr(start_index, end_index - start_index));
                   
                    start_index = 0;
                    end_index = 0;
                }
                catch(...)
                {
                    pcout{} <<"Error stoi\n";
                   status = false;
                }
            }
        }
    }
    return std::make_pair(status, meter);
}
//�������� ������ {����� �����}?
std::pair<bool, std::size_t> ParseLogSrv::is_pollingPoints(const std::string& log) const
{   
    std::size_t pos = log.find(protocol.poll_p);
    if(log.npos != pos)
        return std::make_pair(true, (++pos)+std::strlen(protocol.poll_p));
    return std::make_pair(false, pos);
}

//�������� ������ {����� ������}?
std::pair<bool, std::size_t> ParseLogSrv::is_pointsPolling(const std::string &log) const
{
    std::size_t pos = log.find(protocol.p_poll);
    if (log.npos != pos)
        return std::make_pair(true, (++pos)+std::strlen(protocol.p_poll));
    return std::make_pair(false, pos);
}

//�������� ������ {�����}?
std::pair<bool, std::size_t> ParseLogSrv::is_Polling(const std::string &log) const
{
    std::size_t pos = log.find(protocol.poll);
    if (log.npos != pos)
        return std::make_pair(true, (++pos)+std::strlen(protocol.poll));
    return std::make_pair(false, pos);
}

//�������� ������ {������� ��� ����������}?
std::pair<bool, STATUSOBJECT> ParseLogSrv::pollingStatusStartStop(const std::string& log, std::size_t pos) const
{
    std::size_t pos_s = log.find(protocol.status_start, pos);
        if(log.npos != pos_s)
        {
            return std::make_pair(true, STATUSOBJECT::START_POLL);
        }
    pos_s = log.find(protocol.status_stop, pos);
        if(log.npos != pos_s)
        {
            return std::make_pair(true, STATUSOBJECT::STOP_POLL);
        }
    return std::make_pair(false, STATUSOBJECT::UNKNOWN);
}

//��� ����� COM ��� TCP?
std::pair<bool, Interface> ParseLogSrv::getPort(const std::string& log, std::size_t pos) const
{
    Interface port;
    std::size_t pos_s = log.find(protocol.portCom, pos);
        if(log.npos != pos_s)
        {
            port.type = INTERFACETYPE::COM;
            port.number = std::stoi(log.substr(pos_s+std::strlen(protocol.portCom)));
            return std::make_pair(true, port);
        }
    pos_s = log.find(protocol.postTcp, pos);
        if(log.npos != pos_s)
        {
            port.type = INTERFACETYPE::TCP;
            port.number = -1;
            return std::make_pair(true, port);
        }

    return std::make_pair(false, port);
}
//����� ������� ���� ������ true � ������ �� ������� � ��������
std::pair<bool, const std::string> ParseLogSrv::findTime(const std::string& log) const
{
    std::size_t pos = log.find(protocol.head);
    if (log.npos != pos)
    {
        std::size_t pos_start = log.find(protocol.sbL);
        std::size_t pos_end =log.find(protocol.sbR, pos_start);
        if (pos_start == log.npos)
        {
            pos_start = log.find(protocol.marks);
            pos_end = log.find(protocol.marks, pos_start);
        }
        return std::make_pair(true, log.substr(pos_start + 1, pos_end - pos_start - 1));
    }
    return std::make_pair(false, log);
}

int ParseLogSrv::getId(const std::string& log, std::size_t pos) const
{
    std::size_t pos_end = log.find_first_of(' ', pos);
    if(pos_end == log.npos)
        return -1;
    return std::stoi(log.substr(pos, pos_end - pos ));
}

std::pair<bool, const std::string> ParseLogSrv::getName(const std::string& log) const
{
    std::size_t pos_start = log.find_first_of(protocol.rbL);
    std::size_t pos_end = log.find_last_of(protocol.rbR);
    
    if(pos_start != log.npos || pos_end != log.npos)
    {
        ++pos_start;
        return std::make_pair(true, log.substr(pos_start, pos_end-pos_start));
    }
        
    return std::make_pair(false, log);
}

Time_stamp ParseLogSrv::convertFindTime(const std::string& time) const
{
    Time_stamp ts{};
    try
    {
        ts.day = std::stoi(time.substr(0, 2));
        ts.mon = std::stoi(time.substr(3, 2));
        ts.year = std::stoi(time.substr(6, 4));
        ts.hour = std::stoi(time.substr(11, 2));
        ts.min = std::stoi(time.substr(14, 2));
        ts.sec = std::stoi(time.substr(17, 2));
    }
    catch (const std::exception &e)
    {
        std::cerr <<"[ParserLogSvr] convert find time :" << e.what() << '\n';
    }

    return ts;
}

bool ParseLogSrv::splitRecord(const std::string& log, std::vector<std::string> &record, const char* head)
{
    //���� ������ ��������� ***
    std::size_t start_pos = log.find(head);
    //���� �����
    while (start_pos != log.npos)
    {
        //���� ���������
        std::size_t end_pos = log.find(head, start_pos + std::strlen(head));
        //���� ������ ��� �� ���������� ��� ������ �� start
        if (end_pos == log.npos)
        {
            record.push_back(log.substr(start_pos));
            //� ��������� ����
            start_pos = end_pos;
        }
        //���� ������� ��������� ��������� ***
        else
        {
            //���������� ������ �� start �� end -1
           record.push_back(log.substr(start_pos, end_pos - start_pos));
            //������ ��������� �������� ��� ������ *** �� end
           start_pos = end_pos;
        }
    }
    //For debug
    /* for(const auto& l : _record)
        pcout{} << l;*/
    return !record.empty();
}

void ObjectAskue::setId(int id)
{
    std::lock_guard<std::mutex> lg(_mutex);
    _id = id;
}

int ObjectAskue::getId() const
{
    std::lock_guard<std::mutex> lg(_mutex);
    return _id;
}

void ObjectAskue::setName(const std::string& name)
{
    std::lock_guard<std::mutex> lg(_mutex);
    _name_point = name;
}

std::string ObjectAskue::getName() const
{
    std::lock_guard<std::mutex> lg(_mutex);
    return _name_point;
}

void ObjectAskue::setTime(const STATUSOBJECT status, const Time_stamp& time)
{
    std::lock_guard<std::mutex> lg(_mutex);

    switch (status)
    {
    case STATUSOBJECT::START_POLL:
        _time.start_pool = time;
        break;
    case STATUSOBJECT::STOP_POLL :
        _time.end_pool = time;
        break;
    
    case STATUSOBJECT::WAIT_START_POLL :
        _time.next_pool = time;
        break;
    
    case STATUSOBJECT::UNKNOWN :
        pcout{} <<"set time unknown\n";
        break;
    
    default:
        pcout{} <<"set time error\n";
        _status = STATUSOBJECT::UNKNOWN;
        break;
    }

    _status = status;
}

Time_stamp ObjectAskue::getStatusTime() const
{
    std::lock_guard<std::mutex> lg(_mutex);
    Time_stamp time{};

    switch (_status)
    {
    case STATUSOBJECT::START_POLL:
        time = _time.start_pool;
        break;
    case STATUSOBJECT::STOP_POLL :
        time = _time.end_pool;
        break;
    
    case STATUSOBJECT::WAIT_START_POLL :
        time = _time.next_pool;
        break;
    
    case STATUSOBJECT::UNKNOWN :
   
    default:
        pcout{} <<"get time error\n";
        break;
    }
    return time;
}

void ObjectAskue::setInterface(const Interface& port)
{
    _interface = port;
}

Interface ObjectAskue::getInterface() const
{
    return _interface;
}

std::string ObjectAskue::getInetrface_s() const
{
    std::string type;
    switch (_interface.type)
    {
    case INTERFACETYPE::COM: 
        type = "COM" + std::to_string(_interface.number);
        break;
    case INTERFACETYPE::TCP:
        type = "TCP";
        break;
    case INTERFACETYPE::NONE:
    default:
        type = "unknown";
        break;
    }
    return type;
}

STATUSOBJECT ObjectAskue::getStatus() const
{
    return _status;
}

std::string ObjectAskue::getStatus_s() const
{
    std::string status;
    switch (_status)
    {
    case STATUSOBJECT::START_POLL:
        status = "����� �������";
        break;
    case STATUSOBJECT::STOP_POLL:
        status = "����� ����������";
        break;
    case STATUSOBJECT::WAIT_START_POLL:
         status = "��������� �����";
        break;
    case STATUSOBJECT::UNKNOWN:
    default:
        status = "unknown";
        break;
    }
    return status;
}

void ObjectAskue::setPollMeter(const ObjectPolling& meter)
{
    _pollmeter = meter;
}

ObjectPolling ObjectAskue::getPollMeter() const
{
    return _pollmeter;
}



ObjectAskue::ObjectAskue(const ObjectAskue& object) :
 _id(object._id), _name_point(object._name_point), _interface(object._interface),
 _time(object._time), _status(object._status), _pollmeter(object._pollmeter)
{

}

ObjectAskue& ObjectAskue::operator=(const ObjectAskue& other)
{
    if(this == &other)
        return *this;
    if(_id == other._id)
    {
        if(_name_point == "unknown")
            _name_point = other._name_point;
    }
    else
    {
        _id = other._id;
        _name_point = other._name_point;
    }
    _interface = other._interface;
    _time = other._time;
    _status = other._status;
    _pollmeter = other._pollmeter;
    return *this;
}