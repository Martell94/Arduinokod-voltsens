// include the library code:
#include "functions.h"

void setup() {

	Serial.begin(9600); //Initierar seriell kommunikation i 9600kbps.

	noInterrupts(); // Stänger av interrupts globalt.

	// Timer 2 används till measure_power_avg - funktionen.
	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2B = (1 << CS22); //Prescaler på YNKLIGA 64.
	//TIMSK2 = (1 << TOIE2);

	//Timer 1 används till LCD_sleep-funktionen.
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << CS12); //Prescaler på 256
	//TIMSK1 = (1 << TOIE1);

	//Tryckknappens interruptregister.
	PCICR = (1 << PCIE0);
	PCMSK0 |= (1 << PCINT2);

	interrupts(); // Slår återigen på interrupts.

	pinMode(A2, INPUT);
	pinMode(8, OUTPUT); //Ettställer pin 8 i DDRB-registret, som strömförser backlighten på LCDn.
	pinMode(buttonPin, INPUT_PULLUP);
	pinMode(buttonvoltagePin, OUTPUT);
	digitalWrite(buttonvoltagePin, HIGH);
	pinMode(13, OUTPUT);

	LCD_BACKL_OFF; //Sätter pin åttas output till låg, vilket innebär att skärmen är av vid start.
	lcd.begin(16, 2); //Specifierar vilken typ av LCD vi använder, dvs 16 kolumner, 2 rader.
}

ISR(PCINT0_vect) {//Knappens interruptrutin.

	PCMSK0 &= ~(1 << PCINT2); //Stänger av interrupts på knappen.
	delay(30); //Väntar ett slag.

	TIMSK1 = (1 << TOIE1); //Initierar timer 1 (LCD_SLEEP).

	if (!timer2_init) {//Körs en gång för att initierar timer 2.
		TIMSK2 = (1 << TOIE2);//Initierar timer 2.
		timer2_init = !timer2_init;
	}

	LCD_BACKL_ON;
	PCMSK0 |= (1 << PCINT2);
}

ISR(TIMER1_OVF_vect) { //LCD-timer med avbrott efter 10s.

	if (++timer1_counter >= TIMER1_MAXV) {

		LCD_BACKL_OFF;
		timer1_counter = 0; //Nollställer timer-räknaren.
		TIMSK1 &= ~(1 << TOIE1); //Stänger av timern igen.
	}
}

/*ISR(TIMER1_COMPA_vect) {
	
	LED_ON;
	
}*/

ISR(TIMER2_OVF_vect) { //Timer för att mäta genomsnittlig kraftuveckling, med avbrott varannan sekund.

	if (++timer2_counter >= TIMER2_MAXV){
		measure_power_avg();
		timer2_counter = 0;
	}
}


float get_voltage() { //Levererar spänningsvärde i V.

	adcVoltage = (analogRead(A2) * 5);
	return ((adcVoltage / 1023) * 5) * 1.15;
}

long get_current() { //Levererar strömvärde i mA.

	sensValue = currentSensor.mA_DC();
	if (sensValue < 0.9) { //Filtrerar skräpvärden. 
		sensValue = 0;
	}

	for (int i = 0; i < 5; i++) {
		*values[i] = (sensValue / 10);
		delay(20);
	}
	return ((v1 + v2 + v3 + v4 + v5) / 5);
	//^Plussar ihop 5 mätvärden och dividerar med 5 för att få medelvärde.
}

void print_voltage_amps() {
	
	float voltage = get_voltage();
	long currentAvg = get_current();

	lcd.print("                ");//Rensar raden inför uppdatering.
	lcd.setCursor(0, 0);
	lcd.print("Volt: ");
	lcd.print(voltage);
	lcd.print(" V");

	lcd.print("                ");//Rensar raden inför uppdatering.
	lcd.setCursor(0, 1); //Sätter pekaren till rad 2 på LCD-skärmen.
	lcd.print("Amp = ");

	if (CURRENT_MAMPS) { //Ifall mätvärdet är i mA-range printar vi med mA-suffix.
		lcd.print(currentAvg);
		lcd.print(" mA");
	}
	else if (CURRENT_AMPS) { //Ifall mätvärdet överstiger eller = 1000mA använder vi Ampere-suffix.
		lcd.print(currentAvg / 1000);
		lcd.print(" A");
	}

	return;
}

void print_power() {

	float voltage = get_voltage();
	long currentAvg = get_current();

	powerMoment = voltage * (currentAvg / static_cast<float>(1000));

	lcd.print("                ");//Rensar raden inför uppdatering.
	lcd.setCursor(0, 0);
	lcd.print("PMom = ");
	lcd.print(powerMoment);

	if (POWERMOMENT_WATTS) { //Ifall mätvärdet överstiger 999 mW använder vi W-enheten.
		lcd.print(powerMoment/1000);
		lcd.print(" W");
	}
	else if (POWERMOMENT_MWATTS) { //Ifall mätvärdet är på under 1W använder vi mW-enheten.
		lcd.print(powerMoment);
		lcd.print(" mW");
	}

	lcd.print("                ");//Rensar raden inför uppdatering.
	lcd.setCursor(0, 1); //Sätter pekaren till rad 2 på LCD-skärmen.
	lcd.print("PAvg = ");
		if (POWERTOT_WATTS) { //Ifall mätvärdet överstiger 999 mW använder vi W-enheten.
		lcd.print(powerTot);
		lcd.print(" W");
	}
	else if (POWERTOT_MWATTS) { //Ifall mätvärdet är på under 1W använder vi mW-enheten.
		lcd.print(powerTot * 1000);
		lcd.print(" mW");
	}

	return;
}

void measure_power_avg() {

	float voltage = get_voltage();
	long currentAvg = get_current();

	powerAvg = voltage * (currentAvg /static_cast<float>(1000));

	if (runonce = false) {
		prevpowerAvg = powerAvg;
		runonce = !runonce;
	}

	powerTot = ((prevpowerAvg + powerAvg) / 2);

	return;
}

void lcd_sleep() {

	lcd.setCursor(0, 0);
	lcd.print("                ");
	lcd.setCursor(0, 1);
	lcd.print("                ");
	LCD_BACKL_OFF;

	return;
}

void loop() {

	//print_voltage_amps();
	print_power();
	delay(300);
}