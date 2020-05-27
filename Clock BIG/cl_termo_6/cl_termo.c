#include "clock.h"
// ���� � �������� ���������� � ����� ���������� ������.
// ����� ������� �����.
// ������ ��� ����� �����������.
// ��������� NBO ( �������� ��������� ).

//--------------------------------------
static void port_init(void)
{
    DDRLCD = 0xFF;
    DDRSERIAL |= PIN_SERIAL | sig_min_00;
    DDR_OUT |= PIN_calls1 | PIN_calls_osn | PIN_T_STAT; // | PIN_timer;
    PORT_POWER_DETECT |= _power_;
}
//**************************************
static void timer0_init(void)
{
    TCCR0 = 0x01;
}
//**************************************
static void timer2_init(void)
{
    TCCR2 = 0x00;      //stop
    ASSR = (1 << AS2); //set async mode
    TCCR2 = 0x01;      //start
}
//**************************************
void timer1_init(void)
{
    TCCR1B = 0; //stop
    TCCR1A = 0;
    OCR1AH = ocr1[ocr1ahi];
    OCR1AL = ocr1[ocr1alow];
    DDR_OC1A &= ~OC1A; //OC1A
}
//**************************************
void timer1_init_in_pwm(void)
{
    DDR_OC1A |= OC1A;       //OC1A
    TCCR1B = 0;             //stop
    TCCR1A = (1 << COM1A0); //Toggle OC1A/OC1B on Compare Match //TCCR1B WGM12=1 CTC top-OCR1A
}
//**************************************
static void shim_true(void)
{
    timer1_init_in_pwm();
    TCCR1B = (1 << CS10) | (1 << WGM12);
}
//**************************************

void init_devices(void)
{
    cli(); //disable all interrupts
    port_init();
    timer0_init();
    timer1_init();
    MCUCR = 0x00;
    GICR = 0x00;
    ACSR = _BV(ACD);
    TIMSK = (1 << TOIE2) | (1 << TOIE0); //(1<<OCIE2)| timer interrupt sources |(1<<OCIE1A)
    sei();                               //re-enable interrupts
}

// ***********************************************************
//������ ���� �� ��
unsigned char EEPROM_READ(unsigned int uiAddress)
{
    /* ��������  ���������� ���������� ������*/
    while(EECR & (1 << EEWE))
        ;
    /* ��������� ��������� �������� */
    EEAR = uiAddress;
    /* ���������� ������ �� ����� ����� ��������� EERE */
    EECR |= (1 << EERE);
    /* ������� ������ �� �������� ������ ������*/
    return EEDR;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
    if(EEPROM_READ(uiAddress) != ucData)
    {
        /* ������� ��������� ���������� ������ */
        while(EECR & (1 << EEWE))
            ;
        cli();
        /* �������� ������ � ������ */
        EEAR = uiAddress;
        EEDR = ucData;
        /* ������ ���. 1 �  EEMWE */
        EECR |= (1 << EEMWE);
        /* ������ ������ � ������ ����� ��������� EEWE */
        EECR |= (1 << EEWE);
        sei();
    }
}
//*********************************************
void read_data_EEPROM(void)
{
    unsigned char *a, i;

    a = &calls[0][0];
    for(i = EEPROM_calls; i < kolvo_bud * 3 + EEPROM_calls; i++, a++)
    {
        *a = EEPROM_READ(i);
    }
    for(i = EEPROM_regim_state_time; i < Nreg_st + EEPROM_regim_state_time; i++)
        regim_state_time[i - EEPROM_regim_state_time] = EEPROM_READ(i);

    a = (uint8_t *)&termostat;
    for(i = EEPROM_termostat; i < EEPROM_termostat + sizeof(termostat); i++)
        *a++ = EEPROM_READ(i);

    temper_termostat = termostat.temperature[0];

    otsroshka_num = EEPROM_READ(EEPROM_otsrochka_num);
    signal_bud = EEPROM_READ(EEPROM_sig_pin_or_bip);

    if(EEPROM_READ(EEPROM_verifu_but) == 0xaa)
    {
        for(i = EEPROM_ocr1a; i < 4 + EEPROM_ocr1a; i++)
            ocr1[i - EEPROM_ocr1a] = EEPROM_READ(i);
    }

    if((i = EEPROM_READ(EEPROM_light)) < sizeof(light_swith))
    {
        day_light = i; //EEPROM_READ(EEPROM_light);
        night_light = EEPROM_READ(EEPROM_light + 1);
        tim_c_light = EEPROM_READ(EEPROM_light + 2);
        tim_do_light = EEPROM_READ(EEPROM_light + 3);
    }

    if((i = EEPROM_READ(EEPROM_correction)) < sizeof(corr_in_lcd) / 2)
    {
        correction = i;
        corr_flag = EEPROM_READ(EEPROM_corr_flag);
    }

    vid_t = EEPROM_READ(EEPROM_vid_t);

    lcd_light = day_light;
}

//*************** 1 Wire DALLAS ************************************************
void pullup_off(void)
{
    DDR_1wire &= ~pwire0;
    DDR_1wire &= ~pwire1;
    PORT_1wire &= ~pwire0;
    PORT_1wire &= ~pwire1;
}
//***********************************************
unsigned char readbit(void)
{
    unsigned char i;
    cli();
    DDR_1wire |= out_1wire; //line down

    //	asm("nop");
    asm("nop");
    asm("nop");              //1.25us
    DDR_1wire &= ~out_1wire; //line up through 1.25us
    _delay_us(12);           //11
    i = PIN_1wire;
    sei();
    _delay_us(47); //47
    return i;
}
//***********************************************
void readbyte(uint8_t *pw)
{
    unsigned char i, r, read_byte[2] = {0, 0}, pinwire;

    pullup_off();
    for(i = 0, r = 1; i < 8; i++)
    {
        pinwire = readbit();
        if(pinwire & pwire0) read_byte[0] |= r;
        if(pinwire & pwire1) read_byte[1] |= r;
        r <<= 1;
    }
    PORT_1wire |= power_suplly_1w;
    DDR_1wire |= power_suplly_1w;
    pw[0] = read_byte[0];
    pw[1] = read_byte[1];
}
//***********************************************
static void writebit0(void)
{
    cli();
    DDR_1wire |= out_1wire;
    _delay_us(60); //60
    DDR_1wire &= ~out_1wire;
    sei();
}
//***********************************************
void writebit1(void)
{
    cli();
    DDR_1wire |= out_1wire;
    _delay_us(3);
    DDR_1wire &= ~out_1wire;
    sei();
    _delay_us(57); //57
}
//***********************************************
void writebyte(unsigned char byte)
{
    unsigned char i;
    pullup_off();
    for(i = 0; i < 8; i++)
    {
        if(byte & _BV(0))
            writebit1();
        else
            writebit0();
        byte >>= 1;
    }
    PORT_1wire |= power_suplly_1w;
    DDR_1wire |= power_suplly_1w;
}
//****************************************

void detectPresence(void)
{
    unsigned char i;

    pullup_off();
    out_1wire = pwire0 | pwire1;

    DDR_1wire |= pwire0;
    DDR_1wire |= pwire1;
    _delay_ms(0.48); //0.48

    cli();
    DDR_1wire &= ~pwire0;
    DDR_1wire &= ~pwire1;
    _delay_us(60); //60
    i = PIN_1wire;
    sei();

    if(i & pwire0)
    {
        out_1wire &= ~pwire0;
        PORT_OUT &= ~PIN_T_STAT;
    } // ��� ������� - ����� ���������� � ����
    if(i & pwire1) out_1wire &= ~pwire1;

    if(out_1wire == 0)
    {
        count_com_1w = 0;
        return;
    }

    _delay_ms(0.42); //0.42
    i = PIN_1wire;
    if(!(i & pwire0))
    {
        out_1wire &= ~pwire0;
        PORT_OUT &= ~PIN_T_STAT;
    } // ��� ������� - ����� ���������� � ����
    if(!(i & pwire1)) out_1wire &= ~pwire1;

    if(out_1wire == 0)
    {
        count_com_1w = 0;
        return;
    }

    power_suplly_1w &= out_1wire;
    PORT_1wire |= power_suplly_1w;
    DDR_1wire |= power_suplly_1w;
}

