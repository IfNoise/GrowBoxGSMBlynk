/*



*/

#ifndef functions_h
#define functions_h

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WidgetRTC.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <Preferences.h>
#include <DailyScheduler.h>
#include <Regulator.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <Wire.h>

#include "SparkFunBME280.h"
//===========================Sensors===========================================================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BME280 BloomSens;
float temp, hum, pres;
//=============================RTC ================================================================
BlynkTimer timer;
RTC_DS3231 Hrtc;
DateTime now_;
WidgetRTC rtc;

//========================Settings store=======================================
Preferences prefs;
//=======================================MODEM=======================================================
#define SerialAT Serial1
TinyGsm modem(SerialAT);
int bat_mv = 0;
bool modemConnected;
//===================Timers==========================================================================
Scheduler BloomLightTimer("BLTimer", 1, V6, BLOOM_LIGTH);
Scheduler GrowLightTimer("GLTimer", 1, V3, GROW_LIGTH);
IrrigationScheduler BloomIrrTimer("BIrrTimer", 24, V14, BLOOM_PUMP);
//IrrigationScheduler GrowIrrTimer("GIrrTimer", 8, V9, GROW_PUMP);
//===================Blynk Widgets===================================================================
WidgetTerminal terminal(V61);
//WidgetTable GIrrTable;
//BLYNK_ATTACH_WIDGET(GIrrTable, V65);
WidgetTable BIrrTable;
BLYNK_ATTACH_WIDGET(BIrrTable, V66);
//==============================Config store========================================================
struct ConfigStore
{
    uint32_t magic;
    char version[9];
    uint8_t flagConfig : 1;
    uint8_t flagApFail : 1;
    uint8_t flagSelfTest : 1;

    char apn[34];
    char user[10];
    char pass[10];
    char cloudToken[34];
    char cloudHost[34];
    uint16_t cloudPort;

    uint16_t checksum;
} __attribute__((packed));

ConfigStore configStore;

const ConfigStore configDefault = {
    0x626C6E6B,
    BOARD_FIRMWARE_VERSION,
    0, 0, 0,

    "",
    "",
    "",

    "invalid token",
    "blynk-cloud.com", 80,
    0};
void config_load()
{
    memset(&configStore, 0, sizeof(configStore));
    prefs.getBytes("config", &configStore, sizeof(configStore));
    if (configStore.magic != configDefault.magic)
    {
        DEBUG_PRINT("Using default config.");
        configStore = configDefault;
        return;
    }
}

bool config_save()
{
    prefs.putBytes("config", &configStore, sizeof(configStore));
    return true;
}

void enterResetConfig()
{
    DEBUG_PRINT("Resetting configuration!");
    configStore = configDefault;
    config_save();
    BlynkState::set(MODE_WAIT_CONFIG);
}

template <typename T, int size>
void CopyString(const String &s, T (&arr)[size])
{
    s.toCharArray(arr, size);
}

//====================ds18b20 Data and settings======================================================
struct Sensor
{
    char name[16];
    DeviceAddress addres;
    int v_pin;
    float data;
};
//==============================Sensors Data Array===================================================
Sensor Sensors[NUMSENSORS] =
    {
        {"Bloom Air 2",
         {0x28, 0xAE, 0x69, 0xA0, 0x12, 0x19, 0x01, 0x1F},
         V52,
         0},         
         {"Bloom Heater",
         {0x28, 0xFF, 0x4A, 0xFB, 0x67, 0x14, 0x03, 0x13},
         V54,
         0},
         {"Grow Air",
         {0x28, 0xFF, 0x4A, 0xFB, 0x67, 0x14, 0x03, 0x13},
         V53,
         0},
         {"Bloom Air",
         {0x28, 0xFF, 0x4A, 0xFB, 0x67, 0x14, 0x03, 0x13},
         V51,
         0}

};
//===============================Termostats==========================================================

