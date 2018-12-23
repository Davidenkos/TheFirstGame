#include <LedControl.h>
#include <LiquidCrystal.h>

bool hasStarted = false;
bool winnerDecided = false;

const int intervalPlayer = 75;
const int intervalBall = 200;
const int pinPotentiometer1 = A0;
const int pinPotentiometer2 = A1;
const int buttonPin = 2;
const int displayLcdPin = 9;

int buttonState = 0;
int potentiometerValue1, potentiometerValue2;
int pointsPlayer1, pointsPlayer2;
int ballX, ballY, ballDirection;
int winner;

unsigned long currentMillis, prevMillisPlayer, prevMillisBall;

LedControl lc = LedControl (12, 11, 10, 1);
LiquidCrystal lcd = LiquidCrystal (0, 1, 4, 5, 6, 7);

bool player1[8][8]
{
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   1,0,0,0,0,0,0,0,
   1,0,0,0,0,0,0,0,
   1,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
};
bool player2[8][8]
{
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,1,
   0,0,0,0,0,0,0,1,
   0,0,0,0,0,0,0,1,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
};

void initialization ()
{
    pinMode (buttonPin, INPUT);
      //Here are the matrix settinggs
    lc.shutdown (0, false);
    lc.setIntensity (0, 2);
    lc.clearDisplay (0);
      //Here i'm initializing theLcd with the start message
    lcd.begin (16, 2);
    lcd.clear ();
    lcd.home ();
    lcd.print ("The first game .");
    lcd.setCursor (0, 1);
    lcd.print ("Press the button");
    analogWrite (displayLcdPin, 90);
    pinMode (displayLcdPin, OUTPUT);        
    
    potentiometerValue1 = analogRead (pinPotentiometer1);
    potentiometerValue2 = analogRead (pinPotentiometer2);
    //I'm using a random function to determine which player goes first
    randomSeed (analogRead (0));
    int rand = random (1, 3);    
    if (rand == 1)
    {
      ballX = 3;
      ballY = 1;      
      lc.setLed (0, ballX, ballY, true);
    }
    else
    {
      ballX = 3;
      ballY = 6;
      lc.setLed (0, ballX, ballY, true);
    }

    for (int i = 0; i < 8; i++)
    {
       if (player1[i][0] == 1)
       { 
         lc.setLed (0, i, 0, true);
       }
       if (player2[i][7] == 1)
       {
         lc.setLed (0, i, 7, true);
       }
    } 
}
//Just a simple countdown before the start of the game
void countdown ()
{
  for (int i = 5; i >= 0; i--)
  {
     lcd.clear ();
     lcd.setCursor (6, 0);
     if (i == 0)
     {
       lcd.print ("START");
     }
     else
     {
       lcd.print (i);
     }
     delay (1000);
  } 
  lcd.clear ();
}
//A function for moving the player, turning the knob to the left takes him to the left, and the other way around
int move1 (bool player[8][8], int potentiometer, int pinPotentiometer)
{
    int prevPotentiometer = potentiometer;
    potentiometer = analogRead (pinPotentiometer);
    
    if (potentiometer - prevPotentiometer > 50)
    {
        for (int i = 0; i < 8; i++)
        {
          if (player[i][0] == 1)
          {
              if (i > 0)
              {
                player[i+2][0] = 0;
                player[i-1][0] = 1;
                lc.setLed (0, i+2, 0, false);
                lc.setLed (0, i-1, 0, true);              
              }          
          }
        }
    }
    else if (prevPotentiometer - potentiometer > 50)
    {
        for (int i = 7; i >= 0; i--)
        {
          if (player[i][0] == 1)
          {
              if (i < 7)
              {              
                player[i-2][0] = 0;
                player[i+1][0] = 1;
                lc.setLed (0, i-2, 0, false);
                lc.setLed (0, i+1, 0, true);  
              }          
          }
        }
    }
    return potentiometer;
}
//Same as before
int move2 (bool player[8][8], int potentiometer, int pinPotentiometer)
{
    int prevPotentiometer = potentiometer;
    potentiometer = analogRead (pinPotentiometer);
    
    if (potentiometer - prevPotentiometer > 50)
    {
        for (int i = 0; i < 8; i++)
        {
          if (player[i][7] == 1 )
          {
              if (i > 0)
              {
                player[i+2][7] = 0;
                player[i-1][7] = 1;
                lc.setLed (0, i+2, 7, false);
                lc.setLed (0, i-1, 7, true);                  
              }          
          }
        }
    }
    else if (prevPotentiometer - potentiometer > 50)
    {
        for (int i = 7; i >= 0; i--)
        {
          if (player[i][7] == 1 )
          {
              if (i < 7)
              {              
                player[i-2][7] = 0;
                player[i+1][7] = 1;
                lc.setLed (0, i-2, 7, false);
                lc.setLed (0, i+1, 7, true);  
              }          
          }
        }
    }
    return potentiometer;
}
//Here is the function to move the ball, the ball starts with the direction equal to zero
//ballDirections takes values between one and eight, two meaning west, six north, eight east and so forth
void moveBall ()
{
  if (ballDirection == 0 && ballY == 1)
  {
    
    if (player1[ballX][ballY-1] == 1 && player1[ballX-1][ballY-1] == 0)
    {
      lc.setLed (0, ballX, ballY, false); 
      lc.setLed (0, ballX-1, ballY+1, true);
      ballX--;
      ballY++; 
      ballDirection = 7;
    }
    else if (player1[ballX][ballY-1] == 1 && player1[ballX+1][ballY-1] == 0)
    {
      lc.setLed (0, ballX, ballY, false); 
      lc.setLed (0, ballX+1, ballY+1, true);
      ballX++;
      ballY++;
      ballDirection = 5;
    }
    else
    {
      lc.setLed (0, ballX, ballY, false); 
      lc.setLed (0, ballX, ballY+1, true);
      ballY++;
      ballDirection = 6;
    }
  }
  else if (ballDirection == 0 && ballY == 6)
  {
    if (player2[ballX][ballY+1] == 1 && player2[ballX-1][ballY+1] == 0 )
    {
      lc.setLed (0, ballX, ballY, false);
      lc.setLed (0, ballX-1, ballY-1, true);
      ballX--;
      ballY--;
      ballDirection = 1;
    }
    else if (player2[ballX][ballY+1] == 1 && player2[ballX+1][ballY+1] == 0)
    {
      lc.setLed (0, ballX, ballY, false);
      lc.setLed (0, ballX+1, ballY-1, true);
      ballX++;
      ballY--;
      ballDirection = 3;
    }
    else
    {
      lc.setLed (0, ballX, ballY, false); 
      lc.setLed (0, ballX, ballY-1, true);
      ballY--;
      ballDirection = 2; 
    }
  }
  else if (ballDirection == 1)
  {  
    if (ballX == 0)
    {
      if (player1[ballX+1][ballY-1] == 1)
      {
        if (player1[ballX][ballY-1] == 0)
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX+1, ballY+1, true);
          ballX++;
          ballY++;
          ballDirection = 5;  
        }
        else
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX+1, ballY, true);
          ballX++;
          ballDirection = 6;             
        }        
      }
      else
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1, ballY-1, true);
        ballX++;
        ballY--;
        ballDirection = 3;
      } 
    }
    else if (player1[ballX-1][ballY-1] == 1)
    {
      if (ballX-1 == 0 || player1[ballX-2][ballY-1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1, ballY, true);
        ballX--;
        ballDirection = 7;
      }
      else if (player1[ballX][ballY-1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1,ballY+1, true);
        ballX++;
        ballY++;
        ballDirection = 5;
      }
      else
      { 
        lc.setLed(0, ballX, ballY, false);
        lc.setLed(0, ballX-1, ballY, true);
        ballX--;
        ballDirection = 6;
      }
    }
    else
    {
      lc.setLed(0, ballX, ballY, false);
      lc.setLed(0, ballX-1,ballY-1, true);
      ballX--;
      ballY--;      
    }
    
  }
  else if (ballDirection == 2)
  {
     if (player1[ballX][ballY-1] == 1)
     {
        if ( player1[ballX-1][ballY-1] == 0)
        {
          lc.setLed(0, ballX, ballY, false);
          lc.setLed(0, ballX-1, ballY+1, true);
          ballX--;
          ballY++;
          ballDirection = 7;
        }
        else if ( player1[ballX+1][ballY-1] == 0)
        {
          lc.setLed(0, ballX, ballY, false);
          lc.setLed(0, ballX+1, ballY+1, true);
          ballX++;
          ballY++;          
          ballDirection = 5;
        }
        else
        {
          lc.setLed(0, ballX, ballY, false);
          lc.setLed(0, ballX, ballY+1, true);
          ballY++;
          ballDirection = 6; 
        }        
     }
     else
     {
       lc.setLed(0, ballX, ballY, false); 
       lc.setLed(0, ballX, ballY-1, true);
       ballY--;
     }
  }
  else if (ballDirection == 3)
  {      
    if (ballX == 7)
    {
      if (player1[ballX-1][ballY-1] == 1)
      {
        if (player1[ballX][ballY-1] == 0)
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX-1, ballY+1, true);
          ballX--;
          ballY++;
          ballDirection = 7;  
        }
        else
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX-1, ballY, true);
          ballX--;
          ballDirection = 6;             
        }        
      }
      else
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1,ballY-1,true);
        ballX--;
        ballY--;
        ballDirection = 1;  
      }       
    }
    else if (player1[ballX+1][ballY-1] == 1)
    {
      if (ballX+1 == 7 || player1[ballX+2][ballY-1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1, ballY, true);
        ballX++;
        ballDirection = 5; // de revizuit
      }
      else if(player1[ballX][ballY-1] == 0)
      {       
         lc.setLed (0, ballX, ballY, false);
         lc.setLed (0, ballX-1, ballY+1, true);//ballX-1
         ballX--;
         ballY++;
         ballDirection = 7;    
      }
      else
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1, ballY, true);
        ballX++;
        ballDirection = 6;
      }
    }
    else
    {
      lc.setLed (0, ballX, ballY, false);
      lc.setLed (0, ballX+1,ballY-1, true);
      ballX++;
      ballY--;      
    }
  }
  else if (ballDirection == 5)
  {      
    if (ballX == 7)
    {
      if (player2[ballX-1][ballY+1] == 1)
      {
        if (player2[ballX][ballY+1] == 0)
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX-1, ballY-1, true);
          ballX--;
          ballY--;
          ballDirection = 1;  
        }
        else
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX-1, ballY, true);
          ballX--;
          ballDirection = 2;             
        }        
      }
      else
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1,ballY+1,true);
        ballX--;
        ballY++;
        ballDirection = 7;  
      }
      
    }
    else if (player2[ballX+1][ballY+1] == 1)
    {
      if (ballX+1 == 7 || player2[ballX+2][ballY+1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1, ballY, true);
        ballX++;
        ballDirection = 3;
      }
      else if (player2[ballX][ballY+1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1, ballY-1, true);
        ballX--;
        ballY--;
        ballDirection = 1;
      }
      else
      {    
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1, ballY, true);
        ballX++;
        ballDirection = 2;
      }
    }
    else
    {
      lc.setLed (0, ballX, ballY, false);
      lc.setLed (0, ballX+1,ballY+1, true);
      ballX++;
      ballY++;      
    }
  }
  else if (ballDirection == 6)
  {
     if (player2[ballX][ballY+1] == 1)
     {
        if (player2[ballX-1][ballY+1] == 0)
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX-1, ballY-1, true);
          ballX--;
          ballY--;
          ballDirection = 1;
        }
        else if (player2[ballX+1][ballY+1] == 0)
        {
            lc.setLed (0, ballX, ballY, false);
            lc.setLed (0, ballX+1, ballY-1, true);
            ballX++;
            ballY--;          
            ballDirection = 3;
        }
        else
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX, ballY-1, true);
          ballY--;
          ballDirection = 2; 
        }
     }
     else
     {
       lc.setLed (0, ballX, ballY, false); 
       lc.setLed (0, ballX, ballY+1, true);
       ballY++;
     }
  }
  else if (ballDirection == 7)
  {  
    if (ballX == 0)
    {
      if (player2[ballX+1][ballY+1] == 1)
      {
        if (player2[ballX][ballY+1] == 0)
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX+1, ballY-1, true);
          ballX++ ;
          ballY--;
          ballDirection = 3;  
        }
        else
        {
          lc.setLed (0, ballX, ballY, false);
          lc.setLed (0, ballX+1, ballY, true);
          ballX++;
          ballDirection = 2;             
        }        
      }
      else
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1,ballY+1,true);
        ballX++;
        ballY++;
        ballDirection = 5;
      } 
    }
    else if (player2[ballX-1][ballY+1] == 1)
    {
      if (ballX-1 == 0 || player2[ballX-2][ballY+1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1, ballY, true);
        ballX--;
        ballDirection = 1;
      }
      else if(player2[ballX][ballY+1] == 0)
      {
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX+1,ballY-1, true);
        ballX++;
        ballY--;
        ballDirection = 3;
      }
      else
      { 
        lc.setLed (0, ballX, ballY, false);
        lc.setLed (0, ballX-1, ballY, true);
        ballX--;
        ballDirection = 2;
      }
    }
    else
    {
      lc.setLed (0, ballX, ballY, false);
      lc.setLed (0, ballX-1,ballY+1, true);
      ballX--;
      ballY++;      
    }
    
  }

  if(ballY == 0)
  {
    ballDirection = 0;
    lc.setLed (0, ballX, ballY, false);
    ballX = 3;
    ballY = 1;
    lc.setLed (0, ballX, ballY, true);
    hasStarted = false;
    pointsPlayer2++;
  }
  else if(ballY == 7)
  {
    ballDirection = 0;
    lc.setLed (0, ballX, ballY, false);
    ballX = 3;
    ballY = 6;
    lc.setLed (0, ballX, ballY, true);
    hasStarted = false;
    pointsPlayer1++;
  }
}

