//define pin varaibles 

const int trigPin = 25;
const int echoPin= 26;

//intermediate step to get distance 
long duration ;


//store actual distance in cm 
long distance ;


// create a Queue handler 
QueueHandle_t  xQueueENCODER_Readings;


// Store data for  time and distance 

 struct ENCODER_Motion 
  {
    int    Current_Day ;
    long  Current_Distance;
  } ENCODER_MOTION_Read;


  // two instances of structs 
   struct   ENCODER_Motion   ENCODER_MOTION_NOW;
   struct      ENCODER_Motion   ENCODER_Received;

  
void setup() {
pinMode(trigPin,OUTPUT);
 pinMode(echoPin,INPUT);

 Serial.begin(115200);
 delay(1000);
 
// create a struct queue of size 10 
 xQueueENCODER_Readings  = xQueueCreate( 10  , sizeof( struct  ENCODER_Motion ) );


 /// checking if queue was made correctly 
  if(xQueueENCODER_Readings==NULL){
      Serial.println("ERROR Creating queue");
    }
     

/// giving time for the sensor to collect data since is I/O bound 
for( int i = 0  ; i < 10 ; i++ ){
      
        ENCODER_MOTION_Read.Current_Day= 1000 ;
           
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
        
        Serial.print("Initial distance:");
        Serial.println(distance);
         
        ENCODER_MOTION_Read.Current_Distance = distance;

        //send data to queue  
       xQueueSend(xQueueENCODER_Readings,(void *) &ENCODER_MOTION_Read, portMAX_DELAY );
       
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
    Serial.print(ENCODER_Received.Current_Day);
    Serial.print(" ");
    Serial.print(ENCODER_Received.Current_Distance);
    Serial.print("|");
   }
   Serial.println(); 
  }
}