//****************************************
static void scratchpad_save(void)
{

    detectPresence(); //���� ������ ������������
    writebyte(0xCC);  //SKIP ROM [CCh]
    writebyte(0x4E);  //WRITE SCRATCHPAD [4Eh]
    writebyte(5);     //	TH
    writebyte(5);     //	TL
    writebyte(0x7f);  //	Statys 12bit

    detectPresence(); // ��������� � EEPROM
    writebyte(0xCC);  //SKIP ROM [CCh]
    writebyte(0x48);  //COPY SCRATCHPAD [48h]
    PORT_1wire |= power_suplly_1w;
    DDR_1wire |= power_suplly_1w;
}
//****************************************

void format_t(unsigned char p)
{
    int t;
    t = ((unsigned int)(tempir_msb[p] << 8)) | tempir_lsb[p]; //msb
    temperatura[p] = (t * 10) >> 4;
}
//**************************************************
void izmerenie_t(void)
{

    if(!count_com_1w)
    { //�����
        count_com_1w++;
        power_suplly_1w = 0;
        detectPresence(); //���� ������ �����������
        return;
    }

    if(count_com_1w == 1)
    {
        count_com_1w++;

        writebyte(0xCC); //SKIP ROM [CCh]
        writebyte(0xB4); //READ POWER SUPPLY [B4h]
        pullup_off();
        power_suplly_1w = readbit();        //���� ������ ������� �� ���������� �������
        power_suplly_1w = ~power_suplly_1w; //��� ��� ������ ��� �������, ���������� �������
        power_suplly_1w &= out_1wire;       //pwire0|pwire1;
        PORT_1wire |= power_suplly_1w;
        DDR_1wire |= power_suplly_1w;
        return;
    }

    if(count_com_1w == 2)
    { //����� ��������������
        count_com_1w++;
        detectPresence(); //�����
        return;
    }

    if(count_com_1w == 3)
    {
        count_com_1w++;
        writebyte(0xCC);   //SKIP ROM [CCh]
        writebyte(0x44);   //CONVERT T [44h]
        time[wire1] = 120; //1sek
        return;
    }

    if(count_com_1w == 4)
    {
        if(time[wire1] < -8) count_com_1w++;
        return;
    }

    if(count_com_1w == 5)
    { //������ ����e������
        count_com_1w++;
        detectPresence(); //�����
        return;
    }

    if(count_com_1w == 6)
    {
        count_com_1w++;
        writebyte(0xCC); //SKIP ROM [CCh]
        writebyte(0xBE); //READ SCRATCHPAD [BEh]
        return;
    }

    if(count_com_1w == 7)
    {
        count_com_1w = 0;
        readbyte(tempir_lsb); //������ �����������
        readbyte(tempir_msb);
        format_t(0);
        format_t(1);
#define reg_termostat 3
        if(!(regim == reg_termostat + 2))
        {
            //--------------------------------------------------------
            if(temperatura[ds0] <= temper_termostat * 10 - termostat.gisterezis)
                PORT_OUT |= PIN_T_STAT; //���� (���_������) ������ ��� (�����-����������)  ���
            else if(temperatura[ds0] >= temper_termostat * 10)
                PORT_OUT &= ~PIN_T_STAT;
            //--------------------------------------------------------
        }
    }
}
//*************** END 1 Wire *********************
//***********************************************/
static void change_light_var(void)
{
    if(time_flag & _BV(fl_plus))
    {
        if(++change_light >= lcd_light) time_flag &= ~_BV(fl_plus); // ��� ��� ��������� �����
    }
    else
    {

        if(change_light > 1)
            change_light--;
        else
            time_flag |= _BV(fl_plus); // ���� ������
    }
}
//**************************************
static uint8_t opred_light_current_num(void)
{
    char l;
    unsigned int a = 0x100;

    l = lcd_light;
    if(mask)
    {
        a >>= znakomesto;
        if(mask & a) l = change_light;
    }
    else
    {
        time_flag &= ~_BV(fl_plus);
        change_light = lcd_light;
    }
    return l;
}

//******************74hc595***********************************
void data_shift(unsigned char data)
{
    for(uint8_t i = 8; i; i--)
    { // ������  8 ���
        if(data & _BV(7))
            PORTSERIAL |= _BV(DATA);
        else
            PORTSERIAL &= ~_BV(DATA);
        data <<= 1;
        PORTSERIAL |= _BV(SHIFT);
        PORTSERIAL &= ~_BV(SHIFT);
    }
}

static void display(void)
{
    PORTSERIAL |= _BV(STORAGE); //���� �������
#ifdef katod
    data_shift(~(1 << (znakomesto)));
#else
    data_shift(1 << (znakomesto));
#endif

    PORTSERIAL &= ~_BV(STORAGE);
    PORTSERIAL |= _BV(STORAGE);
    PORTSERIAL &= ~_BV(STORAGE);

    if(znakomesto == 7)
    {
        PORTSERIAL |= _BV(SHIFT);
        PORTSERIAL &= ~_BV(SHIFT);
    }
    if(znakomesto == 8)
#ifdef katod
        data_shift(0x0);
#else
        data_shift(0xFF);
#endif
    PORTSERIAL &= ~(_BV(DATA) | _BV(SHIFT) | _BV(STORAGE));
}

//*************************************************************
void skankey(void)
{
    unsigned char key_lokal = 17;

    key = key_lokal;

    // PORTID|=pin_id;
    PORTSERIAL |= _BV(STORAGE);

    DDRLCD = 0xF0;
    PORTLCD = 0x0F;
    _delay_us(10);

    if(!str0)
        key_lokal = 0;
    else if(!str4)
        key_lokal = 4;
    else if(!str8)
        key_lokal = 8;
    else if(!str12)
        key_lokal = 12;
    else
        key_lokal = 17; //goto vool;

    if(key_lokal != 17)
    {
        if(++anti_drebezg[0] == 35) //35)
        {
            key = key_lokal;
            if((ocr1[ocr1ahi_but]) || (ocr1[ocr1alow_but]))
            {
                OCR1AH = ocr1[ocr1ahi_but];  //0x04;
                OCR1AL = ocr1[ocr1alow_but]; //0x00;
                shim_true();                 //
                time_flag |= _BV(fl_sig_butt);
                time[note_but] = 7;
            }
        }
        if(anti_drebezg[0] == anti_drebezg[2])
        {                                                  //anti_drebezg[0] ��� �����������
            anti_drebezg[0] = 0;                           //anti_drebezg[1] ���-�� ���� ����� �����������
            anti_drebezg[1]++;                             //anti_drebezg[2] �������� ����� ����� �����
            if(anti_drebezg[1] == 3) anti_drebezg[2] = 45; //55;
        }
        time_in_regim = 25;
    }
    else
    {
        anti_drebezg[0] = 0;
        anti_drebezg[1] = 0;
        anti_drebezg[2] = 200;
    }
#ifdef katod
    PORTLCD = 0x00;
#else
    PORTLCD = 0xFF;
#endif
    DDRLCD = 0xFF;
}

//**************************************
void inc_dec_var_bit(unsigned char *var, unsigned char bit_n)
{
    if(PLUS) (*var) |= _BV(bit_n);
    if(MINUS) (*var) &= ~_BV(bit_n);
}

//**************************************
void inc_dec_var(unsigned char *var, unsigned char limit)
{
    if(PLUS)
    {
        if((*var) < limit)
            (*var)++;
        else
            *var = 0;
    }
    if(MINUS)
    {
        if(*var)
            (*var)--;
        else
            *var = limit;
    }
}
//**************************************
void mask_next_regim(unsigned int m, unsigned char next_F, unsigned char SET)
{
    if(!anti_drebezg[0]) mask = m;
    if(F_) regim = next_F;
    if(SET_) regim = SET;
    key = 17;
}

