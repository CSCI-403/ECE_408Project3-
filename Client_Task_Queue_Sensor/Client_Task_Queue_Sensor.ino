#include <WiFi.h>
#include <WiFiUdp.h>

/* WiFi network name and password */
const char* ssid = "";
const char* password =  "";

// IP address to send UDP data to.
// it can be ip address of the server or 
// a network broadcast address
// here is broadcast address
const char * udpAddress = "192.25.1.255";
const int udpPort = 44444;

//create UDP instance
WiFiUDP udp;

// -----End of Wifi---- 



//define pin varaibles 

const int trigPin = 25;
const int echoPin= 26;

//intermediate step to get distance 
long duration ;


//store actual distance in cm 
long distance ;


// create a Queue handler 
QueueHandle_t  xQueueENCODER_Readings;

int x = 0 ; 

// Store data for  time and distance 

 struct ENCODER_Motion 
  {
    int    Current_Day ;
    long  Current_Distance;
  } ;


  // two instances of structs 
   struct   ENCODER_Motion   ENCODER_MOTION_NOW;
   struct      ENCODER_Motion   ENCODER_Received;

  
void setup() {
 Serial.begin(115200);

 
//Connect to the WiFi network
   WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //This initializes udp and transfer buffer
  udp.begin(udpPort);

//-------End of Wifi setup -----------




 pinMode(trigPin,OUTPUT);
 pinMode(echoPin,INPUT);

 delay(1000);
 
// create a struct queue of size 10 
 xQueueENCODER_Readings  = xQueueCreate( 10  , sizeof( struct  ENCODER_Motion ) );


 /// checking if queue was made correctly 
  if(xQueueENCODER_Readings==NULL){
      Serial.println("ERROR Creating queue");
    }
     
 
  xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in words. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
  xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in words. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */





                    
 
}
 
void loop() {
  delay(1000);

  
}





 // task one takes care of collecting data and sending to queue 
void taskOne( void * parameter )
{

  portTickType lastTimeRun;
  
  // checks the sensosor evey 1000 ms 
  const TickType_t xFrequency = 1000;

 
// Initialise the lastTimeRun variable with the current time.
    lastTimeRun = xTaskGetTickCount();
    

  // infinite loop of checking sensor data 
    for(  ; ; ){

       
        ENCODER_MOTION_NOW.Current_Day = 100;

        //block the task for specific amount of time 
          vTaskDelayUntil( &lastTimeRun, xFrequency );
           
            //clears TrigPin
            digitalWrite(trigPin,LOW); 
            delayMicroseconds(2); 

              //sets the trigPin HIGH 
            digitalWrite(trigPin, HIGH) ;
            delayMicroseconds(10); 
             digitalWrite(trigPin, LOW) ;

            // Reads echoPin , returns duration 
            duration = pulseIn(echoPin , HIGH); 


            // calculate distance  
            distance = duration*0.034/2 ;

    
        ENCODER_MOTION_NOW.Current_Distance = distance;
        
           
            //Sending to Queue
       xQueueSend(xQueueENCODER_Readings,(void *) &ENCODER_MOTION_NOW, portMAX_DELAY );
       
    }
 
}


 //Takes care of recieving data and displaying to Screen 
 
void taskTwo( void * parameter)
{
 
  // Infininite loop 
  for(  ; ; ){
    
   if (xQueueReceive ( xQueueENCODER_Readings ,  &(ENCODER_Received) , portMAX_DELAY ) ){
    
       udp.beginPacket(udpAddress, udpPort);
      
      if  ( x  < 1) {
        udp.print(" Day: ");
        udp.print(ENCODER_Received.Current_Day);
        //Serial.println(ENCODER_Received.Current_Day);
        udp.print(" Distance: ");
        udp.print(ENCODER_Received.Current_Distance);
        //Serial.println(ENCODER_Received.Current_Distance);
        // end packet 
       udp.endPacket();
        x++; 
      }

     else{
       // end packet 
      // udp.endPacket();

       // reset counter 
       x = 0 ; 
     }
   
   }
   //Serial.println(); 
  }
}
