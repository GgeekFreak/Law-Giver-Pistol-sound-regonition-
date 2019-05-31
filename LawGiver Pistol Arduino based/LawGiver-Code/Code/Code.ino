//Updated Code 28/1/2018
/*
   Note, Voice commands must be recoreded in the following order:
    1) Rapid Fire
    2) Armor Piercing
    3) Grenade
    4) Double Whammy
    5) Flare
    6) Reload

    But they can be called in any order.
 */
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>
#include <TMRpcm.h>

/*
 * Pin Definitions
 */

#define DEFAULT_AMMO 119
#define DEFAULT_AMMO_2 100
#define NO_OF_LEDS 5
#define DIVIDING_FACTOR 20
#define SD_ChipSelectPin 10

#define LED_INDICATOR_1 5
#define LED_INDICATOR_2 6
#define LED_INDICATOR_3 7
#define LED_INDICATOR_4 8
#define LED_INDICATOR_5 A5

int LED_Array[NO_OF_LEDS] = {LED_INDICATOR_1, LED_INDICATOR_2, LED_INDICATOR_3, LED_INDICATOR_4, LED_INDICATOR_5};

#define LED_FOR_TRIGGER 4

#define SPEAKER_PIN 9

#define TRIGGER_BUTTON 2
#define RELOAD_BUTTON 3

/* Speaker Object */
TMRpcm tmrpcm;
TMRpcm Audio_Out;

/* Audio File names */
char *Rapid_Fire = "rapid_b.wav";
char *Armor_Piercing = "Ar_pie_b.wav";
char *Grenade = "Gren_b.wav";
char *Double_Whammy = "dbe_wmy.wav";
char *Flare = "Flare_b.wav";
char *Reload = "Reload.wav";
char *Start_up = "strt_up.wav";
char *No_Ammo = "no_a.wav";
char *Current_Weapon =  "dflt_b.wav";

/* Please fill this fvariables with the matching file names */
char *ammo_exhausted_track = "no_a.wav";
char *Flare_out = "empty_f.wav"; // change the foo.wav with wave you want for flare out sound
char *double_whammy_selected = "doub_w.wav";
char *grenade_selected = "Gren.wav";
char *flare_selected = "Flare.wav";
char *Armor_Piercing_selected = "Ar_pie.wav";
char *rapid_fire_selected = "rapid_f.wav";

/* Bullet Store */
volatile uint16_t Bullet_Rapid_Fire = DEFAULT_AMMO;
volatile uint16_t Bullet_Armor_Piercing = DEFAULT_AMMO;
volatile uint16_t Bullet_Grenade = DEFAULT_AMMO;
volatile uint16_t Bullet_Double_Whammy = DEFAULT_AMMO;
volatile uint16_t Bullet_Flare = DEFAULT_AMMO;

volatile uint16_t *Bullet_Current_Weapon =  Bullet_Rapid_Fire;

// my section

typedef enum{Rapid_Fire_M ,Armor_Piercing_M , Grenade_M ,Double_Whammy_M,Flare_M} mode_t;
volatile mode_t current_mode = Rapid_Fire_M;
volatile bool is_enabled_flag = true;

/////////////////////////////////////////////////////////////////////////
/*
 * Main setup
 */
void setup() {

  Serial.begin(9600);
  
  /* Pin Initializations */
  pinMode(LED_INDICATOR_1, OUTPUT);
  pinMode(LED_INDICATOR_2, OUTPUT);
  pinMode(LED_INDICATOR_3, OUTPUT);
  pinMode(LED_INDICATOR_4, OUTPUT);
  pinMode(LED_INDICATOR_5, OUTPUT);
  
  pinMode(LED_FOR_TRIGGER, OUTPUT);
  
  pinMode(TRIGGER_BUTTON, INPUT);
  pinMode(RELOAD_BUTTON, INPUT);

  /* Attach interrupt to reload pin */
  attachInterrupt(digitalPinToInterrupt(RELOAD_BUTTON), Reload_Func, RISING);
  
  /* Assign speaker pin */
  Audio_Out.speakerPin = SPEAKER_PIN;

  /* SD Initialization */
  if (!SD.begin(SD_ChipSelectPin)) {   Error_Handler2();
        Error_Handler();
        //Serial.println("sd_failed");
      }

  /* Initialize voice recognition module */
  Voice_Recog_Init();
  
  /* Play start up tone */
  Audio_Out.play(Start_up); //this sound file will play each time the arduino powers up, or is reset
  //Error_Handler2();
  Led_command_Received();
  delay(1000);
  Current_Weapon = Rapid_Fire;
  current_mode = Rapid_Fire_M;
  Bullet_Current_Weapon = &Bullet_Rapid_Fire; 
   
}




/////////////////////////////////////////////////////////////////////////
/*
 * Continous Loop
 */
