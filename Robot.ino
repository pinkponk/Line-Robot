#include <stdio.h>
#include <stdlib.h>


//Constants
const float  pi = 3.1416;
const int  a = 50000;   //[um] micrometer = skivansradie VET EJ INTE ÄNNU
const int  b = 50000;   //avstånd från hjulbas till centrum av skivan, kan nog ändras till vart man vill köra runt

const float sensor1_offset = -4.0/3*pi;//FUNGERAR!!!! SKA VARA 2*pi/3
const float sensor2_offset = 0;
const float sensor3_offset = -2.0/3*pi;

// pins
const int RotorMotor  = 4;
const int sensor1     = 2;
const int sensor2     = 3;
const int sensor3     = 21;
const int sensorRotor = 20;

//Variables
volatile long sensor_one_in_time = 0;
long sensor_two_in_time = 0;
long sensor_three_in_time = 0;
long sensor_rotor_in_time = 0;
long sensor_rotor_out_time = 0;
long sensor_rotor_in_time_old = 0;
long time_at_zero   =0;
double time_rev       =4000;  //Ungefär
//ROTORN GÅR MOTURS SETT OVANIFRÅN
boolean toWhite = false;


//functions
float  getDiskAngle(long time_in, long time_out, const float offset); 
double* calcNewWayPoint(float  diskAngle);
void create(int  distance,float  vinkel,float  diskAngle);
void insert_beg(int  distance,float  vinkel,float  diskAngle);


// the setup routine runs once when you press reset:
void setup() {

  
  pinMode(4, OUTPUT); 
  pinMode(2, INPUT);
  pinMode(3, INPUT); 
  pinMode(21, INPUT); 
  pinMode(20, INPUT); 
  digitalWrite(4, HIGH);
  
  Serial.begin(9600);
  
  create(0,0,0);
  
  insert_beg(0,0,0);
  insert_beg(0,0,0);
  
  attachInterrupt(3, sensor_rotor_rising, RISING);
  delay(1000);

  attachInterrupt(0, sensor_one_change, CHANGE);
  attachInterrupt(1, sensor_two_change, CHANGE);
  attachInterrupt(2, sensor_three_change, CHANGE);

}









//½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½
//½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½
//½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½½



struct node *start;
struct node *currentNode;





struct node{			//Struc måste vara före main! annars finns den inte!

  int  distance;    //avstånd till A (s_1)
  float  vinkel;      //vinkel till A (fi_old)
  float  diskAngle;  //huvudvinkel

  struct node *link;    //skapar en pointer till strukturen "node"

}; 



void create(int distance,float vinkel,float diskAngle){
  struct node *temp;
  temp = (struct node *)malloc(sizeof(struct node));
          if(start == NULL){

            temp->distance=distance;
            temp->vinkel=vinkel;
            temp->diskAngle=diskAngle;
            temp->link=NULL;			
            start=temp;
            currentNode = start;
          }
}



void insert_beg(int distance,float vinkel,float diskAngle){
  struct node *tempnode;
  tempnode = (struct node *)malloc(sizeof(struct node));

            tempnode->distance=distance;
            tempnode->vinkel=vinkel;
            tempnode->diskAngle=diskAngle;
            tempnode->link = start;

  start = tempnode;
}
          











