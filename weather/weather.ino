#include "InternetButton/InternetButton.h"
#include "math.h"

InternetButton b = InternetButton();

const char DATA_DELIMITER = '#';
const int LED_BRIGHTNESS = 125;
const int MAX_COLOUR_RGB = 255;

const int COLD_CODES [] = { 25 };
const int RAIN_CODES [] = { 1, 5, 6, 7, 8, 9, 10, 11, 12, 35, 37, 38, 39, 40, 45, 47 };
const int SNOW_CODES [] = { 13, 14, 15, 16, 18, 41, 42, 43, 46 };
const int CLOUDY_CODES [] = { 26 };
const int MOSTLY_CLOUDY_CODES [] = { 27, 28 };
const int PARTLY_CLOUDY_CODES [] = { 29, 30, 44 };
const int SUNNY_CODES [] = { 31, 32, 33, 34 };
const int DANGER_CODES [] = { 0, 2, 3, 4, 17, 23 };
const int CAUTION_CODES [] = { 19, 20, 21, 22, 24, 36 };

const float TEMP_MIN = -10;
const float TEMP_MAX = 30;

const float TEMP_WHITE = 0;
const float TEMP_GREEN = 16;
const float TEMP_YELLOW = 21;

const int TODAY_FORECAST_LED = 1;
const int TODAY_LOW_LED = 2;
const int TODAY_HIGH_LED = 3;
const int TOMORROW_FORECAST_LED = 5;
const int TOMORROW_LOW_LED = 6;
const int TOMORROW_HIGH_LED = 7;
const int AFTER_TOMORROW_FORECAST_LED = 9;
const int AFTER_TOMORROW_LOW_LED = 10;
const int AFTER_TOMORROW_HIGH_LED = 11;

long last_action_time = NULL;
long last_processing_time = NULL;

void setup() {

    b.begin();
    b.setBrightness(LED_BRIGHTNESS);
    
    Particle.subscribe("hook-response/yahoo-weather", processWeather, MY_DEVICES);
}

void processWeather(const char * event, const char * data) {
    
    if (last_processing_time == NULL || millis() - last_processing_time > 5000) {
        last_processing_time = millis();
        b.allLedsOff();
        
        String dataString = String(data);
        
        char strBuffer[125] = "";
        dataString.toCharArray(strBuffer, 125);
        
        int todayForecastCode = atoi(strtok(strBuffer, "\"#"));
        int todayLow = atoi(strtok(NULL, "#"));
        int todayHigh = atoi(strtok(NULL, "#"));
        
        int tomorrowForecastCode = atoi(strtok(NULL, "#"));
        int tomorrowLow = atoi(strtok(NULL, "#"));
        int tomorrowHigh = atoi(strtok(NULL, "#"));
        
        int afterTomorrowForecastCode = atoi(strtok(NULL, "#"));
        int afterTomorrowLow = atoi(strtok(NULL, "#"));
        int afterTomorrowHigh = atoi(strtok(NULL, "#"));
        
        int todayForecastColour [] = { 0, 0, 0 };
        int tomorrowForecastColour [] = { 0, 0, 0 };
        int afterTomorrowForecastColour [] = { 0, 0, 0 };
        
        getForecastColour(todayForecastColour, todayForecastCode);
        getForecastColour(tomorrowForecastColour, tomorrowForecastCode);
        getForecastColour(afterTomorrowForecastColour, afterTomorrowForecastCode);
        
        b.ledOn(TODAY_FORECAST_LED, todayForecastColour[0], todayForecastColour[1], todayForecastColour[2]);
        b.ledOn(TOMORROW_FORECAST_LED, tomorrowForecastColour[0], tomorrowForecastColour[1], tomorrowForecastColour[2]);
        b.ledOn(AFTER_TOMORROW_FORECAST_LED, afterTomorrowForecastColour[0], afterTomorrowForecastColour[1], afterTomorrowForecastColour[2]);
        
        int todayLowColour [] = { 0, 0, 0 };
        int tomorrowLowColour [] = { 0, 0, 0 };
        int afterTomorrowLowColour [] = { 0, 0, 0 };
        
        getTempColour(todayLowColour, todayLow);
        getTempColour(tomorrowLowColour, tomorrowLow);
        getTempColour(afterTomorrowLowColour, afterTomorrowLow);
        
        b.ledOn(TODAY_LOW_LED, todayLowColour[0], todayLowColour[1], todayLowColour[2]);
        b.ledOn(TOMORROW_LOW_LED, tomorrowLowColour[0], tomorrowLowColour[1], tomorrowLowColour[2]);
        b.ledOn(AFTER_TOMORROW_LOW_LED, afterTomorrowLowColour[0], afterTomorrowLowColour[1], afterTomorrowLowColour[2]);
        
        int todayHighColour [] = { 0, 0, 0 };
        int tomorrowHighColour [] = { 0, 0, 0 };
        int afterTomorrowHighColour [] = { 0, 0, 0 };
        
        getTempColour(todayHighColour, todayHigh);
        getTempColour(tomorrowHighColour, tomorrowHigh);
        getTempColour(afterTomorrowHighColour, afterTomorrowHigh);
        
        b.ledOn(TODAY_HIGH_LED, todayHighColour[0], todayHighColour[1], todayHighColour[2]);
        b.ledOn(TOMORROW_HIGH_LED, tomorrowHighColour[0], tomorrowHighColour[1], tomorrowHighColour[2]);
        b.ledOn(AFTER_TOMORROW_HIGH_LED, afterTomorrowHighColour[0], afterTomorrowHighColour[1], afterTomorrowHighColour[2]);
        
        delay(10000);
        b.allLedsOff();
    }
}

