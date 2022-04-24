#ifndef LedMatrixDriver_h
#define LedMatrixDriver_h

#include <avr/pgmspace.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class LedMatrixDriver{
    private:
        byte *spidata = NULL;

        void clear_spidata();
        
        void set_spidata(int dev_addr, byte opcode, byte data);

        void write_spidata();


        byte *led_status = NULL;

        int din_pin;
        int clk_pin;
        int csld_pin;
        int dev_num;

        int spidata_len;

        int *dev_order = NULL;
        const int dev_width = 8, dev_height = 8;
        int matrix_width, matrix_height;      // num of devs
        int pix_width, pix_height;            //num of pixels

        

    public:
        LedMatrixDriver(const int din_pin, const int clk_pin, const int csld_pin, const int dev_num, const int matrix_width, const int matrix_height, int dev_order[]);

        

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
        bool clear_area(const int row, const int col, const int row_num, const int col_num);

        // set an area
        bool set_area(const int row, const int col, const int row_num, const int col_num, bool pixs[]);

};

#endif	//LedMatrixDriver.h