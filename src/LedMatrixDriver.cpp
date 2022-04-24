#include "LedMatrixDriver.h"

#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

LedMatrixDriver::LedMatrixDriver(const int din_pin, const int clk_pin, const int csld_pin, const int dev_num, const int matrix_width, const int matrix_height){
    this->din_pin = din_pin;
    this->clk_pin = clk_pin;
    this->csld_pin = csld_pin;
    this->dev_num = dev_num;
    this->matrix_width = matrix_width;
    this->matrix_height = matrix_height;

    this->spidata_len = dev_num << 1;
    
    int i = 0;
    
    // init led_status[]
    int t_sum_col_num = dev_num << 3;       // *8
    this->led_status = new byte[t_sum_col_num];
    for(i = 0; i < t_sum_col_num; i++)
        led_status[i] = (byte)0;
    
    // init spidata[]
    this->spidata = new byte[spidata_len];
    clear_spidata();

    
    pinMode(din_pin, OUTPUT);
    pinMode(clk_pin,OUTPUT);
    pinMode(csld_pin,OUTPUT);

    for(int i = 0; i < maxDevices; i++) {
        set_dev_displaytest(i, 0);
        set_dev_scanlimit(i, 7);
        set_dev_decodemode(i, 0);
        set_dev_shutdown(i, 1);
        clear_dev(i);
        set_dev_intensity(i, 0);
    }

}

void set_dev_order(int dev_order[]){
    // set dev_order
    this->dev_order = new int[dev_num];
    for(i = 0; i < dev_num; i++)
        (this->dev_order)[i] = dev_order[i];
}

void LedMatrixDriver::clear_spidata(){
    for(i = 0; i < spidata_len; i++)
        spidata[i] = (byte)0;
}

void LedMatrixDriver::set_spidata(int dev_addr, byte opcode, byte data){
    int base = addr << 1;
    spidata[base + 1] = opcode;
    spidata[base] = data;
}

void LedMatrixDriver::write_spidata(){
    if(dev_order == NULL)
        return;

    digitalWrite(csld_pin, LOW);

    // shift out the data 
    int t, i, op_dev;
    for(i = spidata_len; i > 0; i--){
        op_dev = dev_order[(i - 1) / 2];
        t = i % 2  == 0 ? (op_dev * 2 + 1) : (op_dev * 2);
        shiftOut(din_pin, clk_pin, MSBFIRST, spidata[t]);
    }

    digitalWrite(csld_pin, HIGH);
}


bool LedMatrixDriver::set_dev_decodemode(const int dev_addr, const int value){
    if(dev_addr >= 0 && dev_addr < dev_num){
        if(value == 0 || value == 1 || value == 15 || value == 255){
            clear_spidata();
            set_spidata(dev_addr, OP_DECODEMODE, value);
            write_spidata();
            return true;
        }
    }
    return false;
}


bool LedMatrixDriver::set_dev_intensity(const int dev_addr, const int value){
    if(dev_addr >= 0 && dev_addr < dev_num){
        if(value >= 0 && value <= 15){
            clear_spidata();
            set_spidata(dev_addr, OP_INTENSITY, value);
            write_spidata();
            return true;
        }
    }
    return false;
}


bool LedMatrixDriver::set_dev_scanlimit(const int dev_addr, const int value){
    if(dev_addr >= 0 && dev_addr < dev_num){
        if(value >= 0 && value <= 7){
            clear_spidata();
            set_spidata(dev_addr, OP_SCANLIMIT, value);
            write_spidata();
            return true;
        }
    }
    return false;
}


bool LedMatrixDriver::set_dev_shutdown(const int dev_addr, const bool value){
    if(dev_addr >= 0 && dev_addr < dev_num){
        clear_spidata();
        set_spidata(dev_addr, OP_SHUTDOWN, value);
        write_spidata();
        return true;
    }
    return false;
}


bool LedMatrixDriver::set_dev_displaytest(const int dev_addr, const bool value){
    if(dev_addr >= 0 && dev_addr < dev_num){
        clear_spidata();
        set_spidata(dev_addr, OP_DISPLAYTEST, value);
        write_spidata();
        return true;
    }
    return false;
}

// clear one dev
bool LedMatrixDriver::clear_dev(const int dev_addr){
    clear_spidata();
    int base = dev_addr << 3;
    for(int i = 0; i < 8; i++){
        led_status[base + i] = (byte)0;
        set_spidata(dev_addr, i + 1, (byte)0);
        write_spidata();
    }
}

