// 
// 
// 

#include "Talk.h"
//включаем одно релe при получении
//manualX (X - номер реле )
void talk_manual() {
	if (sms.substring(0, 6) == "manual") {    		
		manual = int(sms[6]) - 48;
		if (manual >= 1 && manual <= 4) {
			//Serial.print("manual ");
			//Serial.println(manual);
			for (i = 1; i <= 4; i++)
				if (i == manual) digitalWrite(relayPin[manual], LOW);
				else digitalWrite(relayPin[i], HIGH);
				smsResult = true;
		}
		else {
			manual = 0;
			//Serial.println("manual must be [1..4]");
		}
	}
}
//выключает ручное управление
//при получении no 
void talk_no() {
	if (sms.substring(0, 2) == "no") {
		manual = 0;
		Serial.println("manual no");
		smsResult = true;
	}
}