byte com = 0;
void loop() {

 
  /* If voice command is available */
  while(Serial.available())
  {
    com = Serial.read();

    if(is_enabled_flag){
    /* Flash LEDS */
    Led_command_Received();
    
    switch(com)
    {
      case 0x11:
      {
       
        /* Play audio */
        Current_Weapon = Rapid_Fire;
        current_mode = Rapid_Fire_M;
        Bullet_Current_Weapon = &Bullet_Rapid_Fire;
        if(*Bullet_Current_Weapon > 19)
        {
        Audio_Out.play(rapid_fire_selected);
        /* Set current weapon */
        
       }else
        {
          Audio_Out.play(ammo_exhausted_track);
          
          
        }
        
      }
       
      break;

      case 0x12:

      {
        
         /* Play audio */
        
        Current_Weapon = Armor_Piercing;
        current_mode = Armor_Piercing_M;
        Bullet_Current_Weapon = &Bullet_Armor_Piercing;
        if(*Bullet_Current_Weapon > 19)
        {
        Audio_Out.play(Armor_Piercing_selected);
        /* Set current weapon */
        }else
        {
          Audio_Out.play(ammo_exhausted_track); 
          
        }
        
      }
      break;

      case 0x13:
      {
      
        /* Play audio */
        
        Current_Weapon = Grenade;
        current_mode = Grenade_M;
        Bullet_Current_Weapon = &Bullet_Grenade;
        if(*Bullet_Current_Weapon > 19)
        {
        Audio_Out.play(grenade_selected);

        /* Set current weapon */
        }else
        {
           Audio_Out.play(ammo_exhausted_track); 
           
        }
        
      }
      break;

      case 0x14:
      {

         
         Current_Weapon = Double_Whammy;
         current_mode = Double_Whammy_M;
         Bullet_Current_Weapon = &Bullet_Double_Whammy;        
         if(*Bullet_Current_Weapon > 19)
         {
        /* Play audio */
        Audio_Out.play(double_whammy_selected);
        /* Set current weapon */
        }else
        {
            Audio_Out.play(ammo_exhausted_track); 
            
        }
      }
      break;

      case 0x15:
      {
         
        Current_Weapon = Flare;
        current_mode = Flare_M;
        Bullet_Current_Weapon = &Bullet_Flare;
        if(*Bullet_Current_Weapon > 19)
        {
         /* Play audio */
        Audio_Out.play(flare_selected);
        /* Set current weapon */
        
        
        
        }else
        {
           Audio_Out.play(Flare_out);
            
        }
      }
      break;
      
    }
    
    Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
       

  }



  }
  /* If trigger is pressed */
  while( digitalRead(TRIGGER_BUTTON) && is_enabled_flag )
  {

    if(*Bullet_Current_Weapon > 19)
    {
      
     /* Fire current wepon */
        Audio_Out.play(Current_Weapon);
     /* Flsh Trigger */
        if(current_mode ==Double_Whammy_M | current_mode ==Rapid_Fire_M |current_mode==Armor_Piercing_M)
        {
          Trigger_Flash();  
          *Bullet_Current_Weapon =  *Bullet_Current_Weapon-1;
          Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
        }
        
     /* Decrement Bullet count*/
        if(current_mode ==Grenade_M)
        {
         *Bullet_Current_Weapon =  *Bullet_Current_Weapon -20;
         Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
         digitalWrite(LED_FOR_TRIGGER,HIGH);
         delay(100);
         digitalWrite(LED_FOR_TRIGGER,LOW);
         delay(50);
        }
        if(current_mode ==Flare_M)
        {
         *Bullet_Current_Weapon =  *Bullet_Current_Weapon -20;
         Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
         delay(1500);
         digitalWrite(LED_FOR_TRIGGER,HIGH);
         delay(100);
         digitalWrite(LED_FOR_TRIGGER,LOW);
         delay(50);
        }
        
     
     //delay(500);
    }
    else
    {
      
      if(current_mode ==Flare_M)
      {
         
         Audio_Out.play(Flare_out); 
      }
      else
      {
      Audio_Out.play(ammo_exhausted_track);
      
      Audio_Out.play(No_Ammo);
      }    
      delay(1000); //!==RF Delay
    }

    //Display_lED( Bullet_Current_Weapon / DIVIDING_FACTOR );
    
    
    if(current_mode != Rapid_Fire_M)
    {
      
      while( digitalRead(TRIGGER_BUTTON))
      {
         delay(15); 
      } 
         
    }
  }

}


/*
 * Function to reload bullets
 */
