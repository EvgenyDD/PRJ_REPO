
//-------------------------- ALIM V3 ----------------------------------------
//    ALIMENTATION VARIABLE CONSTANT VOLTAGE, CONSTANT COURANT,       0 < 25V
//    choix de 4 gammes de courant, 0<1A, 2A, 4A et 8A           VERSION: 3.0
//         PAR VE2EMM                                                avril 04
//---------------------------------------------------------------------------
#include <16F876a.h>
#device ADC=16                      // les 10 bits de gauche sont valable
#use delay(clock=20000000)
#fuses HS,NOWDT,PUT,BROWNOUT,NOPROTECT,NOLVP,NOCPD,NOWRT
//#rom 0x2100={0x00,0x00,0x00,0x00,0x00}   // zero les premieres 4 cases du EEDATA
#use fast_io(A)
#use fast_io(B)
#use fast_io(C)

//------------------------- VARIABLES -------------------

INT8 i;                //  0 < + 255
int8 commande;         //  0 < + 255
int8 car;              //  0 < + 255
int8 nibble;           //  0 < + 255
int8 boutons;          //  0 < + 255
int8 bit_status;       //  0 < + 255
int16 disp_a;           //  pour gamme de l'affichage Amp
int8 disp_ma;          //  pour gamme de l'affichage mAmp
//-----------

signed int16 volt_moy[16];
signed int16 amp_moy[16];
signed int16 volt_set=0;   // -32768 < +32767
signed int16 volt_lue=0;     //    "        "
signed int16 amp_set=0;  //    "        "
signed int16 amp_lue=0;      //    "        "

//------------------------- FONCTIONS ---------------

void pic_ini(void);
void lcd_ini(void);
void write_cmd_lcd(int8);  // LCD
void write_car_lcd(INT8);  // LCD
void send_nibble(int8);    // LCD
void busy_status(void);        // LCD

void lire_voltage(void);   // lire et afficher E
void lire_courant(void);   // lire et afficher I
void lire_PB(void); // lire les boutons, changer les consignes de voltage et de courant
void set_range(void);
void comparer_Iset_Ilue(void); // determiner qui est en controle, E ou I

//********************************* MAIN ***********************************

void main(void)
{
   pic_ini();
   lcd_ini();
   for(i=0;i<=15;i++) volt_moy[i]=0;
   for(i=0;i<=15;i++) amp_moy[i]=0;


//------- afficher ecran d"acceuil -------
   write_cmd_lcd(128);      // adresse du debut de la 1 re ligne
   printf(write_car_lcd,"ALIMENTATION V3 ");
   write_cmd_lcd(192);      // adresse du debut de la 2 ie ligne
   printf(write_car_lcd,"   PAR VE2EMM   ");
   for(i=0;i<4;i++) delay_ms(250); // laisser affichage 1 1/2 seconde

//---- charger les valeurs du eedata
   for(i=0;i<2;i++) *(&volt_set+i)=read_eeprom(10+i); // reprendre les consignes
   if((volt_set>1023)||(volt_set<0))volt_set=0;
   for(i=0;i<2;i++) *(&amp_set+i)=read_eeprom(12+i);  // du dernier usage
   if((amp_set>1023)||(amp_set<0))amp_set=0;

//---- programme principal
   while(true)
   {
      set_range();// choisir la gamme du courant, 1, 2, 4 ou 8 Amp
      lire_PB(); // lire les boutons et placer les consignes de voltage et de courant

      write_cmd_lcd(128);      // adresse du debut de la 1ere ligne
      printf(write_car_lcd,"%01ld,%03ldA SET %02ld,%01ldV",amp_set/disp_a,amp_set*disp_ma,volt_set/40,volt_set/4);
      SET_PWM1_duty(amp_set);  // amp_set
      SET_PWM2_duty(volt_set); // volt_set

      lire_voltage();       // lire et afficher
      lire_courant();       // lire et afficher
      comparer_Iset_Ilue(); // lire et afficher qui est en controle, E ou I
   }
}

