#include <Arduino.h>

/* DECLARATIONS*/

/* DEFINES */
int main_fn(void);

/* GLOBALS */


int main_fn()
{

    /* Put Cube Functions here */
    while(1)
    {
        Serial.println("In infinite while loop:");
        delay(1000);
    }

    return 0;
}


/* ONLY USED TO GET TO MAIN AND SETUP SERIAL !!! */
void setup() 
{
    Serial.begin(9600);
    delay(10);
    main_fn();

}

void loop() 
{
    // should never reach here...
    delay(1000);
}