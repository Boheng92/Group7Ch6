#include <SoftwareSerial.h>
#include "stdlib.h"

#define ELECTION 0
#define NORMAL 1
#define LEADER 2
#define INFECTED 3

#define MSG_RECEIVE_PER_LOOP 5
#define DELAY_AFTER_SENDING_N 1500
#define DELAY_AFTER_SENDING_L 1000
#define TIME_BETWEEN_READING 1000


SoftwareSerial XBee(2, 3); 

int ID = 9;

int STATUS;

int count = 0;

int count2 = 0;

boolean NMODE = false;


boolean spread = false;

///////////////////////////////////
/////// Convert Functionns ////////
///////////////////////////////////
int charToInt(char a)
{
  if(a < '0' || a > '9')
  {
    return 0;
  }
  
  return (a - '0');
}

char intToChar(int m)
{
  char result = ('0' + m);
  
  if(result < '0' || result > '9')
  {
    result = '0';
  }
  
  return result;
}

void sendMyID()
{
  char msg;
  
  msg = intToChar(ID);
      
  XBee.println(msg);

  Serial.print("Sending:");
  Serial.println(msg);
}

///////////////////////////////////
/////// Hardware Functions ////////
///////////////////////////////////
void BlueLight(int i)
{
  if(i == 0)
  {
    digitalWrite(6, LOW);
    //Serial.println("Blue Light turned off.");
  }
  else if(i == 1)
  {
    digitalWrite(6, HIGH);
    //Serial.println("Blue Light turned on.");
  }
}

void GreenLight(int i)
{
  if(i == 0)
  {
    digitalWrite(5, LOW);
    //Serial.println("Green Light turned off.");
  }
  else if(i == 1)
  {
    digitalWrite(5, HIGH);
    //Serial.println("Green Light turned on.");
  }
}

void RedLight(int i)
{
  if(i == 0)
  {
    digitalWrite(4, LOW);
    //Serial.println("Red Light turned off.");
  }
  else if(i == 1)
  {
    digitalWrite(4, HIGH);
    //Serial.println("Red Light turned on.");
  }
}

boolean buttonClick()
{
  int digitalInput = digitalRead(8);
  if(digitalInput == 0) return true;
  else return false;
}

///////////////////////////////////
//////// Setup Functions //////////
///////////////////////////////////
void setup() 
{
  // put your setup code here, to run once:
  Serial.print("Initializing...");
  
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, INPUT);// For the click button
  
  XBee.begin(9600);
  Serial.begin(9600);

  STATUS = ELECTION;

  RedLight(0);
  GreenLight(0);
  BlueLight(0);

  Serial.println("Complete~");
}

///////////////////////////////////
///////// Loop Functions //////////
///////////////////////////////////
void loop() 
{
  // put your main code here, to run repeatedly:
  int i;

  char msg;

  int temp_m;

  int nextState = STATUS;

  switch(STATUS)
  {
    case ELECTION:
    {
      Serial.println("== ELECTION ==");
      
      nextState = LEADER;

      // Send my ID
      sendMyID();

      // Wait for 1 second
      delay(DELAY_AFTER_SENDING_N);

      // keep reading, make sure the buffer is empty after reading
      for(i=0; i < MSG_RECEIVE_PER_LOOP; i++)
      {   
        msg = XBee.read();

        Serial.print("*** Receiving:");
        Serial.println(msg);

        temp_m = charToInt(msg);

        // Find a larger ID in the network
        if(temp_m > ID)
        {
          // I can not be the leader...
          nextState = NORMAL;
        }

        delay(TIME_BETWEEN_READING);
      }

      break;
    }

    case NORMAL:
    {
      Serial.println("== NORMAL ==");

      GreenLight(1);

      NMODE = false;
      // I do not need to send anything~

      nextState = ELECTION;

      delay(DELAY_AFTER_SENDING_N);

      // keep reading, make sure the buffer is empty after reading
      for(i=0; i < MSG_RECEIVE_PER_LOOP; i++)
      {
        msg = XBee.read();

        Serial.print("*** Receiving:");
        Serial.println(msg);

        temp_m = charToInt(msg);

        // Find a larger ID in the network
        if(temp_m > ID)
        {
          // I do not to start another election...
          nextState = NORMAL;
        }

        if(buttonClick() || msg == 'I')
        {
          // Recover
          nextState = INFECTED;

          spread = true;

          break;
        }

        delay(TIME_BETWEEN_READING);
      }
      
      break;
    }

    case LEADER:
    {
      Serial.println("== LEADER ==");

      BlueLight(1);

      sendMyID();

      if(buttonClick())
      {
        // Recover
        XBee.println('R');
      }
      
        
      // keep reading, make sure the buffer is empty after reading
      for(i=0; i < MSG_RECEIVE_PER_LOOP; i++)
      {
        // Broadcast My ID
        //if((i % 3) == 0)
        //{
        //  sendMyID();
        //}
        
        msg = XBee.read();

        Serial.print("*** Receiving:");
        Serial.println(msg);

        temp_m = charToInt(msg);

        // Find a larger ID in the network
        if(temp_m > ID)
        {
          // I can not be the leader anymore...
          nextState = NORMAL;
        }

        delay(TIME_BETWEEN_READING);
      }
      
      break;
    }

    case INFECTED:
    {
      Serial.println("== INFECTED ==");

      RedLight(1);

      if(spread)
      {
        // Spread the Infection
        XBee.println('I');

        spread = false;
      }
      

      // keep reading, make sure the buffer is empty after reading
      for(i=0; i < MSG_RECEIVE_PER_LOOP; i++)
      {
        msg = XBee.read();

        Serial.print("*** Receiving:");
        Serial.println(msg);
        temp_m = charToInt(msg);

        if(msg == 'R')
        {
          nextState = NORMAL;
          NMODE=true;
        }
        else if(temp_m  < ID)
        {
          
          count++;
          
          if(count == 5 && !NMODE)
          {
            count2++;
            if(count2 == 2){
              nextState = LEADER;
              count2 = 0;
            }
            count = 0;
          }  
        }else
        { 
          if(!NMODE)
          nextState = INFECTED;
        }
        
        delay(TIME_BETWEEN_READING);
      }
      count = 0;
      break;
    }

    default:
    {
      Serial.println("== W.T.F. ==");

      nextState = ELECTION;
      
      break;
    }
  }

  RedLight(0);
  GreenLight(0);
  BlueLight(0);

  STATUS = nextState;
}