void next_regim(void)
{
    if(PLUS)
    {
        if(regim < 218)
            regim++;
    }
    if(MINUS)
    {
        if(regim > 210)
            regim--;
    }
}

void reg_see_init(void)
{
    time_view_buf = 3;
    regim_state_time[Nreg_st] = 0;
}

//**************************************
static void key_action(void)
{ //mask_next_regim( maska regima, if (key F_), if (key SET_) )

    mask = 0;

    if(signal)
    {
        mask = 0xFFFF;
        if(key != 17)
            if(!((regim >= 52) && (regim <= 54)))
            {
                time_flag |= _BV(fl_signal_cls);
                time_flag &= ~_BV(fl_signal_out_enable);
                if(F_) time_flag &= ~_BV(fl_sig_otsr); // ���� ������ � ��������� �� ������ ���� ��������,
                regim = 0;
                return;
            }
    }

    if(!regim)
    {
        if(F_)
        {
            regim = 7;
        }
        else if(SET_)
        {
            regim = 100;
        }
        else if(MINUS)
            regim = 31;
        else if(PLUS)
        {
            reg_see_init();
            regim = 91;
        }
        return;
    }

    if(regim == 7)
    {
        mask_next_regim(0xFFFF, 8, 31);
        return;
    } //calls
    if(regim == 8)
    {
        mask_next_regim(0xFFFF, 9, reg_termostat);
        return;
    } //termostat
    if(regim == 9)
    {
        mask_next_regim(0xFFFF, 0, 210);
        return;
    } //setup

    if(regim == 100)
    { //��� ������� set � ������ ���� ��� ����� 100
        if((MINUS) || (PLUS))
        {
            time_flag &= ~_BV(fl_sig_otsr);
            regim = 0;
        }
        mask_next_regim(0, 7, 0);
        return;
    }
    //#define reg_termostat	3 // ��������� � �. izmerenie_t();
    if(regim == reg_termostat)
    {
        mask_next_regim(0, 0, reg_termostat + 1); //t00-00-00
        return;
    }
    if(regim == reg_termostat + 1)
    {
        inc_dec_var(&temper_termostat, 99);
        mask_next_regim(0b110000, 0, reg_termostat + 2);
        return;
    }
    if(regim == reg_termostat + 2)
    {
        if(PLUS) PORT_OUT |= PIN_T_STAT;
        if(MINUS) PORT_OUT &= ~PIN_T_STAT;
        mask_next_regim(0b1000, 0, reg_termostat);
        return;
    }
    //CALLS//****************************
    if(regim == 31)
    { //set number calss
        inc_dec_var(&budilnik_nomer, kolvo_bud - 1);
        mask_next_regim(0b100000000, 0, 32);
        return;
    }
    if(regim == 32)
    { //on off calls
        inc_dec_var_bit(&calls[budilnik_nomer][2], 7);
        mask_next_regim(0b1100000, 31, 33);
        return;
    }
    if(regim == 33)
    { //set hour calss
        inc_dec_var(&calls[budilnik_nomer][c_hour], 23);
        mask_next_regim(0b1100000, 31, 34);
        return;
    }
    if(regim == 34)
    { //set min calls
        uint8_t a;
        inc_dec_var(&calls[budilnik_nomer][c_min], 59);
        if(budilnik_nomer < kolvo_osn)
            a = 35;
        else
            a = 45;
        mask_next_regim(0b11000, 31, a);
        return; //
    }

    if(regim == 35)
    {
        mask = 0b100000;
        if(PLUS) calls[budilnik_nomer][2] |= _BV(p_index);
        if(MINUS) calls[budilnik_nomer][2] &= ~_BV(p_index);
        if(SET_) p_index++;
        if(p_index == 7) p_index = 0, regim = 42;
        if(F_) regim = 31;
        return;
    }

    if(regim == 42)
    { //save current budilnik in eeprom
        for(uint8_t a = EEPROM_calls + budilnik_nomer * 3, i = 0; i < 3; a++, i++)
            EEPROM_write(a, calls[budilnik_nomer][i]);
        regim = 31;
        return;
    }
    if(regim == 45)
    {
        inc_dec_var(&calls_pin[budilnik_nomer - kolvo_osn][h_hour], 23); //c_hour
        mask_next_regim(0b110000000, 31, 46);
        return;
    }
    if(regim == 46)
    {                                                                   // set min
        inc_dec_var(&calls_pin[budilnik_nomer - kolvo_osn][h_min], 59); //c_min
        mask_next_regim(0b1100000, 31, 47);
        return;
    }

    if(regim == 47)
    {                                                                   // set min
        inc_dec_var(&calls_pin[budilnik_nomer - kolvo_osn][h_sek], 59); //c_sek
        mask_next_regim(0b11000, 31, 35);
        return;
    }
    //---------------- end calls -------------------

    //-----------------PORT-------------------------
    if(regim == 211)
    { //120
        next_regim();
        mask_next_regim(0xFFFF, 0, 71); //Port  C
        return;
    }
    if(regim == 71)
    { //_pin-0_ _c
        if(PLUS) PORT_OUT |= PIN_calls1;
        if(MINUS) PORT_OUT &= ~PIN_calls1;
        mask_next_regim(0b1000, 212, 212);
        return;
    }
    //--------------------END PORT-----------------

    //--------------------t-stat-------------------
    if(regim == 212)
    {
        next_regim();
        mask_next_regim(0xFFFF, 0, 80);
        return;
    }
    if(regim == 80)
    {
        uint8_t i;
        inc_dec_var(&p_index, kolvo_zon);
        if(p_index == kolvo_zon)
            i = 84;
        else
            i = 81;
        mask_next_regim(0b100000000, 212, i);
        return;
    }
    if(regim == 81)
    {
        inc_dec_var(&termostat.time[p_index], 23);
        mask_next_regim(0b110000000, 85, 82);
        return;
    }
    if(regim == 82)
    {
        uint8_t j;
        if(p_index < kolvo_zon - 1)
            j = p_index + 1;
        else
            j = 0;
        inc_dec_var(&termostat.time[j], 23);
        mask_next_regim(0b1100000, 85, 83);
        return;
    }
    if(regim == 83)
    {
        inc_dec_var(&termostat.temperature[p_index], 99);
        mask_next_regim(0b11000, 85, 85);
        if(regim == 85) p_index++;
        return;
    }
    if(regim == 84)
    {
        inc_dec_var(&termostat.gisterezis, 99);
        mask_next_regim(0b11000, 85, 85);
        return;
    }
    if(regim == 85)
    {
        uint8_t *p;
        p = (uint8_t *)&termostat;
        for(uint8_t i = EEPROM_termostat; i < EEPROM_termostat + sizeof(termostat); i++)
            EEPROM_write(i, *(p++));
        regim = 80;
        return;
    }
    //--------------------END t-stat---------------

    //--------------------CLOCK--------------------
    if(regim == 210)
    { //121
        next_regim();
        mask_next_regim(0xFFFF, 0, 21);
        return;
    } //clock 4ACb|
    if(regim == 21)
    { //set min hour
        inc_dec_var(&hour[h_min], 59);
        mask_next_regim(0b110000, 9, 22);
        return;
    }
    if(regim == 22)
    { //set hour
        inc_dec_var(&hour[h_hour], 23);
        mask_next_regim(0b110000000, 0, 23);
        return;
    }
    if(regim == 23)
    { //set day hour
        inc_dec_var(&hour[h_day], 7 - 1);
        mask_next_regim(0b100000, 0, 24);
        return;
    }
    if(regim == 24)
    { //set num hour
        inc_dec_var(&hour[h_num], 31 - 1);
        mask_next_regim(0b110000000, 0, 25);
        return;
    }
    if(regim == 25)
    { // set month hour
        inc_dec_var(&hour[h_month], 12 - 1);
        mask_next_regim(0b11000, 0, 26);
        return;
    }
    if(regim == 26)
    { //set hour sek
        if(MINUS)
        {
            if(hour[h_sek] > 31) hour[h_min]++;
            hour[h_sek] = 0;
            clock_ = 0;
        }
        mask_next_regim(0b11000, 0, 27);
        return;
    }
    if(regim == 27)
    { //set corection znak
        inc_dec_var_bit(&corr_flag, fl_corr_znak_pl);
        mask_next_regim(0x80, 0, 28);
        return;
    }
    if(regim == 28)
    { //set hour
        inc_dec_var(&correction, sizeof(corr_in_lcd) / 2 - 1);
        mask_next_regim(0x78, 0, 211);
        if(regim == 211)
        {
            EEPROM_write(EEPROM_correction, correction);
            EEPROM_write(EEPROM_corr_flag, corr_flag);
        }
        return;
    }
    //-----------------end cklock-----------------------

    //-------------SEE----------------
    if(regim == 213)
    { //122
        reg_see_init();
        next_regim();
        mask_next_regim(0xFFFF, 0, 91); //_��t_SEE_
        return;
    }
    if(regim == 91)
    { //F1 r.1-000
        inc_dec_var(&regim_state_time[Nreg_st], Nreg_st / 2 - 1);
        mask_next_regim(0b100000000, 213, 92);
        return;
    }
    if(regim == 92)
    { //F1 r.1-000
        inc_dec_var(&regim_state_time[regim_state_time[Nreg_st] * 2], 8);
        mask_next_regim(0b1000000, 91, 93);
        return;
    }
    if(regim == 93)
    { //r.1-000 F1
        inc_dec_var(&regim_state_time[regim_state_time[Nreg_st] * 2 + 1], 99);
        mask_next_regim(0b11000, 94, 94);
        return;
    }

    if(regim == 94)
    {
        for(uint8_t i = EEPROM_regim_state_time; i < Nreg_st + EEPROM_regim_state_time; i++)
            EEPROM_write(i, regim_state_time[i - EEPROM_regim_state_time]);

        regim = 91;
        return;
    }
    //-------------END SEE----------------

    //-------------yct bud----------------
    if(regim == 214)
    { //123
        next_regim();
        mask_next_regim(0xFFFF, 0, 75); //yct bud
        return;
    }
    if(regim == 75)
    { //CAL. bIP
        inc_dec_var(&signal_bud, 2);
        mask_next_regim(0b000111000, 214, 76);
        return;
    }
    if(regim == 76)
    {                                   //otsroch. on
        time_flag &= ~_BV(fl_sig_otsr); //���� �������� ���� � fl_otsrochka_true=0 ����� 1 ��������� ������, � ��� ���
        inc_dec_var(&otsroshka_num, 30);
        mask_next_regim(0b000011000, 214, 214);
        if(regim == 214)
        { //������ ��� ��� ��� ������
            EEPROM_write(EEPROM_sig_pin_or_bip, signal_bud);
            EEPROM_write(EEPROM_otsrochka_num, otsroshka_num);
        }
        return;
    }
    //-------------------end  yct bud--------------------

    //-------------------light--------------------------
    if(regim == 215)
    { //125
        next_regim();
        mask_next_regim(0xFFFF, 0, 11);
        return;
    } //light
    if(regim == 11)
    {
        inc_dec_var(&day_light, sizeof(light_swith) - 1);
        if(!day_light) day_light = 1;
        mask_next_regim(0b1000000, 215, 14);
        return;
    }
    if(regim == 14)
    {
        inc_dec_var(&night_light, sizeof(light_swith) - 1);
        if(!night_light) night_light = 1;
        mask_next_regim(0b1000000, 215, 15); //noch L.5
        if(regim == 15)
        { //night_light
            EEPROM_write(EEPROM_light, day_light);
            EEPROM_write(EEPROM_light + 1, night_light);
            //regim=215;
        }
        return;
    }
    if(regim == 15)
    {
        inc_dec_var(&tim_c_light, 23);
        mask_next_regim(0b110000000, 215, 16);
        return;
    }
    if(regim == 16)
    {
        inc_dec_var(&tim_do_light, 23);
        mask_next_regim(0b11000, 215, 215);
        if(regim == 215)
        { //night_light
            EEPROM_write(EEPROM_light + 2, tim_c_light);
            EEPROM_write(EEPROM_light + 3, tim_do_light);
        }
        return;
    }
    //---------------end__light--------------------

    //------------SIGHAL---------------------------
    if(regim == 216)
    { //124
        next_regim();
        mask_next_regim(0xFFFF, 0, 52); //SIGHAL
        return;
    }
    if(regim == 52)
    {
        inc_dec_var(&ocr1[4], 1);
        mask_next_regim(0b111111000, 216, 53); //53budil 54button
        time[note] = 3;
        time_flag |= _BV(fl_signal_cls);
        return;
    }
    if(regim == 53)
    {                                          //H000 L000
        inc_dec_var(&ocr1[ocr1[4] * 2], 0xFF); //ocr1ahi
        mask_next_regim(0b111000000, 55, 54);
        return;
    }
    if(regim == 54)
    {                                              //H000 L000
        inc_dec_var(&ocr1[ocr1[4] * 2 + 1], 0xFF); //ocr1ahi
        mask_next_regim(0b000111000, 55, 53);
        return;
    }
    if(regim == 55)
    {
        EEPROM_write(EEPROM_verifu_but, 0xaa);
        for(uint8_t i = EEPROM_ocr1a; i < 4 + EEPROM_ocr1a; i++)
            EEPROM_write(i, ocr1[i - EEPROM_ocr1a]);
        regim = 52;
        return;
    }
    //-------------end__signal--------------

    //-------------ds18b20------------------
    if(regim == 217)
    {
        next_regim();
        mask_next_regim(0xFFFF, 0, 111);
        return;
    }
    if(regim == 111)
    {
        inc_dec_var(&vid_t, 1); //ocr1ahi
        mask_next_regim(0b100000000, 217, 112);
        if(regim == 112) EEPROM_write(EEPROM_vid_t, vid_t);
        return;
    }
    if(regim == 112)
    {
        mask_next_regim(0, 217, 217);
        if(regim == 217)
        {
            scratchpad_save();
        }
        return;
    }
    //-----------end=ds18b20----------------

    //-----------�����-----------------------
    if(regim == 218)
    {
        if(SET_)
        {
            viewstate = 0;
            for(uint8_t i = EEPROM_calls; i < kolvo_bud * 3 + EEPROM_calls; i++)
            {
                EEPROM_write(i, 0); //
            }
            for(uint8_t i = EEPROM_regim_state_time; i < Nreg_st + EEPROM_regim_state_time; i++)
                EEPROM_write(i, 0);
            for(uint8_t i = EEPROM_ocr1a; i < 4 + EEPROM_ocr1a; i++)
                EEPROM_write(i, 4);
            for(uint8_t i = EEPROM_termostat; i < EEPROM_termostat + sizeof(termostat); i++)
                EEPROM_write(i, 0);

            EEPROM_write(EEPROM_light, 5);
            EEPROM_write(EEPROM_light + 1, 2);
            EEPROM_write(EEPROM_light + 2, tim_c_light = 23);
            EEPROM_write(EEPROM_light + 3, tim_do_light = 6);

            EEPROM_write(EEPROM_sig_pin_or_bip, 0);
            EEPROM_write(EEPROM_otsrochka_num, 7);
            EEPROM_write(EEPROM_vid_t, 0);

            read_data_EEPROM();
        }
        next_regim();
        mask_next_regim(0xFFFF, 0, 0);
        return;
    }
    //_______________________________________________
}

