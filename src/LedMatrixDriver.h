#ifndef LedMatrixDriver_h
#define LedMatrixDriver_h

#include <avr/pgmspace.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class LedMatrixDriver{
    public:
    //private:
        //byte *spidata = NULL;
        byte spidata[4];       // linshi

        

        byte led_status[16];    // linshi
        //byte *led_status = NULL;

        int din_pin;
        int clk_pin;
        int csld_pin;
        int dev_num;

        int spidata_len;

        int dev_order[2];      // linshi
        //int *dev_order = NULL;

        const int dev_width = 8, dev_height = 8;
        int matrix_width, matrix_height;      // num of devs
        int pix_width, pix_height;            //num of pixels

        

    //public:

        void clear_spidata();
        
        bool set_spidata(int dev_addr, byte opcode, byte data);

        void write_spidata();



        LedMatrixDriver(const int din_pin, const int clk_pin, const int csld_pin, const int dev_num, const int matrix_width, const int matrix_height);

        bool set_dev_order(int *in_order);

        // value  =  0 / 1 / 15 / 255
        bool set_dev_decodemode(const int dev_addr, const int value);

        // value = [0,15]
        bool set_dev_intensity(const int dev_addr, const int value);

        // value = [0,7]
        bool set_dev_scanlimit(const int dev_addr, const int value);

        // value = 0 / 1
        bool set_dev_shutdown(const int dev_addr, const bool value);

        // value = 0 / 1
        bool set_dev_displaytest(const int dev_addr, const bool value);

        

        


        // clear one dev
        bool clear_dev(const int dev_addr);

        // clear an area
        bool clear_area(const int row1, const int col1, const int row_num, const int col_num);

        // set an area
        bool set_area(const int row1, const int col1, const int row_num, const int col_num, bool *pixs);

        // set one led point
        bool set_point(const int row1, const int col1, bool pix_value);

};

#endif	//LedMatrixDriver.h