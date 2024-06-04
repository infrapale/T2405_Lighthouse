void rd_dht22_temperature(void){
  dht.temperature().getEvent(&Sensor1);
  if (isnan(Sensor1.temperature)) {
    //Serial.println("Error reading temperature!");
   }
   else {
     temp_dht22 = Sensor1.temperature;
     //Serial.print("Temperature: ");Serial.print(Sensor1.temperature);Serial.println(" *C");
   }
}

void rd_dht22_humidity(void){
  dht.humidity().getEvent(&Sensor1);
  if (isnan(Sensor1.relative_humidity)) {
    //Serial.println("Error reading humidity!");
   }
   else {
      hum_dht22 = Sensor1.relative_humidity;
      //Serial.print("Humidity: ");Serial.print(Sensor1.relative_humidity);Serial.println("%");
   }
}