Termostat BloomHeater("BHeater", UP, V31, BLOOM_HEATER_PIN, &Sensors[1].data);
//Termostat GrowHeater("GHeater", UP, V22, GROW_HEATER_PIN, &Sensors[3].data);
Termostat BloomFan("BFan", DOWN, V25, BLOOM_FAN_PIN, &Sensors[3].data);
Termostat BloomFanSpeed("BFanSpd", DOWN, V28, BlOOM_FAN_SPEED_PIN, &Sensors[0].data);
//Termostat GrowFan("GFan", DOWN, V19, GROW_FAN_PIN, &Sensors[1].data);
//================================Humidity regulator==================================================
//Termostat HumReg("Humidity", DOWN, V19, BLOOM_FAN_PIN, &hum);
//===============================Hardware timer======================================================
/*
hw_timer_t *h_timer = NULL;
//volatile SemaphoreHandle_t= timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;



void restartMCU()
{
    esp_task_wdt_init(1, true);
    esp_task_wdt_add(NULL);
    while (true)
        ;
}*/
void syncRTC()
{
    if (Blynk.connected())
    {
        Hrtc.adjust(DateTime(year(), month(),day(), hour(), minute(), second()));
        Serial.println("RTC sync OK");
    }
}
//==============================================Modem functions==========================================

void GSM_ON(uint32_t time_delay)
{
    // Set-up modem reset, enable, power pins
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);

    Serial.println("MODEM_RST & IP5306 IRQ: HIGH"); // IP5306 HIGH
    digitalWrite(MODEM_RST, HIGH);
    delay(time_delay);

    Serial.println("MODEM_PWKEY: HIGH");
    digitalWrite(MODEM_PWKEY, HIGH); // turning modem OFF
    delay(time_delay);

    Serial.println("MODEM_POWER_ON: HIGH");
    digitalWrite(MODEM_POWER_ON, HIGH); //Enabling SY8089 4V4 for SIM800 (crashing when in battery)
    delay(time_delay);

    Serial.println("MODEM_PWKEY: LOW");
    digitalWrite(MODEM_PWKEY, LOW); // turning modem ON
    delay(time_delay);
}

void GSM_OFF()
{
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);

    digitalWrite(MODEM_PWKEY, HIGH);   // turn of modem in case its ON from previous state
    digitalWrite(MODEM_POWER_ON, LOW); // turn of modem psu in case its from previous state
    digitalWrite(MODEM_RST, HIGH);     // Keep IRQ high ? (or not to save power?)
}
void shutdown()
{
    Serial.println(F("GPRS disconnect"));
    modem.gprsDisconnect();

    Serial.println("Radio off");
    modem.radioOff();

    Serial.println("GSM power off");
    GSM_OFF();

    // adc_power_off(); //soposed to save power ?
}
bool setPowerBoostKeepOn(int en)
{
    Wire1.beginTransmission(IP5306_ADDR);
    Wire1.write(IP5306_REG_SYS_CTL0);
    if (en)
    {
        Wire1.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
    }
    else
    {
        Wire1.write(0x35); // 0x37 is default reg value
    }
    return Wire1.endTransmission() == 0;
}
//=============================Light timers==========================================================
void checkTimers()
{
    now_ = Hrtc.now();
    int curmin = now_.hour() * 60 + now_.minute();
    BloomLightTimer.check(curmin);
    GrowLightTimer.check(curmin);
    BloomIrrTimer.check(curmin);
    //GrowIrrTimer.check(curmin);
    Blynk.virtualWrite(BloomLightTimer.v_port(), BloomLightTimer.getState());
    Blynk.virtualWrite(GrowLightTimer.v_port(), GrowLightTimer.getState());
    Blynk.virtualWrite(BloomIrrTimer.v_port(), BloomIrrTimer.getState());
    //Blynk.virtualWrite(GrowIrrTimer.v_port(), GrowIrrTimer.getState());

}

//=================================Termostat functions===============================================

