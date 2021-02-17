#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* DECLARATIONS*/
int main_fn(void);
void LCD_Task(int);
void openGPIO(void);
int scan_I2C_Devices();

/* DEFINES */

/* GLOBALS */
LiquidCrystal_I2C lcd(0x27, 16, 2);

int main_fn()
{
    int loopCounter = 0;
    openGPIO();
    // scan_I2C_Devices();

    /* Put Cube Functions here */
    while(1)
    {
        Serial.println("In infinite while loop:");
        LCD_Task(loopCounter);
        loopCounter++;
        delay(1000);
    }

    return 0;
}

void LCD_Task(int count)
{
    lcd.setCursor(1,0);
    lcd.print("Round: ");
    lcd.print(count);
    lcd.setCursor(0,1);
    lcd.print("Bottom line: ");
    lcd.print(count);
}

void openGPIO()
{
    // Opens the wire library to be used for I2C control of the LCD display
    Wire.begin();
    lcd.init();
    lcd.backlight();
}

int scan_I2C_Devices()
{
    Serial.println("Scanning for I2C Devices");
    byte error, address;
    int deviceCount = 0;

    for(address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if(error == 0)
        {
            Serial.println("I2C device found at address: 0x");
            if(address < 16)
                Serial.print("0");
            Serial.print(address,HEX);
            deviceCount++;
        }
        else if(error = 4)
        {
           Serial.print("Unknown error at address 0x");
           if (address<16)
           Serial.print("0");
           Serial.println(address,HEX); 
        }
    }
    if(deviceCount == 0)
    {
        Serial.println("No I2C Devices detected:");
    }
    else
        Serial.println("Done scanning:");

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