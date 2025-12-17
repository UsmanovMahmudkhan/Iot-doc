// ============================================================================
// Smart Blind Shoes - Optimized Version (Fits in 32KB)
// ============================================================================
// Core Features Only:
// - 3x Ultrasonic Sensors with filtering
// - LCD Display
// - RGB LED Status
// - Haptic Feedback
// - Step Counting
// - Battery Monitoring
// - Emergency System
// ============================================================================

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// -------------------------- Pin Configuration -----------------------------
const uint8_t TRIG_LEFT = 4;
const uint8_t ECHO_LEFT = 5;
const uint8_t TRIG_CENTER = 6;
const uint8_t ECHO_CENTER = 7;
const uint8_t TRIG_RIGHT = 8;
const uint8_t ECHO_RIGHT = 3;
const uint8_t TRIG_DOWN = A2;
const uint8_t ECHO_DOWN = A3;

const uint8_t VIBE_LEFT_PIN = 9;
const uint8_t VIBE_RIGHT_PIN = 9;  // Share with left (can be separated with shift register)
const uint8_t BUZZER_PIN = 10;
const uint8_t RGB_RED = 11;
const uint8_t RGB_GREEN = 12;
const uint8_t RGB_BLUE = 13;

const uint8_t EMERGENCY_BTN_PIN = 2;
const uint8_t MODE_BTN_PIN = A4;
const uint8_t CALIBRATE_BTN_PIN = A5;
const uint8_t SENSITIVITY_POT = A1;

const uint8_t LED_MODE1 = 12;  // Share with RGB_GREEN
const uint8_t LED_MODE2 = 11;  // Share with RGB_RED
const uint8_t LED_MODE3 = 10;  // Share with BUZZER (use different control)
const uint8_t TEMP_SENSOR = A0;
const uint8_t MOISTURE_SENSOR = A1;
const uint8_t SERVO_PIN = 7;

// Shift Register Pins (74HC595)
const uint8_t SHIFT_DATA = 0;    // DS (Data Serial)
const uint8_t SHIFT_CLOCK = 1;   // SHCP (Shift Clock)
const uint8_t SHIFT_LATCH = A0;  // STCP (Storage Clock)

// -------------------------- Configuration -----------------------------
const unsigned int MAX_DISTANCE_CM = 400;
const unsigned int BASE_WARN_DISTANCE = 150;
const unsigned int BASE_ALERT_DISTANCE = 80;
const unsigned int BASE_CRITICAL_DISTANCE = 30;

unsigned int WARN_DISTANCE = BASE_WARN_DISTANCE;
unsigned int ALERT_DISTANCE = BASE_ALERT_DISTANCE;
unsigned int CRITICAL_DISTANCE = BASE_CRITICAL_DISTANCE;

const unsigned long SENSOR_INTERVAL_MS = 100;
const unsigned long LCD_UPDATE_INTERVAL_MS = 200;
const unsigned long EMERGENCY_HOLD_MS = 2000;
const unsigned long STEP_DETECT_INTERVAL_MS = 500;
const unsigned long BATTERY_CHECK_INTERVAL_MS = 10000;

const bool DEBUG_MODE = true;  // Enable for debugging 7-segment displays
const bool JSON_OUTPUT_ENABLED = true;
const bool ENABLE_7SEGMENT_DISPLAY = true; // Set to false if causing issues

const uint8_t FILTER_SIZE = 5;

// --------------------------- Hardware Objects -------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --------------------------- State Variables -------------------------
unsigned long lastSensorMillis = 0;
unsigned long lastLcdMillis = 0;
unsigned long lastJsonOutputMillis = 0;
unsigned long lastStepDetectMillis = 0;
unsigned long lastBatteryCheckMillis = 0;
unsigned long buttonPressStart = 0;

bool buttonWasPressed = false;
bool emergencyActive = false;
uint8_t lastZone = 0;
float sensitivityMultiplier = 1.0;

unsigned int distanceLeft = MAX_DISTANCE_CM;
unsigned int distanceCenter = MAX_DISTANCE_CM;
unsigned int distanceRight = MAX_DISTANCE_CM;
unsigned int distanceDown = MAX_DISTANCE_CM;
unsigned int filteredLeft = MAX_DISTANCE_CM;
unsigned int filteredCenter = MAX_DISTANCE_CM;
unsigned int filteredRight = MAX_DISTANCE_CM;
unsigned int filteredDown = MAX_DISTANCE_CM;
unsigned int closestDistance = MAX_DISTANCE_CM;
uint8_t closestSensor = 1;

