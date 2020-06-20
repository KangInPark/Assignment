#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/i2c-dev.h>
#include "data.h"
#include <time.h>

#define SSD1306_I2C_DEV 0x3C
#define S_WIDTH 128
#define S_HEIGHT 64
#define S_PAGES (S_HEIGHT / 8)
#define MAX_MAP_X 10
#define MAX_MAP_Y 20
#define MAP_X_LOC 1
#define MAP_Y_LOC 40

int map[MAX_MAP_X][MAX_MAP_Y];
int curr[4][4];
int curr_x = 0, curr_y = 0;
int curr_type;
int i2c_fd;
int init = 0;
int is_over = 1;
uint8_t *reset;
uint8_t *data;
uint8_t *frame;

void ssd1306_command(int i2c_fd, uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = (0 << 7) | (0 << 6); //Co = 0, D/C# = 0
    buffer[1] = cmd;
    if (write(i2c_fd, buffer, 2) != 2)
    {
        printf("i2c write failed!\n");
    }
}
void ssd1306_Init(int i2c_fd)
{
    ssd1306_command(i2c_fd, 0xA8); //Set Mux Ratio
    ssd1306_command(i2c_fd, 0x3f);
    ssd1306_command(i2c_fd, 0xD3); //Set Display Offset
    ssd1306_command(i2c_fd, 0x00);
    ssd1306_command(i2c_fd, 0x40); //Set Display Start Line
    ssd1306_command(i2c_fd, 0xA0); //Set Segment re-map
                                   //0xA1 for vertical inversion
    ssd1306_command(i2c_fd, 0xC0); //Set COM Output Scan Direction
                                   //0xC8 for horizontal inversion
    ssd1306_command(i2c_fd, 0xDA); //Set COM Pins hardware configuration
    ssd1306_command(i2c_fd, 0x12); //Manual says 0x2, but 0x12 is required
    ssd1306_command(i2c_fd, 0x81); //Set Contrast Control
    ssd1306_command(i2c_fd, 0x7F); //0:min, 0xFF:max
    ssd1306_command(i2c_fd, 0xA4); //Disable Entire Display On
    ssd1306_command(i2c_fd, 0xA6); //Set Normal Display
    ssd1306_command(i2c_fd, 0xD5); //Set Osc Frequency
    ssd1306_command(i2c_fd, 0x80);
    ssd1306_command(i2c_fd, 0x8D); //Enable charge pump regulator
    ssd1306_command(i2c_fd, 0x14);
    ssd1306_command(i2c_fd, 0xAF); //Display ON
}
void ssd1306_data(int i2c_fd, const uint8_t *data, size_t size)
{
    uint8_t *buffer = (uint8_t *)malloc(size + 1);
    buffer[0] = (0 << 7) | (1 << 6); //Co = 0 , D/C# = 1
    memcpy(buffer + 1, data, size);
    if (write(i2c_fd, buffer, size + 1) != size + 1)
    {
        printf("i2c write failed!\n");
    }
    free(buffer);
}

void update_full(int i2c_fd, uint8_t *data)
{
    ssd1306_command(i2c_fd, 0x20); //addressing mode
    ssd1306_command(i2c_fd, 0x0);  //horizontal addressing mode
    ssd1306_command(i2c_fd, 0x21); //set column start/end address
    ssd1306_command(i2c_fd, 0);
    ssd1306_command(i2c_fd, S_WIDTH - 1);
    ssd1306_command(i2c_fd, 0x22); //set page start/end address
    ssd1306_command(i2c_fd, 0);
    ssd1306_command(i2c_fd, S_PAGES - 1);
    ssd1306_data(i2c_fd, data, S_WIDTH * S_PAGES);
}

void update_area(int i2c_fd, const uint8_t *data, int x, int y, int x_len, int y_len)
{
    ssd1306_command(i2c_fd, 0x20); //addressing mode
    ssd1306_command(i2c_fd, 0x0);  //horizontal addressing mode
    ssd1306_command(i2c_fd, 0x21); //set column start/end address
    ssd1306_command(i2c_fd, x);
    ssd1306_command(i2c_fd, x + x_len - 1);
    ssd1306_command(i2c_fd, 0x22); //set page start/end address
    ssd1306_command(i2c_fd, y);
    ssd1306_command(i2c_fd, y + y_len - 1);
    ssd1306_data(i2c_fd, data, x_len * y_len);
}

