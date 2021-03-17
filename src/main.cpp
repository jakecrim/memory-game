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
bool verifyInput(int);
void gameOver(void);

void actuateSequence(int);


/* DEFINES */
#define SERVO_PIN 9
#define GROUND_JOY_PIN A3
#define VOUT_JOY_PIN A2
#define XJOY_PIN A1
#define YJOY_PIN A0
#define LED_MATRIX_CLK  13
#define LED_MATRIX_CS   12
#define LED_MATRIX_DIN  11
#define BUTTON_PIN 27
#define BUZZER_PIN 23
#define BUTTON2_PIN 6
#define LED_PIN 7

#define TRIG1 2
#define ECHO1 3

#define WAVE_THRESHOLD 10

/* GLOBALS */
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
LedControl dotMatrix1 = LedControl(LED_MATRIX_DIN, LED_MATRIX_CLK, LED_MATRIX_CS, 0);
int sequence[100];
int buttonState2 = 0;

// Input State Flags
bool roundVerifiedFlag = true;
int userInputChoice = 0; 


// example LED Matrix Image
byte Apple [8]={B00011000,B00001000,B01110110,B11111111,B11111111,B11111111,B01111010,B00110100};

int bruhTest = 699;


int main_fn()
{
    // srand(time(NULL));
    randomSeed(analogRead(A4));

    int round = 1;
    // this 
    int iterationCount = 0;
    bool verifiedStatus = true;

    openGPIO();

    // // TESTING LOOP
    // int testVar = 0;
    // while(1)
    // {
    //     testVar = getDistance();
    //     Serial.println("Testing Distance:");
    //     Serial.print(testVar);
    //     actuateSequence(2);
    //     delay(100);
    // }

    /* Put Cube Functions here */
    while(1)
    {
        // Serial.println("In infinite while loop:");

        if(roundVerifiedFlag)
        {
            delay(400);
            Serial.print("New Round:");
            Serial.println(round);
            // Generate A new random number to add to our sequence of inputs
            sequence[round - 1] = generateSequence();

            // activates the actuator to show the user the pattern each time
            //      a random number is generated
            // sequence[round - 1] = 2;
            for(int i = 0; i < round; i++)
            {
                actuateSequence(sequence[i]);
                delay(500);
            }
            // actuateSequence(sequence[round - 1]);

            // Serial.println("Sequence is:");
            for(int i = 0; i < round; i++)
            {
                Serial.print(" | ");
                Serial.print(sequence[i]);
            }
            Serial.println(" ");
            roundVerifiedFlag = false;
        }

        joystickServo(); // 1
        buttonBuzzer();  // 2
        waveLEDTask();   // 3
        buttonLED();  // 4

        LCD_Task(round);


        if(userInputChoice !=0)
        {
            iterationCount = 0;

            while(verifiedStatus && iterationCount < round)
            {
                if(userInputChoice != 0)
                {
                    verifiedStatus = verifyInput(iterationCount);
                    iterationCount++;
                    Serial.println("Input Verified");
                    Serial.println("iteration count vs round ");
                    Serial.println(iterationCount);
                    Serial.println(round);
                }

                joystickServo(); // 1
                buttonBuzzer();  // 2
                waveLEDTask();   // 3
                buttonLED();  // 4
            }
            
            if(verifiedStatus == false)
            {
                Serial.println("Input was wrong!");
                roundVerifiedFlag = false;
                gameOver();
                // break;
            }
            
            // inc round
            round++;
            roundVerifiedFlag = true;
        }

        delay(100);
    }

    Serial.println("!!! GAME OVER !!!");

    return 0;
}

void gameOver()
{
    while(1)
    {
        Serial.println("GAME OVER");
        delay(1000);
    }
}

bool verifyInput(int iterationCount)
{
    bool success = false;
    Serial.print("User Activated Input:");
    Serial.println(userInputChoice);

    Serial.print("Correct Input is:");
    Serial.println(sequence[iterationCount]);

    if(sequence[iterationCount] == userInputChoice)
    {
        success = true;
    }
    else
    {
        success = false;
    }


    
    // Clear Flags
    userInputChoice = 0;

    delay(1000);
    return success;

}

void actuateSequence(int station)
{
    if(station == 1)
    {
        myservo.write(180);
        delay(250);
        myservo.write(0);
    }
    if(station == 2)
    {
        digitalWrite(BUZZER_PIN,HIGH);
        delay(200);//wait for 1ms
        digitalWrite(BUZZER_PIN,LOW);
        delay(10);
    }
    // *************
    // SHOW DOT MATRIX WAVE 
    if(station == 3)
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

    if(station == 4)
    {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED BLUE ON");
        delay(400);
        digitalWrite(LED_PIN, LOW);
    }
}

int generateSequence()
{
    // srand(time(0));
    int randNum = 0;

    for(int i = 0; i<4; i++)
    {
        // randNum = rand() % ((4 + 1) - 1) + 1;;
        randNum = random(1,5);
    }

    return randNum;
}

void waveLEDTask()
{
    int distance = 0;

    distance = getDistance();
    // Serial.print("Distance measured:");
    // Serial.println(distance);

    if(distance < WAVE_THRESHOLD)
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

        userInputChoice = 3;
    }
    // waveDetected = true;
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

    distance = echoTime * 0.034 / 2;

    return distance;

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
    
        userInputChoice = 2;
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
    int joyYVal = analogRead(YJOY_PIN);

    if ((joyXVal > 800 || joyXVal < 300) || (joyYVal > 800 || joyYVal < 300))
    {
        myservo.write(180);
        userInputChoice = 1;
    }
    else
    {
        myservo.write(0);
    }
    
    // Serial.print(joyXVal);                      //print the value from A1
    // Serial.println(" = input from joystick");  //print "=input from joystick" next to the value
    // Serial.print((joyXVal+520)/10);            //print a from A1 calculated, scaled value
    // Serial.println(" = output to servo");      //print "=output to servo" next to the value
    // Serial.println();
    //myservo.write((joyXVal+520)/10); 

    // joystickmoved= true;
}

void buttonLED()
{
    buttonState2 = digitalRead(BUTTON2_PIN);
    if (buttonState2 == HIGH)
    {
        digitalWrite(LED_PIN, HIGH);
        userInputChoice = 4;
        delay(300);
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
    // ultra-sonic sensor setup
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);

    //Joystick and Servo setup
    pinMode(VOUT_JOY_PIN, OUTPUT); //pin A3 is output
    pinMode(GROUND_JOY_PIN, OUTPUT); //pin A2 is output
    digitalWrite(VOUT_JOY_PIN, HIGH); //set A2 to high (+5V)
    digitalWrite(GROUND_JOY_PIN, LOW); //set A3 to low (gnd)
    myservo.attach(SERVO_PIN);
    myservo.write(0);
    delay(200);

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

