#include <SPI.h>
#include <SD.h>

char buff[561];
#define DEC 10

File root;
File current;
bool is_sd_init_sucess=false;
bool is_init_received=false;
void setup() 
{
   
  is_sd_init_sucess= sdInit();

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 
  //Serial.println("team alpha..!");
  
}
bool sdInit()
{
   // Serial.print("Initializing SD card...");

   if (!SD.begin(4)) {
       return false;
   }
   return true;

}
void loop() 
{
  // Wait for serial input

  if (Serial.available() > 0) 
  {
    String message = Serial.readString();
   // Serial.println(message);
    message.trim();
    if(( message == "init" ) && (!is_init_received) ) {
      is_init_received=true;
      if(is_sd_init_sucess)
           Serial.println("ACK");
      else
           Serial.println("NACK");     

    }
    
    if(is_sd_init_sucess && is_init_received) {
      if (message == "list_dir") 
      {
        //Serial.println("Listing files in the current directory:");
        if(!listDirectories())
            Serial.println("NACK");

        //listFiles(current);
      } 

      else if (message.startsWith("ls ")) 
      {
        String dir_name = message.substring(3); 
        File dir_to_open = SD.open(dir_name);
        if (dir_to_open && dir_to_open.isDirectory()) 
        {
         // Serial.println("Listing files in directory: " + dir_name);
          listFiles(dir_to_open);
          dir_to_open.close();

        } 
        else 
        {
          Serial.print("NACK");
        }
      }
      else if(message.startsWith("cat "))
      {
        String file_name = message.substring(4);
        File file_to_send = SD.open(file_name);
        if(file_to_send)
        {
          //writeString("Start"); 
          sendFile(file_to_send);
          writeByte("End"); 
          file_to_send.close();
          //writeString("File closed");
        }
        else
        {
          Serial.println("NACK");
        }
        //SD.close();
        
        
      }
    }
    else {
      if(is_init_received)
          Serial.println("NACK"); 
    }
   // Serial.flush();
  }
}


void printDirectory(File dir, int numTabs) {

   Serial.println(numTabs);
   while(true) {
     
     File entry =  dir.openNextFile();
     if (!entry) {
       // no more files
       break;
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

bool listDirectories() 
{
  
  File  dir = SD.open("/");
  if(!dir) {
    Serial.printf("NACK");
    return false;
  }  
  bool is_dir_available=false;
  while (true) 
  {
    File entry = dir.openNextFile();
    if (!entry) { // No more files
      break ;
    }

    if (entry.isDirectory()) {
      //Serial.print("Directory: ");
      if(!is_dir_available)
        Serial.println("Start");
   
      is_dir_available=true;
      //Serial.println(entry.name());
      writeString(entry.name());
     // listDirectories(dir);
    }
    entry.close();
   }
   if(is_dir_available)
    Serial.println("End");
  return is_dir_available;
}
void writeString(String stringData) { // Used to serially push out a String with Serial.write()

  for (int i = 0; i < stringData.length(); i++)
  {
     if(Serial.availableForWrite()) {
        
      Serial.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
     }
  }
  Serial.write("\n");   // Push each char 1 by 1 on each loop pass
  delay(10);

}
void writeByte(String stringData) { // Used to serially push out a String with Serial.write()

  for (int i = 0; i < stringData.length(); i++)
  {
     if(Serial.availableForWrite()) {
        
      Serial.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
     }
  }
  //Serial.write("\n");   // Push each char 1 by 1 on each loop pass
  delay(10);

}

void listFiles(File dir) 
{
  writeString("Start");
  //Serial.println("Start");
   while (true) 
   {
      File entry = dir.openNextFile();
      if (!entry) 
      {
         // No more files
         break;
      }
      if (!entry.isDirectory()) 
      {
         //Serial.print("File: ");
            writeString(entry.name());
        
         //Serial.print(" - Size: ");
         //Serial.println(entry.size(), DEC);
      }

      entry.close();
   }
   writeString("End");
   //Serial.flush();

}

void sendFile(File file_to_send)
{
   //writeString("Start"); 
   while(file_to_send.available())
   {
     int bytes = file_to_send.read(buff,561);
     Serial.write(buff,bytes);
     memset(buff,0,sizeof(buff));
   }
  // file_to_send.close();
   //writeString("End");
   //Serial.flush(); 
}