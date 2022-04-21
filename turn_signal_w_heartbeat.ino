//TODO
//ADD DEBOUNCE ON ANNOY PIN -- DONE
//ADD HEARTBEAT ON BUILT-IN LED (Will not work for Pro Mini) -- DONE


//PINOUT FOR PRO MINI
/*
int left_led = 10;
int left_sig = 11;
int right_led = 9;
int right_sig = 12;
int on_override_pin = 7;
int off_override_pin = 8;
int annoy_pin = 13;
int aux_led = 3;
*/

//PINOUT FOR PRO MICRO
int left_led = 10;
int left_sig = 16;
int right_led = 9;
int right_sig = 14;
int on_override_pin = 7;
int off_override_pin = 8;
int annoy_pin = 15;
int aux_led = 3;
int heartbeat_led = 17;

//FADE TUNING
int onTime = 1500; 
int fadeAmount = 5;
int delayTime = 15;

//DEBOUNCE TUNING
int debounce_delay = 30;

//ANNOY FLASH SEQUENCE TUNING
int flash_count = 3;
int flash_on_time = 75;
int flash_off_time = 25;


//INITIAL STATES
int annoy_latch = LOW;
int left_brightness = 0;
int left_latchState = LOW;
unsigned long left_previousMillis = 0;
int right_brightness = 0;
int right_latchState = LOW;
unsigned long right_previousMillis = 0;  
int signal_state;
int readIN;
int last_signal_state = HIGH;
int debounce_latch_state = LOW;
int debounce_time = 0;
int last_beat = 0;
int beat = LOW;
String current_millis = "0";
String sliced_millis = "0";

void setup() {
  Serial.begin(9600);
  pinMode(left_led, OUTPUT);
  pinMode(left_sig, INPUT);  
  pinMode(right_led, OUTPUT);
  pinMode(right_sig, INPUT);
  pinMode(on_override_pin, INPUT);
  pinMode(off_override_pin, INPUT);
  pinMode(annoy_pin, INPUT);
  pinMode(aux_led, OUTPUT);
  pinMode(heartbeat_led, OUTPUT);
  digitalWrite(left_sig, HIGH);
  digitalWrite(right_sig, HIGH);
  digitalWrite(on_override_pin, HIGH);
  digitalWrite(off_override_pin, HIGH);
  digitalWrite(annoy_pin, HIGH);  
}

