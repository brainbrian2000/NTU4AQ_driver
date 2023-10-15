/*
  SD card test

  This example shows how use the utility libraries on which the'
  SD library is based in order to get info about your SD card.
  Very useful for testing a card when you're not sure whether its working or not.

  The circuit:
    SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 		Pin 4 used here for consistency with other Arduino examples


  created  28 Mar 2011
  by Limor Fried
  modified 9 Apr 2012
  by Tom Igoe
*/

// include the SD library:
#include <SPI.h>
#include <SD.h>
// File test_file;
// File test_file1, test_file2, test_file3;
// set up variables using the SD utility library functions:
// Sd2Card card;
// SdVolume volume;
// SdFile root;
const String Name = "test0";
const String File_path = "data/NTU4AQ/"+Name+".csv";
const int chipSelect = 53;//arduino mega ->@ NTU4AQ ,need to check the layout of other board.
// char File_path_const[22] = "data/NTU4AQ/Test1.csv";
struct I2C_String {
  String WindSpeed="1";
  String WindDirection="200";
  String Rain="0";
  String PM010="20";
  String PM025="10";
  String PM100="5";
  String TEMP="32.6";
  String HUM="56";
  String kPa="1013";
  String LUX="56";
  String dBA="75";
  String rtcDate="00000000";
  String rtcTime="00001111laland";
  String output(){
    String output = "NTU4AQ";
    output = output+Name;
    output = output+",";
    output = output+TEMP;
    output = output+",";
    output = output+HUM;
    output = output+",";
    output = output+PM100;
    output = output+",";
    output = output+PM025;
    output = output+",";
    output = output+PM010;
    output = output+",";
    output = output+WindSpeed;
    output = output+",";
    output = output+LUX;
    output = output+",";
    output = output+dBA;
    output = output+",";
    output = output+rtcDate;
    output = output+",";
    output = output+rtcTime+"\n";
    return output;
  };
  void write_sd(String Writing,String Path = File_path){
    // file in "data/NTU4AQ", check the file exists or not, if not, mkdir:
  // Serial.println("Now in "+Path+"\n"+Writing);
    if(!SD.exists("data")){
      SD.mkdir("data");
    }
    if(!SD.exists("data/NTU4AQ")){
      SD.mkdir("data/NTU4AQ");
    }
    File file;
    file = SD.open(Path,FILE_WRITE);
    int len = Writing.length();
    for(int i=0; i<len; i++){
      file.write(Writing[i]);
    } 
    // Serial.print(Writing);
    file.close();
    return;
  }
  String read_sd(String _path = File_path){
    File file;
    // Serial.println("Now in "+_path);
    file=SD.open(_path,FILE_READ);
    String file_read_output = "";
    if (file) {
      while (file.available()) {
        file_read_output = file_read_output+char(file.read());
      }
    }
    file.close();
    return file_read_output;
  }
  // /*
  void SD_init(){  
  // if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    // Serial.println("initialization failed. Things to check:");
    // Serial.println("* is a card inserted?");
    // Serial.println("* is your wiring correct?");
    // Serial.println("* did you change the chipSelect pin to match your shield or module?");
  // } else {
    // Serial.println("Wiring is correct and a card is present.");
  // }
  // */
    if(!SD.begin(chipSelect)){
      Serial.println('Fail, No SD card!');
    }
    return;
  }
} i2cStr;

// const int chipSelect = 53;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN

#define modulePower 14
void print_testing(){
 Serial.println(Name);
 Serial.println(File_path);
//  Serial.println(File_path_const);
 
 return; 
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("\nStarting writing sd");
  Serial.println("Now in "+File_path);
  i2cStr.SD_init();
  String output_writing,output_reading;
  output_writing = i2cStr.output();
  i2cStr.write_sd(output_writing);
  output_reading = i2cStr.read_sd();
  Serial.print(output_reading);





  // pinMode(modulePower, OUTPUT);     //
  // digitalWrite(modulePower, HIGH);  // 開啟模組電源

  // pinMode(10, OUTPUT);      // 手動控制LoRa 的CS
  // digitalWrite(10, HIGH);   // 上拉LoRa SPI 的CS腳位，避免抓到LoRa

  // Serial.print("\nInitializing SD card...");
  // print_testing();
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  /*
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  // Serial.print("Volume size (Kb):  ");
  // Serial.println(volumesize);
  // Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  // Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  */
/*

  Serial.println("\nStarting writing sd");
  if(!SD.begin(chipSelect)){
    Serial.println('Fail, No SD card!');
  }
  else{
    if(!SD.exists("data")){
      SD.mkdir("data");
    }
    if(!SD.exists("data/NTU4AQ")){
      SD.mkdir("data/NTU4AQ");
    }
    // test_file = SD.open("data/test.csv",FILE_WRITE);

    // test_file1 = SD.open("data/"+Name+".csv",FILE_WRITE);
    // test_file1.close();
    test_file = SD.open(File_path,FILE_WRITE);
    // test_file.close();
    // test_file3 = SD.open(File_path_const,FILE_WRITE);
    // test_file3.close();
    // test_file.write("this,is,the,testing,data,I,just,want,to,try,it,0.2555644843212316849651894531\n");
    String output_temp = "";
    output_temp = i2cStr.output();
    int len_output = output_temp.length();
    for(int i=0; i<len_output; i++){
    test_file.write(output_temp[i]);
    }
    Serial.println("Writing Done.");
    test_file.close();
  }
    // Serial.println("testfile.txt:");
    // test_file = SD.open("datetime/testfile.csv",FILE_READ);
    //一直讀取檔案內容，直到沒有為 止
  test_file = SD.open(File_path,FILE_READ);
  if (test_file) {
    String file_read_output = "";
    while (test_file.available()) {
      file_read_output = file_read_output+char(test_file.read());
      // Serial.print(test_file.read()); //把讀到的內容顯示在監控視窗
    }
    // 關閉檔案
    test_file.close();
    Serial.println(file_read_output);
  }
  Serial.println("Reading done");
  */



}
/*
*/
// '''
void loop(void)
{
}
