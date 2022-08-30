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

LedMatrixDriver::LedMatrixDriver(const int din_pin, const int clk_pin, const int csld_pin, const int dev_num, const int matrix_width, const int matrix_height, int *dev_order){
    // set pins
    this->din_pin = din_pin;
    this->clk_pin = clk_pin;
    this->csld_pin = csld_pin;

    // set pins pinMode
    pinMode(din_pin, OUTPUT);
    pinMode(clk_pin,OUTPUT);
    pinMode(csld_pin,OUTPUT);

    // init csld_pin value
    digitalWrite(csld_pin, HIGH);

    // set parameters
    this->dev_num = dev_num;
    this->matrix_width = matrix_width;
    this->matrix_height = matrix_height;
    this->pix_width = matrix_width << 3;
    this->pix_height = matrix_height << 3;
    this->spidata_len = dev_num << 1;
    set_dev_order(dev_order);

    int i = 0;
    
    // init led_status[]
    int t_sum_col_num = dev_num << 3;
    for(i = 0; i < t_sum_col_num; i++)
        led_status[i] = (byte)0;
    
    // init spidata[]
    clear_spidata();

    // init all driver chips
    for(i = 0; i < dev_num; i++) {
        set_dev_displaytest(i, 0);
        set_dev_scanlimit(i, 7);
        set_dev_decodemode(i, 0);
        clear_dev(i);
        set_dev_shutdown(i, 0);
        set_dev_intensity(i, 0);
    }
}

// range in [0, dev_num], dev_order.len == dev_num
bool LedMatrixDriver::set_dev_order(int *in_order){
    // set dev_order
    if(in_order == NULL)
        return false;

    //this->dev_order = new int[dev_num];       linshi
    for(int i = 0; i < dev_num; i++)
        dev_order[i] = in_order[i];
    return true;
}

void LedMatrixDriver::clear_spidata(){
    for(int i = 0; i < spidata_len; i++)
        spidata[i] = (byte)0;
}

bool LedMatrixDriver::set_spidata(int dev_addr, volatile byte opcode, volatile byte data){
    if(dev_addr >= dev_num || opcode < 0 || opcode > 15)
        return false;

    int base = dev_addr << 1;
    spidata[base + 1] = opcode;
    spidata[base] = data;
    return true;
}