void loop() {

  while (true){//NEEDED FOR BREAK ON OVERRIDES
  
    unsigned long currentMillis = millis();
    readIN = digitalRead(annoy_pin);
    signal_state = readIN;


    //HEARTBEAT
    /*
    if ((currentMillis - last_beat) > 500){
      last_beat = currentMillis;
      if (beat == LOW){
        beat = HIGH;
        digitalWrite(heartbeat_led, HIGH);
      }
      else{
        beat = LOW;
        digitalWrite(heartbeat_led, LOW);
      }
    }
    */
    //HEARTBEAT
    current_millis = millis();
    sliced_millis = (current_millis.substring((current_millis.length()-4),current_millis.length())).substring(1,2);
    //Serial.println(sliced_millis);
    if(sliced_millis == "9"){
      digitalWrite(heartbeat_led, LOW);
    }
    else{
      digitalWrite(heartbeat_led, HIGH);
    }


      
    //OFF OVERRIDE
    if (digitalRead(off_override_pin) == LOW){
      digitalWrite(left_led, LOW);
      digitalWrite(right_led, LOW);
      Serial.println("OFF OVERRIDE");
      left_brightness = 0;
      right_brightness = 0;
      break;
    }

    
    //ON OVERRIDE
    if (digitalRead(on_override_pin) == LOW){
      digitalWrite(left_led, HIGH);
      digitalWrite(right_led, HIGH);
      Serial.println("ON OVERRIDE");
      break;
    }

    //RESET DEBOUNCE TIME WHEN STATE CHANGES
    if(signal_state != last_signal_state){
      debounce_time = millis();
    }
    
    //DEBOUNCE
    if(signal_state == LOW && (millis() - debounce_time) > debounce_delay){
      debounce_latch_state = HIGH;
      //Serial.println("DEBOUNCE LATCHED!");
    }
    else{
      debounce_latch_state = LOW;
    }
    last_signal_state = readIN;

    //ANNOY OVERRIDE -- FLASH SEQUENCE ONCE, THEN HOLD
    if (debounce_latch_state == HIGH){
      if (annoy_latch == LOW){
        int i = 0;
        Serial.println("ANNOY OVERRIDE");
        while (i < flash_count){
          digitalWrite(left_led, HIGH);
          digitalWrite(right_led, HIGH);
          digitalWrite(aux_led, HIGH);
          delay(flash_on_time);
          digitalWrite(left_led, LOW);
          digitalWrite(right_led, LOW);
          digitalWrite(aux_led, LOW);
          delay(flash_off_time);
          i++;        
          }
        digitalWrite(left_led, HIGH);
        digitalWrite(right_led, HIGH);
        digitalWrite(aux_led, HIGH);
        annoy_latch = HIGH;
        delay(500);
        
       }
       break;
    }
    annoy_latch = LOW;
    digitalWrite(aux_led, LOW);
    
  
    // LEFT TURN SIGNAL
    //CHECK FOR LEFT TURN SIGNAL, RESET WATCHDOG
    if (digitalRead(left_sig) == LOW){
      //Serial.println("Left Signal Active");
      left_latchState = HIGH;
      left_previousMillis = currentMillis;
      }
     
    //IF NO SIGNAL, CHECK TIME, LATCH LOW IF WATCHDOG EXPIRED
    else{
      if (currentMillis - left_previousMillis >= onTime){
        left_latchState = LOW;
      }
    }
    
    //IF LATCHED ON, FADE ON TO FULL BRIGHTNESS
    if (left_latchState == HIGH){
      
      //Serial.print("LEFT LED ON - ");
      //Serial.println(left_brightness);
      left_brightness = left_brightness + fadeAmount;
      
      if (left_brightness >= 255){
        left_brightness = 255;
      }
      
      analogWrite(left_led, left_brightness);
    }
    
    //IF LATCHED OFF, FADE OFF TO ZERO BRIGHTNESS
    else if (left_latchState == LOW){
  
      //Serial.print("LEFT LED ON - ");
      //Serial.println(left_brightness);
      left_brightness = left_brightness - fadeAmount;
  
      if (left_brightness <= 0){
        left_brightness = 0;
      }
      
      analogWrite(left_led, left_brightness);
      
    }
  
  
  
  
  
    //RIGHT TURN SIGNAL
    //CHECK FOR RIGHT TURN SIGNAL, RESET WATCHDOG
    if (digitalRead(right_sig) == LOW){
      Serial.println("Left Signal Active");
      right_latchState = HIGH;
      right_previousMillis = currentMillis;
      }
     
    //IF NO SIGNAL, CHECK TIME, LATCH LOW IF WATCHDOG EXPIRED
    else{
      if (currentMillis - right_previousMillis >= onTime){
        right_latchState = LOW;
      }
    }
    
    //IF LATCHED ON, FADE ON TO FULL BRIGHTNESS
    if (right_latchState == HIGH){
      
      //Serial.println("RIGHT LED ON");
      right_brightness = right_brightness + fadeAmount;
      
      if (right_brightness >= 255){
        right_brightness = 255;
      }
      
      analogWrite(right_led, right_brightness);
    }
    
    //IF LATCHED OFF, FADE OFF TO ZERO BRIGHTNESS
    else if (right_latchState == LOW){
  
      //Serial.println("RIGHT LED OFF");
      right_brightness = right_brightness - fadeAmount;
  
      if (right_brightness <= 0){
        right_brightness = 0;
      }
      
      analogWrite(right_led, right_brightness);
      
    }




    
    delay(delayTime);

  
  } //END OF WHILE LOOP FOR BREAK COMMAND ON OVERRIDES
}