bool gameFinished()
{
  if ( pointsPlayer1 == 3)
  {
    winner = 1;
    for (int i = 0 ;i<8 ;i++)
    {
      lc.setLed (0, i, 0, false);
      lc.setLed (0, i, 7, false);
    }
    lc.setLed (0, ballX, ballY, false);
    lcd.clear ();
    lcd.home ();
    lcd.print ("Winner Player 1!");
    lcd.setCursor (4, 1);
    lcd.print ("Restart?");
    return true;
  }
  else if ( pointsPlayer2 == 3)
  {
    winner = 2;
    for (int i = 0;i<8 ;i++)
    {
      lc.setLed (0, i, 0, false);
      lc.setLed (0, i, 7, false);
    }
    lc.setLed (0, ballX, ballY, false);
    lcd.clear ();
    lcd.home ();
    lcd.print ("Winner player 2!");
    lcd.setCursor (4, 1);
    lcd.print ("Restart?");
    return true;
  }
  return false;
}
//This restarts the game, pretty self explanatory, the score resets and the ball goes to the loser
void restart ()
{
  lcd.clear ();
  hasStarted = false;
  winnerDecided = false;
  pointsPlayer1 = 0;
  pointsPlayer2 = 0;
  
  for (int i = 0; i < 8; i++)
  {
    if (i > 1 && i < 5)
    {
      player1[i][0] = 1;
      player2[i][7] = 1;
      lc.setLed (0, i, 0, true);
      lc.setLed (0, i, 7, true);
    }
    else
    {
      player1[i][0] = 0;
      player2[i][7] = 0;
      lc.setLed (0, i, 0, false);
      lc.setLed (0, i, 7, false);  
    }    
  }
  
  if (winner == 1)
  {
    ballX = 3;
    ballY = 6;
    lc.setLed (0, ballX, ballY, true);
  }
  else if (winner == 2)
  {
    ballX = 3;
    ballY = 1;
    lc.setLed (0, ballX, ballY, true);
  }
  
  countdown();
}