//********************************* FIN MAIN ******************************

//************************************ FONCTIONS *****************************

void pic_ini(void)
{
//-- PORTS
   output_A(0x00);
   output_B(0x00);
   output_C(0x00);
   set_tris_A(0b11011111);
   port_B_pullups(true);
   set_tris_B(0b11111111);
   set_tris_C(0b00000000);
//--- ADC
   setup_adc_ports(RA0_RA1_RA3_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL);
//--- PWM
   SET_PWM1_duty(0); // amp_set
   SET_PWM2_duty(0); // volt_set
   setup_timer_2(t2_div_by_16,255,1);
   setup_ccp1(CCP_PWM_PLUS_3);
   setup_ccp2(CCP_PWM_PLUS_3);
}

//-------------------------------------------------

void lcd_ini(void)
{
	delay_ms(100);        // attendre que le 5V soit stabilise
   output_bit(PIN_A5,0); // E clock
   output_bit(PIN_C0,0); // R/W  R=1,W=0
   output_bit(PIN_C3,0); // R/S 0 pour commande, 1 pour data
	for(i=1;i<=4;i++) {send_nibble(0x30); delay_ms(5);} // reset du LCD
   send_nibble(0x20);    // interface 4 bits
   delay_us(43);
	write_cmd_lcd(0x28);  // interface 4 bits, 5X7
   write_cmd_lcd(0x08);  // display off
	write_cmd_lcd(0x0C);  // display on
	write_cmd_lcd(0x06);  // increment no display shift
}

//*********************** Fonctions du LCD *****************************

void write_cmd_lcd(commande)
{
   busy_status();          // LCD pret?
   output_bit(PIN_C3,0);   // R/S 0 pour commande
	send_nibble(commande);  // placer commande dans LCD
   swap(commande);
   send_nibble(commande);  // placer commande dans LCD
}

//-------------- afficher un caractere sur le LCD --------------------------

void write_car_lcd(car)
{
   busy_status();         // LCD pret?
   output_bit(PIN_C3,1);  // R/S 1 pour data
	send_nibble(car);      // placer commande dans LCD
   swap(car);
   send_nibble(car);      // placer commande dans LCD
}

//-------------------- Envoyer un nibble ----------------

void send_nibble(nibble)
{
   output_bit(PIN_C0,0);  // R/W  R=1, W=0
	output_bit(PIN_A5,0);  //  E line low
   delay_cycles(3);
	output_bit(PIN_A5,1);  // pulser E high
   nibble=nibble&0xF0;    // vider les 4 bits du bas de la byte
   nibble=nibble|(input_C()&0x0F); // charger C0-C3
   output_C(nibble);
   delay_cycles(3);
   output_bit(PIN_A5,0);  // E low
}

//-----------------------

void busy_status() // tester si le LCD est pret a accepter un data
{
   do
   {
      set_tris_C(0b11110000);
      output_bit(PIN_A5,0); // E off
      output_bit(PIN_C0,1); // R/W  R=1,W=0
      output_bit(PIN_C3,0); // R/S 0 pour commande, 1 pour data
      delay_cycles(3);
      output_bit(PIN_A5,1); // pulser E high
      delay_cycles(3);
      bit_status=input_C()&0X80; // lire status
      delay_us(2);
      output_bit(PIN_A5,0); // E off
      delay_cycles(3);

      output_bit(PIN_A5,1); // pulser E high
      delay_us(1);          // faire semblant de lire les 4 bits de status du bas
      output_bit(PIN_A5,0); // E off
      delay_cycles(3);

   }
   while(bit_status==0x80);
   set_tris_C(0b00000000);
}

//***************************** Fin des fonctions LCD ********************************
//************************************************************************************

