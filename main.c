#include <REGX52.H>
#define FREQ_OSC 12000000ul
#include "LunarCalendar.h"
#include "port.h"  // khai bao cac chan

char hour, min, sec, day, date, month, year, a_min, a_hour; 
unsigned char d = 0,ds = 0, kt = 0, alr = 0, a = 0, b = 0, m1 = 0, m2 = 0, m3 = 0, m4 = 0, set0 = 0, set1 = 0, alarm0 = 0, alarm1 = 0;
unsigned char dem = 0, chon = 0, tsp = 0, toff = 0, ton = 0;
unsigned char dateal, monthal, SolarYear;
unsigned int dem1 = 0;
bit pwm = 0, read_time = 0, nhay_led = 0, tam = 0, tam1 = 0, htgiay = 0, set_alarm = 0, alarm_on = 0;
unsigned char led7[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; //led 7 doan
// P3 la port data led doan

//**************************************************************
//======================   delay  ==============================
//**************************************************************
void delay(unsigned int time) // 8us
{ 
	while(time--); 
}

//**************************************************************
//======================  speaker  =============================
//**************************************************************
void speaker(unsigned char s)
{
	if (s == 2)
	{
		if (tsp == 1 || tsp == 3)
			sp = 0;
		if (tsp == 2 || tsp >= 4)
			sp = 1;
	}
	else if(s == 5)
	{
		if (tsp == 1 || tsp == 3 || tsp == 5 || tsp == 7 || tsp == 9)
			sp = 0;
		if (tsp == 2 || tsp == 4 || tsp == 6 || tsp == 8 || tsp >= 10)
			sp = 1;   
		if (tsp > 20)
			tsp = 0;
	}
}
//**************************************************************
//======================    I2C   ==============================
//**************************************************************
void SCL_high()
{
	SCL = 1;
	delay(2);
}
//--------------------------------------------------------------
void SCL_low()
{
	SCL = 0;
	delay(2);
}
//--------------------------------------------------------------
void I2C_Start()
{
	SDA = 1;
	SCL = 1;
	SDA = 0;
	delay(2);
	SCL = 0;
	SDA = 1;
}
//--------------------------------------------------------------
void I2C_Stop()
{
	SDA = 0;
	SCL_high();
	SDA = 1;
}
//--------------------------------------------------------------
bit I2C_Write(unsigned char dat)
{
	unsigned char i;
	bit outbit;
	for (i = 1; i <= 8; i++)
	{
		outbit=dat&0x80;
		SDA = outbit;
		dat = dat << 1;
		SCL_high();
		SCL_low();
	}
		SDA = 1; 
		SCL_high();
		outbit = SDA; 
		SCL_low();
		return(outbit); 
}
//--------------------------------------------------------------
unsigned char I2C_Read(bit ack)
{
	unsigned char i, dat;
	bit inbit;
	dat = 0;
	for(i=1;i<=8;i++) 
	{
		SCL_high();
		inbit = SDA;
		dat = dat << 1;
		dat = dat | inbit;
		SCL_low();
	}
	if (ack) SDA = 0; 
	else SDA = 1;
	SCL_high();
	SCL = 0;
	SDA = 1; 
	delay(2);
	return(dat);
}

//**************************************************************
//======================  DS1307  ==============================
//**************************************************************
void rtc_write(unsigned char add, unsigned char dat)
{
	I2C_Start();
	I2C_Write(0xd0);
	I2C_Write(add); 
	I2C_Write(((dat/10)<<4)|(dat%10)); // chuyen DEC (he 10) sang bin (he 2)
	I2C_Stop();
}
//--------------------------------------------------------------
unsigned char rtc_read(unsigned char add)
{
	unsigned char dat;
	I2C_Start();
	I2C_Write(0xd0); 
	I2C_Write(add);
	I2C_Start(); 
	I2C_Write(0xd1); 
	dat = I2C_Read(0);
	I2C_Stop();
	dat = (dat & 0x0f) + (dat>>4)*10; // chuyen bin (he 2) sang DEC (he 10)
	return (dat);
}
//--------------------------------------------------------------
void writeds()
{
	if(a == 1)
		rtc_write(0x00, 0x00); // Dia chi thanh ghi giay  la 0x00
	
	
	rtc_write(0x01, min);   // Dia chi thanh ghi phut  la 0x01
	rtc_write(0x02, hour);  // Dia chi thanh ghi gio   la 0x02
	rtc_write(0x03, day);   // Dia chi thanh ghi thu   la 0x03
	rtc_write(0x04, date);  // Dia chi thanh ghi ngay  la 0x04
	rtc_write(0x05, month); // Dia chi thanh ghi thang la 0x05
	rtc_write(0x06, year);  // Dia chi thanh ghi nam   la 0x06
}
//--------------------------------------------------------------
void readds()
{
		sec 	 = rtc_read(0x00); // Dia chi thanh ghi giay  la 0x00
		min 	 = rtc_read(0x01); // Dia chi thanh ghi phut  la 0x01
		hour   = rtc_read(0x02); // Dia chi thanh ghi gio   la 0x02
		day    = rtc_read(0x03); // Dia chi thanh ghi thu   la 0x03
		date   = rtc_read(0x04); // Dia chi thanh ghi ngay  la 0x04
		month  = rtc_read(0x05); // Dia chi thanh ghi thang la 0x05
		year   = rtc_read(0x06); // Dia chi thanh ghi nam   la 0x06
}
//--------------------------------------------------------------
void tinhchinh_ds3231()
{ 
	if((hour==1)&&(min==0)&&(sec==0)&&(day == 6))// 1 tuan bu them 4s
	{ 
		sec = 4;//so giay can dieu chinh trong ngày
		rtc_write(0x00,sec);//xong ghi vào ds
	}
	
	if((hour==2)&&(min==0)&&(sec==0)&&(date == 1))// 1 thang bu them 4s
	{ 
		sec = 4;//so giay can dieu chinh trong ngày
		rtc_write(0x00,sec);//xong ghi vào ds
	}
	
	if((hour==3)&&(min==0)&&(sec==0)&&(date == 1) && (month == 1))// 1 nam bu them 3s
	{ 
		sec = 3;//so giay can dieu chinh trong ngày
		rtc_write(0x00,sec);//xong ghi vào ds
	}
}
//--------------------------------------------------------------
void write_alarm()
{
	rtc_write(0x08,a_min);  // Dia chi alarm minute la 0x08
	rtc_write(0x09,a_hour); // Dia chi alarm hour la 0x09
}

//--------------------------------------------------------------
void read_alarm()
{
	a_min  = rtc_read(0x08); // Dia chi alarm minute la 0x08
	a_hour = rtc_read(0x09); // Dia chi alarm hour la 0x09
}

//**************************************************************
//=================  Cai dat thoi gian  ========================
//**************************************************************
//--------------------------------------------------------------
void ktup()	
{
	if(up == 0)
	{
		delay(30000 - m1);

		m1 = 30000; // nhan giu thoi gian delay se giam
		
		if(kt == 1)	//chinh thu
		{
			day++;
			if(day > 7)
				day = 1;
		}   		
		else if(kt == 2)	//chinh ngay
		{
			date++;
			if(date > 31)  
				date = 1;			
		}		
		else if(kt == 3)		//chinh thang
		{
			month++;
			if(month > 12) 
				month = 1;
		}
		else if(kt == 4)		//chinh nam
		{
			year++;
			if(year > 99)
				year = 0;
		}
  	else if (kt == 5)		//chinh gio
		{
			hour++;
			if(hour > 23)
				hour = 0;
		}
		else if(kt == 6)		//chinh phut
		{								   
			min++;
			if(min > 59)
				min = 0;
		}
//---------------------------------------------------		
		if (alr == 1)		//chinh gio alarm
		{
			a_hour++;
			if(a_hour > 23)
				a_hour = 0;
		}
		else if(alr == 2)		//chinh phut alarm
		{								   
			a_min++;
			if(a_min > 59)
				a_min = 0;
		}
		else if(alr == 3)		//chinh on-off alarm							   
			set_alarm = 1;
	
	else 
		m1 = 0;
	}
}

//--------------------------------------------------------------
void ktdown()	
{
	if(down == 0)
	{
		delay(30000 - m2);

		m2 = 30000; // nhan giu thoi gian delay se giam
		
		if(kt == 1)	//chinh thu
		{
			day--;
			if(day <= 0) 
				day = 7;
		}    		
		else if(kt == 2)	//chinh ngay
		{
			date--;
			if(date <= 0) 
				date = 31;		
		}	
		else if(kt == 3)		//chinh thang
		{
			month--;
			if(month <= 0)
				month = 12;
		}	
		else if(kt == 4)		//chinh nam
		{
			year--;
			if(year <= 0)
				year = 99;
		}
  	else if (kt == 5)		//chinh gio
		{
			hour--;
			if(hour < 0)
				hour = 23;
		}
		else if(kt == 6)		//chinh phut
		{								   
			min--;
			if(min < 0)
				min = 59;
		}
//--------------------------------------------------	
		if (alr == 1)		//chinh gio alarm
		{
			a_hour--;
			if(a_hour < 0)
				a_hour = 23;
		}
		else if(alr == 2)		//chinh phut alarm
		{								   
			a_min--;
			if(a_min < 0)
				a_min = 59;
		}
		else if(alr == 3)		//chinh on-off alarm							   
			set_alarm = 0;
	}
	
	else 
		m2 = 0;
}
	

//--------------------------------------------------------------
void ktset()
{
	set0 = set1;
	set1 = set;
	
	if((set0 == 1) && (set1 == 0) && (b == 0))
	{
		delay(35000);
		a = 1;
		kt++;
		if(kt > 6)
			kt = 0;
	}
	if(kt != 0)
	{
		ktup();
		ktdown();
	}
}

//--------------------------------------------------------------
void ktalarm()
{
	alarm0 = alarm1;
	alarm1 = alarm;
	
	if((alarm0 == 1) && (alarm1 == 0) && (a == 0))
	{
		delay(35000);
		b = 1;
		alr++;
		if(alr > 3)
			alr = 0;
	}
	
	if(alr != 0)
	{
		ktup();
		ktdown();
	}
}
//--------------------------------------------------------------
void tinhchinh_ngay()
{
	if(a == 1 && kt == 0) // kiem tra co thay doi ngay thi dieu chinh lai cho phu hop
	{
		
		if(month==4 || month==6 || month==9 || month==11)
		{
			if(date == 31)
				date = 30;
		}
		
		if(month == 2) // tinh nam nhuan
		{
			if(year%100 == 0)
			{
				if(year%400 == 0)					
				{
					if(date > 29)
						date = 29;
				}
			}
			else if(year%4 == 0)
			{
				if(date > 29)
					date = 29;
			}
			else 
			{
				if(date > 28)
					date = 28;
			}
		}
	}
}

//**************************************************************
//===============  Cac chuong trinh ngat  ======================
//**************************************************************
void khoitaongat() 
{
	TMOD = 0x11; 	// Chon Timer 1 che do 1 (16bit) va Timer 0 che do 2 (8bit tu nap lai)
	  
	EA   = 1;	    // Cho phep ngat
	
 	ET0  = 1;		  // Cho ngat Timer0	
	TR0	 = 1; 	  // Khoi dong Timer 0
	TF0  = 0;
	
	ET1  = 1;	   	// Cho phep ngat Timer 1
	TR1  = 1;			// Khoi dong Timer 1
	//TF1  = 0;

}

//--------------------------------------------------------------
void ngat_timer0(void) interrupt 1 // Dung ngat Timer0
{
	TR0=0;
	TF0=0;
	TH0=0x3C; // Gia tri cho Timer0: 50000(us)
	TL0=0xAF;
	dem++;
	
	if(tsp < 25)
		tsp++;
	
	if(!kt && !alr)
		dem1++;
	else
		dem1 = 0;
//bien nhay_led dung de nhay led trong luc cai dat
	if(dem >= 10) // 0.5s (50ms*10)
	{
		dem = 0;
		read_time = 1;
		nhay_led = !nhay_led;
	}
	
	if(dem1 >= 400) // 20s (50ms*400)
		dem1 = 0;
	
	TR0=1;
}

//--------------------------------------------------------------
void hien_thi(void) interrupt 3 // Dung ngat Timer1 de hien thi Led 7 doan
{
	TR1=0;
	//TF1=0;

//*************************** che do binh thuong ****************************
	if(alr == 0)
	{		
		Solar2Lunar(date, month, year, &dateal, &monthal, &SolarYear); // am lich
		
// Bien pwm dung de tao gia tri pwm de ham thuwc hien
// Bien led_giay dung de nhay 2 led don theo giay
// Bien nhay_led duoc thuc hien o timer0, dung de nhay led
		
		if(hour < 21 && hour >= 6)
		{
			TH1=0xFC; // Gia tri cho Timer1: 1000(us)
			TL1=0x18;
			chon++; 	// lua chon led hien thi, nhay led trong luc cai dat thoi gian
			pwm = 1;
			if(nhay_led)
				led_giay = 0;
			else 
				led_giay = 1;
		}
		//dieu chinh xung pwm
		else
		{
			pwm = !pwm;
			
			//chi hien thi 4 led trong khung gio cai dat
			if(chon < 8)
				chon = 8;
			
			if(pwm == 1)
			{
				TH1 = 0xff; // Muc cao 50us
				TL1 = 0xce;
				chon++;
				led_giay = 1;
			}
			else
			{
				TH1 = 0xfe; // Muc thap 450us
				TL1 = 0x3e;
				led_giay = 1;				
			}
		}
		if(pwm) // bat led dung PWM
		{
	//---------------------hien thi thu-----------------------
			if(chon == 1) // led chuc thu
			{
				//che do cai dat, bien nhay_led = 1 trong 0.5s led 7 doan tat, bien nhay_led = 0 trong 0.5s led 7 doan sang
				if(kt == 1 && nhay_led == 1 && up != 0 && down != 0)
					chon++;
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff;
					
					c_thu = 0;
					
					if(day == 1)
						P3 = 0xc6; // Chu "C"
				}
			}
			else if(chon == 2) // led don vi thu
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff;
				
				dv_thu = 0;
					
				if(day > 1)
					P3 = led7[day%10];
				else
					P3 = 0xC8; // chu "N"
			}
			
	//---------------------hien thi ngay-----------------------
			else if(chon == 3) // led chuc ngay
			{
				if(kt == 2 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 	
					if(date < 10 || dateal < 10)
						c_ngay = 1;
					
					if(dem1 <= 200 ) // 10s hien thi Duong Lich
					{
						if(date >= 10)
						{
							c_ngay = 0;
														
							P3 = led7[date/10];
						}					
					}
					else // 10s hien thi Am Lich
					{
						if(dateal >= 10)
						{
							c_ngay = 0;

							P3 = led7[dateal/10];
						}				
					}	
				}
			}
			else if(chon == 4) // led don vi ngay
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				
				dv_ngay = 0;
				
				if(dem1 <= 200 ) // 10s hien thi Duong Lich
					P3 = led7[date%10];
				else // 10s hien thi Am Lich
					P3 = led7[dateal%10];
			}
			
	//---------------------hien thi thang-----------------------
			else if(chon == 5) // led chuc thang
			{
				if(kt == 3 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 
					
					c_thang = 0;
					
					if(date < 10 || dateal < 10)
						c_thang = 1;
					
					if(dem1 <= 200 ) // 10s hien thi Duong Lich
					{
						if(month >= 10)
						{							
							c_thang = 0;
							
							P3 = led7[month/10];
						}					
					}
					else // 10s hien thi Am Lich
					{
						if(monthal >= 10)
						{
							c_thang = 0;
							
							P3 = led7[monthal/10];
						}				
					}			
				}
			}
			else if(chon == 6) // led don vi thang
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				
				dv_thang = 0;
				
				if(dem1 <= 200 ) // 10s hien thi Duong Lich
					P3 = led7[month%10];
				else // 10s hien thi Am Lich
					P3 = led7[monthal%10];
			}
			
	//---------------------hien thi nam-----------------------
			else if(chon == 7) // led chuc nam
			{
				if(kt == 4 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 	
					
					if(htgiay == 1 && kt == 0 && alr == 0) // hien thi giay
					{
						if(sec >= 10)
							c_nam = 0;	
						else 
							c_nam = 1;
						P3 = led7[sec/10]; 
					}
					else // hien thi nam
					{
						c_nam = 0;
						
						if(dem1 <= 200 ) // 10s hien thi Duong Lich
							P3 = led7[year/10]; 
						else // 10s hien thi Am Lich
							P3 = 0x88; // chu "A"
					}
				}
			}
			else if(chon == 8) // led don vi nam
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff;
									
				dv_nam = 0;
				
				if(htgiay == 1 && kt == 0 && alr == 0) // hien thi giay
					P3 = led7[sec%10];
				else  // hien thi nam
				{
					if(dem1 <= 200 ) // 10s hien thi Duong Lich
						P3 = led7[year%10]; // chan P3.7 khong thay doi trang thai
					else // 10s hien thi Am Lich
						P3 = (0xC7); // chu "L"
				}
				
			}

	//---------------------hien thi gio-----------------------
			else if(chon == 9) // led chuc gio
			{
				if(kt == 5 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 
					
					if(hour >= 10)				
						c_gio = 0;

					else 
						c_gio = 1;
					P3 = led7[hour/10];
				}
			}
			else if(chon == 10) // led don vi gio
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				
				dv_gio = 0;
				
				P3 = led7[hour%10];
			}

	//---------------------hien thi phut-----------------------
			else if(chon == 11) // led chuc phut
			{
				if(kt == 6 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 					
					
					c_phut = 0;

					P3 = led7[min/10];
				}
			}
			else if(chon == 12) // led don vi phut
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff;
				
				dv_phut = 0;
				
				if(hour < 21 && hour >= 6)
					chon = 0;
				
				P3 = led7[min%10];
			}	
			
			else if(chon > 12)
			{
				if(hour < 21 && hour >= 6)
					chon = 0;
				
				else
				{
					if(chon > 19)
					chon = 0;
				}		
			}
		}