//**************************************
void result_in_bcd(int result)
{ //0000 0000 0000 (1)0000  // (2)0000 (3)0000 (4)0000 (5)0000 hi2 hi3 low4 low5
    unsigned char dig_out = 0;
    bcd_num1 = 0;
    bcd_num2 = 0;
    bcd_num3 = 0;
    bcd_num4 = 0;
    while(result >= 10000)
    {
        result -= 10000;
        dig_out++;
    }
    bcd_num0 = dig_out;
    dig_out = 0;
    while(result >= 1000)
    {
        result -= 1000;
        dig_out++;
    }
    bcd_num1 = dig_out;
    dig_out = 0;
    while(result >= 100)
    {
        result -= 100;
        dig_out++;
    }
    bcd_num2 = dig_out;
    dig_out = 0;
    while(result >= 10)
    {
        result -= 10;
        dig_out++;
    }
    bcd_num3 = dig_out;
    bcd_num4 = result;
}
//**************************************
static void cls_lcd_buffer(void)
{
    unsigned char i;
    for(i = 0; i < 9; i++)
    {
        lcd_buffer[i] = 0;
    }
}

void load_str_in_lcdbuffer(prog_int8_t *str_lcd)
{
    unsigned char b, e, length; //str_lcd[0] = 0000 1111-��������, 1111 0000 - ����� ������
    length = pgm_read_byte_near(&str_lcd[0]);
    for(b = 0x0F & (length), e = 1; b < (0x0F & (length >> 4)); b++, e++)
    {
        lcd_buffer[b] = pgm_read_byte_near(&str_lcd[e]);
    }
}
//**************************************
void num2_bcd_in_lcd_t(unsigned char znak_mesto, unsigned char param_out)
{
    result_in_bcd(param_out);
    lcd_buffer[znak_mesto] = font[bcd_num3];
    lcd_buffer[znak_mesto + 1] = font[bcd_num4] ^ tochka;
}
void num2_bcd_in_lcd(unsigned char znak_mesto, unsigned char param_out)
{
    result_in_bcd(param_out);
    lcd_buffer[znak_mesto] = font[bcd_num3];
    lcd_buffer[znak_mesto + 1] = font[bcd_num4];
}
void num3_th_in_lcd_buffer(unsigned char m)
{
    lcd_buffer[m + 1] = font[bcd_num3];
    lcd_buffer[m + 2] = font[bcd_num4] ^ tochka;
}
//**************************************
void blank_nul(unsigned char mesto)
{
    if(lcd_buffer[mesto] == font[n0])
        lcd_buffer[mesto] = 0;
}

