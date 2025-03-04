const int AOUTpin=1;               //the AOUT pin of the MQ-4 sensor goes into analog pin A1 of the arduino.

const int DOUTpin=7;              //the DOUT pin of the MQ-4 sensor goes into digital pin D7 of the arduino.

const int ledPin=13;             //the anode of the LED connects to digital pin D13 of the arduino Same for buzzer.



int limit;

int value;



void setup() {

Serial.begin(115200);            //sets the baud rate.

pinMode(DOUTpin, INPUT);        //sets the pin as an input to the arduino.

pinMode(ledPin, OUTPUT);        //sets the pin as an output of the arduino.

}



void loop()

{

value= analogRead(AOUTpin);      //reads the analaog value from the MQ-4 sensor's AOUT pin.

limit= digitalRead(DOUTpin);     //reads the digital value from the MQ-4 sensor's DOUT pin.

Serial.print("Methane value: ");

Serial.println(value);           //prints the Butane value.

Serial.print("Limit: ");

Serial.print(limit);             //prints the limit reached as either LOW or HIGH.

delay(100);

if (limit == HIGH){

digitalWrite(ledPin, LOW);       //if limit has been reached,Both LED and Buzzer turns on as status indicator.

}

else{

digitalWrite(ledPin, HIGH);      //if threshold not reached, LED remains off.

}

}