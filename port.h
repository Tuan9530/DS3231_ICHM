#ifndef _PORT_H_
#define _PORT_H_

//Chan giao tiep giua VXL va DS3231
sbit 	SDA = P0^0;					   
sbit 	SCL = P0^1;					   

//Chan dieu khien led 7 doan
sbit c_gio    = P2^5; // led chuc gio
sbit dv_gio   = P2^4; // led don vi gio

sbit c_phut   = P2^3; // led chuc phut
sbit dv_phut  = P2^2; // led don vi phut

sbit c_thu    = P2^6; // led chuc thu
sbit dv_thu   = P2^7; // led don vi thu

sbit c_ngay   = P0^7; // led chuc ngay
sbit dv_ngay  = P0^6; // led don vi ngay

sbit c_thang  = P0^5; // led chuc thang
sbit dv_thang = P0^4; // led don vi thang

sbit c_nam  	= P0^3; // led chuc nam
sbit dv_nam   = P0^2; // led don  vi nam

//Chan dieu khien nut nhan
sbit set   = P1^0;
sbit up    = P1^2;
sbit down  = P1^1;
sbit alarm = P1^3;

//Coi speaker
sbit sp = P1^6;

//Chan dieu khien led
sbit ledal = P2^0;
sbit led_giay = P2^1;

#endif
