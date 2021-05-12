#ifndef regulator_h
#define regulator_h
/* 
Simple  regulator for physical process control



*/
#include <Arduino.h>
#include <Preferences.h>
#define ON_LOW
#if defined(ON_HIGH)
  #define ON HIGH
  #define OFF LOW
#else
  #define ON LOW
  #define OFF HIGH
#endif 
enum regType
{
    UP,
    DOWN
}; //Positive or negative histeresis

class Regulator
{
protected:
    String _name;
    regType _type;
    bool _state;
    float _setpoint = 0;
    float _hist = 1;
    uint8_t _v_port;
    uint8_t _hw_port;
    float *_inset;
    void on();
    void off();

public:
    Regulator(String name, regType type, uint8_t v_port, uint8_t hw_port, float *inset);
    ~Regulator();
    void setSetpoint(float sp) { _setpoint = sp; }
    void setHist(float h) { _hist = h; }
    void check();
    uint8_t v_port(){return _v_port;}
    bool getState() { return _state; }
    void save(Preferences &pref);
    void load(Preferences &pref);
};
typedef Regulator Termostat; 
#endif