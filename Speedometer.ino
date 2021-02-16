#define HALL_PIN          11
#define ROLLER_CIRCLE_MM  264 

void setup()
{
  Serial.begin(9600);
}

unsigned char measuredState = 0;
bool skipNext = true;
unsigned int rotations = 0;
unsigned long calculationTimeMs = 0;

void loop()
{
  checkRotation();

  unsigned long nowMs = millis();
  unsigned long timeDeltha = nowMs - calculationTimeMs;

  if (timeDeltha > 1000) {
    calculationTimeMs = nowMs;
    calculateAndReportSpeed(timeDeltha);
  }
}

void checkRotation() {
  uint8_t state = digitalRead(HALL_PIN);
  
  if (state != measuredState) {
    measuredState = state;
    if (!skipNext) {
      rotations++;
      skipNext = true;
    } else {
      skipNext = false;
    }
  }
}

void calculateAndReportSpeed(unsigned long periodMs) {
  unsigned int passedDistanceMm = rotations * ROLLER_CIRCLE_MM;
  unsigned int speedMph = passedDistanceMm * 3600.0 / periodMs;
  rotations = 0;
  Serial.print("S ");
  Serial.print(speedMph);
  Serial.print(", D ");
  Serial.println(passedDistanceMm);
}
