#ifndef LedMatrixDriver_h
#define LedMatrixDriver_h

// #include <avr/pgmspace.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class LedMatrixDriver{
    //public:
    private:
        int din_pin, clk_pin, csld_pin;
        int dev_num;
        int spidata_len;    // length of spidata[]

        const int dev_width = 8, dev_height = 8;    // 8*8 matrix device
        int matrix_width, matrix_height;            // num of devices
        int pix_width, pix_height;                  // num of pixels

        static const int max_dev_num = 100;

        // storage data output to din pin
        byte spidata[max_dev_num << 1];

        // storage all leds status
        byte led_status[max_dev_num << 3];

        // storage device order
        int dev_order[max_dev_num];

        void clear_spidata();
        
        bool set_spidata(int dev_addr, byte opcode, byte data);

        void write_spidata();


        

    public:
        LedMatrixDriver(const int din_pin, const int clk_pin, const int csld_pin, const int dev_num, const int matrix_width, const int matrix_height, int *dev_order = nullptr);

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


        // update display by led_status[]
        bool update_display();      // return value about error info, false is error

        // clear an area
        bool clear_area(const int row1, const int col1, const int row_num, const int col_num);

        // set an area
        bool set_area(const int row1, const int col1, const int row_num, const int col_num, bool *pixs);

        // set one led point
        bool set_point(const int row1, const int col1, bool pix_value);

        // set devices order
        bool set_dev_order(int *in_order);

        // preset all pixs value (not update display)
        bool preset_allpixs(bool *pixs);

        // preset an area (not update display)
        bool preset_area(const int row1, const int col1, const int row_num, const int col_num, bool *pixs);

        // preset one led point (not update display)
        bool preset_point(const int row1, const int col1, bool pix_value);


};

#endif	//LedMatrixDriver.h