unsigned int leftBuffer[FILTER_SIZE] = {0};
unsigned int centerBuffer[FILTER_SIZE] = {0};
unsigned int rightBuffer[FILTER_SIZE] = {0};
unsigned int downBuffer[FILTER_SIZE] = {0};
uint8_t bufferIndex = 0;

uint8_t operationMode = 0; // 0=normal, 1=sensitive, 2=aggressive
bool calibrationMode = false;
float temperature = 25.0;
uint8_t moistureLevel = 0;
unsigned long lastModeChange = 0;
unsigned long lastCalibrationCheck = 0;
unsigned long lastSegmentUpdate = 0;
uint8_t segmentDisplayMode = 0; // 0=steps, 1=distance, 2=battery, 3=speed

// Shift register output tracking
uint8_t shift1Output = 0;  // Tracks all shift1 outputs (RGB LEDs, bar graph, segment1)
uint8_t shift2Output = 0;  // Tracks all shift2 outputs (status LEDs, segment2)

// Countdown timer
uint8_t countdownValue = 99;
unsigned long lastCountdownUpdate = 0;
const unsigned long COUNTDOWN_INTERVAL_MS = 5000; // 5 seconds
bool countdownActive = true;

// 7-Segment Display Patterns (common cathode: segments A-G)
// Bit pattern: 0bGFEDCBA (G=MSB, A=LSB)
// Wokwi 7-segment: 1 = segment ON, 0 = segment OFF
// Mapping: A=Q0, B=Q1, C=Q2, D=Q3, E=Q4, F=Q5, G=Q6
const uint8_t SEGMENT_PATTERNS[10] = {
  0b0111111, // 0 - segments A,B,C,D,E,F (0x3F) - all except G
  0b0000110, // 1 - segments B,C (0x06)
  0b1011011, // 2 - segments A,B,D,E,G (0x5B)
  0b1001111, // 3 - segments A,B,C,D,G (0x4F)
  0b1100110, // 4 - segments B,C,F,G (0x66)
  0b1101101, // 5 - segments A,C,D,F,G (0x6D)
  0b1111101, // 6 - segments A,C,D,E,F,G (0x7D)
  0b0000111, // 7 - segments A,B,C (0x07)
  0b1111111, // 8 - all segments (0x7F)
  0b1101111  // 9 - segments A,B,C,D,F,G (0x6F)
};

float currentLat = 37.5514;
float currentLng = 127.0748;
float walkingSpeed = 0.0;
float totalDistance = 0.0;
unsigned long stepCount = 0;
unsigned long lastStepTime = 0;
float averageStepInterval = 0.0;
bool stepDetected = false;
uint8_t batteryLevel = 100;
bool lowBatteryWarning = false;
unsigned long systemUptime = 0;

bool sensorErrorLeft = false;
bool sensorErrorCenter = false;
bool sensorErrorRight = false;

// ----------------------------- Utilities ------------------------------
uint8_t getZone(unsigned int distance) {
  if (distance < CRITICAL_DISTANCE) return 3;
  if (distance < ALERT_DISTANCE) return 2;
  if (distance < WARN_DISTANCE) return 1;
  return 0;
}

const char* zoneLabel(uint8_t zone) {
  switch (zone) {
    case 1: return "WARN";
    case 2: return "ALERT";
    case 3: return "CRIT";
    default: return "CLEAR";
  }
}

const char* sensorLabel(uint8_t sensor) {
  switch (sensor) {
    case 0: return "LEFT";
    case 2: return "RIGHT";
    default: return "FRONT";
  }
}

// ----------------------- Sensor Filtering -------------------------
unsigned int applyFilter(unsigned int newValue, unsigned int *buffer, unsigned int currentFiltered) {
  buffer[bufferIndex] = newValue;
  bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
  
  unsigned long sum = 0;
  for (uint8_t i = 0; i < FILTER_SIZE; i++) {
    sum += buffer[i];
  }
  unsigned int average = sum / FILTER_SIZE;
  
  int change = abs((int)average - (int)currentFiltered);
  if (change > 50) {
    return (currentFiltered * 7 + average * 3) / 10;
  }
  return average;
}