void Reload_Func()
{
   /* Play audio */
   volatile uint32_t reloadCounts = 0;
   while(digitalRead(RELOAD_BUTTON))reloadCounts++;
   if (reloadCounts>50000)
   {
    if (is_enabled_flag)
    {
      //turn off the lights
     digitalWrite(LED_INDICATOR_5, LOW);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_4, LOW);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_3, LOW);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_2, LOW);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_1, LOW);
      is_enabled_flag = false;
    }else
    {
      //turn on the lights
     Audio_Out.play(Start_up);
     digitalWrite(LED_INDICATOR_1, HIGH);
     for(volatile uint32_t i = 0;i<30000;i++);
     digitalWrite(LED_INDICATOR_2, HIGH);
     for(volatile uint32_t i = 0;i<30000;i++);
     digitalWrite(LED_INDICATOR_3, HIGH);
     for(volatile uint32_t i = 0;i<30000;i++);
     digitalWrite(LED_INDICATOR_4, HIGH);
     for(volatile uint32_t i = 0;i<30000;i++);
     digitalWrite(LED_INDICATOR_5, HIGH);
      is_enabled_flag = true;
      
      Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
    }
   }else
   {
    if(is_enabled_flag)
    {
      Display_reload_led();
   
     Audio_Out.play(Reload);
     Bullet_Rapid_Fire = DEFAULT_AMMO;
     Bullet_Armor_Piercing = DEFAULT_AMMO;
     Bullet_Grenade = DEFAULT_AMMO;
     Bullet_Double_Whammy = DEFAULT_AMMO;
     Bullet_Flare = DEFAULT_AMMO;
     
     Display_lED( *Bullet_Current_Weapon / DIVIDING_FACTOR );
    }
   }
   
   
   
}

/*
 *  Function to initialize voice module
 */
void Voice_Recog_Init()
{
  Serial.write(0xAA);
  Serial.write(0x37);
  delay(1000);
  Serial.write(0xAA);
  Serial.write(0x21);
}


/*
 * Error handler function
 */
void Error_Handler()
{
  while(1)
  {
     digitalWrite(LED_INDICATOR_5, HIGH);
     digitalWrite(LED_INDICATOR_4, HIGH);
     digitalWrite(LED_INDICATOR_3, HIGH);
     digitalWrite(LED_INDICATOR_2, HIGH);
     digitalWrite(LED_INDICATOR_1, HIGH);

     //delay(500);

     digitalWrite(LED_INDICATOR_5, LOW);
     digitalWrite(LED_INDICATOR_4, LOW);
     digitalWrite(LED_INDICATOR_3, LOW);
     digitalWrite(LED_INDICATOR_2, LOW);
     digitalWrite(LED_INDICATOR_1, LOW);

     delay(500);
  }
}
void Error_Handler2()
{
  while(1)
  {
     digitalWrite(LED_INDICATOR_5, HIGH);
     digitalWrite(LED_INDICATOR_4, HIGH);
     digitalWrite(LED_INDICATOR_3, HIGH);
     digitalWrite(LED_INDICATOR_2, HIGH);
     digitalWrite(LED_INDICATOR_1, HIGH);
  }
}

/*
 * Function to be called when voice command is received
 */
void Led_command_Received()
{
     /* Write all LEDS low */
     
     digitalWrite(LED_INDICATOR_5, LOW);
     digitalWrite(LED_INDICATOR_4, LOW);
     digitalWrite(LED_INDICATOR_3, LOW);
     digitalWrite(LED_INDICATOR_2, LOW);
     digitalWrite(LED_INDICATOR_1, LOW);

     /* Switch LEDS on one after the other */

     delay(500);
     digitalWrite(LED_INDICATOR_1, HIGH);
     delay(175);
     digitalWrite(LED_INDICATOR_2, HIGH);
     delay(175);
     digitalWrite(LED_INDICATOR_3, HIGH);
     delay(175);
     digitalWrite(LED_INDICATOR_4, HIGH);
     delay(175);
     digitalWrite(LED_INDICATOR_5, HIGH);
     

}

/*
 * Trigger led
 */
 void Trigger_Flash()
 {
       
       digitalWrite(LED_FOR_TRIGGER,HIGH);
       delay(30);
       digitalWrite(LED_FOR_TRIGGER,LOW);
       delay(30);
     
   
  
 }

void Display_reload_led()
{
     
     digitalWrite(LED_INDICATOR_5, LOW);
     digitalWrite(LED_INDICATOR_4, LOW);
     digitalWrite(LED_INDICATOR_3, LOW);
     digitalWrite(LED_INDICATOR_2, LOW);
     digitalWrite(LED_INDICATOR_1, LOW);

     /* Switch LEDS on one after the other */
     
     digitalWrite(LED_INDICATOR_1, HIGH);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_2, HIGH);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_3, HIGH);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_4, HIGH);
     for(volatile uint32_t i = 0;i<10000;i++);
     digitalWrite(LED_INDICATOR_5, HIGH);
     
}
void Display_lED(int n)
{
  

     for(int i=0; i<n; i++)
     {
       digitalWrite(LED_Array[i], HIGH);
     }

       for(int i=n; i<NO_OF_LEDS; i++)
     {
       digitalWrite(LED_Array[i], LOW);
     }
}





