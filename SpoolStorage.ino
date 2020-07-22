/*
 Name:		SpoolStorage.ino
 Created:	20/07/2020 01:46:41
 Author:	Nicosmind
 website: www.nicosmind.com
*/

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define touchPad           32

long buttonTimer = 0;
long longPressTime = 2000; 

boolean buttonActive = false;
boolean longPressActive = false;

int z_offset = 0;         // number of steps for proper positioning to manualy close the drawer after homming
int stepDrawer = 1200;    // number of steps required to close the drawer currently open and move to the next one
int stepSpeed = 500;      // speed of rotation 

const int CW = LOW;
const int CCW = HIGH;

bool enableStateZ = false;

int countingStepsZ = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(touchPad, INPUT);
    hommingZ(stepSpeed, z_offset);
}

void loop()
{

    touchPadControl();
    //serialCommand();
}

void stepperMov(int enablePin, int dirPin, int stepPin, int steps, int dirMotor, int speedMotor)
{

    digitalWrite(enablePin, LOW);
    digitalWrite(dirPin, dirMotor);
    for (int x = 0; x < steps; x++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speedMotor);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speedMotor);
    }
    digitalWrite(enablePin, HIGH);
}

void serialCommand()
{
    if (Serial.available() > 0)
    {
        if (Serial.peek() == 'Z')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            if (numSteps > 0)
            {
                //Serial.println("tourne CW");
                stepperMov(Z_ENABLE_PIN, Z_DIR_PIN, Z_STEP_PIN, numSteps, CW, 500);
                countingStepsZ = (countingStepsZ + numSteps);
            }
            else if (numSteps < 0)
            {
                //Serial.println("tourne CCW");
                numSteps = abs(numSteps);
                stepperMov(Z_ENABLE_PIN, Z_DIR_PIN, Z_STEP_PIN, numSteps, CCW, 500);
                countingStepsZ = (countingStepsZ - numSteps);
            }
        }
        else if (Serial.peek() == 'H')
        {
            int clrBuffer;
            Serial.read();
            clrBuffer = Serial.parseInt();
            hommingZ(stepSpeed, z_offset);
        }
        else
        {
            int clrBuffer;
            Serial.read();
            clrBuffer = Serial.parseInt();
        }
        Serial.print("StepsZ : ");
        Serial.println(countingStepsZ);
    }
    else
    {

    }
}


void enableZ()
{
    if (enableStateZ == true)
    {
        digitalWrite(Z_ENABLE_PIN, !enableStateZ);
        enableStateZ = !enableStateZ;
    }
    else
    {
        digitalWrite(Z_ENABLE_PIN, !enableStateZ);
        enableStateZ = !enableStateZ;
    }
}


void hommingZ(int speedMotor, int homePosition)
{
    int endState;
    digitalWrite(Z_ENABLE_PIN, LOW);
    pinMode(Z_MIN_PIN, INPUT);
    digitalWrite(Z_DIR_PIN, LOW); delayMicroseconds(5);
    endState = digitalRead(Z_MIN_PIN);
    while (endState == HIGH)
    {
        digitalWrite(Z_STEP_PIN, HIGH); delayMicroseconds(speedMotor);
        digitalWrite(Z_STEP_PIN, LOW);  delayMicroseconds(speedMotor);
        endState = digitalRead(Z_MIN_PIN);
    }
    stepperMov(Z_ENABLE_PIN, Z_DIR_PIN, Z_STEP_PIN, homePosition, CW, speedMotor);
    countingStepsZ = 0;
}




void touchPadControl()
{
    if (digitalRead(touchPad) == HIGH)
    {
        if (buttonActive == false)
        {
            buttonActive = true;
            buttonTimer = millis();
        }
        if ((millis() - buttonTimer > longPressTime) && (longPressActive == false))
        {
            longPressActive = true;
            Serial.println("Homming");
            hommingZ(stepSpeed, z_offset);
        }
    }
    else
    {
        if (buttonActive == true)
        {
            if (longPressActive == true)
            {
                longPressActive = false;
            }
            else
            {
                stepperMov(Z_ENABLE_PIN, Z_DIR_PIN, Z_STEP_PIN, 1200, CW, 500);
            }

            buttonActive = false;

        }

    }

}