
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// --- Mapeamento de Hardware ---
#define  ADDO  7    //Data Out
#define  ADSK  6    //SCK         
#define SD_CS 5



// --- Protótipo das Funções Auxiliares ---
unsigned long ReadCount(); //conversão AD do HX711


// --- Variáveis Globais ---
unsigned long convert;
String dataMessage;

// --- Configurações Iniciais ---
void setup()
{
   pinMode(ADDO, INPUT_PULLUP);   //entrada para receber os dados
   pinMode(ADSK, OUTPUT);         //saída para SCK
   
   Serial.begin(115200);

 // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Dados do Teste Estático \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
} //end setup


// --- Loop Infinito ---
void loop()
{
  
  convert = ReadCount();
  logSDCard();
  
  delay(100); //dúvida sobre o tempo de delay 



} //end loop


// --- Funções ---
unsigned long ReadCount()
{
  unsigned long Count = 0;
  unsigned char i;
  
  digitalWrite(ADSK, LOW);
  
  while(digitalRead(ADDO));
  
  for(i=0;i<24;i++)
  {
     digitalWrite(ADSK, HIGH);
     Count = Count << 1;
     digitalWrite(ADSK, LOW);
     if(digitalRead(ADDO)) Count++;
  
  } //end for
  
  digitalWrite(ADSK, HIGH);
  Count = Count^0x800000;
  digitalWrite(ADSK, LOW);
  
  return(Count);


} //end ReadCount

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage =  " -> " + String(convert) + "\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
