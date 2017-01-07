
// This #include statement was automatically added by the Particle IDE.
#include "lib1.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMAX17043/SparkFunMAX17043.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunRHT03/SparkFunRHT03.h"



/******************************************************************************


    Soil Moisture Sensor 1----- Photon - pink 10'
        GND ------------------- GND
        VCC ------------------- D6
        SIG ------------------- A2
        
    Soil Moisture Sensor 2----- Photon - orange 8'
        GND ------------------- GND 
        VCC ------------------- D5
        SIG ------------------- A1

    Soil Moisture Sensor 3----- Photon - green 6'
        GND ------------------- GND
        VCC ------------------- D4
        SIG ------------------- A3

    RTH03 Humidity/Temp Sensor Photon
        VCC (Red) ------------- 3.3V (VCC)
        GND (Black) ----------- GND
        SIG (White) ----------- D3





  Development environment specifics:
  	IDE: Particle Dev
  	Hardware Platform: Particle Photon
                       Particle P1



*******************************************************************************/

#define RHT03_DATA_PIN D3
#define SOIL_MOIST1 A2
#define SOIL_MOIST_POWER1 D6
#define SOIL_MOIST2 A1
#define SOIL_MOIST_POWER2 D5
#define SOIL_MOIST3 A3
#define SOIL_MOIST_POWER3 D4

RHT03 rht;

PRODUCT_ID(2590);  
PRODUCT_VERSION(1);

//Global Variables
String device_uid = ""; // photon hard coded device id, UID
double voltage = 0; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
float humidity = 0;//humidity from rht03
float tempC = 0;// temperature in C from RHT03
float tempF = 0;// temperature in F from RHT03
int count = 0;//This triggers a post and print on the first time through the loop
int val1 = 0;//variable to store soil value
int soil1 = A2;//Declare a variable for the soil moisture sensor 
int soilPower1 = D6;
int val2 = 0;//variable to store soil value
int soil2 = A1;//Declare a variable for the soil moisture sensor 
int soilPower2 = D5;
int val3 = 0;//variable to store soil value
int soil3 = A3;//Declare a variable for the soil moisture sensor 
int soilPower3 = D4;
////////////PHANT STUFF//////////////////////////////////////////////////////////////////
//const char server[] = "data.sparkfun.com";
const char server[] = "ec2-52-40-13-117.us-west-2.compute.amazonaws.com";
const char path[] = "api/metrics/soil"; //old = QGyxppE3dKFd4RrmbrXg
const char port[] = "8080"; // old = Jqyx44PGleUzWRr2Mr4A
//String myIDStr = "notsetyet";
PhantRest phant(server, path, port);
/////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------
void setup()
{
Serial.begin(9600);   // open serial over USB
// RHT03 initializtion

rht.begin(RHT03_DATA_PIN);


pinMode(soilPower1, OUTPUT);//Set D6 as an OUTPUT
digitalWrite(soilPower1, LOW);//Set to LOW so no power is flowing through the sensor

pinMode(soilPower2, OUTPUT);//Set D5 as an OUTPUT
digitalWrite(soilPower2, LOW);//Set to LOW so no power is flowing through the sensor

pinMode(soilPower3, OUTPUT);//Set D4 as an OUTPUT
digitalWrite(soilPower3, LOW);//Set to LOW so no power is flowing through the sensor

// Set up PArticle variables (voltage, soc, and alert):
	Particle.variable("voltage", &voltage, DOUBLE);
	Particle.variable("soc", &soc, DOUBLE);
	
	//myIDStr = Spark.deviceID();

// Set up the MAX17043 LiPo fuel gauge:
	lipo.begin(); // Initialize the MAX17043 LiPo fuel gauge
// Quick start restarts the MAX17043 in hopes of getting a more accurate
// guess for the SOC.
	lipo.quickStart();

// We can set an interrupt to alert when the battery SoC gets too low.
// We can alert at anywhere between 1% - 32%:
//	lipo.setThreshold(20); // Set alert threshold to 20%.

}
//---------------------------------------------------------------
void loop()
{
// Call rht.update() to get new humidity and temperature values from the sensor.
	int updateRet = rht.update();
	
	// If successful, the update() function will return 1.
	// If update fails, it will return a value <0
	if (updateRet == 1)
	{
		// The humidity(), tempC(), and tempF() functions can be called -- after 
		// a successful update() -- to get the last humidity and temperature
		// value 
		float humidity = rht.humidity();
		float tempC = rht.tempC();
		float tempF = rht.tempF();
		
		// Now print the values:
		Serial.println("Humidity: " + String(humidity, 1) + " %");
		Serial.println("Temp (F): " + String(tempF, 1) + " deg F");
		Serial.println("Temp (C): " + String(tempC, 1) + " deg C");
	}
	else
	{
		// If the update failed, try delaying for RHT_READ_INTERVAL_MS ms before
		// trying again.
		delay(RHT_READ_INTERVAL_MS);
	}
	{ /// get deviceID. code block isolated in brackets
      device_uid=System.deviceID();
      Serial.print("Deviceid: ");
      Serial.println(device_uid);
    }
	delay(200);

  
//get soil moisture values from the functions below and print it
Serial.println(readSoil1());

delay(100);


Serial.println(readSoil2());

delay(100);//take a reading every second



Serial.println(readSoil3());
 
delay(100);//take a reading every second


//This time is used so you can test the sensor and see it change in real-time.
//For in-plant applications, you will want to take readings much less frequently.
	
    getSensorData();//Get readings from all sensors

    printInfo();//print readings to serial line
    postToPhant();//upload data to Phant

   delay(12000);//stay awake for 12 seconds to allow for App updates
    //Power down between sends to save power, measured in seconds.
   System.sleep(SLEEP_MODE_DEEP, 300);  //for Particle Photon 5 minutes
    //(includes 20 sec update delay) between postings-change this to alter update rate
   
}