void checkTermos()
{
    BloomHeater.check();
    Blynk.virtualWrite(BloomHeater.v_port(), BloomHeater.getState());
    //GrowHeater.check();
    //Blynk.virtualWrite(GrowHeater.v_port(), GrowHeater.getState());
    //GrowFan.check();
    //Blynk.virtualWrite(GrowFan.v_port(), GrowFan.getState());
    BloomFan.check();
    Blynk.virtualWrite(BloomFan.v_port(), BloomFan.getState());
    BloomFanSpeed.check();
    Blynk.virtualWrite(BloomFanSpeed.v_port(), BloomFanSpeed.getState());
   // HumReg.check();
    //Blynk.virtualWrite(V19, HumReg.getState());
}
//=================================Irrigation Table Functions=======================================
String StarttoStr(int start)
{
    int startH, startM;
    startH = start / 60;
    startM = start - startH * 60;
    return (String(startH) + ":" + String(startM));
}
String StoptoStr(int stop)
{

    int stopH, stopM;
    stopH = stop / 60;
    stopM = stop - stopH * 60;
    return (String(stopH) + ":" + String(stopM));
}
void updateIrrTable(WidgetTable &tbl, IrrigationScheduler &tmr)
{
    tbl.clear();
    for (int i = 0; i < tmr.getCount(); i++)
    {
        tbl.addRow(i, StarttoStr(tmr.getItem(i).startMin), StoptoStr(tmr.getItem(i).stopMin));
    }
}
//===============================================================================

void serialtimecode()
{
    Serial.print("[");
    Serial.print(now_.hour(), DEC);
    Serial.print(':');
    Serial.print(now_.minute(), DEC);
    Serial.print(':');
    Serial.print(now_.second(), DEC);
    Serial.print("]");
}

void clockDisplay()
{
    String currentTime = String(now_.hour()) + ":" + now_.minute() + ":" + now_.second();
    String currentDate = String(now_.day()) + " " + now_.month() + " " + now_.year();
    Serial.print("Current time: ");
    Serial.print(currentTime);
    Serial.print(" ");
    Serial.println(currentDate);

    // Send time to the App
    Blynk.virtualWrite(V62, currentTime);
    // Send date to the App
    Blynk.virtualWrite(V63, currentDate);
}
void sensorsInit()
{
    //======================DS18B20===========================
    sensors.begin();
    delay(1000);
    for (int i = 0; i < NUMSENSORS; i++)
    {
        if (!sensors.getAddress(Sensors[i].addres, i))
        {
            DEBUG_PRINT("Unable to find");
            DEBUG_PRINT(Sensors[i].name);
        }
        else
        {
            sensors.setResolution(Sensors[i].addres, TEMPERATURE_PRECISION);
        }
    }
    //======================BME280======================================
    BloomSens.setI2CAddress(0x76);
    if (BloomSens.beginI2C(Wire) == false)
    {
        DEBUG_PRINT("BME280 Sensor  connect failed");
    }else DEBUG_PRINT("BME280 Sensor  connected");
   
}
void printSensors()
{
    serialtimecode();
    for (int i = 0; i < NUMSENSORS; i++)
    {
        Serial.print(Sensors[i].name);
        Serial.print(" temp is ");
        Serial.println(Sensors[i].data);
    }
    Serial.print("Humidity");
    Serial.println(hum);
    Serial.print("Pressure");
    Serial.println(pres);
}
void getSensors()
{
    sensors.requestTemperatures();
    for (int i = 0; i < NUMSENSORS; i++)
    {
        float tmp_temp = sensors.getTempC(Sensors[i].addres);
        if ((int)tmp_temp != -127)
        {
            Sensors[i].data = tmp_temp;
            DEBUG_PRINT(Sensors[i].name);
            DEBUG_PRINT("temp is");
            DEBUG_PRINT(Sensors[i].data);
        }
        //else
        //{

        //}
    }
    hum = BloomSens.readFloatHumidity();
    temp = BloomSens.readTempC();
    pres = BloomSens.readFloatPressure();
}
void sendSensor()
{
    if (Blynk.connected())
    { //ds18b20
        for (int i = 0; i < NUMSENSORS; i++)
        {
            Blynk.virtualWrite(Sensors[i].v_pin, Sensors[i].data);
            //Serial.print(Sensors[i].name);
            //Serial.println(Sensors[i].data);
        }
        //bme280
       Blynk.virtualWrite(V70, hum);
        Blynk.virtualWrite(V71, temp);
        Blynk.virtualWrite(V72, pres);
        Serial.println("Temp is sending");
    }
}

