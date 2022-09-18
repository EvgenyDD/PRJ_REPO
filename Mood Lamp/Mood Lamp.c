float red, green, blue;     //now colors
unsigned int tred, tgreen, tblue;//tagert colors
float delta_red, delta_green, delta_blue;       //âeëè÷èía ïpèáaâêè ça øaã
unsigned int time;  //äëèòeëüíocòü øaãa


void generate_tagert_color(void)
{
      tred = rand();
      tgreen = rand();
      tblue = rand();
      tred = tred & 0xFF;       //to max 255
      tgreen = tgreen & 0xFF;
      tblue = tblue & 0xFF;
};

void delay(unsigned int delayt)               //çaäepæêa delay_ms() ía âxoä ïpèíèìaeò òoëüêo êoícòaíòy( 
{
  while (delayt != 0)
  {
    delay_ms(1);
    delayt--;
  };
};

unsigned int temp1, temp2, temp3, temp4;
============    
       do 
       {
        generate_tagert_color();
        temp1 = (abs(tred - tgreen));
        temp2 = (abs(tred - tblue));
        temp3 = (abs(tgreen - tblue));
       }
       while (( temp1 < 150 ) && (temp2 < 150) && (temp3 < 150));  //ecëè âce öâeòa áyäyò ïpèìepío paâíûe òo áyäeò áeëûé ÷òo íe èíòepecío
      
       if (rand() < 13100)                                         //c âepoÿòíocòüş 0,4 (32767) âûøèáaeì oäèí öâeò
       {
         do
         {
            temp4 = rand() & 0x000F ;
         }
        while ((temp4 == 0) || (temp4 > 3));
       
        if (temp4 == 1)                                            //íe ïoâeçëo êpacíoìy
        {                                                          //ãacèì äo 0 - 20% oò ècxoäíoão
          do
           {
             temp4 = rand() & 0x0FFF;
           }
          while ((temp4 == 0) || (temp4 > 3276));
          
          tred = ceil ((float) (temp4 / 32767) * tred);
        };
        
        if (temp4 == 2)
        {
          do
           {
             temp4 = rand() & 0x0FFF;
           }
          while ((temp4 == 0) || (temp4 > 3276));
          
          tgreen = ceil ((float) (temp4 / 32767) * tgreen);
        };
        
        if (temp4 == 3)
        {
          do
           {
             temp4 = rand() & 0x0FFF;
           }
          while ((temp4 == 0) || (temp4 > 3276));
          
          tblue = ceil ((float) (temp4 / 32767) * tblue);
        };
        
       }
       
  //çía÷eíèe ïpèáaâêè. äoxoäèì äo íyæíoão öâeòa ça 255 øaãoâ
       delta_red = (float) (tred - red) / 255;
       delta_green = (float) (tgreen - green) / 255;
       delta_blue = (float) (tblue - blue) / 255; 
      
       
       do
        {
          temp4 = rand() & 0x00FF;
        }
       while ((temp4 < 50) || (temp4 > 100));   //cêoêpocòü ïepexoäa cëy÷aéía oò 5 äo 10c
      
       time = ceil( ((float) temp4 / 255) * 10);             //çaäepæêa äoëæía áûòü öeëoé
       
            
       do 
       {
        if (tred != ceil(red))
         red = red + delta_red;
        if (tgreen != ceil(green))
         green = green + delta_green;
        if (tblue != ceil(blue))
         blue = blue + delta_blue;
       
        OCR1A = ceil(blue);
        OCR1B = ceil(green);
        OCR2 = ceil(red);
      
        delay(time);
       }
       while ((tred != ceil(red)) || (tgreen != ceil(green)) || (tblue != ceil(blue))); 
       
       delay(time * 255);                         //còpeìèëècü ê ıòoìy öâeòy, çaäepæèì eão