//---------------------------------------------------------------
void printInfo()
{
  //This function prints the sensor data out to the default Serial Port
      Serial.print("Humidity:");
      Serial.print(humidity);
      Serial.print("H, ");
      Serial.print("Temp C:");
      Serial.print(tempC);
      Serial.print("C, ");
      Serial.print("Temp F:");
      Serial.print(tempF);
      Serial.print("F, ");
      Serial.print("Soil_Moisture1:");
      Serial.print(val1);
      Serial.print(", ");
      Serial.print("Soil_Moisture2:");
      Serial.print(val2);
      Serial.print(", ");
      Serial.print("Soil_Moisture3:");
      Serial.print(val3);
      Serial.print(", ");
      Serial.print("Voltage: ");
	  Serial.print(voltage);  // Print the battery voltage
	  Serial.println(" V");
      Serial.print("Percentage: ");
      Serial.print(soc); // Print the battery state of charge
      Serial.println(" %");
	  Serial.println();

}
//---------------------------------------------------------------
//This is a function used to get the soil moisture content of probe 1
int readSoil1()
{
    digitalWrite(soilPower1, HIGH);//turn D6 "On"
    delay(10);//wait 10 milliseconds 
    val1 = analogRead(soil1);
    digitalWrite(soilPower1, LOW);//turn D6 "Off"
    return val1;
}

//---------------------------------------------------------------
//This is a function used to get the soil moisture content of probe 2
int readSoil2()
{
    digitalWrite(soilPower2, HIGH);//turn D5 "On"
    delay(10);//wait 10 milliseconds 
    val2 = analogRead(soil2);
    digitalWrite(soilPower2, LOW);//turn D5 "Off"
    return val2;
}
//---------------------------------------------------------------


//This is a function used to get the soil moisture content of probe 3
int readSoil3()
{
    digitalWrite(soilPower3, HIGH);//turn D4 "On"
    delay(10);//wait 10 milliseconds 
    val3 = analogRead(soil3);
    digitalWrite(soilPower3, LOW);//turn D4 "Off"
    return val3;
}
//---------------------------------------------------------------

void getSensorData()
{

    getBattery();//Updates battery voltage, state of charge and alert threshold



}

//---------------------------------------------------------------
void getBattery()
{
  // lipo.getVoltage() returns a voltage value (e.g. 3.93)
voltage = lipo.getVoltage();
// lipo.getSOC() returns the estimated state of charge (e.g. 79%)
soc = lipo.getSOC();


}
//---------------------------------------------------------------

//---------------------------------------------------------------
int postToPhant()//sends datat to data.sparkfun.com
{
    
   
    //test
    phant.add("battery", soc);
    phant.add("deviceid", device_uid);
    phant.add("soilmoisture1", val1);
    phant.add("soilmoisture2", val2);
    phant.add("soilmoisture3", val3);
    phant.add("humidity", rht.humidity(), 1); 
    phant.add("tempc", rht.tempC(), 1); 
    phant.add("tempf", rht.tempF(), 1);
	phant.add("volts", voltage);
	//phant.add("botID", myIDStr);

    TCPClient client;
    char response[512];
    int i = 0;
    int retVal = 0;

    if (client.connect(server, 8080))
    {
        Serial.println("Posting!");
        client.print(phant.post());
        
        //client.print("{\"beg\": \"beg\", \"battery\":\"155555\",\"humidity\":\"88\",\"soilmoisture1\":\"45\",\"soilmoisture2\":\"58\",\"soilmoisture3\":\"25\"}");
        delay(1000);
        while (client.available())
        {
            char c = client.read();
            Serial.print(c);
            if (i < 512)
                response[i++] = c;
        }
        if (strstr(response, "200 OK"))
        {
            Serial.println("Post success!");
            retVal = 1;
        }
        else if (strstr(response, "400 Bad Request"))
        {
            Serial.println("Bad request");
            retVal = -1;
        }
        else
        {
            retVal = -2;
        }
    }
    else
    {
        Serial.println("connection failed");
        retVal = -3;
    }
    client.stop();
    return retVal;

}