// ----------------------- Sensor Functions -------------------------
unsigned int readUltrasonic(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  unsigned long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return MAX_DISTANCE_CM;
  
  unsigned int distance = duration * 0.034 / 2;
  return constrain(distance, 2, MAX_DISTANCE_CM);
}

void readAllSensors() {
  distanceLeft = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
  distanceCenter = readUltrasonic(TRIG_CENTER, ECHO_CENTER);
  distanceRight = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
  distanceDown = readUltrasonic(TRIG_DOWN, ECHO_DOWN);
  
  sensorErrorLeft = (distanceLeft < 2 || distanceLeft > MAX_DISTANCE_CM);
  sensorErrorCenter = (distanceCenter < 2 || distanceCenter > MAX_DISTANCE_CM);
  sensorErrorRight = (distanceRight < 2 || distanceRight > MAX_DISTANCE_CM);
  
  filteredLeft = applyFilter(distanceLeft, leftBuffer, filteredLeft);
  filteredCenter = applyFilter(distanceCenter, centerBuffer, filteredCenter);
  filteredRight = applyFilter(distanceRight, rightBuffer, filteredRight);
  filteredDown = applyFilter(distanceDown, downBuffer, filteredDown);
  
  closestDistance = filteredCenter;
  closestSensor = 1;
  
  if (filteredLeft < closestDistance) {
    closestDistance = filteredLeft;
    closestSensor = 0;
  }
  
  if (filteredRight < closestDistance) {
    closestDistance = filteredRight;
    closestSensor = 2;
  }
  
  if (filteredDown < closestDistance && filteredDown < 50) {
    closestDistance = filteredDown;
    closestSensor = 3; // Downward
  }
  
  uint8_t zone = getZone(closestDistance);
  if (zone != lastZone) {
    lastZone = zone;
  }
  
  if (JSON_OUTPUT_ENABLED) {
    outputJSONData();
  }
}

// ----------------------- JSON Output -------------------------
void outputJSONData() {
  Serial.print(F("JSON:"));
  Serial.print(F("{"));
  Serial.print(F("\"t\":"));
  Serial.print(millis());
  Serial.print(F(",\"sensors\":{"));
  Serial.print(F("\"left\":"));
  Serial.print(filteredLeft);
  Serial.print(F(",\"center\":"));
  Serial.print(filteredCenter);
  Serial.print(F(",\"right\":"));
  Serial.print(filteredRight);
  Serial.print(F(",\"closest\":"));
  Serial.print(closestDistance);
  Serial.print(F(",\"closestSensor\":"));
  Serial.print(closestSensor);
  Serial.print(F("}"));
  Serial.print(F(",\"zones\":{"));
  Serial.print(F("\"warn\":"));
  Serial.print(WARN_DISTANCE);
  Serial.print(F(",\"alert\":"));
  Serial.print(ALERT_DISTANCE);
  Serial.print(F(",\"critical\":"));
  Serial.print(CRITICAL_DISTANCE);
  Serial.print(F(",\"current\":"));
  Serial.print(getZone(closestDistance));
  Serial.print(F("}"));
  Serial.print(F(",\"stats\":{"));
  Serial.print(F("\"steps\":"));
  Serial.print(stepCount);
  Serial.print(F(",\"speed\":"));
  Serial.print(walkingSpeed, 2);
  Serial.print(F(",\"distance\":"));
  Serial.print(totalDistance, 2);
  Serial.print(F("}"));
  Serial.print(F(",\"system\":{"));
  Serial.print(F("\"battery\":"));
  Serial.print(batteryLevel);
  Serial.print(F(",\"uptime\":"));
  Serial.print(systemUptime);
  Serial.print(F(",\"sensitivity\":"));
  Serial.print(sensitivityMultiplier, 2);
  Serial.print(F("}"));
  Serial.print(F(",\"gps\":{"));
  Serial.print(F("\"lat\":"));
  Serial.print(currentLat, 6);
  Serial.print(F(",\"lng\":"));
  Serial.print(currentLng, 6);
  Serial.print(F("}"));
  Serial.print(F(",\"health\":{"));
  Serial.print(F("\"sensorLeft\":"));
  Serial.print(sensorErrorLeft ? 0 : 1);
  Serial.print(F(",\"sensorCenter\":"));
  Serial.print(sensorErrorCenter ? 0 : 1);
  Serial.print(F(",\"sensorRight\":"));
  Serial.print(sensorErrorRight ? 0 : 1);
  Serial.print(F("}"));
  Serial.print(F(",\"emergency\":"));
  Serial.print(emergencyActive ? 1 : 0);
  Serial.print(F(",\"mode\":"));
  Serial.print(operationMode);
  Serial.print(F(",\"calibration\":"));
  Serial.print(calibrationMode ? 1 : 0);
  Serial.print(F(",\"environment\":{"));
  Serial.print(F("\"temperature\":"));
  Serial.print(temperature, 1);
  Serial.print(F(",\"moisture\":"));
  Serial.print(moistureLevel);
  Serial.print(F("}"));
  Serial.print(F("}"));
  Serial.println();
}

