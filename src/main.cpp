#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <LedControl.h>
#include <time.h>

/* DECLARATIONS*/
int main_fn(void);
void LCD_Task(int);
void openGPIO(void);
int scan_I2C_Devices(void);
void joystickServo(void);
void waveLEDTask(void);
void buttonBuzzer(void);
void buttonLED(void);
int generateSequence(void);
int getDistance(void);


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
#define BUTTON2_PIN 6
#define LED_PIN 7

#define TRIG1 2
#define ECHO1 3

/* GLOBALS */
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
LedControl dotMatrix1 = LedControl(LED_MATRIX_DIN, LED_MATRIX_CLK, LED_MATRIX_CS, 0);
int sequence[100];
int buttonState2 = 0;

// example LED Matrix Image
byte Apple [8]={B00011000,B00001000,B01110110,B11111111,B11111111,B11111111,B01111010,B00110100};



int main_fn()
{
    srand(time(0));

    int round = 0;

    openGPIO();

    /* Put Cube Functions here */
    while(1)
    {
        int distance = 0;
        Serial.println("In infinite while loop:");

        // // Generate A new random number to add to our sequence of inputs
        // sequence[round] = generateSequence();

        // // Serial.println("Sequence is:");
        //  for(int i = 0; i <= round; i++)
        //  {
        //     Serial.print(" | ");
        //     Serial.print(sequence[i]);
        //  }

        joystickServo(); // 1
        buttonBuzzer();  // 2
        waveLEDTask();   // 3
        buttonLED();  // 4

        LCD_Task(round);

        distance = getDistance();
        Serial.print("Distance measured:");
        Serial.println(distance);

        // //verifyRound();

        // // inc round
        round++;



    }

    return 0;
}

int getDistance()
{
    int distance = 0;
    long echoTime = 0;

    digitalWrite(TRIG1, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG1, LOW);

    echoTime = pulseIn(ECHO1, HIGH);
    Serial.println(echoTime);
    distance = echoTime * 0.034 / 2;

    return distance;
}

int generateSequence()
{
    int randNum = 0;

    for(int i = 0; i<4; i++)
    {
        randNum = rand() % ((4 + 1) - 1) + 1;;
    }

    return randNum;
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

    // waveDetected = true;
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

void buttonBuzzer()
{
    int buttonState = digitalRead(BUTTON_PIN);
    if(buttonState == 1)
  {
    //output a frequency
    digitalWrite(BUZZER_PIN,HIGH);
    delay(200);//wait for 1ms
    digitalWrite(BUZZER_PIN,LOW);

    // buttonBuzzerPushed = true;
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
  delay(10); //reading delay
     
}


void joystickServo()
{
    int joyXVal = analogRead(XJOY_PIN);
    // Serial.print(joyXVal);                      //print the value from A1
    // Serial.println(" = input from joystick");  //print "=input from joystick" next to the value
    // Serial.print((joyXVal+520)/10);            //print a from A1 calculated, scaled value
    // Serial.println(" = output to servo");      //print "=output to servo" next to the value
    // Serial.println();
    myservo.write((joyXVal+520)/10); 

    // joystickmoved= true;
}

void buttonLED()
{
    buttonState2 = digitalRead(BUTTON2_PIN);
    if (buttonState2 == HIGH)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }


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

    //Pushbutton 2 and LED Setup
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON2_PIN, INPUT);

    // Ultrasonic sensor setup
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);

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
        else if(error == 4)
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

