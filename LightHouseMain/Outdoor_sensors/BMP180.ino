void rd_bmp180_temperature(void){
  char status;
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0) {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(temp_bmp180);
    if (status == 0){
       Serial.println("error retrieving temperature measurement\n");
    }
  }
  else ;  //Serial.println("error starting temperature measurement\n");

}

void rd_bmp180_pressure(void){
  char status;
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  
  status = pressure.startPressure(3);
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed pressure measurement:
    // Note that the measurement is stored in the variable P.
    // Note also that the function requires the previous temperature measurement (T).
    // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getPressure(pres_bmp180,temp_bmp180);
    if (status == 0)
    {
       //Serial.println("error retrieving pressure measurement\n");
    }
  }
  else ;  //Serial.println("error starting pressure measurement\n");

}