//================================Init Functions================================================
void preferenceInit()
{
    prefs.begin("settings", false);
    if (!prefs.getBool("first", false))
    { //if first load,then save default settings
        GrowLightTimer.addItem(0, 60);
        GrowLightTimer.save(prefs);
        //GrowIrrTimer.save(prefs);
        BloomLightTimer.addItem(0, 60);
        BloomLightTimer.save(prefs);
        BloomIrrTimer.save(prefs);
        BloomHeater.save(prefs);
        //GrowHeater.save(prefs);
        BloomFan.save(prefs);
        BloomFanSpeed.save(prefs);
        //HumReg.save(prefs);
        config_save();
        prefs.putBool("first", true);
        DEBUG_PRINT("Defaults saved");
    }
    else
    { //load saved settings
        GrowLightTimer.load(prefs);
        BloomLightTimer.load(prefs);
        //GrowIrrTimer.load(prefs);
        BloomIrrTimer.load(prefs);
        BloomHeater.load(prefs);
        //GrowHeater.load(prefs);
        BloomFan.load(prefs);
        BloomFanSpeed.load(prefs);
        //HumReg.load(prefs);
        config_load();
        DEBUG_PRINT("Setting Loaded ");
    }
}

void gpioInit()
{
    pinMode(GROW_LIGTH, OUTPUT);
    pinMode(BLOOM_LIGTH, OUTPUT);
    digitalWrite(BLOOM_LIGTH, HIGH);
    //pinMode(GROW_HEATER_PIN, OUTPUT);
    pinMode(BLOOM_HEATER_PIN, OUTPUT);
    digitalWrite(BLOOM_HEATER_PIN, HIGH);
    //pinMode(GROW_FAN_PIN, OUTPUT);
    pinMode(BLOOM_FAN_PIN, OUTPUT);
    digitalWrite(BLOOM_FAN_PIN, HIGH);
    pinMode(BLOOM_PUMP, OUTPUT);
    digitalWrite(BLOOM_PUMP, HIGH);
    pinMode(BlOOM_FAN_SPEED_PIN, OUTPUT);
    digitalWrite(BlOOM_FAN_SPEED_PIN, HIGH);
}
void tcall_init()
{
    if(Wire1.begin(I2C_SDA, I2C_SCL, 400000))Serial.println("I2C 1 init OK");
    if(Wire.begin(I2C_SDA_2, I2C_SCL_2, 100000))Serial.println("I2C 2 init OK");
    Hrtc.begin();
    GSM_ON(100);
    delay(100);
    bool isOk = setPowerBoostKeepOn(1);
    Serial.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

    
    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);
}

//==================================Grow Light Timer =============================

