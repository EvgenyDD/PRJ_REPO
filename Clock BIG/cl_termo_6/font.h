

#define str_length	27
//------------------------------------------------------------------------------------------
//								--��� ���--
//#ifdef katod
//unsigned char str_noch[str_length]=	{0x00,0xBD,0x8F,0xDE,0x04,0,0,0,0,0x04,0,0,0,0x9E,0xBB,0xBD,0x37,0xBB,0x9B,0x00,0x97,0xBB,0x87,0x9B,0x00,0x42,0x00}; 	
//unsigned char str_utro[str_length]=	{0x0,0xBD,0x8F,0xDE,0x4,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x9E,0xBB,0xBD,0x37,0xBB,0x3D,0x0,0x8F,0x1D,0x37,0xBB,0x0,0x42,0x0};
//unsigned char str_day[str_length]=	{0x0,0xBD,0x8F,0xDE,0x4,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x9E,0xBB,0xBD,0x37,0x9D,0x11,0x9B,0x0,0x9E,0x3D,0x97,0x9D,0x42,0x00};
//unsigned char str_vecher[str_length]=	{0x0,0xBD,0x8F,0xDE,0x4,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x9E,0xBB,0xBD,0x37,0x9D,0x11,0x9B,0x0,0xBF,0x3D,0x87,0x3D,0x37,0x42};
//---------------------------------------------------------------------------------
prog_uint8_t h_day_str[14]={0xB3, 0x97, 0x9D, 0x1D, 0x39, 0x37, 0x87, 0x31, 0xB3, 0x1D, 0x39, 0xBD, 0x9D, 0x39};//��� �� ��..

//������ ���� 0x81 8-[����� ������(7)+��������(1)]  1-[��������(1)]
prog_uint8_t  str_default[]=		{0x50, 0x31, 0x3D, 0xAD, 0x3D, 0x1D};//R,E,S,E,T //0,9 regim127
prog_uint8_t  str_ds18b20[]=		{0x50,  0x82, 0xBF, 0x9D, 0x3E, 0xBB};//1, 8, b, 2, 0 // 0,9 regim126
prog_uint8_t  str_budil[]=			{0x50 ,0x9D, 0x98, 0x9E, 0x11, 0x58};//, 0x0, 0x0, 0xAD};// b, u, d, I, I. //0,5 regim52
prog_uint8_t  str_button[]=		{0x60, 0x9D, 0x98, 0x1D, 0x1D, 0x9C, 0x94};//, 0x0, 0xAD};// b, u, t, t, o, n //0,6 regim53
prog_uint8_t  str_yst_bud[]=		{0x50, 0xCF, 0x0, 0xBD, 0x8F, 0xDE};//y, 0, 6, u, d.//1,7 regim123
prog_uint8_t  str_yst_see[]=		{0x50, 0xCF,  0x0, 0xAD, 0x3D, 0x3D};//Y, 0, S, E, E, 0 //1,6 regim122
//prog_uint8_t  str_timmer[]=		{0x82, 0x1D, 0x11, 0x33, 0xA3, 0x3D, 0x31};//0, t, i, m1, m2, E, R, 0, 0 // 1,6 //regim8
prog_uint8_t  str_monht[]=			{0x40, 0x94, 0x94, 0x9C, 0xD4};//n, n, o, n //0,6 regim25 
prog_uint8_t  str_signal[]=		{0x60, 0xAD, 0x11, 0xB9, 0x97, 0xB7, 0x19};//S, I, G, H, A, L //0,6 regim125
prog_uint8_t  str_otsro4[]=		{0x30, 0xBB, 0x1D, 0xED};//O, t, S, R, O, 4., 0, 0, 0 //0,6 regim76
prog_uint8_t  str_calls[]=			{0x50, 0x39, 0xB7, 0x19, 0x19, 0xAD}; //f_C, f_A, f_L, f_L, f_S, 0, //0,5 regim7
prog_uint8_t  str_setup[]=			{0x50, 0xAD, 0x3D, 0x1D, 0x9B, 0x37}; //S, E, t, U, P, 0,//0,5  //regim 9
prog_uint8_t  str_light[]=			{0x50, 0x19, 0x11, 0xB9, 0x97, 0x1D};//L, I, G, H, t, 0,//0,5 reim124
prog_uint8_t  str_clock[]=			{0x50, 0x87, 0xB7, 0x39, 0x9D, 0x11};//4, A, C, b, I// 2,5 regim 121
prog_uint8_t  str_port[]=			{0x40, 0x37, 0x9C, 0x14, 0x1D};//P, o, r, t //2,4 regim120
prog_uint8_t  str_pin_c[]=			{0x51, 0x37, 0x10, 0x94, 0x4};//0, P, i, n, -, 0000 1,4 regim71
prog_uint8_t  str_corr[]=			{0x10, 0x39};//C
prog_uint8_t  str_cal[]=			{0x30, 0x39, 0xB7, 0x59};//CAL. 0x9B, 0x71
prog_uint8_t  str_zona[]=			{0x62, 0xAE, 0xBB, 0x97, 0xB7};//0,0,Z,O,N,A
prog_uint8_t  str_vid1_t[]=		{0x60, 0x82, 0x0, 0x4, 0xBB, 0xE2, 0xAD};
prog_uint8_t  str_vid2_t[]=		{0x60, 0x3E, 0x0, 0x0, 0x4, 0xE2, 0xAE};
prog_uint8_t  str_t_stat[]=		{0x60, 0x1D, 0x4, 0xAD, 0x1D, 0xB7, 0x1D};//t, -, s, t, a, t, 
prog_uint8_t  str_gisterezis[]=		{0x62, 0xB9, 0x11, 0xAD, 0x5D};//0x31, 0x98, 0x1C, 0x5D};
prog_uint8_t  str_gisterezis_1[]=		{0x40, 0xB9, 0x11, 0xAD, 0x5D};

const unsigned char font[]={	0xBB, 0x82, 0x3E, 0xAE, 0x87, 0xAD, 0xBD, 0xA2, 0xBF, 0xAF, 
								0xB7, 0x9D, 0x39, 0x9E, 0x3D, 0x35, 0xB9, 0x97, 0x95, 0x1C,
								0xAD, 0x19, 0x37, 0x11, 0x9B, 0x8F, 0x1D, 0xB3, 0x94, 
								0x33, 0xA3, 0x20, 0x2, 0x80, 0x8, 0x10, 0x1, 0x4, 0x98, 
								0x9C, 0x14, 0x31, 0x27}; 

enum fontN 					{n0, n1, n2, n3, n4, n5, n6, n7, n8, n9,
	 						 f_A, f_b, f_C, f_d, f_E, f_F, f_G, f_H, f_h, f_c,
	 						 f_S, f_L, f_P, f_i, f_U, f_y, f_t, f_N, f_n,
							 f_m1, f_m2, f_seg_a, f_seg_b, f_seg_c, f_seg_d, f_seg_e, f_seg_f, f_seg_g, f_u,
							 f_o, f_r, f_R, f_gr};//aef 04 08 80 0100 1000 1000 // abcdef 04 40 01 10 08 80

#define tochka 0x40
#define f_O n0