// tat led bang PWM cac chan data
		else 
			P3 = 0xff;
}
	
//************************ che do cai dat bao thuc ************************************
//*************************************************************************************
	else
	{
		pwm = !pwm;
			
			if(pwm == 1)
			{
				TH1 = 0xff; // Muc cao 250us
				TL1 = 0x06;
				chon++;
				if(nhay_led)
					led_giay = 0;
			}
			else
			{
				TH1 = 0xff; // Muc thap 250us
				TL1 = 0x06;
				led_giay = 1;				
			}
		if(pwm)
		{
			if(chon > 6)
				chon = 0;
	//---------------------hien thi gio-----------------------		
			else if(chon == 1) // led chuc gio
			{
				if(alr == 1 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 
					if(a_hour >= 10)
						c_gio = 0;
					else 
						c_gio = 1;
					P3 = led7[a_hour/10];
				}
			}
			else if(chon == 2) // led don vi gio
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				dv_gio = 0;
				P3 = led7[a_hour%10];
			}

	//---------------------hien thi phut-----------------------
			else if(chon == 3) // led chuc phut
			{
				if(alr == 2 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 
					c_phut = 0;
					P3 = led7[a_min/10];
				}
			}
			else if(chon == 4) // led don vi phut
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				dv_phut = 0;
				P3 = led7[a_min%10];
			}	

	//---------------------hien thi on-off-----------------------
			else if(chon == 5) // chu "O"
			{
				if(alr == 3 && nhay_led == 1 && up != 0 && down != 0)
					chon++; // bo qua hien thi de nhay led 7 doan
				else
				{
					c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
					c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
					P3 = 0xff; 
					c_nam = 0;
					P3 = 0xC0; // Chu "o"
				}
			}
			else if(chon == 6) // led don vi on-off
			{
				c_ngay = dv_ngay = c_thang = dv_thang = c_nam = dv_nam = 1; //tat led 7 doan
				c_gio = dv_gio = c_phut = dv_phut = c_thu = dv_thu = 1; // tat led 7 doan
				P3 = 0xff; 
				dv_nam = 0;
				if(set_alarm == 0)
					P3 = 0x8E; // chu "F"
				else
					P3 = 0xC8; // chu "N"
				
				chon = 0;
			}
		}
		else
			P3 = 0xff;
	}
	
	if(alarm_on == 1)
		speaker(5);
