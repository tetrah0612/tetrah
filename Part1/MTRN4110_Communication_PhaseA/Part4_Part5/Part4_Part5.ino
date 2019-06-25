
//////////////////////////////////

//PUT SERIAL MONITOR IN NEWLINE MODE

//MUST PRESS ENTER AFTER EACH INPUT

//////////////////////////////////

#define ROWS_wall 2
#define COLS_wall 3
#define H_size 9
#define V_size 8

//MAZE STUFF
/////////////

bool horizontalWalls[ROWS_wall+1][COLS_wall];
bool verticalWalls[ROWS_wall][COLS_wall+1];
int x = 0;

/*
// vertical walls array
  bool verticalWalls[ROWS_wall][COLS_wall + 1] = { {1,0,1,1},               //verticalWalls[2][4]
                                         {1,0,1,1}};
                                                       
  // horizontal walls array                                                 
  bool horizontalWalls[ROWS_wall + 1][COLS_wall]  = { {1,1,1},              //horizontalWalls[3][3]
                                            {1,0,1},
                                            {1,1,1}};
*/

///STORING VALUES STUFF FROM SERIAL INPUT
//////////////
//arrays to store stuff in
int array_H[H_size];
int array_V[V_size];

//var from serial
unsigned int integerValue=0;  // Max value is 65535
char incomingByte;

int h = 0;
int v = 0;

////////////////////////////////////
                                                      
void setup() {

  //Setup Serial
  Serial.begin(9600);
  Serial1.begin(9600);

/////////////////////////////////////

  //Start storing horizontal array
  Serial1.print("Please enter 9 values for array_H from top to bottom and left to right\n");
  while(h<H_size){
      if (Serial1.available() > 0) {   // something came across serial
        integerValue = 0;         // throw away previous integerValue
        while(1) {            // force into a loop until 'n' is received
          incomingByte = Serial1.read();
          if (incomingByte == '\n') break;   // exit the while(1), we're done receiving
          if (incomingByte == '\r') break;   // exit the while(1), we're done receiving
          if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
          integerValue *= 10;  // shift left 1 decimal place
          // convert ASCII to integer, add, and shift left 1 decimal place
          integerValue = ((incomingByte - 48) + integerValue);
        }
        array_H[h] = integerValue;  
        h++;
    }
  }


  //Start storing vertical array
  Serial1.print("Please enter 8 values for array_V from top to bottom and left to right\n");
  while(v<V_size){
      if (Serial1.available() > 0) {   // something came across serial
        integerValue = 0;         // throw away previous integerValue
        while(1) {            // force into a loop until 'n' is received
          incomingByte = Serial1.read();
          if (incomingByte == '\n') break;   // exit the while(1), we're done receiving
          if (incomingByte == '\r') break;   // exit the while(1), we're done receiving
          if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
          integerValue *= 10;  // shift left 1 decimal place
          // convert ASCII to integer, add, and shift left 1 decimal place
          integerValue = ((incomingByte - 48) + integerValue);
        }
        array_V[v] = integerValue;  
        v++;
    }
  }

/////////////////////////////////////

  Serial.print("The array string entered for Horizontal array is ");
  //Print array contents
  //print array_H
  h = 0 ;
  while(h< H_size){
      Serial.print(bool(array_H[h]));  
      h++;
  }
  
  Serial.print("\n");

  Serial.print("The array string entered for Vertical array is ");
  //print array_V
  v = 0 ;
  while(v<V_size){
      Serial.print(bool(array_V[v]));  
      v++;
  }

  Serial.print("\n");

/////////////////////////////////////
//Convert array to suitable size array
//store values

      //horizontal
      for(int row = 0; row < 3; row++){
        for(int col = 0; col < 3; col++){
          if(x != H_size){
            horizontalWalls[row][col] = array_H[x++];
          }
        }
      }
      x=0;
      //vertical
      for(int row = 0; row < 2; row++){
        for(int col = 0; col < 4; col++){
          if(x != V_size){
            verticalWalls[row][col] = array_V[x++];
          }
        }
      }



/////////////////////////////////////

//Print maze
         for(int i = 0;i < 2*ROWS_wall+1;i++)
        {
            for(int j = 0;j < 2*COLS_wall+1;j++)
            {
                //Add Horizontal Walls
                if(i%2 == 0 && j%2 == 1)
                {
                    if(horizontalWalls[i/2][j/2] == true)
                    {
                        Serial.print(" ---");
                    }
                    else
                    {
                        Serial.print("    ");
                    }
                }

                //Add Vertical Walls
                if(i%2 == 1 && j%2 == 0)
                {
                    if(verticalWalls[i/2][j/2] == true)
                    {
                        Serial.print("|   ");

                    }
                    else
                    {
                        Serial.print("    ");
                    }
            }
        }
        Serial.print("\n");
    }
}


void loop() {
}
