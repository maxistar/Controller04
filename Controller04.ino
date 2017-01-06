#include "SimpleSwitcher.h"
#include "ModbusRtu.h"
#include "LedDimmer.h"
#include "Switcher.h"
#include "Dht11Sensor.h"
#include "Ds18b20Sensor.h"

#include <OneWire.h>
#include <EEPROM.h>

/**
 * 1 - 1 word - registers to read
 * 1.0 light switch shower room 
 * 1.1 door sensor shower room
 * 1.2 shower water sensor 
 * 1.3 door sensor kitchen room
 * 1.4 PIR sensor shower room
 * 
 * 2 - 1 word - registers to write/read
 * 2.0 light shower room
 * 2.1 fan shower room
 * 2.2 warm floor heater
 * 2.3 warm floor near door
 * 
 * 3 - 5 words
 * 3.1 temperature in shower room 
 * 3.2 humidity
 * 3.3 temperature of warm floor in shower room
 * 3.4 temperature of warm floor near main entry door
 * 3.5 count of modbus errors
 * 
 * 4 - 6 words hold registers
 * 4.1 - light preset num
 * 4.2 - maximum pwd value W1
 * 4.3 - maximum pwd value W2
 * 4.4 - maximum pwd value R
 * 4.5 - maximum pwd value G
 * 4.6 - maximum pwd value B
 * 
 */

// номер пина, к которому подсоединен датчик температуры
#define TEMP_SENSOR_PIN_SHOWER 4 
#define TEMP_SENSOR_PIN_SHOWER_FLOOR 8
#define TEMP_SENSOR_PIN_ENTRY_FLOOR 11
#define SHOWER_DOOR_SWITCH A4
#define SHOWER_LIGHT_SWITCH A5
#define SHOWER_WATER_SENSOR A1
#define KITCHEN_DOOR_SWITCH A0
#define PIR_SENSOR 12
#define RGB_R 6
#define RGB_G 9
#define RGB_B 10
#define W1_PIN 3
#define W2_PIN 5
#define LED_POWER_PIN 7

// temperature read interval
//once per minute will be ok
#define TEMP_READ_INTERVAL 10000

// адрес ведомого
#define ID   4

#define txControlPin  0 
#define PRESETS_COUNT 5


struct LightsState {
  byte w1;
  byte w2;
  byte r;
  byte g;
  byte b;
};

struct LightsPresets {
  byte presetNum;
  LightsState presets[PRESETS_COUNT];
};

// массив данных modbus
#define MODBUS_SIZE 13 
uint16_t modbus[MODBUS_SIZE];
int8_t state = 0;

//кнопка
LedDimmer ld(SHOWER_LIGHT_SWITCH, W1_PIN, LED_POWER_PIN, modbus, 1, 1, 9);

Switcher sw1(SHOWER_DOOR_SWITCH); //геркон на дверь в душ

Modbus slave(ID, 0, txControlPin);


OneWire ds(TEMP_SENSOR_PIN_ENTRY_FLOOR); // на пине 10 (нужен резистор 4.7 КОм)

//sample temperature sensor on bredboard
byte addr[8] = {0x28,0x96,0xB0,0xAC,0x05,0x00,0x00,0xDC};

void sw1_change(char value) {
    //write to modbus changed state
    bitWrite(modbus[0], 1, !value);

    if (value == 0 && ld.isOff()) {
        ld.on();
    }
}

void dht1TemperatureChange(float value) {
    modbus[2] = (uint16_t) value;
}

void dht1HumidityChange(float value) {
    modbus[3] = (uint16_t) value;
}

void dht2TemperatureChange(float value) {
    modbus[4] = (uint16_t) value;
}

void dht2HumidityChange(float value) {
    //modbus[4] = (uint16_t) value;
}

void floorTemperatureChange(float value) {
    modbus[5] = (uint16_t) (value * 100);
}


Dht11Sensor dht1(TEMP_SENSOR_PIN_SHOWER, TEMP_READ_INTERVAL);
Dht11Sensor dht2(TEMP_SENSOR_PIN_SHOWER_FLOOR, TEMP_READ_INTERVAL);
Ds18b20Sensor ds19b20(&ds, addr, TEMP_READ_INTERVAL);

void io_poll() {
  bitWrite(modbus[0], 0, digitalRead( SHOWER_LIGHT_SWITCH ));
  bitWrite(modbus[0], 2, digitalRead( SHOWER_WATER_SENSOR ));
  bitWrite(modbus[0], 3, digitalRead( KITCHEN_DOOR_SWITCH ));
  bitWrite(modbus[0], 4, digitalRead( PIR_SENSOR ));
  
  modbus[6] = slave.getErrCnt();
}

void setup() 
{
  
  pinMode(PIR_SENSOR, INPUT);
  
  pinMode(SHOWER_DOOR_SWITCH, INPUT_PULLUP);
  pinMode(KITCHEN_DOOR_SWITCH, INPUT_PULLUP);
  pinMode(SHOWER_WATER_SENSOR, INPUT_PULLUP);
  
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  analogWrite(RGB_R, 255);
  analogWrite(RGB_G, 255);
  analogWrite(RGB_B, 255);

  dht1.onTemperatureChanged(dht1TemperatureChange);
  dht1.onHumidityChanged(dht1HumidityChange);
  dht1.setup();

  dht2.onTemperatureChanged(dht2TemperatureChange);
  dht2.setup();  
  
  ld.setup();

  sw1.setChangeCallback(sw1_change);
  sw1.setup();
  
  int i;
  //Serial.begin(9600);
  ds19b20.onTemperatureChanged(floorTemperatureChange);
  
  slave.begin( 19200 );
}
 
void loop() 
{
  
  state = slave.poll(modbus, MODBUS_SIZE);
  ld.loop();
  sw1.loop();
  dht1.loop();
  dht2.loop();
  
  ds19b20.loop();
  
  //обновляем данные в регистрах Modbus и в пользовательской программе
  io_poll();
}
