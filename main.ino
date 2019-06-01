#include <stdio.h>
#include <math.h>

#define MAX7219_DIN P1_5
#define MAX7219_CS  P2_0
#define MAX7219_CLK P2_1

///////////////////////////////////////////////////////
// Enter destination in dd.mmmmm (dd° mm.mmm) format //
const float destin_loc[] = {10.08318,-1.79153};      //
///////////////////////////////////////////////////////

int wait = 100;
float last_loc[] = {0.00000,0.00000};
float current_loc[] = {0.00000,0.00000};
float tmp_loc[] = {0.00000,0.00000};

int incomingByte = 0;   // for incoming serial data
char buffer[2];
char dollar[2] = "$";
char gpsmatch[7];

char time[21];
char lat[21];
float lat_f;
char lat_dir[6];
char lon[21];
float lon_f;
double lon_f2;
char lon_dir[6];

char zero[2] = "0";
char point[2] = ".";

int lat_length;
int lat_point;
int lon_length;
int lon_point;

int counter;
char south[2] = "S";
char west[2] = "W";

int loopnum = 0;

void initialise()
{
  digitalWrite(MAX7219_CS, HIGH);
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_CS, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
}

void output(byte address, byte data)
{
  digitalWrite(MAX7219_CS, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, address);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, data);
  digitalWrite(MAX7219_CS, HIGH);
  write8x8(0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0);
}

void setTestMode(boolean on)
{
  output(0x0f, on ? 0x01 : 0x00);
}

void setShutdown(boolean off)
{s
  output(0x0c, off ? 0x00 : 0x01); //shutdown register - normal operation
}

void showDigits(byte numDigits)
{
  output(0x0b, numDigits-1); //scan limit register
}

void setBrightness(byte brightness)
{
  output(0x0a, brightness); //intensity register - max brightness
}

void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);           // get bitmask
    digitalWrite(MAX7219_CLK, LOW);   // tick
    if (data & mask){                s // choose bit
      digitalWrite(MAX7219_DIN, HIGH);// send 1
    }else{
      digitalWrite(MAX7219_DIN, LOW); // send 0
    }
    digitalWrite(MAX7219_CLK, HIGH);  // tock
    --i;                              // move to lesser bit
  }
}

void maxSingle(byte reg, byte col) {
  digitalWrite(MAX7219_CS, LOW);       // CS has to transition from LOW to HIGH    
  putByte(reg);                        // specify register
  putByte(col);                        // put data  
  digitalWrite(MAX7219_CS, LOW);       // Load by switching CS HIGH
  digitalWrite(MAX7219_CS, HIGH);
}

void write8x8(byte a, byte b, byte c, byte d, byte e, byte f, byte g, byte h){
   maxSingle(1,a);
   maxSingle(2,b);
   maxSingle(3,c);
   maxSingle(4,d);
   maxSingle(5,e);
   maxSingle(6,f);
   maxSingle(7,g);
   maxSingle(8,h);
  delay(wait);
}

void setup() {
    Serial.begin(9600);  // opens serial port, sets data rate to 9600 bps
    initialise();
    setTestMode(false);
    setShutdown(false);
    setBrightness(0x0f); // Brightness range 1..0x0f
    showDigits(8);       // Make sure all digits are visible
    output(0x09, 0);     // using an led matrix (not digits)
}

