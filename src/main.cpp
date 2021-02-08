#include <Arduino.h>

void setup() 
{
    Serial.begin(9600);
    delay(10);

}

void loop() 
{
    Serial.println("In Loop:");
    printf("Test /n");
    delay(1000);
}