// ----------------------- RGB LED Control -------------------------
void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(RGB_RED, 255 - r);
  analogWrite(RGB_GREEN, 255 - g);
  analogWrite(RGB_BLUE, 255 - b);
}

void updateRGBStatus(unsigned int distance) {
  if (emergencyActive) {
    static unsigned long lastFlash = 0;
    static bool flashState = false;
    if (millis() - lastFlash >= 200) {
      lastFlash = millis();
      flashState = !flashState;
      setRGBColor(flashState ? 255 : 0, 0, 0);
    }
    return;
  }
  
  if (lowBatteryWarning) {
    static unsigned long lastFlash = 0;
    static bool flashState = false;
    if (millis() - lastFlash >= 1000) {
      lastFlash = millis();
      flashState = !flashState;
      setRGBColor(flashState ? 255 : 0, flashState ? 100 : 0, 0);
    }
    return;
  }
  
  if (distance < CRITICAL_DISTANCE) {
    setRGBColor(255, 0, 0);
  } else if (distance < ALERT_DISTANCE) {
    setRGBColor(255, 100, 0);
  } else if (distance < WARN_DISTANCE) {
    setRGBColor(255, 255, 0);
  } else {
    setRGBColor(0, 255, 0);
  }
}

// ----------------------- LCD Display -------------------------
void updateLCD() {
  lcd.clear();
  
  if (emergencyActive) {
    lcd.setCursor(0, 0);
    lcd.print(F("!! EMERGENCY !!"));
    lcd.setCursor(0, 1);
    lcd.print(F("Help on way!"));
    return;
  }
  
  lcd.setCursor(0, 0);
  lcd.print(F("L"));
  lcd.print(filteredLeft);
  lcd.print(F(" C"));
  lcd.print(filteredCenter);
  lcd.print(F(" R"));
  lcd.print(filteredRight);
  
  lcd.setCursor(0, 1);
  lcd.print(zoneLabel(getZone(closestDistance)));
  lcd.print(F(" M"));
  lcd.print(operationMode);
  if (calibrationMode) {
    lcd.print(F("C"));
  }
  lcd.print(F(" B"));
  lcd.print(batteryLevel);
  lcd.print(F("%"));
}

// ----------------------- Haptic Feedback -------------------------
void updateHaptics(unsigned int distance) {
  static unsigned long lastToggle = 0;
  static bool vibeState = false;
  unsigned long now = millis();

  if (distance >= WARN_DISTANCE) {
    analogWrite(VIBE_LEFT_PIN, 0);
    analogWrite(VIBE_RIGHT_PIN, 0);
    noTone(BUZZER_PIN);
    return;
  }

  unsigned long interval = 800;
  int vibeLevel = 100;
  int toneFreq = 0;
  int leftVibe = 0;
  int rightVibe = 0;

  if (distance < CRITICAL_DISTANCE) {
    interval = 150;
    vibeLevel = 255;
    toneFreq = 2000;
  } else if (distance < ALERT_DISTANCE) {
    interval = 400;
    vibeLevel = 180;
    toneFreq = 1500;
  } else {
    interval = 600;
    vibeLevel = 120;
    toneFreq = 1000;
  }

  // Directional vibration based on sensor
  if (closestSensor == 0) {
    leftVibe = vibeLevel;
    rightVibe = vibeLevel / 2;
  } else if (closestSensor == 2) {
    leftVibe = vibeLevel / 2;
    rightVibe = vibeLevel;
  } else {
    leftVibe = vibeLevel;
    rightVibe = vibeLevel;
  }

  if (now - lastToggle >= interval) {
    lastToggle = now;
    vibeState = !vibeState;
    // Use same pin for both (can be separated with shift register in hardware)
    analogWrite(VIBE_LEFT_PIN, vibeState ? max(leftVibe, rightVibe) : 0);
    if (toneFreq) {
      if (vibeState) {
        tone(BUZZER_PIN, toneFreq, interval / 2);
      }
    } else {
      noTone(BUZZER_PIN);
    }
  }
}

