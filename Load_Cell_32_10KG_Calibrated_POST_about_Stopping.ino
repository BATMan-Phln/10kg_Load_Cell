



/* Arduino Scale using HX711 and Load Cell
 
Created by Yvan / https://Brainy-Bits.com

This code is in the public domain...

You can: copy it, use it, modify it, share it or just plain ignore it!
Thx!
*/


#include <HX711.h>    // Library for Load Cell amplifier board
            
            #include <Wire.h> 
            #include <LiquidCrystal_I2C.h>
            LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

volatile boolean TurnDetected;  // variable used to detect rotation of Rotary encoder
volatile int Rotary_Flag=0;     // flag to indicate rotation as occured

// Rotary Encoder Module connections
#define RotaryCLK 2   // Rotary encoder CLK pin connected to pin 2 of Arduino
#define RotaryDT 3    // Rotary encoder DT pin connected to pin 3
#define RotarySW 4    // Rotary encoder Switch pin connected to pin 4

// HX711 Module connections
#define CLK 5   // CLK of HX711 connected to pin 5 of Arduino
#define DOUT 6  // DOUT of HX711 connected to pin 6 of Arduino

int reset_screen_counter=0;      // Variable used to decide what to display on Oled
volatile int current_units=0;    // Used to select which measuring unit to use (KG,Grams,Pounds)
float unit_conversion;           // Used to convert between measuring units
int decimal_place;               // how many decimal number to display

            int switchState = 0 ;  //TRYING to get this damn thing to work!

HX711 scale;  // Init of the HX711

// Interrupt routine runs if Rotation detected from Rotary encoder
void rotarydetect ()  {
Rotary_Flag=1; // Set Rotary flag from 0 to 1
delay(500);
}


// Used to change the measurement units (0=grams, 1=KG, 2=pounds)
void change_units ()  { 
  if (current_units == 0) current_units=1;
  else if (current_units == 1) current_units=2;
  else if (current_units == 2) current_units=0;
}


// Run at Startup and when Resetting with Rotary encoder switch
void startupscreen(void) {

//        do {
            lcd.init();
            lcd.setCursor(2,0);
            lcd.print("Clear Scale");
            lcd.setCursor(1,1);
            lcd.print("Click to Zero");
            Serial.println("Clear Scale");
            Serial.println("Click to Zero");
            Serial.println(" ");
//        } while ((switchState) ==1);

}


// Reset Scale to zero
void tare_scale(void) {
  scale.set_scale(228550.00);  //Calibration Factor obtained from calibration sketch
                    //373500 Reading = 205 DING DING DING!!!!
                    //Calibrating 10KG 229609.76 = 203 and it stopped after 11 readings
                    //Calibrating 10KG 228000 = 206                  
                    //Calibrating 10KG 228500 = 205  Still bobs around a bit
                    //Calibrating 10KG 228600 = 207  Stopped after 14
                    //Calibrating 10KG 228600.00 = 207  Stopped after 25
                    //Calibrating 10KG 228550.00 = 205 Still bobs around about 4-5gs.
                    //Still locks up after random readings.                                       
                    
                    
  scale.tare();             //Reset the scale to 0  
}


// Start Displaying information on OLED
void start_scale(void) {
  char temp_current_units[15];  // Needed to store String to Char conversion
  String KG="KG";
  String GRAMS="GRAMS";
  String LBS="POUNDS";
  
  if (current_units == 0) {                     // 0 = grams
    GRAMS.toCharArray(temp_current_units, 15);  // Convert String to Char for OLED display
    unit_conversion=1000;                        // conversion value for grams
    decimal_place=0;                            // how many decimal place numbers to display
  } else if (current_units == 1) {              // 1 = Kilograms
    KG.toCharArray(temp_current_units, 15);
    unit_conversion=1;
    decimal_place=3;
  } else {                                      // else 2 = Pounds
    LBS.toCharArray(temp_current_units, 15);
    unit_conversion=2.2046226218;
    decimal_place=3;
  }
                          do {  
              lcd.init();                      // initialize the lcd 
              lcd.setCursor(2,0);
              lcd.print(temp_current_units);
              lcd.setCursor(1,1);
              lcd.print(scale.get_units(3)*unit_conversion, decimal_place);  // Display the average of 3 scale value reading
              Serial.println(temp_current_units);
              Serial.println(scale.get_units(3)*unit_conversion, decimal_place);  // Display the average of 3 scale value reading
              } while ((switchState) ==1);

}

void setup(void) {
            
  scale.begin(DOUT, CLK);

            Serial.begin(9600);

            lcd.begin(16,2); // sixteen characters across - 2 lines
            lcd.init();
            // Print a message to the LCD.
            lcd.backlight();
            lcd.setCursor(2,0);
            lcd.print("W6LU's Scale");
            lcd.setCursor(1,1);
            lcd.print("Initializing...");
            Serial.println("W6LU Scale");
            Serial.println("Initializing...");
            Serial.println(" ");
  // Set pinmode for Rotary encoder pins
  pinMode(RotarySW,INPUT_PULLUP);
  pinMode(RotaryCLK,INPUT_PULLUP);
  pinMode(RotaryDT,INPUT_PULLUP);  

  // Attach interrupt 0 (Pin 2 on UNO) to the Rotary Encoder
  attachInterrupt (0,rotarydetect,RISING);   // interrupt 0 always connected to pin 2 on Arduino UNO


  String start_count_string="Starting up....";  // Message to display at Startup
  char start_count[15];  // Used to String to Char conversion

               lcd.init();
                Serial.println("W6LU Scale");
            lcd.setCursor(1,0);
                lcd.print("W6LU Scale");
     
  // Loop to display counting dots
  for (int x=12; x < 17; x++) {  // Select the first 12 to 16 character of String
    start_count_string.toCharArray(start_count, x);
                Serial.println(start_count);
            lcd.setCursor(0,1);
                lcd.print(start_count);
                  delay(500);  // Delay between dots
}
}

void loop(void) {

    //Serial.println("The Loop has started");
  
// If Switch is pressed on Rotary Encoder
  if (digitalRead(RotarySW)== LOW) {       // Check to see which action to take
    if(reset_screen_counter == 1) {   
      tare_scale();                   // 1 = zero and start scale
      reset_screen_counter=2;
      //Serial.println("IF has been triggered");
      delay(500);
    } else {
      if(reset_screen_counter == 2) { // 2 = Scale already started so restart from begining
        reset_screen_counter=0;
      //Serial.println("ELSE has been triggered");
        delay(500); 
      }
    }
  }

// If Rotation was detected
  if (Rotary_Flag == 1) {
    change_units();  // change the measuring units
    Rotary_Flag=0;   // reset flag to zero
  }

// If system was just started display intro screen  
  if (reset_screen_counter == 0) {    
    startupscreen();
  reset_screen_counter=1;  // set to 1 and wait for Rotary click to zero scale
  }
 
// if zero (tare) of scale as occured start display of weight
  if (reset_screen_counter == 2) {
          start_scale();
  }
}
