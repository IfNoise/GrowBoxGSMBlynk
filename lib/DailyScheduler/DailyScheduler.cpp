#include "DailyScheduler.h"

Scheduler::Scheduler(String name, uint8_t size, uint8_t v_port, uint8_t hw_port) : _name(name), _size(size), _v_port(v_port), _hw_port(hw_port)
{
    _items = new SchedulerItem[size];
}

Scheduler::~Scheduler()
{
    delete[] _items;
    _items = NULL;
    _counter = 0;
    _size = 0;
}
bool Scheduler::addItem(int start, int stop)
{
    if (_counter < _size)
    {
        _items[_counter].startMin = start;
        _items[_counter].stopMin = stop;
        ++_counter;
        return true;
    }
    else
    {
        Serial.println();
        return false;
    }
}
void Scheduler::clear()
{
    delete[] _items;
    _items = NULL;
    _counter = 0;
    _items = new SchedulerItem[_size];
}
void Scheduler::on()
{
    _state = true;
    digitalWrite(_hw_port, ON);
    Serial.print(_name);
    Serial.print(" Timer");
    Serial.println(" ON");
}
SchedulerItem Scheduler::getItem(int i)
{
    if (i < _counter && (_items != NULL))
        return _items[i];
}
void Scheduler::off()
{
    _state = false;
    digitalWrite(_hw_port, OFF);
    Serial.print(_name);
    Serial.print(" Timer");
    Serial.println(" Off");
}
void Scheduler::manOn()
{
    if (_manual)
    {
        if (!_state)
            on();
    }
}
void Scheduler::manOff()
{
    if (_manual)
    {
        if (_state)
            off();
    }
}

void Scheduler::save(Preferences &prefs)
{
    Serial.println("Saving " + _name + "timer items");
    String key = String(_name + "c");
    prefs.putUChar(key.c_str(), _counter);
    key = String(_name + "m");
    prefs.putBool(key.c_str(), _manual);
    for (uint8_t i = 0; i < _counter; i++)
    {
        key = String(_name);
        key += i;
        prefs.putBytes(key.c_str(), &_items[i], sizeof(SchedulerItem));
        Serial.print("Save item ");
        Serial.println(i);
    }
}
void Scheduler::load(Preferences &prefs)
{
    String key = String(_name) + "c";
    uint8_t counter = prefs.getUChar(key.c_str());
    Serial.print("counter=");
    Serial.println(counter);
    key = String(_name + "m");
    _manual = prefs.getBool(key.c_str());
    Serial.println("Loading " + _name + "timer items");
    for (uint8_t i = 0; i < counter; i++)
    {

        key = String(_name);
        key += i;
        size_t schLen = prefs.getBytesLength(key.c_str());
        char buffer[schLen]; // prepare a buffer for the data
        prefs.getBytes(key.c_str(), buffer, schLen);
        if (schLen % sizeof(SchedulerItem))
        { // simple check that data fits
            log_e("Data is not correct size!");
        }
        SchedulerItem *tmp = (SchedulerItem *)buffer;
        Serial.print("Load item ");
        Serial.println(i);
        Serial.println(tmp->startMin);
        Serial.println(tmp->stopMin);
        addItem(tmp->startMin, tmp->stopMin);
    }
}
void Scheduler::check(int curmin)
{
    bool prevState = _state;
    if (!_manual)
    {
        for (int i = 0; i < _counter; i++)

        {
            _state = _items[i].startMin > _items[i].stopMin ? ((curmin >= _items[i].startMin) || (curmin < _items[i].stopMin)) : ((curmin >= _items[i].startMin) && (curmin < _items[i].stopMin));

            if (_state)
            {

                break;
            }
        }
    }
    if (prevState != _state)
    {
        if (_state)
        {
            on();
        }
        else
        {
            off();
        }
    }
}

IrrigationScheduler::IrrigationScheduler(String name, uint8_t size, uint8_t v_port, uint8_t hw_pin) : Scheduler(name, size, v_port, hw_pin), _startmin(0), _stopmin(1200), _irrNumber(1), _irrWindow(2)
{
}

IrrigationScheduler::~IrrigationScheduler()
{
}
void IrrigationScheduler::setStartStop(int start, int stop)
{
    _startmin = start;
    _stopmin = stop;
}
void IrrigationScheduler::update()
{
    int begin, end, step;
    begin = _startmin - _irrWindow;
    end = _stopmin + _irrWindow;
    if (begin > end)
    {
        step = _irrNumber < 2 ? (1440 - begin + end) / 2 : (1440 - begin + end) / (_irrNumber - 1);
    }
    else
    {

        step = _irrNumber < 2 ? (end - begin) / 2 : (end - begin) / (_irrNumber - 1);
    }

    clear();
    for (int i = 0; i < _irrNumber; ++i)
    {
        int start = begin + step * i;
        if (start > 1440)
            start -= 1440;
        int stop = start + _irrWindow;
        if (stop > 1440)
            stop -= 1440;
        addItem(start, stop);
    }
}