// clear an area
/* old
bool LedMatrixDriver::clear_area(const int row, const int col, const int row_num, const int col_num){
    clear_spidata();
    for(int i_addr = 0; i_addr < dev_num; i++){
        
    }


    
    
    
    
    clear_spidata();
    
    int &row1 = row, &col1 = col;
    int row2 = row1 + row_num, col2 = col1 + col_num;
    
    int dev_pix_row1, dev_pix_col1;
    
    int dev_row1 = row1 >> 3;
    int dev_col1 = col1 >> 3;
    int dev_row2 = row2 >> 3;
    int dev_col2 = col2 >> 3;
    int devbase = 0;
    for(int i = dev_row1; i < dev_row2 + 1; i++){
        for(int j = dev_col1; j < dev_col2 + 1; j++){
            devbase = i * matrix_width + j;

            dev_pix_row1 = i << 3;
            dev_pix_col1 = j << 3;

            // 从该块点阵左上角的 led 点开始
            for(int m = dev_pix_row1; m < dev_pix_row1 + 8; m++){
                byte r = led_status[devbase * 8 + (m % 8)];     // 获取修改前状态
                byte t = B10000000;
                for(int n = dev_pix_col1; n < dev_pix_col1 + 8; n++){
                    if(m >= row1 && m < row2 && n >= col1 && n < col2){     // 该点在目标区域内，就进行与操作
                        r = r & (~ t);      // (~t)中只有一位是0，与操作后，r 中对应位变为 0
                    }
                    t = t >> 1;
                }
                set_spidata(devbase, m % 8 +1, r);
                led_status[devbase * 8 + (m % 8)] = r;
            }


        }
    }
    write_spidata();
}
*/

bool LedMatrixDriver::clear_area(const int row, const int col, const int row_num, const int col_num){
    clear_spidata();
    
    int &row1 = row;
    int &col1 = col;
    int row2 = row1 + row_num;
    int col2 = col1 + col_num;

    int dev_row1 = row1 >> 3;
    int dev_col1 = col1 >> 3;
    int dev_row2 = row2 >> 3;
    int dev_col2 = col2 >> 3;

    int dev = 0;
    for(int r = dev_row1; r < dev_row2; r++){
        for(int c = dev_col1; c < dev_col2; c++){
            dev_addr = r * matrix_width + c;

            int i = row1 < (r << 3) ? 0 : (row1 % 8);
            int i_max = row2 >= ((r + 1) << 3) ? 8 : (row2 % 8);

            int j = col1 < (c << 3) ? 0 : (col1 % 8);
            int j_max = col2 >= ((c + 1) << 3) ? 8 : (col2 % 8);

            
            byte mask = B10000000;
            for(i = i; i < i_max; i++){
                for(j = j; j < j_max; j++){
                    led_status[i] = ( ~ (mask >> j) ) & led_status[i];
                }
            }

        }
    }

    for(int i = 0; i < 8; i++){
        for(int addr = 0; addr < dev_num; addr++){
            set_spidata(addr, i + 1, led_status[(addr << 3) + i]);
        }
        write_spidata();
    }
    
    
    return true;
}

/*
↑
修改思路：直接对led_status进行操作，
        再矩阵每次写入所有模块的同一行，写入din端口，循环8次每次一行。
↓
*/


// set an area
bool LedMatrixDriver::set_area(const int row, const int col, const int row_num, const int col_num, bool pixs[]){
    clear_spidata();
    
    int &row1 = row;
    int &col1 = col;
    int row2 = row1 + row_num;
    int col2 = col1 + col_num;

    int dev_row1 = row1 >> 3;
    int dev_col1 = col1 >> 3;
    int dev_row2 = row2 >> 3;
    int dev_col2 = col2 >> 3;

    int dev = 0;
    for(int r = dev_row1; r < dev_row2; r++){
        for(int c = dev_col1; c < dev_col2; c++){
            dev_addr = r * matrix_width + c;

            int i = row1 < (r << 3) ? 0 : (row1 % 8);
            int i_max = row2 >= ((r + 1) << 3) ? 8 : (row2 % 8);

            int j = col1 < (c << 3) ? 0 : (col1 % 8);
            int j_max = col2 >= ((c + 1) << 3) ? 8 : (col2 % 8);

            
            byte mask = B10000000;
            for(i = i; i < i_max; i++){
                for(j = j; j < j_max; j++){
                    int pix_x = i + r * 8 - row1;
                    int pix_y = j + c * 8 - col1;
                    if(pixs[pix_x * row_num + pix_y] == true){
                        led_status[i] = (mask >> j) | led_status[i];
                    }
                    else{
                        led_status[i] = ( ~ (mask >> j) ) & led_status[i];
                    }
                }
            }

        }
    }

    for(int i = 0; i < 8; i++){
        for(int addr = 0; addr < dev_num; addr++){
            set_spidata(addr, i + 1, led_status[(addr << 3) + i]);
        }
        write_spidata();
    }
    
    
    return true;
}