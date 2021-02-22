#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <LedControl.h>

/* DECLARATIONS*/
int main_fn(void);
void LCD_Task(int);
void openGPIO(void);
int scan_I2C_Devices(void);
void JoystickServo(void);
void waveLEDTask(void);
void ButtonBuzzer(void);

/* DEFINES */
#define SERVO_PIN 9
#define GROUND_JOY_PIN A3
#define VOUT_JOY_PIN A2
#define XJOY_PIN A1
#define LED_MATRIX_CLK  13
#define LED_MATRIX_CS   12
#define LED_MATRIX_DIN  11
#define BUTTON_PIN 10
#define BUZZER_PIN 8

/* GLOBALS */
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
LedControl dotMatrix1 = LedControl(LED_MATRIX_DIN, LED_MATRIX_CLK, LED_MATRIX_CS, 0);

// example LED Matrix Image
byte Apple [8]={B00011000,B00001000,B01110110,B11111111,B11111111,B11111111,B01111010,B00110100};


int main_fn()
{
    
    int loopCounter = 0;
    openGPIO();
    scan_I2C_Devices();

    /* Put Cube Functions here */
    while(1)
    {
        Serial.println("In infinite while loop:");
        JoystickServo();
        ButtonBuzzer();

        waveLEDTask();
        LCD_Task(loopCounter);
        loopCounter++;

        delay(100);

    }

    return 0;
}

void waveLEDTask()
{
    for(int i = 0; i < 8; i++)
    {
       dotMatrix1.clearDisplay(0);
       dotMatrix1.setColumn(0,i, B11111111);
       dotMatrix1.setColumn(0,i+1,B01111110);
       dotMatrix1.setColumn(0,i+2,B00111100);
       dotMatrix1.setColumn(0,i+3,B00011000);
    }
    delay(10);
    for(int i = 11; i > 0; i--)
    {
       dotMatrix1.clearDisplay(0);
       dotMatrix1.setColumn(0,i+3, B11111111);
       dotMatrix1.setColumn(0,i+2,B01111110);
       dotMatrix1.setColumn(0,i+1,B00111100);
       dotMatrix1.setColumn(0,i,B00011000);
    }
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

void ButtonBuzzer()
{
    int buttonState = digitalRead(BUTTON_PIN);
    if(buttonState == 1)
  {
    //output a frequency
    digitalWrite(BUZZER_PIN,HIGH);
    delay(200);//wait for 1ms
    digitalWrite(BUZZER_PIN,LOW);
    delay(200);//wait for 1ms
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
  delay(10); //reading delay
     
}


void JoystickServo()
{
    int joyXVal = analogRead(XJOY_PIN);
    // Serial.print(joyXVal);                      //print the value from A1
    // Serial.println(" = input from joystick");  //print "=input from joystick" next to the value
    // Serial.print((joyXVal+520)/10);            //print a from A1 calculated, scaled value
    // Serial.println(" = output to servo");      //print "=output to servo" next to the value
    // Serial.println();
    myservo.write((joyXVal+520)/10); 
}

void openGPIO()
{
    // Opens the wire library to be used for I2C control of the LCD display
    Wire.begin();
    lcd.init();
    lcd.backlight();

    // LED Matrix Setup
    dotMatrix1.shutdown(0, false);
    dotMatrix1.setIntensity(0,0);
    dotMatrix1.clearDisplay(0);

    //Joystick and Servo setup
    pinMode(VOUT_JOY_PIN, OUTPUT); //pin A3 is output
    pinMode(GROUND_JOY_PIN, OUTPUT); //pin A2 is output
    digitalWrite(VOUT_JOY_PIN, HIGH); //set A2 to high (+5V)
    digitalWrite(GROUND_JOY_PIN, LOW); //set A3 to low (gnd)
    myservo.attach(SERVO_PIN);

    //Pushbutton and Active Buzzer Setup
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

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
    Serial.println("In Loop: hello");
    printf("Test /n");
    // should never reach here...
    delay(1000);
}

