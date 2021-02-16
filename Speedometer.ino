#define HALL_PIN          11
#define ROLLER_CIRCLE_MM  264

void setup()
{
  Serial.begin(9600);
}

unsigned char measuredState = 0;
bool skipNextStateChange = true;
unsigned int rotations = 0;
unsigned int rotationsTotalTimeMs = 0;
unsigned long lastRotationTimeMs = 0;
unsigned long lastReportTimeMs = 0;

void loop()
{
  unsigned long nowMs = millis();
  
  checkRotation(nowMs);

  // Report the data if it's significant 
  // or if nothing has been reported for a long time.
  if (rotationsTotalTimeMs > 1000 || nowMs - lastReportTimeMs > 2000) {
    reportData(nowMs);

    // If we had no rotations for a long time, 
    // then last rotation time is no more relevant.
    if (rotations == 0) {
      lastRotationTimeMs = 0;
    }
    
    rotations = 0;
    rotationsTotalTimeMs = 0;
  }
}

void checkRotation(unsigned long nowMs) {
  uint8_t state = digitalRead(HALL_PIN);

  if (state != measuredState) {
    measuredState = state;
    if (!skipNextStateChange) {
      // We can only count the rotation 
      // if we know it's duration.
      if (lastRotationTimeMs != 0) {
        rotations++;
        rotationsTotalTimeMs += nowMs - lastRotationTimeMs;
      }
      
      lastRotationTimeMs = nowMs;
      
      // Next state change will be caused by the opposite 
      // magnet pole so we have to skip it.
      skipNextStateChange = true;
    } else {
      skipNextStateChange = false;
    }
  }
}

void reportData(unsigned long nowMs) {
  unsigned int passedDistanceMm = rotations * ROLLER_CIRCLE_MM;
  unsigned int speedMph = (rotationsTotalTimeMs != 0)
                          ? passedDistanceMm * 3600.0 / rotationsTotalTimeMs
                          : 0;
  lastReportTimeMs = nowMs;
  Serial.print(speedMph);
  Serial.print(" ");
  Serial.println(passedDistanceMm);
}