void LedMatrixDriver::write_spidata(){
    if(dev_order == NULL)
        return;

    digitalWrite(csld_pin, LOW);

    // shift out the data 
    int t, i, op_dev;
    for(i = spidata_len; i > 0; i--){
        ///*
        op_dev = dev_order[(i - 1) / 2];
        t = (i % 2  == 0) ? (op_dev * 2 + 1) : (op_dev * 2);
        //*/
        shiftOut(din_pin, clk_pin, MSBFIRST, spidata[t/*i-1*/]);
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
    if(dev_addr >= dev_num)
        return false;
    
    int base = dev_addr << 3;
    for(int i = 0; i < 8; i++){
        led_status[base + i] = (byte)0;
        clear_spidata();
        set_spidata(dev_addr, i + 1, (byte)0);
        write_spidata();
    }

    return true;
}

bool LedMatrixDriver::update_display(){
    bool re = true;
    for(int i = 0; i < 8; i++){
        for(int addr = 0; addr < dev_num; addr++){
            re = set_spidata(addr, i + 1, led_status[(addr << 3) + i]);
        }
        write_spidata();
    }
    return re;
}

// clear an area
/* old
bool LedMatrixDriver::clear_area(const int row, const int col, const int row_num, const int col_num){    
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

bool LedMatrixDriver::clear_area(const int row1, const int col1, const int row_num, const int col_num){
    int len = row_num * col_num;
    bool b_value[len];
    for(int i = 0; i < len; i++)
        b_value[i] = false;
    return set_area(row1, col1, row_num, col_num, b_value);
}

/*
↑
修改思路：直接对led_status进行操作，
        再矩阵每次写入所有模块的同一行，写入din端口，循环8次每次一行。
↓
*/

// set an area
bool LedMatrixDriver::set_area(const int row1, const int col1, const int row_num, const int col_num, bool *pixs){
    if(preset_area(row1, col1, row_num, col_num, pixs)== false){
        return false;
    }
    if(update_display() == false){
        return false;
    }
    return true;
}

bool LedMatrixDriver::set_point(const int row1, const int col1, bool pix_value){
    bool b_value[1];
    b_value[0] = pix_value;
    return set_area(row1, col1, 1, 1, b_value);
}

// preset an area (not update display)
bool LedMatrixDriver::preset_area(const int row1, const int col1, const int row_num, const int col_num, bool *pixs){
    // Serial.println("  set_area() called");
    
    if(pixs == NULL)
        return false;

    clear_spidata();
    
    int row2 = row1 + row_num;
    int col2 = col1 + col_num;

    int dev_row1 = row1 >> 3;
    int dev_col1 = col1 >> 3;
    int dev_row2 = (row2 >> 3) + 1;
    int dev_col2 = (col2 >> 3) + 1;

    // Serial.print("  row1:");
    // Serial.println(row1);
    // Serial.print("  col1:");
    // Serial.println(col1);
    // Serial.print("  row2:");
    // Serial.println(row2);
    // Serial.print("  col2:");
    // Serial.println(col2);

    // Serial.print("  dev_row1:");
    // Serial.println(dev_row1);
    // Serial.print("  dev_col1:");
    // Serial.println(dev_col1);
    // Serial.print("  dev_row2:");
    // Serial.println(dev_row2);
    // Serial.print("  dev_col2:");
    // Serial.println(dev_col2);
    

    //int dev = 0;
    for(int r = dev_row1; r < dev_row2; r++){
        for(int c = dev_col1; c < dev_col2; c++){
            //dev_addr = r * matrix_width + c;

            // Serial.print("    r:");
            // Serial.println(r);
            // Serial.print("    c:");
            // Serial.println(c);

            int i_min = row1 < (r << 3) ? 0 : (row1 % 8);
            int i_max = row2 >= ((r + 1) << 3) ? 8 : (row2 % 8);

            int j_min = col1 < (c << 3) ? 0 : (col1 % 8);
            int j_max = col2 >= ((c + 1) << 3) ? 8 : (col2 % 8);

            // Serial.print("      i: ");
            // Serial.print(i_min);
            // Serial.print(" to ");
            // Serial.println(i_max);
            // Serial.print("      j: ");
            // Serial.print(j_min);
            // Serial.print(" to ");
            // Serial.println(j_max);
            
            
            byte mask = B10000000;
            for(int i = i_min; i < i_max; i++){
                int byte_loc = i + (((c % matrix_width) + (r % matrix_height) * matrix_width) << 3);
                // Serial.print("        i:");
                // Serial.print(i);
                // Serial.print("   r = ");
                // Serial.println(r);
                for(int j = j_min; j < j_max; j++){
                    int pix_x = i + r * 8 - row1;
                    int pix_y = j + c * 8 - col1;
                    if(pixs != NULL){
                        
                        if(pixs[pix_x * col_num + pix_y] == true){      // col_num or row_num
                            led_status[byte_loc] = (mask >> j) | led_status[byte_loc];
                            // Serial.print("          j:");
                            // Serial.println(j);
                            // Serial.print("            (mask >> j)");
                            // Serial.println((mask >> j));
                        }
                        else{
                            led_status[byte_loc] = ( ~ (mask >> j) ) & led_status[byte_loc];
                        }
                    }
                }
                
                //Serial.print("          led_status[byte_loc]:");
                //Serial.println(led_status[byte_loc]);
            }

        }
    }

    if(row_num < 1 || row_num > pix_height || col_num < 1 || col_num > pix_width)
        return false;
    
    return true;
}

// preset one led point (not update display)
bool LedMatrixDriver::preset_point(const int row1, const int col1, bool pix_value){
    bool b_value[1];
    b_value[0] = pix_value;
    return preset_area(row1, col1, 1, 1, b_value);
}