void loop() {
        Serial.println("///////////////////////////////////////////////////////////////////////////////////////////");
        Serial.println("Loop number");
        Serial.println(loopnum);
        loopnum += 1;

        if (Serial.available() > 0) {
                // wait until incoming byte = $
                Serial.readBytes(buffer,1);
                while(buffer[0] != dollar[0]){
                     Serial.readBytes(buffer,1);
                }
                //Check if = GPGGA
                Serial.readBytes(gpsmatch,6);
                if(gpsmatch[0] == 'G' && gpsmatch[1] == 'P' && gpsmatch[2] == 'G' && gpsmatch[3] == 'G' && gpsmatch[4] == 'A'){
                    Serial.readBytesUntil(',',time,20);
                    lat_length = Serial.readBytesUntil(',',lat,20);
                    Serial.readBytesUntil(',',lat_dir,5);
                    lon_length = Serial.readBytesUntil(',',lon,20);
                    Serial.readBytesUntil(',',lon_dir,5);
                    
                    lat_point = 0;
                    lat_f = 0.0;
                    lon_point = 0;
                    lon_f = 0.0;
                    
                    while(lat[lat_point] != point[0]){
                        lat_point = lat_point + 1;
                    }
                    while(lon[lon_point] != point[0]){
                        lon_point = lon_point + 1;
                    }
                    counter = 0;
                    while(counter < lat_length){
                        if(lat[counter] != point[0]){
                          
                          if((float(lat_point)-counter) == 5.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 10000.0);
                          }
                          else if((float(lat_point)-counter) == 4.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 1000.0);
                          }
                          else if((float(lat_point)-counter) == 3.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 100.0);
                          }
                          else if((float(lat_point)-counter) == 2.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 10.0);
                          }
                          else if((float(lat_point)-counter) == 1.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 1.0);
                          }
                          else if((float(lat_point)-counter) == -1.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 0.1);
                          }
                          else if((float(lat_point)-counter) == -2.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 0.01);
                          }
                          else if((float(lat_point)-counter) == -3.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 0.001);
                          }
                          else if((float(lat_point)-counter) == -4.00){
                              lat_f = lat_f + ((float(lat[counter])-48.0) * 0.0001);
                          }
                        }
                        counter = counter + 1;
                    }
                    
                    counter = 0;
                    while(counter < lon_length){
                        if(lon[counter] != point[0]){
                          if((float(lon_point)-counter) == 5.00){
                              //Serial.println(((float(lon[counter])-48.0) * 10000.0));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 10000.0);
                          }
                          else if((float(lon_point)-counter) == 4.00){
                              //Serial.println(((float(lon[counter])-48.0) * 1000.0));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 1000.0);
                          }
                          else if((float(lon_point)-counter) == 3.00){
                              //Serial.println(((float(lon[counter])-48.0) * 100.0));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 100.0);
                          }
                          else if((float(lon_point)-counter) == 2.00){
                              //Serial.println(((float(lon[counter])-48.0) * 10.0));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 10.0);
                          }
                          else if((float(lon_point)-counter) == 1.00){
                              //Serial.println(((float(lon[counter])-48.0) * 1.0));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 1.0);
                          }
                          else if((float(lon_point)-counter) == -1.00){
                              //Serial.println(((float(lon[counter])-48.0) * 0.1));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 0.1);
                          }
                          else if((float(lon_point)-counter) == -2.00){
                              //Serial.println(((float(lon[counter])-48.0) * 0.01));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 0.01);
                          }
                          else if((float(lon_point)-counter) == -3.00){
                              //Serial.println(((float(lon[counter])-48.0) * 0.001));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 0.001);
                          }
                          else if((float(lon_point)-counter) == -4.00){
                              //Serial.println(((float(lon[counter])-48.0) * 0.0001));
                              lon_f = lon_f + ((float(lon[counter])-48.0) * 0.0001);
                          }
                        }
                        counter = counter + 1;
                    }

                    if(lat_dir[0] == south[0]){
                        lat_f = lat_f * -0.01;
                    }
                    else{
                        lat_f = lat_f * 0.01;
                    }
                    if(lon_dir[0] == west[0]){
                        lon_f = lon_f * -0.01;
                    }
                    else{
                        lon_f = lon_f * 0.01;
                    }
                    tmp_loc[0] = lat_f;
                    tmp_loc[1] = lon_f;
                    
                    if(tmp_loc[0] != current_loc[0] || tmp_loc[1] != current_loc[1]){
                        last_loc[0] = current_loc[0];
                        last_loc[1] = current_loc[1];
                        current_loc[0] = tmp_loc[0];
                        current_loc[1] = tmp_loc[1];
                        
                        float delta_x = current_loc[1] - last_loc[1];
                        float delta_y = current_loc[0] - last_loc[0];
                        float theta = atan(delta_y/delta_x) * 57.2958;
                        if(delta_x < 0){
                          theta = theta + 180;
                        }
                        else if(delta_y < 0){
                          theta = theta + 360;
                        }
                        Serial.println("Current heading:");
                        Serial.println(theta);
                        

                        float destin_x = destin_loc[1] - current_loc[1];
                        float destin_y = destin_loc[0] - current_loc[0];
                        float destin_theta = atan(destin_y/destin_x) * 57.2958;
                        if(destin_x < 0){
                          destin_theta = destin_theta + 180;
                        }
                        else if(destin_y < 0){
                          destin_theta = destin_theta + 360;
                        }
                        
                        Serial.println("Heading to destination:");
                        Serial.println(destin_theta);
                        
                        float delta_theta = destin_theta - theta;
                        
                        if(delta_theta < 0){
                          delta_theta = delta_theta + 360;
                        }
                        else{  
                          delta_theta = delta_theta;
                        }
                        
                        
  
                        if(delta_theta < 22.5 || delta_theta > 337.5){
                          write8x8(0x18,0x0,0x0,0x18,0x18,0x0,0x0,0x0);
                          Serial.println("Heading Up");
                        }
                        else if(delta_theta < 67.5){
                          write8x8(0x40,0x80,0x0,0x18,0x18,0x0,0x0,0x0);
                          Serial.println("Heading Up Left");
                        }
                        else if(delta_theta < 112.5){
                          write8x8(0x0,0x0,0x0,0x98,0x98,0x0,0x0,0x0);
                          Serial.println("Heading Left");
                        }
                        else if(delta_theta < 157.5){
                          write8x8(0x0,0x0,0x0,0x18,0x18,0x0,0x80,0x40);
                          Serial.println("Heading Down Left");
                        }
                        else if(delta_theta < 202.5){
                          write8x8(0x0,0x0,0x0,0x18,0x18,0x0,0x0,0x18);
                          Serial.println("Heading Down");
                        }
                        else if(delta_theta < 247.5){
                          write8x8(0x0,0x0,0x0,0x18,0x18,0x0,0x1,0x2);
                          Serial.println("Heading Down Right");
                        }
                        else if(delta_theta < 292.5){
                          write8x8(0x0,0x0,0x0,0x19,0x19,0x0,0x0,0x0);
                          Serial.println("Heading Right");
                        }
                        else{
                          write8x8(0x2,0x1,0x0,0x18,0x18,0x0,0x0,0x0);
                          Serial.println("Heading Up Right");
                        }
                        
                    }
                    else if (tmp_loc[0] == destin_loc[0] && tmp_loc[1] == destin_loc[1]){
                        write8x8(0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
                    }
                  }     
        }
}