BLYNK_WRITE(V1)
{
    TimeInputParam t(param);

    // Process start time
    int startMin = 0, stopMin = 0;
    if (t.hasStartTime())
    {
        Serial.println(String("Start: ") +
                       t.getStartHour() + ":" +
                       t.getStartMinute() + ":" +
                       t.getStartSecond());
        startMin = t.getStartHour() * 60 + t.getStartMinute();
    }
    if (t.hasStopTime())
    {
        Serial.println(String("Stop: ") +
                       t.getStopHour() + ":" +
                       t.getStopMinute() + ":" +
                       t.getStopSecond());
        stopMin = t.getStopHour() * 60 + t.getStopMinute();
    }
    GrowLightTimer.clear();
    GrowLightTimer.addItem(startMin, stopMin);
    GrowLightTimer.save(prefs);
    //GrowIrrTimer.setStartStop(startMin, stopMin);
    //GrowIrrTimer.update();
    //updateIrrTable(GIrrTable, GrowIrrTimer);
    //GrowIrrTimer.save(prefs);
    checkTimers();
}
BLYNK_WRITE(V2)
{
    if (param.asInt() == 0)
    {
        GrowLightTimer.setAuto();
        DEBUG_PRINT("GrowLightTimer set Auto mode");
    }
    else
    {
        GrowLightTimer.setManual();
        DEBUG_PRINT("GrowLightTimer set manual mode");
    }

    checkTimers();
}
BLYNK_WRITE(V3)
{
    if (param.asInt() == 0)
    {
        GrowLightTimer.manOff();
    }
    else
    {
        GrowLightTimer.manOn();
    }
    checkTimers();
}

//===================================Bloom light timer===============================
BLYNK_WRITE(V4)
{
    TimeInputParam t(param);

    // Process start time
    int startMin = 0, stopMin = 0;
    if (t.hasStartTime())
    {
        Serial.println(String("Start: ") +
                       t.getStartHour() + ":" +
                       t.getStartMinute() + ":" +
                       t.getStartSecond());
        startMin = t.getStartHour() * 60 + t.getStartMinute();
    }
    if (t.hasStopTime())
    {
        Serial.println(String("Stop: ") +
                       t.getStopHour() + ":" +
                       t.getStopMinute() + ":" +
                       t.getStopSecond());
        stopMin = t.getStopHour() * 60 + t.getStopMinute();
    }

    BloomLightTimer.clear();
    BloomLightTimer.addItem(startMin, stopMin);
    BloomLightTimer.save(prefs);
    BloomIrrTimer.setStartStop(startMin, stopMin);
    BloomIrrTimer.update();
    updateIrrTable(BIrrTable, BloomIrrTimer);
    BloomIrrTimer.save(prefs);
    checkTimers();
}
BLYNK_WRITE(V5)
{
    if (param.asInt() == 0)
    {
        BloomLightTimer.setAuto();
        DEBUG_PRINT("BloomLightTimer set Auto mode");
    }
    else
    {
        BloomLightTimer.setManual();
        DEBUG_PRINT("BloomLightTimer set manual mode");
    }
    checkTimers();
}

