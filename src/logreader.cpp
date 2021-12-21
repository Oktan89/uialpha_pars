#include <iostream>
#include <thread>
#include <cassert>
#include "logreader.h"
#include "getdate.h"

#define DEBUG


Logreader::Logreader(const std::filesystem::path &path) noexcept :
 _savepos(0), _file{}, _path(path), _status(Logerstatus::LOG_FILE_STOP), _log_thread{},
 run(false), run_auto_set(false), _m_locfilepatch{}, _getdate(Getdate::GetObjectDate()){}

void Logreader::intit(bool autopath)
{
    //Проверяем название файла и устанаваливаем правильное в соостветсвии с датой
    if(autopath)
        setNewfileDependingCurdate(_path);
    //Открываем файл на чтение с позицией курсора на конец файла
    _file.open(_path, std::ios::binary | std::ios::ate);

    if (_file.is_open())
    {
        #ifdef DEBUG
            pcout{} << "[Logreader "<<this<<"]: Check file open... OK\n";
        #endif
        //Если файл открыт, сохраняем позицию курсора конца файла
        _savepos = _file.tellg();
        _file.close();
        _status = Logerstatus::LOG_FILE_OK;
        //Запускаем отдельный поток для коррекции имени фала лога в 00:00:01 ночи
        if (run_auto_set)
        {
            #ifdef DEBUG
            pcout{} << "[Thread Autopath]: the stream is already running\n";
            #endif
        }
        else
        {
            std::thread autopath = std::thread(&Logreader::autoSetNewfileDependingCurdate, this);
            if (autopath.joinable())
            {
                autopath.detach();
                run_auto_set = true;
            }
            else
            {
                #ifdef DEBUG
                pcout{} << "[Thread Autopath " << autopath.get_id() << "]: Thread...error start\n";
                #endif
            }
        }
    }
    else
    {
        #ifdef DEBUG
            pcout{} << "[Logreader "<<this<<"]: Check file open... ERROR\n";
        #endif
        _status = Logerstatus::LOG_FILE_ERROR;
    }
}

bool Logreader::start(std::shared_ptr<threadsafe_queue<std::string>> queue, const int32_t timer_ms)
{
    if(Logerstatus::LOG_FILE_ERROR == _status)
    {
        #ifdef DEBUG
            pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]: start false\n";
        #endif
        run = false;
        return run;
    }
    
    run = true;
    assert(!_log_thread.joinable());
    
    auto t = std::thread(&Logreader::thred_log_read, this, queue, timer_ms);
    _log_thread.swap(t);
    _status = Logerstatus::LOG_FILE_RUN;
    
    return run;
}

void Logreader::thred_log_read(std::shared_ptr<threadsafe_queue<std::string>> queue, int64_t timer_ms)
{
    #ifdef DEBUG
        pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]: Start... OK\n";
    #endif
    while (run)
    {
        _file.open(_path, std::ios::binary | std::ios::ate);

            if (_file.is_open())
            {
                _status = Logerstatus::LOG_FILE_RUN;
                //читаем новую позицию курсора в конце файла
                auto gnew = _file.tellg();
                //если сохраненая позиция меньше то нужно прочитать новую запись в отслеживаемом файле
                if (_savepos < gnew)
                {
                    auto temp = gnew; //сохраняем во временную переменную позицию конца файла
                    gnew -= _savepos; //вычисляем колличество символов с последнего окрытия файла
                    std::string strbuff(static_cast<std::size_t>(gnew), '\0'); //создаем буфер для чтения вычесленного коллиества символов
                    _file.seekg(_savepos);//переводим курсор на ранее сохраненное место конца файла
                    _file.read(&strbuff[0], gnew);//и читаем с этого места колличество вычисленных символов
                    //pcout{} << strbuff;//что то делаем с информацией!!!!!
                    queue->push(strbuff);
                    /*#ifdef DEBUG
                        pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]:<-------------------------->\n";
                    #endif*/
                    _savepos = temp; //сохраняем новую позицию конца файла
                }
                else
                {
                    _savepos = gnew; //Для случая если файл был усечен, сохраняем новою позицию конца файла
                }
                _file.close();
            }
            else
            {
                if(_status != Logerstatus::LOG_FILE_ERROR)
                {
                    #ifdef DEBUG
                        pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]: The file "<<_path <<" may have been renamed \n";
                    #endif
                    _status = Logerstatus::LOG_FILE_ERROR;
                }
                
            }
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }
    _status = Logerstatus::LOG_FILE_STOP;
    queue->end_push();
}

void Logreader::stop()
{
    run = false;
    
    if(_log_thread.joinable())
    {
        #ifdef DEBUG
            pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]: stop... OK\n";
        #endif
        _log_thread.join();
        _status = Logerstatus::LOG_FILE_STOP;
    }
}

Logreader::~Logreader()
{
    stop();
    #ifdef DEBUG
            pcout{} << "[Logreader "<<this<<"]: ~Delete... OK\n";
    #endif
}

bool Logreader::status() const noexcept
{
    return run;
}

std::string Logreader::status_s() const noexcept
{
    switch(_status)
    {
        case Logerstatus::LOG_FILE_ERROR :
            return "Error";
        break;
        case Logerstatus::LOG_FILE_OK :
            return "Ok";
        break;
        case Logerstatus::LOG_FILE_RUN :
            return "Run";
        break;
        case Logerstatus::LOG_FILE_STOP :
            return "Stop";
        break;
        default:
            return "Unknown status";
    }
}

std::filesystem::path Logreader::getPatch() const noexcept
{
    return _path;
}

void Logreader::autoSetNewfileDependingCurdate()
{
    //Запуск потока в опредлееное время!!!!!
  
    #ifdef DEBUG
        pcout{} << "[Thread Autopath "<<this<<"]: Start... OK\n";
    #endif

    while(true)
    {
        std::tm tm = _getdate->getStructTmTimeNow();//Текущее время
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 1;
        tm.tm_mday += 1; //устанавливаем сработку на следуюзий день в 00:00:01
  
        auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm)); //конвертируем tm в time_point
        std::this_thread::sleep_until(tp); //Засыпаем до установленного времени
        setNewfileDependingCurdate(_path); //меняем название файла
    }
}

//Если сменилась дата то меняем название файла
void Logreader::setNewfileDependingCurdate(const std::filesystem::path &oldpatch)
{
    AlphacentrPatch patch_name;
    std::string file_name = _getdate->getdate_time({'y', 'm', 'd'});
    std::filesystem::path newpath{patch_name.logsrv + file_name + patch_name.extension};
    if(oldpatch != newpath)
    {
        std::lock_guard<std::mutex> loc(_m_locfilepatch);
        _path.swap(newpath);
        #ifdef DEBUG
            pcout{} << "[Thread Logreader "<<_log_thread.get_id()<<"]: Set new file name "<< _path<<"\n";
        #endif
    }
}