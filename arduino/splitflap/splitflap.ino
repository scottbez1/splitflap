#include <math.h>

#define NUM_FLAPS (40)

#define STEPS_PER_REVOLUTION (63.68395 * 64)
#define STEPS_PER_FLAP (STEPS_PER_REVOLUTION / NUM_FLAPS)

int flaps[NUM_FLAPS] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

void setup() {
  // put your setup code here, to run once:
  DDRA = 0xFF;
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

uint8_t step_pattern[] = {
  B0001,
  B1001,
  B1000,
  B1100,
  B0100,
  B0110,
  B0010,
  B0011,
};

unsigned long lastUpdate = 0;
long current = 0;
float desired = 0;

long MAX_PERIOD = 6000;
long MIN_PERIOD = 700;
long VEL_STEP = 80;

long maxVel = MAX_PERIOD - MIN_PERIOD;
long accelSteps = maxVel / VEL_STEP;

long curVel = 0;
long stepPeriod = 0;


int desiredFlapIndex = 0;

int findFlapIndex(int character) {
  for (int i = 0; i < NUM_FLAPS; i++) {
    if (character == flaps[i]) {
      return i;
    }
  }
  return -1;
}

void loop() {
  unsigned long now = micros();
  if (now - lastUpdate >= stepPeriod) {
    if (curVel > 0) {
      current++;
    } else if (curVel < 0) {
      current--;
    }
    lastUpdate = now;

    if (Serial.available() > 0) {
      int b = Serial.read();
      switch (b) {
        case '{':
          desired += STEPS_PER_FLAP / 4;
          break;
        case '[':
          desired += STEPS_PER_FLAP;
          break;
        case ']':
          desired -= STEPS_PER_FLAP;
          break;
        case '}':
          desired -= STEPS_PER_FLAP / 4;
          break;
        case '!':
          if (PORTA == 0) {
            desiredFlapIndex = 0;
          }
          break;
        default:
          int flapIndex = findFlapIndex(b);
          if (flapIndex != -1) {
            int deltaFlaps = flapIndex - desiredFlapIndex;
            if (deltaFlaps <= 0) {
              deltaFlaps += NUM_FLAPS;
            }
            desiredFlapIndex = (desiredFlapIndex + deltaFlaps) % NUM_FLAPS;
            desired += STEPS_PER_FLAP * deltaFlaps;
          }
          break;
      }
      Serial.print(desired);
      Serial.write('\n');
      Serial.print(current);
      Serial.write("\n\n");
    }

    float delta = desired - current;
    if (delta > -1 && delta < 1) {
      curVel = 0;
      stepPeriod = 0;
      PORTA = 0;
      while (current > 64) {
        current -= 64;
        desired -= 64;
      }
      while (current < -64) {
        current += 64;
        desired += 64;
      }
      return;
    }

    if (PORTA == 0) {
      PORTA = step_pattern[current & B111];
      delay(10);
      PORTA = step_pattern[(current-1) & B111];
      delay(10);
      PORTA = step_pattern[(current-2) & B111];
      delay(10);
      PORTA = step_pattern[(current-3) & B111];
      delay(10);
      PORTA = step_pattern[(current-2) & B111];
      delay(10);
      PORTA = step_pattern[(current-1) & B111];
      delay(10);
      PORTA = step_pattern[current & B111];
      delay(10);
    }

    PORTA = step_pattern[current & B111];

    long desiredVel = 0;
    if (delta > accelSteps) {
      desiredVel = maxVel;
    } else if (delta < -accelSteps) {
      desiredVel = -maxVel;
    } else {
      desiredVel = delta * VEL_STEP;
    }
    if (curVel > desiredVel) {
      curVel -= VEL_STEP;
    } else if (curVel < desiredVel) {
      curVel += VEL_STEP;
    }
    if (curVel > maxVel) curVel = maxVel;
    if (curVel < -maxVel) curVel = -maxVel;

    stepPeriod = curVel > 0 ? MAX_PERIOD - curVel : MAX_PERIOD + curVel;
  }
}
