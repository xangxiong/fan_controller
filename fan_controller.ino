#include <math.h>

// Analog Channel to Read Temperature from Temp Sensor
const int TEMP_ANALOG_CHN = 0;
// Digital Channel to determine if Manual Switch Override is Activated
const int ENABLE_DIGITAL_CHN = 7;
// Digital Channel to trigger Fan activation
const int TRIGGER_DIGITAL_CHN = 8;

// ######### Change These Settings specific to your Car's optimal operating temperature #########
// Temperature threshold trigger which will deactivate Fan
const double LOW_TEMP_TRIGGER = 190.0f;
// Temperature threshold trigger which will activate Fan
const double HIGH_TEMP_TRIGGER = 200.0f;
// ######### End #########

//Function to perform the fancy math of the Steinhart-Hart equation
double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  // Convert Kelvin to Celsius
  Temp = Temp - 273.15;
  // Celsius to Fahrenheit - comment out this line if you need Celsius
  Temp = (Temp * 9.0)/ 5.0 + 32.0;
  return Temp;
}

void setup() {
  // setup to allow com port serial output capture at 115200 Baud Rate
  Serial.begin(115200);

  // set the enable digital channel as input
  pinMode(ENABLE_DIGITAL_CHN, INPUT);
  // set the trigger digital channel as output
  pinMode(TRIGGER_DIGITAL_CHN, OUTPUT);
}

void loop() {
  int enabled;
  double temp;

  enabled = digitalRead(ENABLE_DIGITAL_CHN);
  if (enabled == HIGH) {
    // Read analog value and convert to temperature
    temp = Thermister(analogRead(TEMP_ANALOG_CHN));
    // Print the captured values
    Serial.print("Water Temp: ");
    Serial.println(temp);

    if (temp > HIGH_TEMP_TRIGGER) {
      // the temperature reading has surpassed our configured high threshold, time to activate the fan
      digitalWrite(TRIGGER_DIGITAL_CHN, HIGH);
    } else if (temp < LOW_TEMP_TRIGGER) {
      // the temperature reading has dropped below our configured low threshold, time to deactivate the fan
      digitalWrite(TRIGGER_DIGITAL_CHN, LOW);
    }
  } else {
    // user has disabled temp reading by activating manual switch
    Serial.println("Temp monitoring disabled");
  }
  
  // Wait 1000ms (1s) to run again
  delay(1000);
}
