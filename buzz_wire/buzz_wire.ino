// Buzz Wire v1 (ESP32 DevKit V1 + DY-SV17F in I/O Independent mode 0)
// Touch the start pad to begin, get to the finish pad without touching the wire.
// 3 lives: each touch boos and puts out a red LED. Lose all 3: evil laugh. Reach the finish: cheer.
// Board: "DOIT ESP32 DEVKIT V1" (or "ESP32 Dev Module"). No libraries needed.

// ---------- Inputs (wand is GND, touching pulls these LOW) ----------
const byte WIRE_PIN   = 32;
const byte START_PIN  = 33;
const byte FINISH_PIN = 27;

// ---------- LEDs (330R to each anode, cathodes to GND) ----------
const byte GREEN_LED = 23;
const byte AMBER_LED = 22;
const byte LIFE_LED[3] = { 21, 25, 26 };
const byte HEARTBEAT_LED = 2;          // Blue LED on the ESP32 board

// ---------- DY-SV17F triggers (pull LOW briefly to play) ----------
const byte SND_BOO     = 19;           // IO0 -> 00001.mp3
const byte SND_CHEER   = 18;           // IO1 -> 00002.mp3
const byte SND_LAUGH   = 17;           // IO3 -> 00004.mp3
const byte SND_ATTRACT = 16;           // IO4 -> 00005.mp3

// ---------- Tuning ----------
const byte LIVES = 3;
const unsigned long GRACE_MS     = 1500;   // After a touch, ignore the wire this long
const unsigned long DEBOUNCE_MS  = 8;      // Touch must last this long to count
const unsigned long GAME_TIMEOUT = 90000;  // Abandoned game returns to waiting
const unsigned long ATTRACT_MIN  = 60000;  // "Come and play" every 1–3 minutes while waiting
const unsigned long ATTRACT_MAX  = 180000;

enum State { WAITING, PLAYING, WON, LOST };
State state = WAITING;
byte lives = LIVES;
unsigned long stateSince = 0, lastHit = 0, lastActivity = 0, nextAttract = 0;

void playSound(byte pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(100);
  pinMode(pin, INPUT);                 // Release: the module's own pull-up takes it high
}

bool touching(byte pin) {              // LOW for DEBOUNCE_MS = a real touch
  if (digitalRead(pin) == HIGH) return false;
  delay(DEBOUNCE_MS);
  return digitalRead(pin) == LOW;
}

void showLives() {
  for (byte i = 0; i < LIVES; i++) digitalWrite(LIFE_LED[i], i < lives ? HIGH : LOW);
}

void allOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(AMBER_LED, LOW);
  for (byte i = 0; i < LIVES; i++) digitalWrite(LIFE_LED[i], LOW);
}

void enter(State s) {
  state = s;
  stateSince = millis();
  allOff();
  switch (s) {
    case WAITING:
      Serial.println(F("Waiting: touch the start pad"));
      nextAttract = millis() + random(ATTRACT_MIN, ATTRACT_MAX);
      break;
    case PLAYING:
      Serial.println(F("Go!"));
      lives = LIVES;
      lastHit = lastActivity = millis();
      digitalWrite(AMBER_LED, HIGH);
      showLives();
      break;
    case WON:
      Serial.print(F("Finished with "));
      Serial.print(lives);
      Serial.println(F(" lives left"));
      playSound(SND_CHEER);
      break;
    case LOST:
      Serial.println(F("Game over"));
      playSound(SND_LAUGH);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(WIRE_PIN, INPUT_PULLUP);
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(FINISH_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(AMBER_LED, OUTPUT);
  for (byte i = 0; i < LIVES; i++) pinMode(LIFE_LED[i], OUTPUT);
  pinMode(HEARTBEAT_LED, OUTPUT);
  pinMode(SND_BOO, INPUT);             // Left as inputs so nothing plays at power-up
  pinMode(SND_CHEER, INPUT);
  pinMode(SND_LAUGH, INPUT);
  pinMode(SND_ATTRACT, INPUT);
  randomSeed(esp_random());

  // Lamp test: every LED on for 1 s
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(AMBER_LED, HIGH);
  for (byte i = 0; i < LIVES; i++) digitalWrite(LIFE_LED[i], HIGH);
  delay(1000);
  Serial.println(F("Buzz wire ready"));
  enter(WAITING);
}

void loop() {
  unsigned long now = millis();
  digitalWrite(HEARTBEAT_LED, (now % 2000) < 50);

  switch (state) {
    case WAITING:
      digitalWrite(GREEN_LED, (now % 2000) < 1000);   // Slow green blink
      if (touching(START_PIN)) enter(PLAYING);
      else if ((long)(now - nextAttract) >= 0) {
        playSound(SND_ATTRACT);
        nextAttract = now + random(ATTRACT_MIN, ATTRACT_MAX);
      }
      break;

    case PLAYING:
      if (touching(FINISH_PIN)) { enter(WON); break; }
      if (now - lastHit > GRACE_MS && touching(WIRE_PIN)) {
        lastHit = lastActivity = now;
        lives--;
        Serial.print(F("Touch! Lives left: "));
        Serial.println(lives);
        if (lives == 0) { enter(LOST); break; }
        playSound(SND_BOO);
        for (byte f = 0; f < 3; f++) {               // Flash the remaining lives
          for (byte i = 0; i < LIVES; i++) digitalWrite(LIFE_LED[i], LOW);
          delay(80);
          showLives();
          delay(80);
        }
      }
      if (touching(START_PIN)) lastActivity = now;   // Back at the start: still playing
      if (now - lastActivity > GAME_TIMEOUT) enter(WAITING);
      break;

    case WON:                                        // Green and amber flash for 5 s
      digitalWrite(GREEN_LED, (now / 150) % 2);
      digitalWrite(AMBER_LED, !((now / 150) % 2));
      if (now - stateSince > 5000) enter(WAITING);
      break;

    case LOST:                                       // All reds flash for 5 s
      for (byte i = 0; i < LIVES; i++) digitalWrite(LIFE_LED[i], (now / 120) % 2);
      if (now - stateSince > 5000) enter(WAITING);
      break;
  }
  delay(2);
}
