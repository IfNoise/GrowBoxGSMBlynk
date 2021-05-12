#ifndef dailyscheduler_h
#define dailyscheduler_h
/*
/
/
/
/
/
/
*/
#define ON_LOW
#include <Arduino.h>
#include <Preferences.h>
#include <RTClib.h>
#if defined(ON_HIGH)
  #define ON HIGH
  #define OFF LOW
#else
  #define ON LOW
  #define OFF HIGH
#endif 
//===On Off Pin Daily Scheduler
struct SchedulerItem //On Interval setpoint data structure
{
    int startMin;
    int stopMin;
};
//+++Base class on off scheduler
class Scheduler
{
protected:
    SchedulerItem *_items = NULL; //Setpoints array pointer
    String _name;
    uint8_t _size;
    bool _state = false;
    bool _manual = true;
    uint8_t _v_port;
    uint8_t _hw_port;
    uint8_t _counter = 0;
    void on();
    void off();

public:
    Scheduler(String name, uint8_t size,uint8_t v_port, uint8_t hw_port);
    ~Scheduler();
    bool addItem(int start, int stop);
    SchedulerItem getItem(int i);
    void clear();
    void check(int curmin);
    void setManual() { _manual = true; }
    void setAuto() { _manual = false; }
    bool getState() { return _state; }
    uint8_t getCount(){return _counter;}
    uint8_t v_port(){return _v_port;}
    void manOn();
    void manOff();
    void save(Preferences &pref);
    void load(Preferences &pref);
};

class IrrigationScheduler : public Scheduler
{
private:
    int _startmin;
    int _stopmin;
    uint8_t _irrNumber;
    uint8_t _irrWindow;

public:
    IrrigationScheduler(String name, uint8_t size,uint8_t v_port, uint8_t hw_pin);
    ~IrrigationScheduler();
    void setStartStop(int start, int stop);
    void setIrrNumber(int8_t num) { _irrNumber = num; }
    void setIrrWindow(int8_t win) { _irrWindow = win; }
    void update();
};


#endif