BLYNK_WRITE(V6)
{
    if (param.asInt() == 0)
    {
        BloomLightTimer.manOff();
    }
    else
    {
        BloomLightTimer.manOn();
    }
    checkTimers();
}
/*
//==============================GrowIrrigation timer=================================
BLYNK_WRITE(V8)
{
    if (param.asInt() == 0)
    {
        GrowIrrTimer.setAuto();
        DEBUG_PRINT("BloomLightTimer set Auto mode");
    }
    else
    {
        GrowIrrTimer.setManual();
        DEBUG_PRINT("BloomLightTimer set manual mode");
    }
    checkTimers();
}

BLYNK_WRITE(V9)
{
    if (param.asInt() == 0)
    {
        GrowIrrTimer.manOff();
    }
    else
    {
        GrowIrrTimer.manOn();
    }
    checkTimers();
}
BLYNK_WRITE(V10)
{
    GrowIrrTimer.setIrrWindow(param.asInt());
    GrowIrrTimer.update();
    updateIrrTable(GIrrTable, GrowIrrTimer);
    GrowIrrTimer.save(prefs);
    checkTimers();
}
BLYNK_WRITE(V11)
{
    GrowIrrTimer.setIrrNumber(param.asInt());
    GrowIrrTimer.update();
    updateIrrTable(GIrrTable, GrowIrrTimer);
    GrowIrrTimer.save(prefs);
    checkTimers();
}
*/
//==================Bloom Irrigation Timer=============================================
BLYNK_WRITE(V13)
{
    if (param.asInt() == 0)
    {
        BloomIrrTimer.setAuto();
        DEBUG_PRINT("BloomLightTimer set Auto mode");
    }
    else
    {
        BloomIrrTimer.setManual();
        DEBUG_PRINT("BloomLightTimer set manual mode");
    }
    checkTimers();
}
BLYNK_WRITE(V14)
{
    if (param.asInt() == 0)
    {
        BloomIrrTimer.manOff();
    }
    else
    {
        BloomIrrTimer.manOn();
    }
    checkTimers();
}
BLYNK_WRITE(V15)
{
    BloomIrrTimer.setIrrWindow(param.asInt());
    BloomIrrTimer.update();
    updateIrrTable(BIrrTable, BloomIrrTimer);
    BloomIrrTimer.save(prefs);
    checkTimers();
}
BLYNK_WRITE(V16)
{
    BloomIrrTimer.setIrrNumber(param.asInt());
    BloomIrrTimer.update();
    updateIrrTable(BIrrTable, BloomIrrTimer);
    BloomIrrTimer.save(prefs);
    checkTimers();
}
BLYNK_CONNECTED()
{
    // Synchronize time on connection
    rtc.begin();
    Blynk.syncAll();
   // Blynk.setProperty(V103, "labels", "Sensor1", "Sensor2", "Sensor3");
}
/*
//======================Grow Heater==================================
BLYNK_WRITE(V20)
{
    float sp = param.asFloat();
    GrowHeater.setSetpoint(sp);
    GrowHeater.save(prefs);
    GrowHeater.check();
}
BLYNK_WRITE(V21)
{
    float h = param.asFloat();
    GrowHeater.setHist(h);
    GrowHeater.save(prefs);
    GrowHeater.check();
}
//======================Grow Fan=====================================
BLYNK_WRITE(V17)
{
    float sp = param.asFloat();
    GrowFan.setSetpoint(sp);
    GrowFan.save(prefs);
    GrowFan.check();
}
BLYNK_WRITE(V18)
{
    float h = param.asFloat();
    GrowFan.setHist(h);
    GrowFan.save(prefs);
    GrowFan.check();
}
*/
//======================Bloom Heater=================================
BLYNK_WRITE(V29)
{
    float sp = param.asFloat();
    BloomHeater.setSetpoint(sp);
    BloomHeater.save(prefs);
    BloomHeater.check();
}
BLYNK_WRITE(V30)
{
    float h = param.asFloat();
    BloomHeater.setHist(h);
    BloomHeater.save(prefs);
    BloomHeater.check();
}
//======================Bloom Fan====================================
BLYNK_WRITE(V23)
{
    float sp = param.asFloat();
    BloomFan.setSetpoint(sp);
    BloomFan.save(prefs);
    BloomFan.check();
}
BLYNK_WRITE(V24)
{
    float h = param.asFloat();
    BloomFan.setHist(h);
    BloomFan.save(prefs);
    BloomFan.check();
}
//======================Bloom Fan Speed==============================
BLYNK_WRITE(V26)
{
    float sp = param.asFloat();
    BloomFanSpeed.setSetpoint(sp);
    BloomFanSpeed.save(prefs);
    BloomFanSpeed.check();
}
BLYNK_WRITE(V27)
{
    float h = param.asFloat();
    BloomFanSpeed.setHist(h);
    BloomFanSpeed.save(prefs);
    BloomFanSpeed.check();
}
//======================Humidity regulator=============================
/*BLYNK_WRITE(V73)
{
    float sp = param.asFloat();
    HumReg.setSetpoint(sp);
    HumReg.save(prefs);
    HumReg.check();
}
BLYNK_WRITE(V74)
{
    float h = param.asFloat();
    HumReg.setHist(h);
    HumReg.save(prefs);
    HumReg.check();
}*/
#endif