void update_full_block(int i2c_fd)
{
    int x = MAX_MAP_X / 2;
    int y = MAX_MAP_Y * 4;
    uint8_t *res = (uint8_t *)calloc(y * x, sizeof(uint8_t));
    uint8_t tmp;
    for (int i = 0; i < MAX_MAP_X; i++)
    {
        for (int j = 0; j < MAX_MAP_Y; j++)
        {
            if (map[i][j] == 1)
            {
                if (i % 2 == 0)
                    tmp = 0b00001111;
                else
                    tmp = 0b11110000;
                res[i / 2 * y + j * 4] |= tmp;
                res[i / 2 * y + j * 4 + 1] |= tmp;
                res[i / 2 * y + j * 4 + 2] |= tmp;
                res[i / 2 * y + j * 4 + 3] |= tmp;
            }
        }
    }
    ssd1306_command(i2c_fd, 0x20); //addressing mode
    ssd1306_command(i2c_fd, 0x0);  //horizontal addressing mode
    ssd1306_command(i2c_fd, 0x21); //set column start/end address
    ssd1306_command(i2c_fd, MAP_Y_LOC);
    ssd1306_command(i2c_fd, MAP_Y_LOC + y - 1);
    ssd1306_command(i2c_fd, 0x22); //set page start/end address
    ssd1306_command(i2c_fd, MAP_X_LOC);
    ssd1306_command(i2c_fd, MAP_X_LOC + x - 1);
    ssd1306_data(i2c_fd, res, x * y);
    free(res);
}

void update_curr_block(int i2c_fd, int x, int y, int x_len, int y_len)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (curr[i][j] == 1)
            {
                map[curr_x + i][curr_y + j] = 1;
            }
        }
    }
    if (x % 2 == 1)
    {
        x -= 1;
        x_len += 1;
    }
    if (x_len % 2 == 1)
    {
        x_len += 1;
    }
    int res_x = x_len / 2;
    int res_y = y_len * 4;
    uint8_t *res = (uint8_t *)calloc(res_x * res_y, sizeof(uint8_t));
    uint8_t tmp;
    for (int i = x; i < x + x_len; i++)
    {
        for (int j = y; j < y + y_len; j++)
        {
            if (map[i][j] == 1)
            {
                if (i % 2 == 0)
                    tmp = 0b00001111;
                else
                    tmp = 0b11110000;
                res[(i - x) / 2 * res_y + (j - y) * 4] |= tmp;
                res[(i - x) / 2 * res_y + (j - y) * 4 + 1] |= tmp;
                res[(i - x) / 2 * res_y + (j - y) * 4 + 2] |= tmp;
                res[(i - x) / 2 * res_y + (j - y) * 4 + 3] |= tmp;
            }
            else if(j>=20){
                res[(i - x) / 2 * res_y + (j - y) * 4] |= 0b11111111;
                res[(i - x) / 2 * res_y + (j - y) * 4 + 1] |= 0b11111111;
            }
            else if(i==10){
                res[(i - x) / 2 * res_y + (j - y) * 4] |= 0b00000011;
            }
        }
    }
    ssd1306_command(i2c_fd, 0x20); //addressing mode
    ssd1306_command(i2c_fd, 0x0);  //horizontal addressing mode
    ssd1306_command(i2c_fd, 0x21); //set column start/end address
    ssd1306_command(i2c_fd, MAP_Y_LOC + y * 4);
    ssd1306_command(i2c_fd, MAP_Y_LOC + y * 4 + res_y - 1);
    ssd1306_command(i2c_fd, 0x22); //set page start/end address
    ssd1306_command(i2c_fd, MAP_X_LOC + x / 2);
    ssd1306_command(i2c_fd, MAP_X_LOC + x / 2 + res_x - 1);
    ssd1306_data(i2c_fd, res, res_x * res_y);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (curr[i][j] == 1)
            {
                map[curr_x + i][curr_y + j] = 0;
            }
        }
    }
    free(res);
}

