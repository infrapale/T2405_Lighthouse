// T055  SMT16030 Library test file
// 2011-04-25 
// This example code is in the public domain.

#include <SMT16030.h>

SMT16030 temp_01(7);
float t01;

void setup()
{
  Serial.begin(9600);  // start serial for output
  temp_01.begin(20.0);
}

void loop()
{
  t01 = temp_01.rd_temp();
  Serial.println(t01,4);
  delay(1000);
}