// ----------------------- Sensitivity Adjustment -------------------------
void updateSensitivity() {
  int potValue = analogRead(SENSITIVITY_POT);
  float newMultiplier = map(potValue, 0, 1023, 50, 200) / 100.0;
  
  if (abs(newMultiplier - sensitivityMultiplier) > 0.05) {
    sensitivityMultiplier = newMultiplier;
    WARN_DISTANCE = BASE_WARN_DISTANCE * sensitivityMultiplier;
    ALERT_DISTANCE = BASE_ALERT_DISTANCE * sensitivityMultiplier;
    CRITICAL_DISTANCE = BASE_CRITICAL_DISTANCE * sensitivityMultiplier;
  }
}

// ----------------------- Step Detection -------------------------
void detectSteps() {
  unsigned long now = millis();
  
  if (closestDistance < WARN_DISTANCE && closestDistance > 20) {
    if (!stepDetected && (now - lastStepTime > 300)) {
      stepCount++;
      lastStepTime = now;
      stepDetected = true;
      
      if (stepCount > 1) {
        unsigned long stepInterval = now - lastStepTime;
        if (averageStepInterval == 0.0) {
          averageStepInterval = stepInterval;
        } else {
          averageStepInterval = (averageStepInterval * 0.7) + (stepInterval * 0.3);
        }
        
        if (averageStepInterval > 0) {
          float stepsPerSecond = 1000.0 / averageStepInterval;
          walkingSpeed = stepsPerSecond * 0.7 * 3.6;
          totalDistance += 0.7;
        }
      }
    }
  } else {
    stepDetected = false;
  }
}

// ----------------------- Battery Monitoring -------------------------
void checkBattery() {
  static unsigned long lastBatteryDrain = 0;
  unsigned long now = millis();
  
  if (now - lastBatteryDrain > 600000) {
    if (batteryLevel > 0) {
      batteryLevel--;
      lastBatteryDrain = now;
    }
  }
  
  if (batteryLevel <= 20 && !lowBatteryWarning) {
    lowBatteryWarning = true;
  } else if (batteryLevel > 20) {
    lowBatteryWarning = false;
  }
}

// ----------------------- GPS Simulation -------------------------
void updateGPS() {
  float t = millis() / 1000.0f;
  currentLat = 37.5514f + 0.00025f * sinf(t / 60.0f);
  currentLng = 127.0748f + 0.00025f * cosf(t / 75.0f);
}

// ----------------------- Shift Register Control -------------------------
void shiftOutByte(uint8_t data) {
  // Use bit-banging for shift register
  // Shift MSB first (bit 7 down to bit 0) - standard for 74HC595
  for (int8_t i = 7; i >= 0; i--) {
    digitalWrite(SHIFT_DATA, (data >> i) & 1);
    delayMicroseconds(5);
    digitalWrite(SHIFT_CLOCK, HIGH);
    delayMicroseconds(5);
    digitalWrite(SHIFT_CLOCK, LOW);
    delayMicroseconds(5);
  }
}

void updateShiftRegisters(uint8_t shift1Data, uint8_t shift2Data) {
  noInterrupts(); // Disable interrupts for clean shift register update
  digitalWrite(SHIFT_LATCH, LOW);
  shiftOutByte(shift2Data);  // Shift register 2 first (daisy-chained)
  shiftOutByte(shift1Data);  // Then shift register 1
  digitalWrite(SHIFT_LATCH, HIGH);
  interrupts(); // Re-enable interrupts
}

