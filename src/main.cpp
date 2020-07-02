/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief Smart Door
 * @version 0.1
 * @date 2020-04-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Arduino.h>
#include "AntaresESP8266HTTP.h"

#define ACCESSKEY "your-access-key"
#define WIFISSID "your-wifi-ssid"
#define PASSWORD "your-wifi-password"

#define projectName "smart-door"
#define deviceDoorState "door-state"
#define deviceSwitchStatus "switch-status"

#define ALARM D7 // Alarm pin on D7
#define MAGNET D4 // Sensor pin on D4

AntaresESP8266HTTP antares(ACCESSKEY);

String doorState;
bool currentState = false;
bool lastState = false;

void setup()
{
    // Serial communication for debugging purposes
    Serial.begin(115200);

    // Alarm pin set as output
    pinMode(ALARM, OUTPUT);
    digitalWrite(ALARM, LOW);

    // Button pin set as input
    pinMode(MAGNET, INPUT_PULLUP);

    // Antares connection setup
    antares.setDebug(true);
    antares.wifiConnection(WIFISSID,PASSWORD);
}

void loop()
{
    // Read magnetic sensor
	int readSensor = digitalRead(MAGNET);
    Serial.println(readSensor);

    if (readSensor == HIGH)
    {
        Serial.println("OPEN");
        doorState = "open";
        currentState = true;
    }
    else
    {
        Serial.println("CLOSED");
        doorState = "closed";
        currentState = false;
    }

    // Send data when door state changed
    if (currentState != lastState)
    {
        // Send data to Antares
        antares.add("doorState", doorState);
        antares.send(projectName, deviceDoorState);
        lastState = currentState;
    }

    // Get switch status from Antares
    antares.get(projectName, deviceSwitchStatus);
    if (antares.getSuccess())
    {
        int status = antares.getInt("status");

        // Alarm ON only when status = 1 and door open
        if (status == 1 && doorState == "open")
        {
            digitalWrite(ALARM, HIGH);
        }

        // Alarm OFF when status = 0
        else if (status == 0)
        {
            digitalWrite(ALARM, LOW);
        }
    }

    delay(5000);
}
