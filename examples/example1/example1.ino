#include "LedMatrixDriver.h"

#define N 9
#define NUM_H 5
#define NUM_W 4

bool num0[20] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 0, 0, 1,
	1, 1, 1, 1};

bool num1[20] = {
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0};

bool num2[20] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 1, 1, 1};

bool num3[20] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	1, 1, 1, 1,
	0, 0, 0, 1,
	1, 1, 1, 1};

bool num4[20] = {
	1, 0, 1, 0,
	1, 0, 1, 0,
	1, 0, 1, 0,
	1, 1, 1, 1,
	0, 0, 1, 0};

bool num5[20] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 1,
	1, 1, 1, 1};

bool num6[20] = {
	1, 1, 1, 1,
	1, 0, 0, 0,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 1, 1, 1};

bool num7[20] = {
	1, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1};

bool num8[20] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 1, 1, 1};

bool num9[20] = {
	1, 1, 1, 1,
	1, 0, 0, 1,
	1, 1, 1, 1,
	0, 0, 0, 1,
	1, 1, 1, 1};


/*
	Now we create a LedMatrixDriver through the constructor to drive the LED matrix.
	pin 12 is connected to the DataIn
	pin 11 is connected to the CLK
	pin 10 is connected to LOAD
	We have 1 MAX72XX.
*/
const int din_pin = 12;
const int clk_pin = 11;
const int csld_pin = 10;
const int dev_num = 2;
const int matrix_width = 2;
const int matrix_height = 1;
int dev_order[dev_num] = {0, 1};
LedMatrixDriver lmd = LedMatrixDriver(din_pin, clk_pin, csld_pin, dev_num, matrix_width, matrix_height, dev_order);

void set_number(int row, int col, int num = 0);

int cnt = 0;

void setup()
{
	// put your setup code here, to run once:

	Serial.begin(9600); // for debug

	for(int i = 0; i < dev_num; i++){
		lmd.set_dev_shutdown(i, 1);
		//lmd.set_dev_intensity(i, 0);
		//lmd.clear_dev(i);
	}

}


void loop()
{
	Serial.println(cnt);
	for(int i = 0; i < matrix_height; i++)
		for(int j = 0; j < matrix_width; j++){
			set_number(i * matrix_width * 8 + 2, j * matrix_height * 8 + 2, (cnt + i * matrix_width + j) % 10);
			Serial.print("    i : ");
			Serial.println(i);
			Serial.print("    j : ");
			Serial.println(j);
			Serial.print("        row :");
			Serial.println(i * matrix_width * 8 + 2);
			Serial.print("        col : ");
			Serial.println(j * matrix_height * 8 + 2);
		}
	
	// set_number(2, 2, cnt % 10);
	// set_number(2, 2 + 8, cnt % 10);

	Serial.println("");
	cnt++;
	delay(2000);
	
}


// num is number to display 0-9
void set_number(int row, int col, int num = 0)
{
	bool *p = NULL;
	switch (num)
	{
	case 0:
		p = num0;
		break;
	case 1:
		p = num1;
		break;
	case 2:
		p = num2;
		break;
	case 3:
		p = num3;
		break;
	case 4:
		p = num4;
		break;
	case 5:
		p = num5;
		break;
	case 6:
		p = num6;
		break;
	case 7:
		p = num7;
		break;
	case 8:
		p = num8;
		break;
	case 9:
		p = num9;
		break;
	}
	lmd.set_area(row, col, NUM_H, NUM_W, p);
}