void loop() {
    if(b.buttonOn(2) && checkLastAction()){
        last_action_time = millis();
        b.rainbow(10);
        Particle.publish("yahoo-weather", NULL, PRIVATE);
    }
}

bool checkLastAction() {
    return (last_action_time == NULL || millis() - last_action_time > 5000);
}

void getForecastColour(int * colour, int code) {
    
    if (doesArrayContainsValue(COLD_CODES, sizeof(COLD_CODES) / sizeof(int), code)) {
        colour[0] = 255;
        colour[1] = 0;
        colour[2] = 255;
    } else if (doesArrayContainsValue(RAIN_CODES, sizeof(RAIN_CODES) / sizeof(int), code)) {
        colour[0] = 30;
        colour[1] = 144;
        colour[2] = 255;
    } else if (doesArrayContainsValue(SNOW_CODES, sizeof(SNOW_CODES) / sizeof(int), code)) {
        colour[0] = 255;
        colour[1] = 255;
        colour[2] = 255;
    } else if (doesArrayContainsValue(CLOUDY_CODES, sizeof(CLOUDY_CODES) / sizeof(int), code)) {
        colour[0] = 0;
        colour[1] = 128;
        colour[2] = 0;
    } else if (doesArrayContainsValue(MOSTLY_CLOUDY_CODES, sizeof(MOSTLY_CLOUDY_CODES) / sizeof(int), code)) {
        colour[0] = 0;
        colour[1] = 180;
        colour[2] = 0;
    } else if (doesArrayContainsValue(PARTLY_CLOUDY_CODES, sizeof(PARTLY_CLOUDY_CODES) / sizeof(int), code)) {
        colour[0] = 0;
        colour[1] = 255;
        colour[2] = 0;
    } else if (doesArrayContainsValue(SUNNY_CODES, sizeof(SUNNY_CODES) / sizeof(int), code)) {
        colour[0] = 255;
        colour[1] = 255;
        colour[2] = 0;
    } else if (doesArrayContainsValue(CAUTION_CODES, sizeof(CAUTION_CODES) / sizeof(int), code)) {
        colour[0] = 255;
        colour[1] = 140;
        colour[2] = 0;
    } else if (doesArrayContainsValue(DANGER_CODES, sizeof(DANGER_CODES) / sizeof(int), code)) {
        colour[0] = 255;
        colour[1] = 0;
        colour[2] = 0;
    }
}

void getTempColour(int * colour, int temp) {
    // (0, 0, 255)
    if (temp <= TEMP_WHITE) {
        float index = (max(temp, TEMP_MIN) - TEMP_MIN) / (TEMP_WHITE - TEMP_MIN);
        colour[0] = MAX_COLOUR_RGB * index;
        colour[1] = MAX_COLOUR_RGB * index;
        colour[2] = MAX_COLOUR_RGB;
        // (255, 255, 255)
    } else if (temp <= TEMP_GREEN) {
        float index = (temp - TEMP_WHITE) / (TEMP_GREEN - TEMP_WHITE);
        colour[0] = MAX_COLOUR_RGB * (1 - index);
        colour[1] = MAX_COLOUR_RGB;
        colour[2] = MAX_COLOUR_RGB * (1 - index);
        // (0, 255, 0)
    } else if (temp <= TEMP_YELLOW) {
        float index = (temp - TEMP_GREEN) / (TEMP_YELLOW - TEMP_GREEN);
        colour[0] = MAX_COLOUR_RGB * index;
        colour[1] = MAX_COLOUR_RGB;
        colour[2] = 0;
        // (255, 255, 0)
    } else {
        float index = (min(temp, TEMP_MAX) - TEMP_YELLOW) / (TEMP_MAX - TEMP_YELLOW);
        colour[0] = MAX_COLOUR_RGB;
        colour[1] = MAX_COLOUR_RGB * (1 - index);
        colour[2] = 0;
        // (255, 0, 0)
    }
}

bool doesArrayContainsValue(const int * array, int arraySize, int value) {
    int index = 0;
    while (index < arraySize) {
        if (array[index] == value) {
            return true;
        }
        index++;
    }
    return false;
}