//	if(hour >= 7 && hour <= 20 && min == 0 && sec == 0 && alr == 0 && kt == 0)
//		speaker(2);
	
	TR1=1;
}

//**************************************************************
//================  Chuong trinh chinh   =======================
//**************************************************************
void main()
{
	khoitaongat();
	
	P0=0xff;				   // tat port quet led
	P1=0xff;
	P2=0xff;					//  tat port data
	P3=0xff;				 //   tat port data

	I2C_Start();
 	I2C_Write(0xD0);
	I2C_Write(0x07); 
	I2C_Write(0x10); 
	I2C_Stop();
	
	rtc_write(0x0e,0x04);
	rtc_write(0x0f,0x0b10000000);
	
	readds(); // Doc thoi gian
	
	read_alarm();  // Doc thoi gian bao thuc da cai dat
	
	pwm = 0;
  while(1)
 	{		
		ktset();
		tinhchinh_ngay();
		tinhchinh_ds3231();
			
		if(kt == 0 && a == 1)
		{
			writeds();
			a = 0;
		}
		
		if(!kt && !alr && read_time)
		{
			readds();	
			read_time = 0;
		}
		
		ktalarm();
		if(alr == 0 && b == 1)
		{
			write_alarm();
			b = 0;
		}
		
		if(kt == 0 && alr == 0)
		{
			if(up == 0 || down ==0)
			{
				delay(50000 - m3);
				if(!tam)	//tam1 == 0, kiem tra xem nut nhan duoc nhan hay nhan giu, nhan giu tam = 1 
				{
					tam  = 1;	//giu nut nhan thi khong thuc hien, cho den khi nut nhan duoc nha ra 
					m3 = 50000;
					set_alarm = 0;
					if(set_alarm == 0)
						sp = 1;
				}
			}
			else 
			{
				tam = 0;
				m3 = 0;
			}
			
			if(up == 0 && down ==0)
			{
				delay(50000 - m4);
				if(!tam1)	//tam1 == 0, kiem tra xem nut nhan duoc nhan hay nhan giu, nhan giu tam = 1 
				{
					tam1  = 1;	//giu nut nhan thi khong thuc hien, cho den khi nut nhan duoc nha ra 
					m4 = 50000;
					htgiay = !htgiay;
				}
			}
			else 
			{
				tam1 = 0;
				m4 = 0;
			}
		}
		
		if(set_alarm == 1)
		{
			if(a_min == min && a_hour == hour && set_alarm == 1)
				alarm_on = 1;
			ledal = 0;
		}		
		else
		{
			ledal = 1;
			alarm_on = 0;
		}	

	}
}