//**************************************
void clock_in_lcd_gl(unsigned char znakomesto)
{
    num2_bcd_in_lcd(znakomesto, hour[h_hour]);
    blank_nul(znakomesto);
#ifdef miganie
    if(time_flag & _BV(fl_sek_1_switch))
#endif
        lcd_buffer[znakomesto + 2] = font[f_seg_g];
    num2_bcd_in_lcd(znakomesto + 3, hour[h_min]);
}

//**************************************
void day_str(void)
{
    lcd_buffer[4] = pgm_read_byte_near(&h_day_str[hour[h_day] * 2]);
    lcd_buffer[5] = pgm_read_byte_near(&h_day_str[hour[h_day] * 2 + 1]);
}
//**************************************
void hh_mm(void)
{
    num2_bcd_in_lcd_t(0, hour[h_hour]);
    blank_nul(0);
    num2_bcd_in_lcd(2, hour[h_min]);
}

static void clock_day_str(void)
{
    hh_mm();
    day_str();
}
//**************************************
void lcd_hh_mm(unsigned char *param)
{
    num2_bcd_in_lcd_t(2, *param);
    num2_bcd_in_lcd(4, *(param - 1));
}
//**************************************
void hh_mm_ss(uint8_t *param)
{
    num2_bcd_in_lcd_t(0, *(param + 2));
    blank_nul(0);
    lcd_hh_mm(param + 1);
}
//**************************************
void num_day(void)
{
    num2_bcd_in_lcd_t(0, hour[h_num] + 1);
    blank_nul(0);
    num2_bcd_in_lcd_t(2, hour[h_month] + 1);
    blank_nul(2);
    day_str();
}
//**************************************
void tempir_in_bcd(unsigned char t, unsigned char mesto, unsigned char pin_wire, unsigned char point)
{
    unsigned char znak = 0, n1 = 0, n2 = 0;

    if(!(pin_wire & out_1wire))
    {
        lcd_buffer[mesto] = font[f_seg_g];
        lcd_buffer[mesto + 1] = font[f_seg_g];
        lcd_buffer[mesto + 2] = font[f_seg_g];
    }
    else
    {
        int te;
        te = temperatura[t];

        if(te < 0)
        {
            te *= -1;
            znak = 1;
        }

        result_in_bcd(te); // 15.7 104.3 -55.2 5.2 -5.3

        if(vid_t == 0)
        {
            if(bcd_num1)
                n1 = font[bcd_num1];
            else if(znak)
                n1 = font[f_seg_g];
            n2 = font[bcd_num2];
        }
        else
        {
            if(bcd_num1)
            {
                n1 = font[bcd_num1];
                n2 = font[bcd_num2];
            }
            else
            {
                if(bcd_num2)
                {
                    n2 = font[bcd_num2];
                    if(znak) n1 = font[f_seg_g];
                }
                else if(znak)
                    n2 = font[f_seg_g];
            }
        }

        lcd_buffer[mesto] = n1;
        lcd_buffer[mesto + 1] = n2;
        lcd_buffer[mesto + 2] = font[bcd_num3];
        lcd_buffer[mesto + 3] = font[bcd_num4];

        if(point)
            lcd_buffer[mesto + 2] ^= tochka;
        else if(bcd_num4 > 5)
            lcd_buffer[mesto + 2] ^= tochka;
    }
}

void tempir_2_in_lcd(unsigned char point)
{
    tempir_in_bcd(ds0, 0, pwire0, point);
    tempir_in_bcd(ds1, 3, pwire1, point);
}

void grad_celsia(void)
{
    lcd_buffer[4] = font[f_gr];
    lcd_buffer[5] = font[f_C];
}
//**************************************/
static void one_tempir_0_in_lsd(void)
{
    tempir_in_bcd(ds0, 1, pwire0, true);
    lcd_buffer[0] = font[f_d] ^ tochka;
    lcd_buffer[5] = font[f_gr];
}
//**************************************/
static void one_tempir_1_in_lsd(void)
{
    tempir_in_bcd(ds1, 1, pwire1, true); //0-00.0 * //inl
    lcd_buffer[0] = font[f_y] ^ tochka;
    lcd_buffer[5] = font[f_gr];
}

//**************************************/
static void one_tempir_0(void)
{
    tempir_in_bcd(ds0, 1, pwire0, false);
    grad_celsia();
    lcd_buffer[0] = font[f_d] ^ tochka;
}
//**************************************/
static void one_tempir_1(void)
{
    tempir_in_bcd(ds1, 1, pwire1, false); //0-00 *c //inl
    grad_celsia();
    lcd_buffer[0] = font[f_y] ^ tochka;
}

static void on__(unsigned char zn, unsigned char tr_fl)
{
    unsigned char a, b;
    if(tr_fl)
    {
        a = font[f_o];
        b = font[f_n] ^ tochka;
    }
    else
    {
        a = font[n0];
        b = font[f_F] ^ tochka;
    }
    lcd_buffer[zn] = a;
    lcd_buffer[zn + 1] = b;
}

//*************************************
void power_in_lcd(unsigned char pin_wire, unsigned char mesto)
{
    unsigned char a, b;
    if(!(pin_wire & out_1wire))
    {
        a = font[f_seg_g];
        b = font[f_seg_g];
    }
    else if(power_suplly_1w & pin_wire)
    {
        a = font[f_P];
        b = font[f_A];
    }
    else
    {
        a = font[f_o];
        b = font[f_n];
    }
    lcd_buffer[mesto] = a;
    lcd_buffer[mesto + 1] = b;
}

