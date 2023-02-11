#include <EEPROM.h>
#include <PINOUT.h>
#include <BUTTON.h>

const int outputEnabled = 2;  // write LOW to turn on Note, may not be hooked up.
// const int s0 = 3;            // sensor pins
// const int s1 = 4;            // sensor pins
const int s2 = 5;    // sensor pins
const int s3 = 6;    // sensor pins
const int nLED = 7;  // illuminating LED
const int out = 8;   // TCS230 output

PINOUT Alarm(11);  // NG sound alarm
PINOUT AC(10);     // On relay contact AC voltage
BUTTON BTmode_01(15);
BUTTON BTmode_02(16);
BUTTON BTmode_03(17);
BUTTON BTmode_04(18);
BUTTON BTcal(19);

BUTTON BTcalibrationGreen(3);
BUTTON BTcalibrationRed(4);

// Variables for time keeping
unsigned long period = 500;
int period_overspend = -1000;
unsigned long last_time_cs = 0;

// Variables count down
const int setCount = 20;  // Set count down time 10 seconds
int countDown = 0;           // Count down time

// Variables for color pulse width values
int redPW = 0;
int greenPW = 0;
int bluePW = 0;
// Variables to store color values
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
bool modeChange = true;
/*************** Green Parameter **********************/
// R
int GRedValue = 0;
// G
int GGreenValue = 0;
// B
int GblueValue = 0;

/*************** Red Parameter **********************/
// R
int RRedValue = 0;
// G
int RGreenValue = 0;
// B
int RblueValue = 0;

int Tolerance = 100;

void setup() {
  pinMode(outputEnabled, OUTPUT);
  // pinMode(s0, OUTPUT);
  // pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(nLED, OUTPUT);
  pinMode(out, INPUT);
  Serial.begin(9600);
  // This pin may be set to ground and not available on the breakout
  // If not available don't worry about it.
  digitalWrite(outputEnabled, LOW);  // turn on sensor
  // Set Frequency scaling to largest value
  // digitalWrite(s0, HIGH);  // set frequency scaling to 20%
  // digitalWrite(s1, HIGH);  // set frequency scaling to 20%
  digitalWrite(nLED, LOW);  // turn off LED
  // Load calibration values from EEPROM
  // Green color
  modeChange = true;
}

void loop() {
  Working();
}
bool statemde_01 = true;
bool statemde_02 = true;
bool statemde_03 = true;
bool statemde_04 = true;

void Working() {
  if (millis() - last_time_cs > period) {

    LoadCalibrationGreen();
    LoadCalibrationRed();
    // Working code here 1 second
    // Read pulse width values
    redPW = getRedPW();
    // Read pulse width values
    greenPW = getGreenPW();
    // Read pulse width values
    bluePW = getBluePW();
    if (!BTmode_01.getState() || !BTmode_02.getState() || !BTmode_03.getState() || !BTmode_04.getState()) {
      if (isRange(redPW, GRedValue - Tolerance, GRedValue + Tolerance) && isRange(greenPW, GGreenValue - Tolerance, GGreenValue + Tolerance) && isRange(bluePW, GblueValue - Tolerance, GblueValue + Tolerance)) {
        Serial.println("Green");
        // Green is the color of the Ac
        AC.on();               // Turn on AC
        Alarm.off();           // Turn off Alarm
        countDown = setCount;  // Reset count down time
      } else if (isRange(redPW, RRedValue - Tolerance, RRedValue + Tolerance) && isRange(greenPW, RGreenValue - Tolerance, RGreenValue + Tolerance) && isRange(bluePW, RblueValue - Tolerance, RblueValue + Tolerance)) {
        Serial.println("Red");
        Alarm.on();
        AC.off();
      } else {
        countDown--;  // Count down
        if (countDown <= 0) {
          AC.off();  // Turn off AC
        }
        Alarm.off();  // Turn on Alarm
        // Serial.println("Unknown");
      }
    }else{
       AC.off();  // Turn off AC
       Alarm.off();  // Turn on Alarm
    }
    Serial.println("");
    Serial.print("Red: ");
    Serial.print(redPW);
    Serial.print(" ,Green: ");
    Serial.print(greenPW);
    Serial.print(" ,Blue: ");
    Serial.println(bluePW);
    // Serial.println("");

    Serial.print("Red Df OK: ");
    Serial.print(GRedValue);
    Serial.print(" ,Green Df OK: ");
    Serial.print(GGreenValue);
    Serial.print(" ,Blue Df OK: ");
    Serial.println(GblueValue);
    // Serial.println("");

    Serial.print("Red Df NG: ");
    Serial.print(RRedValue);
    Serial.print(" ,Green Df NG: ");
    Serial.print(RGreenValue);
    Serial.print(" ,Blue Df NG: ");
    Serial.println(RblueValue);
    Serial.println("");
    Serial.println("--------------------------");
    last_time_cs = millis();

  } else if (millis() < 1000) {
    last_time_cs = millis();
  }
  if (BTcalibrationGreen.isPressed() && !BTcal.getState()) {
    // Save calibration values to EEPROM
    Serial.println("Save Green");
    SaveCalibrationGreen();
    // LoadCalibrationGreen();
    delay(100);
  }

  if (BTcalibrationRed.isPressed() && !BTcal.getState()) {
    // Save calibration values to EEPROM
    Serial.println("Save Red");
    SaveCalibrationRed();
    // LoadCalibrationRed();
    delay(100);
  }
}

