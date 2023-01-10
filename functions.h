// functions.h
#ifndef _FUNCTIONS_h
#define _FUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ACS712.h> //Bibliotek innehållande kod för omvandling av data från ACS712
#include <LiquidCrystal.h> //Standardbibliotek för arduino inneållande drivrutiner för LCD-skärm.

float adcVoltage;
float sensValue;

float powerMoment;
float powerAvg;
float prevpowerAvg;
float powerTot;
bool runonce = false;

uint16_t timer2_counter = 0;
uint16_t timer1_counter = 0;
volatile bool timer2_init = false;
volatile uint32_t iterator = 0;

int v1, v2, v3, v4, v5;
int* values[5] = { &v1,&v2,&v3,&v4,&v5 };

const uint8_t buttonPin = 10;
const uint8_t buttonvoltagePin = 11;
volatile uint8_t buttonState;

const uint8_t rs = 12, en = 9, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //Sätter div. portar på LCDn.
ACS712 currentSensor(A0); //Initierar mätning på ACS712-sensor på pin A0 via konstruktor.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Initierar LCDn via konstruktor. 

#define LED PORTB5
#define LED_ON PORTB |= (1<<LED)
#define LED_OFF PORTB &= ~(1<<LED)


#define BUTTON_PRESSED digitalRead(buttonPin) == HIGH
#define BUTTON_NPRESSED digitalRead(buttonPin) == LOW
#define LCD_BACKL_ON digitalWrite(8,HIGH)
#define LCD_BACKL_OFF digitalWrite(8,LOW)
#define CURRENT_AMPS currentAvg >=1000
#define CURRENT_MAMPS currentAvg <1000

#define POWERMOMENT_WATTS powerMoment >=1
#define POWERMOMENT_MWATTS powerMoment <1
#define POWERTOT_WATTS powerTot >=1
#define POWERTOT_MWATTS powerTot <1

#define TIMER2_ELAPSED_MS 2000 // Timer 2 löper ut efter 2 sekunder.
#define TIMER2_MAXV (uint16_t)(TIMER2_ELAPSED_MS / 1 + 0.5) // Här definierar vi maxvärdet för Timer2.
#define TIMER1_ELAPSED_MS 10000 // Timer 1 löper ut efter 10s.
#define TIMER1_MAXV (uint16_t)(TIMER1_ELAPSED_MS / 1048 + 0.5) // Här definierar vi maxvärdet för Timer1.


void setup();
float get_voltage();
long get_current();
void print_voltage_amps();
void print_power();
void measure_power_avg();
void lcd_sleep();
void loop();


#endif