//**************************************
static void view_in_indikator(unsigned char view)
{

    if(view == 0)
        clock_in_lcd_gl(0); // hour-min
    else if(view == 1)
        hh_mm_ss(&hour[h_sek]); //hh.min.sek
    else if(view == 2)
        clock_day_str(); //hour.min day
    else if(view == 3)
        num_day(); //num mon day
    else if(view == 4)
        one_tempir_0_in_lsd(); //  -000 * 1������
    else if(view == 5)
        one_tempir_1_in_lsd(); //  -000 * 2������
    else if(view == 6)
        tempir_2_in_lcd(false); // -00 -00//
    else if(view == 7)
        one_tempir_0(); //  -00 *C 1������
    else if(view == 8)
        one_tempir_1(); //  -00 *C 2������
}
//--------------------------------------
void indikator_manager(void)
{
    unsigned char i;

    i = regim_state_time[Nreg_st];
    if(++i >= (Nreg_st / 2)) i = 0; // ���� ���������� ��� ��������� ����� �� �������.

    while(regim_state_time[i * 2 + 1] == 0)
    {
        if(i == regim_state_time[Nreg_st])
        {
            time_view_buf = 1;
            return;
        }
        if(++i >= (Nreg_st / 2)) i = 0;
    }
    regim_state_time[Nreg_st] = i;
    time_view_buf = regim_state_time[i * 2 + 1];
    viewstate = regim_state_time[i * 2];
}
//**************************************/
void data_lcd(void)
{
    cls_lcd_buffer();

    if(regim)
    {
        viewstate = 0;
        time_view_buf = 3;
    }

    if(regim == 0)
    {
        view_in_indikator(viewstate);
        if(time_flag & _BV(fl_sig_otsr))
            if(!(time_flag & _BV(fl_sek_1_switch)))
                lcd_buffer[5] ^= tochka;
        return;
    }

    if(regim == 100)
    {                       // ��� ��� ������� ������ ���� ������ ���
        clock_in_lcd_gl(0); // �����
        if(time_flag & _BV(fl_sig_otsr))
            lcd_buffer[3] = font[f_o]; //if (time_flag & _BV(fl_sig_otsr))
        return;
    }

    if(regim == 7)
    {                                                     //f_C, f_A, f_L, f_L, f_S, 0, //0,5
        load_str_in_lcdbuffer((prog_uint8_t *)str_calls); //load_str_in_lcdbuffer(&str_[0]);
        return;
    }
    if(regim == 9)
    { //S, E, t, U, P, 0, //0,5
        load_str_in_lcdbuffer((prog_uint8_t *)str_setup);
        return;
    } //*/
    if(regim == 8)
    {
        load_str_in_lcdbuffer((prog_uint8_t *)str_t_stat);
        return;
    }
    if(regim == 215)
    { // L, I, G, H, t, 0 //0,5
        load_str_in_lcdbuffer((prog_uint8_t *)str_light);
        return;
    }

    if(regim == reg_termostat)
    {
        tempir_in_bcd(ds0, 1, pwire0, true);
        lcd_buffer[0] = font[f_d] ^ tochka;
        lcd_buffer[5] = font[f_gr];
        return;
    }

    if((regim >= reg_termostat + 1) && (regim <= reg_termostat + 3))
    {
        lcd_buffer[0] = font[f_S];
        lcd_buffer[1] = font[f_E];
        lcd_buffer[2] = font[f_t] ^ tochka;
        result_in_bcd(temper_termostat);
        num3_th_in_lcd_buffer(2);
        if(!bcd_num3)
            lcd_buffer[3] = 0;
        if(PIN_OUT & PIN_T_STAT)
            lcd_buffer[5] = font[n1];
        else
            lcd_buffer[5] = font[n0];
        return;
    }

    //light
    if(regim == 11)
    {
        lcd_buffer[0] = font[f_d];
        lcd_buffer[1] = font[f_seg_g];
        lcd_buffer[2] = font[day_light];
        lcd_light = day_light;
        return;
    }
    if(regim == 14)
    {
        lcd_buffer[0] = font[f_H];
        lcd_buffer[1] = font[f_seg_g];
        lcd_buffer[2] = font[night_light];
        lcd_light = night_light;
        return;
    }
    if((regim > 14) && (regim < 17))
    { //H, -
        num2_bcd_in_lcd(0, tim_c_light);
        lcd_buffer[2] = font[f_o];
        lcd_buffer[3] = font[f_o];
        num2_bcd_in_lcd(4, tim_do_light);
        return;
    } // end light

    //----------����------------------
    if(regim == 210)
    {
        load_str_in_lcdbuffer((prog_uint8_t *)str_clock);
        return;
    }
    if((regim > 20) && (regim < 23))
    {
        clock_in_lcd_gl(0);
        return;
    }
    if((regim > 22) && (regim < 25))
    {
        num2_bcd_in_lcd(0, hour[h_num] + 1);
        lcd_buffer[1] ^= tochka;
        result_in_bcd(hour[h_day]);
        lcd_buffer[2] = font[f_d];
        lcd_buffer[3] = font[bcd_num4 + 1];
        return;
    }
    if(regim == 25)
    { //(regim==25) {//n, n, o, n, t, h, 0, 0, 0,
        load_str_in_lcdbuffer((prog_uint8_t *)str_monht);
        num2_bcd_in_lcd(4, hour[h_month] + 1);
        return;
    }

    if(regim == 26)
    {
        hh_mm_ss(&hour[h_sek]); //hh_min-sek �������� �����
        return;
    }
    if((regim > 26) && (regim < 29))
    { //C., //0x39,
        load_str_in_lcdbuffer((prog_uint8_t *)str_corr);
        if(corr_flag & _BV(fl_corr_znak_pl))
            lcd_buffer[1] = font[f_seg_d];
        else
            lcd_buffer[1] = font[f_seg_g];
        lcd_buffer[2] = font[n0] ^ tochka;
        result_in_bcd(pgm_read_word_near(&corr_in_lcd[correction]));
        lcd_buffer[3] = font[bcd_num2];
        lcd_buffer[4] = font[bcd_num3];
        lcd_buffer[5] = font[bcd_num4];
        return;
    }

    if((regim > 30) && (regim <= 32))
    {
        lcd_buffer[0] = font[budilnik_nomer];
        if(budilnik_nomer >= 8) lcd_buffer[0] ^= tochka;
        on__(2, ((calls[budilnik_nomer][2]) & (_BV(7))));
        return;
    }

    if((regim > 32) && (regim < 35))
    {
        lcd_hh_mm(&calls[budilnik_nomer][c_hour]);
        return;
    }

    if((regim > 44) && (regim < 48))
    {
        hh_mm_ss(&calls_pin[budilnik_nomer - kolvo_osn][h_sek]);
        return;
    }

    if((regim >= 35) && (regim <= 42))
    {
        lcd_buffer[0] = font[f_d] ^ tochka;
        lcd_buffer[1] = font[p_index + 1];
        if((calls[budilnik_nomer][2]) & _BV(p_index))
            lcd_buffer[3] = font[f_o];
        else
            lcd_buffer[3] = font[f_seg_d];
        return;
    }

    //--SIGNAL--
    if(regim == 216)
    { // S, I, G, H, A, L //0,6
        load_str_in_lcdbuffer((prog_uint8_t *)str_signal);
        return;
    }
    if(regim == 52)
    { // b, u, d, I, I. //0,5
        if(ocr1[4] == 0)
            load_str_in_lcdbuffer((prog_uint8_t *)str_budil);
        else
            load_str_in_lcdbuffer((prog_uint8_t *)str_button);
        return;
    }
    if((regim >= 53) && (regim <= 54))
    {
        if(ocr1[4] == 0)
        {
            signal = 10;
            time_signal = 10;
            time_flag |= _BV(fl_signal_out_enable);
        }
        result_in_bcd(ocr1[ocr1[4] * 2]);
        num3_th_in_lcd_buffer(0);
        result_in_bcd(ocr1[ocr1[4] * 2 + 1]);
        num3_th_in_lcd_buffer(3);
        return;
    }
    //--END--SIGNAL--

    if(regim == 218)
    { //R, E, S, E, t //0,9
        load_str_in_lcdbuffer((prog_uint8_t *)str_default);
        return;
    }
    if(regim == 211)
    { //P, o, r, t, 0, 0 //0,4
        load_str_in_lcdbuffer((prog_uint8_t *)str_port);
        return;
    }
    if(regim == 71)
    { //_pin-0_ _c	//0, P, i, n, -, 0000 1,4
        load_str_in_lcdbuffer((prog_uint8_t *)str_pin_c);
        if(PIN_OUT & PIN_calls1)
            lcd_buffer[5] = font[n1];
        else
            lcd_buffer[5] = font[n0];
        return;
    }
    //===============================
    //-----------------t-stat-----------------------
    if(regim == 212)
    {
        load_str_in_lcdbuffer((prog_uint8_t *)str_t_stat);
        return;
    }
    if(regim == 80)
    {
        if(p_index < kolvo_zon)
        {
            lcd_buffer[0] = font[p_index + 1] ^ tochka;
            load_str_in_lcdbuffer((prog_uint8_t *)str_zona);
        }
        else
        {
            lcd_buffer[0] = font[p_index + 1] ^ tochka;
            load_str_in_lcdbuffer((prog_uint8_t *)str_gisterezis);
        }
        return;
    }
    if((regim >= 81) && (regim <= 83))
    {
        if(p_index < kolvo_zon)
        {
            num2_bcd_in_lcd_t(0, termostat.time[p_index]);
            //lcd_buffer[3]=font[f_seg_g];
            uint8_t j;
            if(p_index < kolvo_zon - 1)
                j = p_index + 1;
            else
                j = 0;
            num2_bcd_in_lcd(2, termostat.time[j]);
            num2_bcd_in_lcd(4, termostat.temperature[p_index]);
        }
        return;
    }

    if(regim == 84)
    {
        load_str_in_lcdbuffer((prog_uint8_t *)str_gisterezis_1);
        num2_bcd_in_lcd(4, termostat.gisterezis);
        lcd_buffer[4] ^= tochka;
    }

    //----------------------------------------------
    if(regim == 214)
    { //  _yct_bud_ //0, y, C, t, 0, 6, u, d., 0 //1,7
        load_str_in_lcdbuffer((prog_uint8_t *)str_yst_bud);
        return;
    }
    if(regim == 75)
    { //CAL. bIP C //C, A, L., //0x39, 0xB7, 0x59,
        load_str_in_lcdbuffer((prog_uint8_t *)str_cal);
        uint8_t i, a, b;
        if(signal_bud == 0)
        {
            a = font[f_b];
            b = font[n1];
            i = font[f_P];
        }
        else if(signal_bud == 1)
        {
            a = font[f_P];
            b = font[f_seg_e];
            i = font[f_n];
        }
        else
        {
            a = font[f_O];
            b = font[f_b];
            i = font[f_A];
        }
        lcd_buffer[3] = a;
        lcd_buffer[4] = b;
        lcd_buffer[5] = i;
        return;
    }
    if(regim == 76)
    { //O, t, S.//0,3
        load_str_in_lcdbuffer((prog_uint8_t *)str_otsro4);
        num2_bcd_in_lcd(4, otsroshka_num);
        return;
    }

    if(regim == 213)
    { //� see	//{}//0, Y, 0, S, E, E, 0 //1,6
        load_str_in_lcdbuffer((prog_uint8_t *)str_yst_see);
        return;
    }
    if((regim > 90) && (regim < 94))
    {
        lcd_buffer[0] = font[regim_state_time[Nreg_st] + 1];
        lcd_buffer[1] = font[f_r] ^ tochka;
        lcd_buffer[2] = font[regim_state_time[regim_state_time[Nreg_st] * 2]];
        lcd_buffer[3] = font[f_seg_g];
        result_in_bcd(regim_state_time[regim_state_time[Nreg_st] * 2 + 1]);
        lcd_buffer[4] = font[bcd_num3];
        lcd_buffer[5] = font[bcd_num4];
        return;
    }

    if(regim == 217)
    { //1, 8, b, 2, 0, // 0,9
        load_str_in_lcdbuffer((prog_uint8_t *)str_ds18b20);
        return;
    }
    if(regim == 111)
    {
        if(vid_t == 0)
            load_str_in_lcdbuffer((prog_uint8_t *)str_vid1_t);
        else
            load_str_in_lcdbuffer((prog_uint8_t *)str_vid2_t);

        return;
    }
    if(regim == 112)
    {
        lcd_buffer[0] = font[f_d] ^ tochka;
        power_in_lcd(pwire0, 1);
        lcd_buffer[3] = font[f_y] ^ tochka;
        power_in_lcd(pwire1, 4);
        return;
    }

    if(regim == 150)
    { //������� ������ str_bud
        lcd_buffer[0] = font[budilnik_nomer];
        lcd_hh_mm(&hour[h_hour]);
    }
}

