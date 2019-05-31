void setup() {
  // put your setup code here, to run once:
  for(int i= 0;i<13 ;i++)pinMode( i , INPUT_PULLUP );
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2) == 0)
  {
    Serial.write(0x11);
    while(digitalRead(2)==0);
  }
  else if(digitalRead(3) == 0)
  {
    Serial.write(0x12);
    while(digitalRead(3)==0);
  }
  else if(digitalRead(4) == 0)
  {
    Serial.write(0x13);
    while(digitalRead(4)==0);
  }
  else if(digitalRead(5) == 0)
  {
    Serial.write(0x14);
    while(digitalRead(5)==0);
  }
  else if(digitalRead(6) == 0)
  {
    Serial.write(0x15);
    while(digitalRead(6)==0);
  }
}
