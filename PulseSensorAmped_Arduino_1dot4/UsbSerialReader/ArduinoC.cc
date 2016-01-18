#include <fcntl.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string>
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <sstream>
#include <vector>

using namespace std;

struct ArduinoData {
  string direction;
  int pulseSensorStableValue;
  int pulseSensorRealValue;
  int skinResistenceStableValue;
  int skinResistenceRealValue;
  int hungry;
} ;

class Arduino
{

  string str, last_str;

  public:
    void getData();
    ArduinoData getGameData();
};




void Arduino::getData() 
{
   unsigned char buffer[16]; 
  size_t offset = 0; 
  size_t bytes_read; 
  int i; 
  
 
  /* Open the file for reading.  */ 
  int fd = open ("/dev/cu.usbmodem1411", O_RDONLY & F_SETFL); 

  /* Read from the file, one chunk at a time. Continue until read 
     "comes up short", that is, reads less than we asked for. 
     This indicates that we've hit the end of the file.  */ 
  while(true){
  do {
    /* Read the next line's worth of bytes.  */ 
    //bytes_read = read (fd, buffer, sizeof (buffer)); 
    /* Print the offset in the file, followed by the bytes themselves.  */ 
    //printf ("0x%06x : ", offset); 
//dona'm valors
    int pos = 0;

    while( pos < 99 ) {
      read(fd, buffer+pos, 1); 
      //printf("%c",buffer[pos]); 
      str += buffer[pos];
              // Note you should be checking the result
      if( buffer[pos] == '$' ){
        last_str=str.c_str();
        str="";
      }
      pos++;
    }
    //printf("%s",buffer); 

    /* Keep count of our position in the file.  */ 
    offset += bytes_read; 
  } 
  while (bytes_read == sizeof (buffer)); 
 }
  
} 

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


ArduinoData Arduino::getGameData(){


  vector<string> vString = split(last_str, ';');

  string direction = split(vString[0].c_str(),'@')[1].c_str();

  int pulseSensorStableValue = atoi(vString[1].c_str());
  int pulseSensroRealValue = atoi(vString[2].c_str());

  int skinResistenceStableValue = atoi(vString[3].c_str());
  int skinResistenceRealValue = atoi(vString[4].c_str());

  string hungryS = split(vString[5].c_str(),'$')[0].c_str();

  int hungry = atoi(hungryS.c_str());

  ArduinoData data;

  data.direction = direction;
  data.pulseSensorStableValue = pulseSensorStableValue;
  data.pulseSensorRealValue = pulseSensroRealValue;
  data.skinResistenceStableValue = skinResistenceStableValue;
  data.skinResistenceRealValue = skinResistenceRealValue;
  data.hungry = hungry;
  

  return data;
}





/*
void stop(){
  close (fd); 
}*/
Arduino arduino;

void readData(){
  
  arduino.getData();
}



int main(int argc, char const *argv[])
{
  


  thread th1 (readData);

  while(true){
    getchar();
    ArduinoData ardData= arduino.getGameData();

    printf("-----------------------------------------------\n");
    printf("Direction: %s\n", ardData.direction.c_str());
    printf("Pulse Sensor Stable Value: %i\n",ardData.pulseSensorStableValue);
    printf("Pulse Sensor Real Value: %i\n",ardData.pulseSensorRealValue);
    printf("Skin Resistence Stable Value: %i\n",ardData.skinResistenceStableValue);
    printf("Skin Resistence Real Value: %i\n",ardData.skinResistenceRealValue);
    printf("Hungry: %i\n",ardData.hungry);
    printf("-----------------------------------------------\n");

    //printf("||||||||||||%s||||||||||||\n",str.c_str() );
  }

  th1.join();




  return 0;
}