//**************************************
void init_signal(void)
{
    //	str_init();
    count_note = 0;
    time[note] = 3;
    signal = 1;
    time_in_regim = time_signal = 59;
    time_flag &= ~_BV(fl_signal_cls);       // ���� ���������� ��������
    time_flag |= _BV(fl_signal_out_enable); // ���� ������ �������
}

void shim(void)
{
    unsigned char tmp;
    if(time[note] <= 0)
    {
        tmp = uprav_TCCR1B[count_note];
        if(tmp & 0x01)
            timer1_init_in_pwm();
        else
            timer1_init();
        time[note] = length_note[count_note];
        TCCR1B = tmp;
        if(++count_note == 4) count_note = 0;
    }
}

static void budilnik_out(void)
{
    if(!signal_bud)
    {
        shim();
    }
    else if(signal_bud == 1)
    {
        PORT_OUT |= PIN_calls_osn;
    }
    else
    {
        shim();
        PORT_OUT |= PIN_calls_osn;
    }
}

static void signal_off(void)
{
    // ������� �� �������---------------------------
    if(time_signal == 0)
    {
        if(signal == 0) // ���� ��� �������
        {
            if(time_flag & _BV(fl_sig_otsr)) // ���� ���� ��������
            {
                if(budilnik_play_num++ == otsroshka_num)
                { // ���� ����. ��������� X ���, //���������
                    time_flag &= ~_BV(fl_sig_otsr);
                }
                else
                { // ���� ��� �������, ���� ���� ��������, �� ��� ������
                    init_signal();
                    regim = 150;
                    signal = 1; //sig_on_otsr;
                    return;     // ����� �� �������
                }
            }
        }
        else
            time_flag |= _BV(fl_signal_cls); // ����� ������� �� �������
    }
    // ���������� �������
    if(!(time_flag & _BV(fl_sig_butt)))
        if(time_flag & _BV(fl_signal_cls)) // ���������� �������
        {
            time_flag &= ~(_BV(fl_signal_cls) | _BV(fl_signal_out_enable));

            PORT_OUT &= ~PIN_calls_osn;
            //PORT_OUT &=~  PIN_timer;
            timer1_init();

            time_signal = 0xFF;

            signal = 0;
        }
}
//*************************************/

//**************************************
static void calls_verify(void)
{
    unsigned char i;
    for(i = 0; i < kolvo_bud; i++)
        if(_BV(7) & calls[i][c_day])                   //�������� ��� ���
            if(_BV(hour[h_day]) & calls[i][c_day])     //���� ����
                if(calls[i][c_hour] == hour[h_hour])   //���
                    if(calls[i][c_min] == hour[h_min]) //���
                    {
                        if(i < kolvo_osn)
                        {
                            budilnik_nomer = i;
                            //signal=1;
                            if(otsroshka_num) time_flag |= _BV(fl_sig_otsr);
                            budilnik_play_num = 0; //��� �� ���� ���������� ����� �������� ��� ����������� ������ ��� ����������� ��������
                            init_signal();
                            regim = 150;
                        }
                        else
                        {
                            time_bud_pin = (uint32_t)calls_pin[i - kolvo_osn][h_hour] * 3600;
                            time_bud_pin += calls_pin[i - kolvo_osn][h_min] * 60;
                            time_bud_pin += calls_pin[i - kolvo_osn][h_sek];
                            if(time_bud_pin)
                            {
                                fl_calls_pin |= _BV(fl_calls_pin_A_on);
                                PORT_OUT |= PIN_calls1;
                            }
                        }
                    }
}
//**************************************
static void calls_pin_verify(void)
{

    if(fl_calls_pin & _BV(fl_calls_pin_A_on))
    {
        if(--time_bud_pin == 0x00)
        {
            PORT_OUT &= ~PIN_calls1;
            fl_calls_pin &= ~_BV(fl_calls_pin_A_on);
        }
    }
}

