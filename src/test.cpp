// defines pins numbers
const int stepPin1 = 3; 
const int dirPin1 = 2; 
const int stepPin2 = 5; 
const int dirPin2 = 4; 
int steps;
bool dir;
int motor;
int pin;
int dirPin;
String input;
String input2;
String input3;
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin1,OUTPUT); 
  pinMode(dirPin1,OUTPUT);
  pinMode(stepPin2,OUTPUT); 
  pinMode(dirPin2,OUTPUT);
  Serial.begin(9600);
  Serial.println("Motor(0-1),Steps(0-500),Direction(CW-CCW)");
}
void loop(){
    if(Serial.available()){
      input = Serial.readStringUntil(',');
      input2 = Serial.readStringUntil(',');
      input3 = Serial.readStringUntil('\n');
      input3.toUpperCase();
    motor = input.toInt();
    steps = input2.toInt();
    if(input3.equals("CW")){
      dir = true;
      }
    else if(input3.equals("CCW")){
      dir = false;
      }
    else{
      Serial.println("Meine Güte du dummer Hund da gibt man dir einmal was du willst und dann dass, die 13 14 Jährigen die du bei mir bestellt hast kannst du übrigens abholen.");
      }
    Serial.print("Motor: ");
    Serial.println(motor);
    Serial.print("Steps: ");
    Serial.println(steps);
    Serial.print("Direction: ");
    Serial.println(input3);
    // Makes 500 pulses for making one full cycle rotation
    if(motor==0){
    pin = stepPin1;
    dirPin = dirPin1;
      }
    if(motor==1){
    pin = stepPin2;
    dirPin = dirPin2;
      }
    digitalWrite(dirPin, dir);
    for(int x = 0; x < steps; x++){
      digitalWrite(pin,HIGH);
      delayMicroseconds(500); 
      digitalWrite(pin,LOW); 
      delayMicroseconds(500);
    }
    delay(250);
    Serial.println("Motor(0-1),Steps(0-500),Direction(CW-CCW)");
    }
  if (blueToothVal=='1') //wenn das Bluetooth Modul eine „1“ empfängt..
  {
    digitalWrite(13,HIGH);   //...soll die LED leuchten
    if (lastValue!='1') //wenn der letzte empfangene Wert keine „1“ war...
      Serial.println(F("LED is on")); //..soll auf dem Seriellen Monitor „LED is on“ angezeigt werden
    lastValue=blueToothVal;
  }
  else if (blueToothVal=='0') //wenn das Bluetooth Modul „0“ empfängt...
  {           
    digitalWrite(13,LOW);  //..soll die LED nicht leuchten
    if (lastValue!='0')  //wenn der letzte empfangene Wert keine „0“ war...
      Serial.println(F("LED is off")); //..soll auf dem seriellen Monitor „LED is off“ angezeigt werden 
    lastValue=blueToothVal;
  }

} 