// ----------------------- 7-Segment Display Control -------------------------
void displayNumberOnSegments(uint8_t digit1, uint8_t digit2) {
  // Constrain digits to 0-9
  digit1 = constrain(digit1, 0, 9);
  digit2 = constrain(digit2, 0, 9);
  
  // Get segment patterns
  uint8_t seg1Pattern = SEGMENT_PATTERNS[digit1];
  uint8_t seg2Pattern = SEGMENT_PATTERNS[digit2];
  
  // Map segment patterns to shift register outputs
  // segment1 uses shift1:Q0-Q6 (bits 0-6) for segments A-G
  // segment2 uses shift2:Q0-Q6 (bits 0-6) for segments A-G
  
  // Combine segment patterns with other outputs
  // For shift1: Q0-Q6 = segments, Q7 = RGB right green (preserve if needed)
  // For shift2: Q0-Q6 = segments, Q7 = unused (preserve if needed)
  
  // Set segment bits (Q0-Q6) while preserving Q7
  shift1Output = (shift1Output & 0x80) | (seg1Pattern & 0x7F);
  shift2Output = (shift2Output & 0x80) | (seg2Pattern & 0x7F);
  
  // Update shift registers immediately
  updateShiftRegisters(shift1Output, shift2Output);
  
  // Debug output (if enabled)
  if (DEBUG_MODE && JSON_OUTPUT_ENABLED) {
    Serial.print(F("Display: "));
    Serial.print(digit1);
    Serial.print(digit2);
    Serial.print(F(" Shift1: 0x"));
    Serial.print(shift1Output, HEX);
    Serial.print(F(" Shift2: 0x"));
    Serial.println(shift2Output, HEX);
  }
}

void update7SegmentDisplays() {
  // Always display countdown value
  uint8_t digit1 = countdownValue / 10;
  uint8_t digit2 = countdownValue % 10;
  displayNumberOnSegments(digit1, digit2);
}

// ----------------------- Countdown Timer -------------------------
void updateCountdown() {
  unsigned long now = millis();
  
  if (countdownActive && (now - lastCountdownUpdate >= COUNTDOWN_INTERVAL_MS)) {
    lastCountdownUpdate = now;
    
    if (countdownValue >= 5) {
      countdownValue -= 5;
    } else {
      countdownValue = 0;
      // Play charging sound when reaching 0
      playChargingSound();
      // Reset countdown after sound
      countdownValue = 99;
    }
    
    // Update display immediately
    update7SegmentDisplays();
  }
}

void playChargingSound() {
  // Play a charging/beep sound pattern
  // Three ascending beeps
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 800 + (i * 200), 200);
    delay(250);
  }
  // Long beep (charging complete sound)
  tone(BUZZER_PIN, 1000, 500);
  delay(600);
  noTone(BUZZER_PIN);
  
  // Brief pause before resetting
  delay(200);
}

// Function to change what's displayed on 7-segment displays
void setSegmentDisplayMode(uint8_t mode) {
  segmentDisplayMode = mode % 4; // Cycle through 0-3
}

// Test function to verify 7-segment displays work
void test7SegmentDisplays() {
  // Show "88" first (all segments on - easiest to see)
  displayNumberOnSegments(8, 8);
  delay(1000);
  
  // Count from 00 to 99
  for (uint8_t i = 0; i < 100; i++) {
    uint8_t d1 = i / 10;
    uint8_t d2 = i % 10;
    displayNumberOnSegments(d1, d2);
    delay(200);
  }
  
  // Show "88" again
  displayNumberOnSegments(8, 8);
  delay(1000);
}

// Force display test - directly sets shift registers
void forceDisplayTest() {
  // Test pattern: show "88" (all segments on)
  uint8_t pattern8 = SEGMENT_PATTERNS[8]; // 0x7F = all segments
  
  shift1Output = pattern8 & 0x7F;
  shift2Output = pattern8 & 0x7F;
  
  // Update multiple times to ensure it sticks
  for (int i = 0; i < 5; i++) {
    updateShiftRegisters(shift1Output, shift2Output);
    delay(10);
  }
  
  if (JSON_OUTPUT_ENABLED) {
    Serial.print(F("Force test - Pattern: 0x"));
    Serial.print(pattern8, HEX);
    Serial.print(F(" Shift1: 0x"));
    Serial.print(shift1Output, HEX);
    Serial.print(F(" Shift2: 0x"));
    Serial.println(shift2Output, HEX);
  }
}