void update_area_x_wrap(int i2c_fd, const uint8_t *data, int x, int y, int x_len, int y_len)
{
    if (x + x_len <= S_WIDTH)
        update_area(i2c_fd, data, x, y, x_len, y_len);
    else
    {
        int part1_len = S_WIDTH - x;
        int part2_len = x_len - part1_len;
        uint8_t *part1_buf = (uint8_t *)malloc(part1_len * y_len);
        uint8_t *part2_buf = (uint8_t *)malloc(part2_len * y_len);
        for (int x = 0; x < part1_len; x++)
        {
            for (int y = 0; y < y_len; y++)
            {
                part1_buf[part1_len * y + x] = data[x_len * y + x];
            }
        }
        for (int x = 0; x < part2_len; x++)
        {
            for (int y = 0; y < y_len; y++)
            {
                part2_buf[part2_len * y + x] = data[x_len * y + part1_len + x];
            }
        }
        update_area(i2c_fd, part1_buf, x, y, part1_len, y_len);
        update_area(i2c_fd, part2_buf, 0, y, part2_len, y_len);
        free(part1_buf);
        free(part2_buf);
    }
}

int chk_map(int x, int y)
{
    int chk = 1;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (curr[i][j] == 1)
            {
                if (x + i >= MAX_MAP_X || y + j >= MAX_MAP_Y || x + i < 0)
                {
                    chk = 0;
                    break;
                }
                else if (map[x + i][y + j] == 1)
                {
                    chk = 0;
                    break;
                }
            }
        }
    }
    return chk;
}

int chk_line()
{
    int cnt = 0;
    for(int j = 0 ; j < MAX_MAP_Y; j++){
        int chk = 1;
        for(int i = 0 ; i < MAX_MAP_X; i++){
            if(map[i][j] == 0)chk = 0;
        }
        if(chk == 1){
            for(int y = j; y>0;y--){
                for(int x = 0; x< MAX_MAP_X; x++){
                    map[x][y] = map[x][y-1];  
                }
            }
            cnt++;
        }
    }
    return cnt;
}

void new_block()
{
    curr_type = rand() % 7;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            curr[i][j] = blocks[curr_type][0][i][j];
        }
    }
    curr_x = 3;
    curr_y = 0;
    if (chk_map(curr_x, curr_y) == 0)
        is_over = 1;
    else
    {
        update_curr_block(i2c_fd, curr_x, curr_y, 4, 4);
    }
}

void down()
{
    if (chk_map(curr_x, curr_y + 1))
    {
        curr_y++;
        update_curr_block(i2c_fd, curr_x, curr_y - 1, 4, 5);
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (curr[i][j] == 1)
                    map[curr_x + i][curr_y + j] = 1;
            }
        }
        int line = chk_line();
        update_full_block(i2c_fd);
        printf("%d\n",line);
        new_block();
    }
}

void move(int val)
{
    if (chk_map(curr_x + val, curr_y))
    {
        curr_x += val;
        if (val > 0)
            update_curr_block(i2c_fd, curr_x - 1, curr_y, 5, 4);
        else
            update_curr_block(i2c_fd, curr_x, curr_y, 5, 4);
    }
}
void intro()
{
}

void main_menu()
{
}
void gameover()
{
    printf("Game_Over\n");
    memset(map, 0, sizeof(map));
    update_full(i2c_fd, frame);
}
int main()
{
    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0)
    {
        printf("err opening device\n");
        return -1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_I2C_DEV) < 0)
    {
        printf("err setting i2c slave address\n");
        return -1;
    }
    ssd1306_Init(i2c_fd);
    reset = (uint8_t *)calloc(S_WIDTH * S_PAGES, sizeof(uint8_t));
    frame = (uint8_t *)calloc(S_WIDTH * S_PAGES, sizeof(uint8_t));
    for (int i = 0; i < S_WIDTH * S_PAGES; i++)
    {
        if (i < S_WIDTH && i % S_WIDTH >= 40 && i % S_WIDTH <= 121)
            frame[i] = 0b11000000;
        else if (i >= 6 * S_WIDTH && i < S_WIDTH * 7 && i % S_WIDTH >= 40 && i % S_WIDTH <= 121)
            frame[i] = 0b00000011;
        else if ((i % S_WIDTH == 120 || i % S_WIDTH == 121) && i >= S_WIDTH && i < S_WIDTH * 6)
            frame[i] = 0b11111111;
    }
    update_full(i2c_fd, frame);
    intro();
    while (1)
    {
        main_menu();
        srand((unsigned int)time(NULL));
        is_over = 0;
        new_block();
        while (!is_over)
        {
            usleep(300000);
            down();
        }
        gameover();
    }
    close(i2c_fd);
    return 0;
}