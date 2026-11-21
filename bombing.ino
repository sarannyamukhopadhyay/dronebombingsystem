
// Pin Definitions - Exactly as your hardware is wired
const int INPUT_PIN_1  = 2;  // FC Signal 1 Input (Standard Interrupt)
const int MOSFET_PIN_1 = 3;  // MOSFET 1 Output

const int INPUT_PIN_2  = 4;  // FC Signal 2 Input (Pin Change Interrupt)
const int MOSFET_PIN_2 = 5;  // MOSFET 2 Output

// Volatile variables for Channel 1
volatile unsigned long pulseStart_1 = 0;
volatile unsigned long pulseWidth_1 = 0;

// Volatile variables for Channel 2
volatile unsigned long pulseStart_2 = 0;
volatile unsigned long pulseWidth_2 = 0;

void setup() {
  // Configure Channel 1 Pins
  pinMode(INPUT_PIN_1, INPUT_PULLUP);
  pinMode(MOSFET_PIN_1, OUTPUT);
  digitalWrite(MOSFET_PIN_1, HIGH); // Safe default (Inverted Logic)

  // Configure Channel 2 Pins
  pinMode(INPUT_PIN_2, INPUT_PULLUP);
  pinMode(MOSFET_PIN_2, OUTPUT);
  digitalWrite(MOSFET_PIN_2, HIGH); // Safe default (Inverted Logic)

  // 1. Attach standard hardware interrupt to Pin D2 (Channel 1)
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_1), readPWM_1, CHANGE);

  // 2. Enable Pin Change Interrupts on Pin D4 (Channel 2)
  cli();                           // Disable interrupts during setup
  PCICR |= (1 << PCIE2);           // Enable Pin Change Interrupts for Port D (Pins D0-D7)
  PCMSK2 |= (1 << PCINT20);        // Specifically enable interrupt for Arduino Pin D4 (PCINT20)
  sei();                           // Re-enable all interrupts
}

void loop() {
  // --- Channel 1 Processing ---
  if (pulseWidth_1 > 1600 && pulseWidth_1 < 2200) {
    digitalWrite(MOSFET_PIN_1, HIGH);   // Turn MOSFET 1 ON
  } else {
    digitalWrite(MOSFET_PIN_1, LOW);  // Turn MOSFET 1 OFF
  }

  // --- Channel 2 Processing ---
  if (pulseWidth_2 > 1600 && pulseWidth_2 < 2200) {
    digitalWrite(MOSFET_PIN_2, HIGH);   // Turn MOSFET 2 ON
  } else {
    digitalWrite(MOSFET_PIN_2, LOW);  // Turn MOSFET 2 OFF
  }
}

// Interrupt routine for Channel 1 (Pin D2) via standard attachInterrupt
void readPWM_1() {
  unsigned long now = micros();
  if (digitalRead(INPUT_PIN_1) == HIGH) {
    pulseStart_1 = now;
  } else {
    if (pulseStart_1 > 0) {
      pulseWidth_1 = now - pulseStart_1;
    }
  }
}

// Instant Pin Change Interrupt vector for Port D (Handles Pin D4)
ISR(PCINT2_vect) {
  unsigned long now = micros();
  // Check the current digital state of Pin D4 directly using fast register reading
  if ((PIND & (1 << PIND4)) != 0) { 
    pulseStart_2 = now;             // Pin D4 went HIGH
  } else {
    if (pulseStart_2 > 0) {
      pulseWidth_2 = now - pulseStart_2; // Pin D4 went LOW
    }
  }
}