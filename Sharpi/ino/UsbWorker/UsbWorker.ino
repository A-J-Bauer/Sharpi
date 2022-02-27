
#define DEVICEID 1

#define STR_H(x) #x
#define STR(x) STR_H(x)
#define ULC "\xF" STR(DEVICEID) "\xE"

// ------------------------ UsbWorker --------------------------
// 1. set DEVICEID to a unique positive integer value  1, 2,.. 
//
// 2. prepend your data with ULC or send ULC alone.
//
//    example: 
//      Serial.print(ULC); Serial.println("my data 12345");
//    or:
//      Serial.println(ULC);
//      
// 3. if no ULC is sent within x milliseconds (default 1500)
//    the device is considered dead (failure or unplugged)
// -------------------------------------------------------------


void setup() 
{
  Serial.begin(38400);
  
  
  pinMode(LED_BUILTIN, OUTPUT);
  
}

String cmd = "";

void loop() 
{
    while (Serial.available() > 0)
    {
        char cmdChar = Serial.read();        
        
        if (cmdChar == '\n') 
        {
            if (cmd == "on") 
            {
                digitalWrite(LED_BUILTIN, HIGH);
                Serial.print(ULC); Serial.println("Led on");
            }
            else if (cmd == "off")
            {
                digitalWrite(LED_BUILTIN, LOW);
                Serial.print(ULC); Serial.println("Led off");
            }
            else
            {
                Serial.print(ULC); Serial.println("unknown command");
            }

            cmd="";
        }
        else 
        {
            cmd += cmdChar;
        }
  }
  
  Serial.println(ULC);
  
  delay(100);        // delay
 

 
}