// ----------------------- Mode Selection -------------------------
void checkModeButton() {
  static bool lastModeBtnState = HIGH;
  static unsigned long lastModeDebounce = 0;
  static unsigned long lastPressTime = 0;
  bool modeBtnState = digitalRead(MODE_BTN_PIN);
  unsigned long now = millis();

  if (modeBtnState != lastModeBtnState && (now - lastModeDebounce > 50)) {
    lastModeDebounce = now;
    if (modeBtnState == LOW) {
      // Short press: change operation mode
      // Long press: change segment display mode
      lastPressTime = now;
    } else {
      // Button released
      unsigned long pressDuration = now - lastPressTime;
      if (pressDuration > 50 && pressDuration < 500) {
        // Short press - change operation mode
        operationMode = (operationMode + 1) % 3;
        lastModeChange = now;
        
        // Adjust sensitivity based on mode
        if (operationMode == 0) {
          sensitivityMultiplier = 1.0;
        } else if (operationMode == 1) {
          sensitivityMultiplier = 1.5;
        } else {
          sensitivityMultiplier = 0.7;
        }
        
        WARN_DISTANCE = BASE_WARN_DISTANCE * sensitivityMultiplier;
        ALERT_DISTANCE = BASE_ALERT_DISTANCE * sensitivityMultiplier;
        CRITICAL_DISTANCE = BASE_CRITICAL_DISTANCE * sensitivityMultiplier;
      } else if (pressDuration >= 500) {
        // Long press - change segment display mode
        setSegmentDisplayMode(segmentDisplayMode + 1);
      }
    }
  }
  lastModeBtnState = modeBtnState;
  
  // Update mode LEDs
  digitalWrite(LED_MODE1, operationMode == 0 ? HIGH : LOW);
  digitalWrite(LED_MODE2, operationMode == 1 ? HIGH : LOW);
  digitalWrite(LED_MODE3, operationMode == 2 ? HIGH : LOW);
}

// ----------------------- Calibration Mode -------------------------
void checkCalibrationButton() {
  static bool lastCalBtnState = HIGH;
  static unsigned long lastCalDebounce = 0;
  bool calBtnState = digitalRead(CALIBRATE_BTN_PIN);
  unsigned long now = millis();

  if (calBtnState != lastCalBtnState && (now - lastCalDebounce > 50)) {
    lastCalDebounce = now;
    if (calBtnState == LOW) {
      calibrationMode = !calibrationMode;
      lastCalibrationCheck = now;
    }
  }
  lastCalBtnState = calBtnState;
  
  if (calibrationMode) {
    // Calibration mode - adjust thresholds based on environment
    unsigned int avgDistance = (filteredLeft + filteredCenter + filteredRight) / 3;
    if (avgDistance < 100) {
      // Indoor/close environment
      WARN_DISTANCE = avgDistance * 0.8;
      ALERT_DISTANCE = avgDistance * 0.5;
      CRITICAL_DISTANCE = avgDistance * 0.2;
    }
  }
}

// ----------------------- Temperature Sensor -------------------------
void readTemperature() {
  int rawValue = analogRead(TEMP_SENSOR);
  // Simple thermistor calculation (adjust based on your thermistor)
  float resistance = 10000.0 * (1023.0 / rawValue - 1.0);
  float steinhart = log(resistance / 10000.0) / 3950.0;
  steinhart += 1.0 / (25.0 + 273.15);
  steinhart = 1.0 / steinhart;
  temperature = steinhart - 273.15;
}

// ----------------------- Moisture Sensor -------------------------
void readMoisture() {
  int rawValue = analogRead(MOISTURE_SENSOR);
  moistureLevel = map(rawValue, 0, 1023, 0, 100);
}

// ----------------------- Emergency System -------------------------
void checkEmergency() {
  bool buttonPressed = digitalRead(EMERGENCY_BTN_PIN) == LOW;
  unsigned long now = millis();

  if (buttonPressed && !buttonWasPressed) {
    buttonWasPressed = true;
    buttonPressStart = now;
  }

  if (buttonPressed && buttonWasPressed && !emergencyActive) {
    unsigned long holdTime = now - buttonPressStart;
    if (holdTime >= EMERGENCY_HOLD_MS) {
      emergencyActive = true;
    }
  }

  if (!buttonPressed && buttonWasPressed) {
    buttonWasPressed = false;
    if (emergencyActive) {
      emergencyActive = false;
    }
  }
}