void SaveCalibrationGreen() {
  // Save calibration values
  // Split the value into two bytes and save them to EPROOM
  if (!BTcal.getState() && !BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Green color
    // Addresses 0 and 1
    EEPROM.write(0, highByte(redPW));
    EEPROM.write(1, lowByte(redPW));
    // Address 2 and 3
    EEPROM.write(2, highByte(greenPW));
    EEPROM.write(3, lowByte(greenPW));
    // Address 4 and 5
    EEPROM.write(4, highByte(bluePW));
    EEPROM.write(5, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_02.getState() && BTmode_01.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Green color
    // Address 6 and 7
    EEPROM.write(6, highByte(redPW));
    EEPROM.write(7, lowByte(redPW));
    // Address 8 and 9
    EEPROM.write(8, highByte(greenPW));
    EEPROM.write(9, lowByte(greenPW));
    // Address 10 and 11
    EEPROM.write(10, highByte(bluePW));
    EEPROM.write(11, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_03.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Green color
    // Address 12 and 13
    EEPROM.write(12, highByte(redPW));
    EEPROM.write(13, lowByte(redPW));
    // Address 14 and 15
    EEPROM.write(14, highByte(greenPW));
    EEPROM.write(15, lowByte(greenPW));
    // Address 16 and 17
    EEPROM.write(16, highByte(bluePW));
    EEPROM.write(17, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_04.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState()) {
    // Save calibration values to EEPROM
    // Green color
    // Address 18 and 19
    EEPROM.write(18, highByte(redPW));
    EEPROM.write(19, lowByte(redPW));
    // Address 20 and 21
    EEPROM.write(20, highByte(greenPW));
    EEPROM.write(21, lowByte(greenPW));
    // Address 22 and 23
    EEPROM.write(22, highByte(bluePW));
    EEPROM.write(23, lowByte(bluePW));
  }
}

void SaveCalibrationRed() {
  // Save calibration values
  // Split the value into two bytes and save them to EEPROM
  if (!BTcal.getState() && !BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Red color
    // Addresses 30 and 31
    EEPROM.write(30, highByte(redPW));
    EEPROM.write(31, lowByte(redPW));
    // Address 32 and 33
    EEPROM.write(32, highByte(greenPW));
    EEPROM.write(33, lowByte(greenPW));
    // Address 34 and 35
    EEPROM.write(34, highByte(bluePW));
    EEPROM.write(35, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_02.getState() && BTmode_01.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Red color
    // Address 36 and 37
    EEPROM.write(36, highByte(redPW));
    EEPROM.write(37, lowByte(redPW));
    // Address 38 and 39
    EEPROM.write(38, highByte(greenPW));
    EEPROM.write(39, lowByte(greenPW));
    // Address 40 and 41
    EEPROM.write(40, highByte(bluePW));
    EEPROM.write(41, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_03.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_04.getState()) {
    // Save calibration values to EEPROM
    // Red color
    // Address 42 and 43
    EEPROM.write(42, highByte(redPW));
    EEPROM.write(43, lowByte(redPW));
    // Address 44 and 45
    EEPROM.write(44, highByte(greenPW));
    EEPROM.write(45, lowByte(greenPW));
    // Address 46 and 47
    EEPROM.write(46, highByte(bluePW));
    EEPROM.write(47, lowByte(bluePW));
  } else if (!BTcal.getState() && !BTmode_04.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState()) {
    // Save calibration values to EEPROM
    // Red color
    // Address 48 and 49
    EEPROM.write(48, highByte(redPW));
    EEPROM.write(49, lowByte(redPW));
    // Address 50 and 51
    EEPROM.write(50, highByte(greenPW));
    EEPROM.write(51, lowByte(greenPW));
    // Address 52 and 53
    EEPROM.write(52, highByte(bluePW));
    EEPROM.write(53, lowByte(bluePW));
  }
}

void LoadCalibrationGreen() {
  // Load calibration values from EEPROM
  if (!BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Green color
    Serial.print("G Mode 01 ");
    GRedValue = word(EEPROM.read(0), EEPROM.read(1));
    GGreenValue = word(EEPROM.read(2), EEPROM.read(3));
    GblueValue = word(EEPROM.read(4), EEPROM.read(5));
  } else if (!BTmode_02.getState() && BTmode_01.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Green color
    GRedValue = word(EEPROM.read(6), EEPROM.read(7));
    GGreenValue = word(EEPROM.read(8), EEPROM.read(9));
    GblueValue = word(EEPROM.read(10), EEPROM.read(11));
  } else if (!BTmode_03.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Green color
    GRedValue = word(EEPROM.read(12), EEPROM.read(13));
    GGreenValue = word(EEPROM.read(14), EEPROM.read(15));
    GblueValue = word(EEPROM.read(16), EEPROM.read(17));
  } else if (!BTmode_04.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState()) {
    // Load calibration values from EEPROM
    // Green color
    GRedValue = word(EEPROM.read(18), EEPROM.read(19));
    GGreenValue = word(EEPROM.read(20), EEPROM.read(21));
    GblueValue = word(EEPROM.read(22), EEPROM.read(23));
  }
}

void LoadCalibrationRed() {

  // Load calibration values from EEPROM
  if (!BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Red color
    Serial.print("R Mode 01 ");
    RRedValue = word(EEPROM.read(30), EEPROM.read(31));
    RGreenValue = word(EEPROM.read(32), EEPROM.read(33));
    RblueValue = word(EEPROM.read(34), EEPROM.read(35));
  } else if (!BTmode_02.getState() && BTmode_01.getState() && BTmode_03.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Red color
    RRedValue = word(EEPROM.read(36), EEPROM.read(37));
    RGreenValue = word(EEPROM.read(38), EEPROM.read(39));
    RblueValue = word(EEPROM.read(40), EEPROM.read(41));
  } else if (!BTmode_03.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_04.getState()) {
    // Load calibration values from EEPROM
    // Red color
    RRedValue = word(EEPROM.read(42), EEPROM.read(43));
    RGreenValue = word(EEPROM.read(44), EEPROM.read(45));
    RblueValue = word(EEPROM.read(46), EEPROM.read(47));
  } else if (!BTmode_04.getState() && BTmode_01.getState() && BTmode_02.getState() && BTmode_03.getState()) {
    // Load calibration values from EEPROM
    // Red color
    RRedValue = word(EEPROM.read(48), EEPROM.read(49));
    RGreenValue = word(EEPROM.read(50), EEPROM.read(51));
    RblueValue = word(EEPROM.read(52), EEPROM.read(53));
  }
}

int getRedPW() {
  // count OUT, pRed, RED
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  int PW = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  return PW;
}
int ByteToInt(byte storedHighByte, byte storedLowByte) {
  return (storedHighByte << 8) | storedLowByte;
}
int getGreenPW() {

  // count OUT, pRed, RED
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  int PW = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  return PW;
}

int getBluePW() {
  // count OUT, pRed, RED
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  int PW = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  return PW;
}

bool isRange(int value, int min, int max) {
  if (value > min && value < max) {
    return true;
  }
  return false;
}