void lire_voltage()           // lire et afficher
{
      SET_ADC_CHANNEL(0);
      delay_ms(1);
      for(i=15;i>=1;i--) volt_moy[i]=volt_moy[i-1];
      volt_moy[0]=READ_ADC()/64; // tasser a droite la lecture dans les 2 bytes, 6 bits
      volt_lue=0;
      for(i=0;i<=15;i++) volt_lue=volt_lue+volt_moy[i];
      volt_lue=volt_lue/16;
      volt_lue=volt_lue+2;// compenser pour le restant de la division a l'affichage
      //if(volt_lue<=7)volt_lue=0;
      write_cmd_lcd(203);     // adresse du 3/4 de la 2ie ligne
      printf(write_car_lcd,"%02ld,%01ldV",volt_lue/40,volt_lue/4);
}

//----------------------------

void lire_courant() // lire et afficher, tester si I depasse la limite et est en controle
{
      SET_ADC_CHANNEL(1);
      delay_ms(1);
      for(i=15;i>=1;i--) amp_moy[i]=amp_moy[i-1];
      amp_moy[0]=READ_ADC()/64; // tasser a droite la lecture dans les 2 bytes, 6 bits
      amp_lue=0;
      for(i=0;i<=15;i++) amp_lue=amp_lue+amp_moy[i];
      amp_lue=amp_lue/16;
      amp_lue=amp_lue+1;
      write_cmd_lcd(192);    // adresse du debut de la 2ie ligne
      printf(write_car_lcd,"%01ld,%03ldA",amp_lue/disp_a,amp_lue*disp_ma);
}

//----------------------------

void set_range()
{
   if((input(pin_A2)==1)&&(input(pin_A4)==1)){disp_a=1000;disp_ma=1;}
   if((input(pin_A2)==0)&&(input(pin_A4)==1)){disp_a=500;disp_ma=2;}
   if((input(pin_A2)==1)&&(input(pin_A4)==0)){disp_a=250;disp_ma=4;}
   if((input(pin_A2)==0)&&(input(pin_A4)==0)){disp_a=125;disp_ma=8;}
}

//--------------------------------------------------------------

void lire_PB() // lire les boutons et setter voltage et le courant
{
   boutons=input_b();
   if(boutons!=0xFF)
   {
      switch(boutons)
     {
         case 0b01111111:      // grande reduction du voltage *************
            volt_set=volt_set-40;
            if(volt_set<=0) volt_set=0;
            break;
        case 0b10111111:      // grande reduction du courant
            amp_set=amp_set-50;
            if(amp_set<=0) amp_set=0;
            break;
         case 0b11011111:      // grande augmentation du voltage **********
            volt_set=volt_set+40;
            if(volt_set>=1000) volt_set=1000;
            break;
         case 0b11101111:      // grande augmentation du courant
            amp_set=amp_set+50;
            if(amp_set>=1000) amp_set=1000;
            break;
         case 0b11110111:      // petite augmentation du voltage **********
            volt_set=volt_set+4;
            if(volt_set>=1000) volt_set=1000;
            break;
         case 0b11111011:      // petite augmentation du courant
            amp_set=amp_set+1;
            if(amp_set>=1000) amp_set=1000;
            break;
         case 0b11111101:      // petite reduction du voltage *************
            volt_set=volt_set-4;
            if(volt_set<=0) volt_set=0;
            break;
         case 0b11111110:      // petite reduction du courant
            amp_set=amp_set-1;
            if(amp_set<=0) amp_set=0;
            break;
         default:              //  plus qu'un bouton pese
            break;
      }
      for(i=0;i<2;i++) write_eeprom(10+i,*(&volt_set+i));
      for(i=0;i<2;i++) write_eeprom(12+i,*(&amp_set+i));
      delay_ms(350);           // taux de repetition
   }
}

//-------------------------------

comparer_Iset_Ilue()
{
   write_cmd_lcd(198);  // milieu de la 2ie ligne
   if (amp_lue<(amp_set-1)) printf(write_car_lcd," >>> ");
   else printf(write_car_lcd," <<< ");
}

//******************************** Fin des fonctions **************************************