void setup ()
{
  initialization ();
  //Waiting until the button is pressed for the game to start
  while (buttonState == LOW)
  {        
    buttonState = digitalRead (buttonPin);
  }
  countdown ();
}

void loop ()
{
   buttonState = digitalRead (buttonPin);
   currentMillis = millis ();
   //Display the score.
   if (!winnerDecided )
   {
     lcd.home ();
     lcd.print ("Player1:");
     lcd.print (pointsPlayer1);
     lcd.setCursor (0, 1);
     lcd.print ("Player2:");
     lcd.print (pointsPlayer2);
   
     winnerDecided = gameFinished ();
   }
   //Interval to move the players
   if (currentMillis - prevMillisPlayer >= intervalPlayer && !winnerDecided)
   {
      potentiometerValue1 = move1 (player1, potentiometerValue1, pinPotentiometer1);
      potentiometerValue2 = move2 (player2, potentiometerValue2, pinPotentiometer2);  
      prevMillisPlayer = currentMillis;
   }
   //Interval to move the ball
   if (currentMillis-prevMillisBall >= intervalBall && hasStarted && !winnerDecided)
   {
      moveBall();
      prevMillisBall = currentMillis;
   }
   //Waiting for the button to be pressed for the ball to move
   if (buttonState == HIGH && !winnerDecided)
   {
      hasStarted = true;
   }

   if (buttonState == HIGH && winnerDecided)
   {
      restart ();
   }
   
}
