const int sensorPin = A0; // Analog pin connected to the MQ-4 sensor
float sensorValue;
float methaneConcentration; // Concentration of methane in ppm (parts per million)

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read sensor value
  sensorValue = analogRead(sensorPin);
  
  // Convert sensor value to methane concentration (adjust based on calibration)
  methaneConcentration = map(sensorValue, 0, 1023, 0, 10000); // Example calibration
  Serial.println(methaneConcentration);
  // Check if methane concentration exceeds threshold (adjust based on specific food and conditions)
  if (methaneConcentration < 250) { // Example threshold
    Serial.println("Food may be spoiled!");
    // Add additional actions here, e.g., sounding an alarm
  } else {
    Serial.println("Food is fresh.");
  }
  
  delay(100); // Delay for 1 second before next reading
}
