/* 
 *  NAME:    3servo_ascii_comm
 *  DATE:    2019-11-28
 *  VERSION: 0.0.2
 *  
 *  VERSION  DESCRIPTION 
 *  =======  =========================================================
 *  0.0.1    Initial Revision
 *  0.0.2    Corrected command position validation
 *  
 *  Command Message Structure:  
 *  
 *  TURRET POSITION (Degrees 0 to 180 ...00H to B4H)
 *  Byte 0: ASCII UPPER NIBBLE
 *  Byte 1: ASCII LOWER NIBBLE
 *  
 *  ARM2 POSITION (Degrees 0 to 180 ...00H to B4H)
 *  Byte 2: ASCII UPPER NIBBLE
 *  Byte 3: ASCII LOWER NIBBLE
 *  
 *  ARM2 POSITION (Degrees 0 to 180 ...00H to B4H)
 *  Byte 4: ASCII UPPER NIBBLE
 *  Byte 5: ASCII LOWER NIBBLE
 *  
 *  TERMINATION
 *  Byte 6: 0x0D (ASCII Carriage Return)
 *  Byte 7: 0x0A (ASCII Line Feed)
 *  
*/

#include <Servo.h>

#define CMD_STREAM_SIZE    8

#define TURRET_MSN    0
#define TURRET_LSN    1
#define ARM1_MSN      2
#define ARM1_LSN      3
#define ARM2_MSN      4
#define ARM2_LSN      5




Servo Turret_Servo;  // create servo object to control Turret
Servo Arm1_Servo;    // create servo object to control Arm1
Servo Arm2_Servo;    // create servo object to control Arm2

int cmd_byte[CMD_STREAM_SIZE];
int cmd_count;

int turret_val;
int arm1_val;
int arm2_val;

int turret_val_prev;
int arm1_val_prev;
int arm2_val_prev;


void setup() {
  Turret_Servo.attach(9);   // attaches Turret servo on pin 9 to the servo object
  Arm1_Servo.attach(8);     // attaches Arm1 servo on pin 8 to the servo object
  Arm2_Servo.attach(7);     // attaches Arm2 servo on pin 7 to the servo object
  Serial.begin(9600);       // opens serial port, sets data rate to 9600 bps
}

/*
* THIS FUNCTION IS NOT YET USED
*/
//int is_ascii( int data )
//{
//  data = toupper(data);
//  if( ((data >= '0') && (data <= '9')) || ((data >= 'A') && (data <= 'F') )
//  {
//     data = 0x01;
//  }
//  else
//  {
//     data = 0x00;
//  }
//
//  return( data );
//}
//




int byte_asc2hex( int msn, int lsn )
{
  int retval;

  msn = toupper(msn);
  if(msn > '9'){ msn = msn - 0x37; }
  else         { msn = msn - 0x30; }

  lsn = toupper(lsn);
  if(lsn > '9'){ lsn = lsn - 0x37; }
  else         { lsn = lsn - 0x30; }

  return( (msn << 4) + lsn );
}

void loop() {

  cmd_count = 0;
  while(cmd_count < CMD_STREAM_SIZE)
  {
     if (Serial.available() > 0) {
        // read the incoming byte:
        cmd_byte[cmd_count] = Serial.read();
        cmd_count++;
     }     
  }


  if( (cmd_byte[6]==0x0D) && (cmd_byte[7]==0x0A) )  //CR (carriage return & LF (line feed)
  {
     turret_val = byte_asc2hex( cmd_byte[TURRET_MSN], cmd_byte[TURRET_LSN] );
     arm1_val   = byte_asc2hex( cmd_byte[ARM1_MSN], cmd_byte[ARM1_LSN] );
     arm2_val   = byte_asc2hex( cmd_byte[ARM2_MSN], cmd_byte[ARM2_LSN] );
    
     if( (turret_val < 181) && (arm1_val < 181) && (arm2_val < 181) ) 
     {
        /* IS ARM #1 DEGREES GREATER THAN THE PREVIOUS VALUE? */
        if( arm1_val > arm1_val_prev )
        {  /* IF SO, MOVE ARM #1 FIRST */
           Arm1_Servo.write(arm1_val);       // tell servo to go to position  
           Arm2_Servo.write(arm2_val);       // tell servo to go to position
           Turret_Servo.write(turret_val);   // tell servo to go to position 
        }
        else
        {  /* OTHERWISE, MOVE ARM #2 FIRST */
           Arm2_Servo.write(arm2_val);       // tell servo to go to position  
           Arm1_Servo.write(arm1_val);       // tell servo to go to position
           Turret_Servo.write(turret_val);   // tell servo to go to position 
        }
 
        turret_val_prev = turret_val;
        arm1_val_prev   = arm1_val;
        arm2_val_prev   = arm2_val;

     }
  
  }
  
}