// ------------------------------- Setup/Loop ---------------------------
void setup() {
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_CENTER, OUTPUT);
  pinMode(ECHO_CENTER, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_DOWN, OUTPUT);
  pinMode(ECHO_DOWN, INPUT);
  
  pinMode(VIBE_LEFT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  // LED_MODE pins share with RGB pins, already set
  
  pinMode(MODE_BTN_PIN, INPUT_PULLUP);
  pinMode(CALIBRATE_BTN_PIN, INPUT_PULLUP);
  
  // Shift register pins
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  
  digitalWrite(SHIFT_DATA, LOW);
  digitalWrite(SHIFT_CLOCK, LOW);
  digitalWrite(SHIFT_LATCH, LOW);
  
  // Initialize shift registers - clear all outputs
  shift1Output = 0;
  shift2Output = 0;
  updateShiftRegisters(shift1Output, shift2Output);
  delay(200);
  
  // Initialize countdown timer
  countdownValue = 99;
  lastCountdownUpdate = millis();
  
  // Display "99" on startup and test all segments
  if (ENABLE_7SEGMENT_DISPLAY) {
    // Force test first to ensure displays work
    forceDisplayTest();
    delay(500);
    
    // Test with "88" (all segments on)
    displayNumberOnSegments(8, 8);
    delay(500);
    
    // Then show countdown value "99"
    displayNumberOnSegments(9, 9);
    delay(300);
    
    if (DEBUG_MODE && JSON_OUTPUT_ENABLED) {
      Serial.println(F("7-Segment displays initialized - showing countdown"));
    }
  }
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  
  pinMode(EMERGENCY_BTN_PIN, INPUT_PULLUP);
  pinMode(SENSITIVITY_POT, INPUT);

  analogWrite(VIBE_LEFT_PIN, 0);
  noTone(BUZZER_PIN);
  setRGBColor(0, 0, 0);

  // Mode LEDs controlled via RGB pins
  digitalWrite(LED_MODE1, LOW);
  digitalWrite(LED_MODE2, LOW);
  digitalWrite(LED_MODE3, LOW);

  for (uint8_t i = 0; i < FILTER_SIZE; i++) {
    leftBuffer[i] = MAX_DISTANCE_CM;
    centerBuffer[i] = MAX_DISTANCE_CM;
    rightBuffer[i] = MAX_DISTANCE_CM;
    downBuffer[i] = MAX_DISTANCE_CM;
  }

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("Smart Boots"));
  lcd.setCursor(0, 1);
  lcd.print(F("Starting..."));
  delay(1000);

  if (JSON_OUTPUT_ENABLED) {
  Serial.begin(115200);
    delay(100);
  }
  lcd.clear();
  
  // Countdown already initialized above in shift register section
}

void loop() {
  unsigned long now = millis();
  systemUptime = now / 1000;

  updateSensitivity();

  if (now - lastSensorMillis >= SENSOR_INTERVAL_MS) {
    lastSensorMillis = now;
    readAllSensors();
    updateHaptics(closestDistance);
    updateRGBStatus(closestDistance);
    updateGPS();
  }

  if (now - lastLcdMillis >= LCD_UPDATE_INTERVAL_MS) {
    lastLcdMillis = now;
    updateLCD();
  }

  if (now - lastStepDetectMillis >= STEP_DETECT_INTERVAL_MS) {
    lastStepDetectMillis = now;
    detectSteps();
  }

  if (now - lastBatteryCheckMillis >= BATTERY_CHECK_INTERVAL_MS) {
    lastBatteryCheckMillis = now;
    checkBattery();
    readTemperature();
    readMoisture();
  }

  // Update countdown timer
  updateCountdown();
  
  // Update 7-segment displays continuously (for smooth display)
  if (ENABLE_7SEGMENT_DISPLAY) {
    // Always update display every loop iteration to ensure it's visible
    update7SegmentDisplays();
  }

  checkModeButton();
  checkCalibrationButton();
  checkEmergency();
}