//**************************************/

void set_temper_termostat(uint8_t j)
{
    if(hour[h_hour] == termostat.time[j]) temper_termostat = termostat.temperature[j];
}

//**************************************
static void clock(void)
{

    if(isr_1sek_) //������ �������
    {
        correct = 0;
        PORT_sig_min &= ~sig_min_00;
        calls_pin_verify(); // ��������� ��� ��������

        hour[h_sek]++;
        if(hour[h_sek] == 60)
        {
            hour[h_sek] = 0;
            hour[h_min]++;
            if(hour[h_min] == 60)
            {
                if(corr_flag & _BV(fl_corr_znak_pl)) //�������� �����
                    correct = correction;            //��� ����������
                else
                    correct = 0x00 - correction;

                hour[h_min] = 0;
                hour[h_hour]++;

                if(hour[h_hour] == 24)
                {
                    hour[h_hour] = 0;
                    hour[h_day]++;
                    hour[h_num]++;
                    if(hour[h_day] >= 7) hour[h_day] = 0;
                    if(hour[h_num] >= num_in_month[hour[h_month]])
                    {
                        hour[h_num] = 0;
                        hour[h_month]++;
                        if(hour[h_month] >= 12)
                        {
                            hour[h_month] = 0;
                        }
                    }
                }

                if(hour[h_hour] == tim_c_light) lcd_light = night_light;
                if(hour[h_hour] == tim_do_light) lcd_light = day_light;

                for(uint8_t i = 0; i < kolvo_zon; i++)
                    set_temper_termostat(i);
                if(!pin_power_)
                    if(lcd_light == day_light)
                        PORT_sig_min |= sig_min_00;
                //������� ������/������ �����
                //�� ������ ����� �������������� � ���� �� ��������� ����������� ����� � 2:00 ��������� ������� ������� �� 1 ��� ������
                //�������� ������� �������������� � ���� �� ��������� ����������� ������� � 3:00 ��������� ������� �� 1 ��� �����.
                if(hour[h_month] == 3 - 1)
                {                                //�� ������ �����
                    if(hour[h_num] > 24 - 1)     //���� ����� 25 � �����
                        if(hour[h_day] == 7 - 1) //���� �����������
                            if(hour[h_hour] == 2) hour[h_hour] = 3;
                }
                if(hour[h_month] == 10 - 1)
                {                                //�� ������ �����
                    if(hour[h_num] > 24 - 1)     //���� ����� 25 � �����
                        if(hour[h_day] == 7 - 1) //���� �����������
                            if(hour[h_hour] == 3)
                            {
                                static uint8_t zimnie = 0;
                                if(zimnie == 0)
                                {
                                    zimnie = 1;
                                    hour[h_hour] = 2;
                                }
                                else
                                    zimnie = 0;
                            }
                }
            }
            calls_verify();
        }
        isr_1sek_ = 0;
    }
}
//****************************************
void time_count(void)
{
    static unsigned char t, sek;
    unsigned char i;
    if(t != clock_)
    { //  0,0078125 ���.
        t = clock_;
        if(t >= 64)
            time_flag |= _BV(fl_sek_1_switch);
        else
            time_flag &= ~_BV(fl_sek_1_switch);
        for(i = 0; i < 4; i++)
            time[i]--;

        if(time_flag & _BV(fl_sig_butt)) // ����������� ������� �� ������
            if(time[note_but] < 0)
            {
                time_flag &= ~_BV(fl_sig_butt);
                timer1_init();
            }

        if(sek != hour[h_sek])
        { //������ �������
            sek = hour[h_sek];

            if(--time_view_buf == 0) indikator_manager();
            if(--time_in_regim == 0)
                if(regim >= 4) regim = 0;
            time_signal--;
        }
        // ����� �������
        signal_off();
        if(time_flag & _BV(fl_signal_out_enable)) budilnik_out(); // ����� ����������� ��� ������ ��� ��� ���
        if(time_flag & _BV(fl_power_init_device))                 //��� ������� �� �������
            skankey();

        if(regim >= 4) //if (mask)
        {
            static uint8_t time_mig = 0;
            if(++time_mig >= light_time_mig[lcd_light]) //(10-lcd_light)
            {
                time_mig = 0;
                change_light_var();
            }
        }
    }
} //*/
//*/**************************************
static void init_devices_save_mode(void)
{
    TIMSK = _BV(TOIE2); //|(1<<OCIE1A)
    PORTLCD = 0;
    PORTSERIAL |= _BV(STORAGE); //��� ������� ����
#ifdef katod
    data_shift(0xFF);
#else
    data_shift(0);
#endif
    PORT_OUT &= ~PIN_T_STAT; // ����� ���������� � ����
} //*/
//**************************************
int main(void)
{

    cli();
    read_data_EEPROM();
    timer2_init();
    init_devices();

    while(1)
    { //while
        clock();
        if(!pin_power_)
        { //begin power on
            if(time_flag & _BV(fl_power_init_device))
            {
                time_flag &= ~(_BV(fl_power_init_device) | _BV(fl_sig_otsr));
                //time_flag &= ~_BV(fl_sig_otsr);// ��� �� ��� ��������� ���. �� ��������� ����������� �������
                init_devices();
                time_view_buf = 2;
                time_in_regim = 2;
            }
            /*������� ����*/
            if(lcd_time_switch >= 30) /*������ 2msek*/
            {
                lcd_time_switch = 0x00;
                znakomesto++;
                //DDRB|=1;
                //PORTB|=1;
                if(znakomesto < 6)
                {
                    skankey();                                      // ���� �� ��� ����������:)
                    key_action();                                   // ������������ ����� ����������
                    light = light_swith[opred_light_current_num()]; //���������� ������� ���������� �����
                    display();                                      // �������� �����
                                                                    //if  (znakomesto==8) PORTB&=~1;
                }
                else
                {
                    PORTSERIAL |= _BV(STORAGE); //��� ������� ����
#ifdef katod
                    PORTLCD = 0x00;
#else
                    PORTLCD = 0xFF;
#endif

                    lcd_time_switch = 25;
#ifdef katod
                    data_shift(0xFF);
#else
                    data_shift(0);
#endif
                    if(regim)
                    {
                        if(hour[h_hour] >= tim_do_light)
                            lcd_light = day_light;
                        else
                            lcd_light = night_light;
                        if(hour[h_hour] >= tim_c_light) lcd_light = night_light;
                    }
                    data_lcd();
                    znakomesto = 0xFF;
                    //izmerenie_t();	/*��������� �����������*/
                }
            }

            time_count();
            if(lcd_time_switch < 5) izmerenie_t(); /*��������� �����������*/

        } //end power on
        else
        {
            if(!(time_flag & _BV(fl_power_init_device)))
            {
                time_flag |= _BV(fl_power_init_device);
                init_devices_save_mode();
            }

            if(signal || (time_flag & _BV(fl_sig_butt))) // ������� ��������� ������� ��� ��� ������� � ������� �� �������
            {
                time_count();
                if(key != 17)
                {
                    time_flag |= _BV(fl_signal_cls);
                    time_flag &= ~_BV(fl_signal_out_enable);
                }
            }
            else
            { //  �� ����� ���� ���� ������ �� ������
                MCUCR = BIT(SM1) | BIT(SM0) | BIT(SE);
                asm("sleep");
                MCUCR = 0;
            }
        }
    } //while
}

//**************************************
ISR(SIG_OVERFLOW0)
{ //0,000064
    lcd_time_switch++;
    if(lcd_time_switch == light)
#ifdef katod
        PORTLCD = lcd_buffer[znakomesto];
#else
        PORTLCD = ~lcd_buffer[znakomesto];
#endif
}
//**************************************
ISR(SIG_OVERFLOW2)
{
    clock_++;
    if(clock_ == 128)
    {
        clock_ = correct;
        isr_1sek_ = 1;
    }
}
