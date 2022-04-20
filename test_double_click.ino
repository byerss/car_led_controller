
int led_pin = 30;
int led_pin2 = 17;
int button_pin = 15;
unsigned long currentMillis = 0;
bool signal_state = HIGH;
bool last_signal_state = HIGH;
bool debounce_latch_state = LOW;
bool last_debounce_latch_state = LOW;
unsigned long debounce_time = 0;
int debounce_delay = 20;
int double_click_window = 1000;
unsigned long double_click_expires = 0;
bool double_click_state = LOW;
bool annoy_latch = LOW;

//ANNOY FLASH SEQUENCE TUNING
int flash_count = 3;
int flash_on_time = 75;
int flash_off_time = 25;



void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(button_pin, INPUT);
  digitalWrite(button_pin, HIGH);

}

void loop() {

  currentMillis = millis();
  signal_state = digitalRead(button_pin);


  //RESET TIMERS WHEN STATE CHANGES TO LOW (BUTTON PRESSED)
  if(signal_state != last_signal_state && signal_state == LOW){
    
    debounce_time = millis();

    if(double_click_expires == 0){
      double_click_expires = millis() + double_click_window;
      double_click_state = LOW;
      Serial.print("Timer Started: ");
      Serial.println(millis());
    }
  }

  //DO SOMETHING WHEN STATE CHANGES TO HIGH (BUTTON RELEASED)?
  if(signal_state != last_signal_state && signal_state == HIGH){
    //Something
  }

  //ZERO DOUBLE-CLICK TIMER AFTER EXPIRES
   if(millis() > double_click_expires && double_click_expires != 0 && annoy_latch == LOW){
    //TIMER EXPIRED
    Serial.print("Timer Expired: ");
    Serial.println(double_click_expires);
    double_click_expires = 0;
   }


  //DEBOUNCE
  if(signal_state == LOW && (millis() - debounce_time) > debounce_delay){
    debounce_latch_state = HIGH;
    //Serial.println("DEBOUNCE LATCHED!");
    

  }
  else if(signal_state == HIGH){
    debounce_latch_state = LOW; 
  }


  if(signal_state == LOW && last_debounce_latch_state != debounce_latch_state){
    
     //DETECT SINGLE CLICK FIRST TIME THROUGH LOOP
     if(millis() < (double_click_expires - double_click_window + 250)){
      double_click_state = LOW;
      Serial.println("Single Click!");
     }    
     
     //DOUBLE CLICK BEHAVIOR
     else if (millis() < double_click_expires){
      double_click_state = HIGH;
      Serial.println("Double Click!");
     }
   }



  //SINGLE CLICK BEHAVIOR
  if(debounce_latch_state == HIGH && double_click_state == LOW){
    digitalWrite(led_pin, HIGH);
    digitalWrite(led_pin2, LOW);
  }

  //DOUBLE CLICK BEHAVIOR
  else if (debounce_latch_state == HIGH && double_click_state == HIGH){
    //digitalWrite(led_pin, LOW);
    //digitalWrite(led_pin2, HIGH);    

    //BEHAVIOR COMING INTO DOUBLE CLICK
    if (annoy_latch == LOW){
      int i = 0;
      Serial.println("ANNOY OVERRIDE");
      while (i < flash_count){
        digitalWrite(led_pin, LOW);
        digitalWrite(led_pin2, LOW);
        delay(flash_on_time);
        digitalWrite(led_pin, HIGH);
        digitalWrite(led_pin2, HIGH);
        delay(flash_off_time);
        i++;        
        }
      digitalWrite(led_pin, LOW);
      digitalWrite(led_pin2, LOW);
      annoy_latch = HIGH;
      delay(500);
    }
    //BEHAVIOR AFTER ANNOY_LATCH SWITCHED HIGH
    if (annoy_latch == HIGH){
      //Serial.println("Into annoy_latch = HIGH state");
      digitalWrite(led_pin, LOW);
      digitalWrite(led_pin2, LOW);
    }
    //break; //Do I need this? Maybe not if move annoy_latch = LOW elsewhere
  }


  //Serial.println("On now");
  //digitalWrite(led_pin, LOW);
  //digitalWrite(led_pin2, LOW);

  //OFF BEHAVIOR
  if (debounce_latch_state == LOW){
    digitalWrite(led_pin, HIGH);
    digitalWrite(led_pin2, HIGH);
    annoy_latch = LOW;
  }


  last_debounce_latch_state = debounce_latch_state;
  last_signal_state = signal_state;
  delay(10);

}