//==========================================================================================================================================
//==========================================================================================================================================
//==========================================================================================================================================


 
  float getDiskAngle(long time_in, long time_out,const float offset){
    double time_angle = (time_in+time_out)/2-time_at_zero;
    float  diskAngle = time_angle/time_rev*2.0*pi;
    diskAngle = diskAngle+offset;
    if (diskAngle>=2*pi){
      diskAngle = diskAngle-2*pi;
      Serial.println("HIGH ");
    }else if(diskAngle<=-2*pi){
      diskAngle = diskAngle+2*pi;
      Serial.println("LOW");
    }
    return diskAngle;
  }
  
  
  
  double* calcNewWayPoint(float diskAngle){  
  
    float fi_1 = diskAngle;
    float fi_2 = start->diskAngle;
    float fi_3 = (start->link)->diskAngle;
    float fi_old_1 = currentNode->vinkel;
    float fi_old_2 = (currentNode->link)->vinkel;
    int  s_1 = currentNode->distance;
    int  s_2 = (currentNode->link)->distance;
    
    
  //fi_1 är vinkeln på rotorskivan
  //a    = skivansradie
  //b    = avstånd från hjulbas till centrum av skivan, kan nog ändras till vart man vill köra runt 
  //punkt_A  = punkten som just skannades
  //fi_old_1 = vinkeln som punkten man är mellan den punkt man ÄR vid och nästa
  //s_1 = avståndet mellan current position och den innan
  
  int  punktA_X =    a*sin(fi_1);
  int  punktA_Y =  b+a*cos(fi_1);
  
  
  int  punktB_X = -b*sin(fi_old_1)+a*sin(fi_2-fi_old_1);
  int  punktB_Y = -s_1+b*cos(fi_old_1)+a*cos(fi_2-fi_old_1);
  
  int  punktC_X = s_2*sin(fi_old_1)     -b*sin(fi_old_1+fi_old_2)+a*sin(fi_3-fi_old_1-fi_old_2);    
  int  punktC_Y = -s_1 - s_2*cos(fi_old_1) + b*cos(fi_old_1+fi_old_2)+a*cos(fi_3-fi_old_1-fi_old_2);
  
  int  r_x = punktA_X-punktB_X;
  int  r_y = punktA_Y-punktB_Y;
  
  int  r_length = sqrt(pow(r_x,2)+pow(r_y,2));
  
  int  r_old_x = punktB_X-punktC_X;
  int  r_old_y = punktB_Y-punktC_Y;
  
  double r2_length = sqrt(pow(r_old_x,2)+pow(r_old_y,2));
  
  double vinkel = acos((r_x*r_old_x+r_y*r_old_y)/(r_length*r2_length));
  
  
  double length_and_angle[2] = { r2_length, vinkel };
  return length_and_angle;
  }


//***************************************************************************************************************************************
//***************************************************************************************************************************************
//***************************************************************************************************************************************








//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

void sensor_one_change(){      //Sensor ett har en offset på XXX


 if (toWhite == true){
     long sensor_one_out_time = micros();
     float  diskAngle = getDiskAngle(sensor_one_in_time, sensor_one_out_time,sensor1_offset);
     //double* length_and_angle = calcNewWayPoint(diskAngle);   //[distance, angle]
     
     //insert_beg(length_and_angle[0],length_and_angle[1],diskAngle);
     insert_beg(0,0,diskAngle);
     toWhite = false;
 
 }else{
     sensor_one_in_time = micros();
     toWhite = true;
 }
}
//#####################################################################################
void sensor_two_change(){      //Sensor ett har en offset på XXX


 if (toWhite == true){
     long sensor_two_out_time = micros();
     float  diskAngle = getDiskAngle(sensor_two_in_time, sensor_two_out_time,sensor2_offset);
     //double* length_and_angle = calcNewWayPoint(diskAngle);   //[distance, angle]
     
     //insert_beg(length_and_angle[0],length_and_angle[1],diskAngle);
     insert_beg(0,0,diskAngle);
     toWhite = false;
 
 }else{
     sensor_two_in_time = micros();
     toWhite = true;
 }
}
////#####################################################################################
void sensor_three_change(){      //Sensor ett har en offset på XXX


 if (toWhite == true){
     long sensor_three_out_time = micros();
     float  diskAngle = getDiskAngle(sensor_three_in_time, sensor_three_out_time,sensor3_offset);
     //double* length_and_angle = calcNewWayPoint(diskAngle);   //[distance, angle]
     
     //insert_beg(length_and_angle[0],length_and_angle[1],diskAngle);
     insert_beg(0,0,diskAngle);
     toWhite = false;
 
 }else{
     sensor_three_in_time = micros();
     toWhite = true;
 }
}
//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤



void sensor_rotor_rising(){
  long sensor_rotor_in_time = micros();
  time_at_zero = sensor_rotor_in_time;
  time_rev = sensor_rotor_in_time-sensor_rotor_in_time_old;
  sensor_rotor_in_time_old = sensor_rotor_in_time;
}
//void sensor_rotor_falling(){     //BEHÖVS INTE ÄN
//  sensor_rotor_out_time = micros();
//}




void encoder_Left(){
  
  
}


void encoder_Right(){
  
  
}

void sensor_error(){ //To Black
Serial.println("ERROR");
}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\




void loop() {
  delay(1000);
  Serial.print("DiskAngle, ");
  Serial.println(start->diskAngle/(2*pi)*360);
  Serial.print("Distance, ");
  Serial.println(start->distance);
  Serial.println("ttttttttttttttttttttttttttttttttttt");

}


