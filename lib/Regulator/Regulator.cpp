#include "Regulator.h"

Regulator::Regulator(String name, regType type, uint8_t v_port, uint8_t hw_port, float *inset) : _name(name),
                                                                                                  _type(type),
                                                                                                  _state(false),
                                                                                                  _setpoint(0),
                                                                                                  _hist(1),
                                                                                                  _v_port(v_port),
                                                                                                  _hw_port(hw_port),
                                                                                                  _inset(inset)
{
}
Regulator::~Regulator()
{
    _inset = NULL;
}
void Regulator::on()
{
    if (!_state)
    {
        _state = true;
        digitalWrite(_hw_port, ON);
        Serial.print(_name);
        Serial.println(" ON");
    }
}
void Regulator::off()
{
    if (_state)
    {
        _state = false;
        digitalWrite(_hw_port, OFF);
        Serial.print(_name);
        Serial.println(" Off");
    }
}
void Regulator::save(Preferences &prefs)
{
    String key = _name + "setpoint";
    prefs.putFloat(key.c_str(), _setpoint);
    key = _name + "hist";
    prefs.putFloat(key.c_str(), _hist);
}
void Regulator::load(Preferences &prefs)
{

    String key = _name + "setpoint";
    _setpoint = prefs.getFloat(key.c_str());
    key = _name + "hist";
    _hist = prefs.getFloat(key.c_str());
}
void Regulator::check()
{
    if (_type == UP)
    {
        if (*_inset < _setpoint - _hist)
            on();
        else if (*_inset >= _setpoint + _hist)
            off();
    }
    if (_type == DOWN)
    {
        if (*_inset >= _setpoint + _hist)
            on();
        else if (*_inset < _setpoint - _hist)
            off();
    }
}