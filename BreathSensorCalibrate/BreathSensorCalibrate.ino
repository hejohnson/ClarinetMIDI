#define BREATHSENSEPIN 10

void setup() {
  Serial.begin(9600);

  Serial.println("Starting calibration...");
  Serial.println("Please do not blow into the sensor");

  int highestResting = 0;
  int currentReading = 0;
  for (int i = 0; i < 200; i++) {
    currentReading = analogRead(BREATHSENSEPIN);
    if (currentReading > highestResting) {
      highestResting = currentReading;
    }
    delay(10);
  }

  Serial.println("Please blow into the device with the maximum force you will play with (where you expect max volume) for the next three seconds");

  int highestPlaying = 0;
  for (int i = 0; i < 200; i++) {
    currentReading = analogRead(BREATHSENSEPIN);
    if (currentReading > highestPlaying) {
      highestPlaying = currentReading;
    }
    delay(10);
  }

  Serial.print("The resting cut off is: ");
  Serial.print(highestResting);
  Serial.print(" The highest playing is: ");
  Serial.println(highestPlaying);

  Serial.println("These values can now be used to calibrate the volume scaling. Thank you.");
  
}

void loop() {
  delay(1000);
}
