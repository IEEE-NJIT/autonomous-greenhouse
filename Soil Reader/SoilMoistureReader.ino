//const int WATER_VAL = _____ to be declared
//const int AIR_VAL = 474;

int soilMoisture = 0;
int soilmoisturepercent = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  soilMoisture = analogRead(A0);
  soilmoisturepercent = map(soilMoisture, AIR_VAL, WATER_VAL, 0, 100);
  
  Serial.println(soilmoisturepercent);
  delay(1000);
}
