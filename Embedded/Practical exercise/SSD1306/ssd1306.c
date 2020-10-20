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

#define SSD1306_I2C_DEV 0x3C
#define S_WIDTH 128
#define S_HEIGHT 64
#define S_PAGES (S_HEIGHT / 8)
#define CLOCK_Y_LOC 3
#define FONT_Y_LOC 4
#define FONT_X_LOC 46
#define CLOCK_WIDTH 40
#define CLOCK_HEIGHT 5
#define FONT_HEIGHT 3
#define FONT_WIDTH 14
#define MS_X_LOC 119
#define MS_Y_LOC 3
#define MS_HEIGHT 1
#define MS_WIDTH 5
#define MAN_WIDTH 13
#define MAN_HEIGHT 2
#define MAN_MOVE 2

int i2c_fd;
int init =0;
uint8_t *data_clock;
uint8_t *data_man;

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

void handler(int sig)
{
    static int fps_10 = 4;
    static int fps_1 = 49;
    static int clock_i = 1;
    static int sign = 1;
    static int ms = 0;
    static int time[5] = {0, 0, 0, 0, -1};
    static int man_pos = 0;
    static int tmp = (MAN_WIDTH + MAN_MOVE) * MAN_HEIGHT;
    fps_10++;
    fps_1++;
    if ((man_pos / 2) % 3 == 0)
        update_area_x_wrap(i2c_fd, data_man, man_pos, 0, MAN_WIDTH + MAN_MOVE, MAN_HEIGHT);
    else if ((man_pos / 2) % 3 == 1)
        update_area_x_wrap(i2c_fd, data_man + tmp, man_pos, 0, MAN_WIDTH + MAN_MOVE, MAN_HEIGHT);
    else
        update_area_x_wrap(i2c_fd, data_man + tmp + tmp, man_pos, 0, MAN_WIDTH + MAN_MOVE, MAN_HEIGHT);
    man_pos += MAN_MOVE;
    if (man_pos >= S_WIDTH)
        man_pos = 0;
    if (fps_10 >= 5)
    {
        update_area(i2c_fd, data_clock, clock_i, CLOCK_Y_LOC, CLOCK_WIDTH + 4, CLOCK_HEIGHT);
        if (clock_i == 2 || clock_i == 0)
            sign *= -1;
        clock_i += sign;
        int tmp = mini_Descriptors[ms];
        update_area(i2c_fd, mini_Bitmaps + tmp, MS_X_LOC, MS_Y_LOC, MS_WIDTH, MS_HEIGHT);
        ms++;
        if (ms > 9)
            ms = 0;
        fps_10 = 0;
    }
    if (fps_1 >= 50)
    {
        time[4]++;
        if (time[4] >= 10)
        {
            time[4] = 0;
            time[3]++;
        }
        if (time[3] >= 6)
        {
            time[3] = 0;
            time[1]++;
        }
        if (time[1] >= 10)
        {
            time[1] = 0;
            time[0]++;
        }
        if (time[0] >= 6)
            time[0] = 0;
        for (int x = 0; x < 5; x++)
        {
            if (x == 2)
            {
                update_area(i2c_fd, Bitmaps + Descriptors[10], FONT_X_LOC + x * (FONT_WIDTH + 1), FONT_Y_LOC, FONT_WIDTH, FONT_HEIGHT);
            }
            else
            {
                int tmp = Descriptors[time[x]];
                update_area(i2c_fd, Bitmaps + tmp, FONT_X_LOC + x * (FONT_WIDTH + 1), FONT_Y_LOC, FONT_WIDTH, FONT_HEIGHT);
            }
        }
        fps_1 = 0;
    }
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
    uint8_t *reset = (uint8_t *)calloc(S_WIDTH * S_PAGES, sizeof(uint8_t));
    for (int x = 0; x < S_WIDTH; x++)
    {
        reset[S_WIDTH * 2 + x] = 0x03;
    }
    update_full(i2c_fd, reset);

    signal(SIGALRM, handler);
    ualarm(20000, 20000);
    data_clock = (uint8_t *)malloc((CLOCK_WIDTH + 4) * CLOCK_HEIGHT);
    for (int y = 0; y < CLOCK_HEIGHT; y++)
    {
        for (int x = 0; x < 2; x++)
            data_clock[(CLOCK_WIDTH + 4) * y + x] = 0x00;
        for (int x = 0; x < CLOCK_WIDTH; x++)
            data_clock[(CLOCK_WIDTH + 4) * y + x + 2] = clock[CLOCK_WIDTH * y + x];
        for (int x = 0; x < 2; x++)
            data_clock[(CLOCK_WIDTH + 4) * y + x + 2 + CLOCK_WIDTH] = 0x00;
    }
    data_man = (uint8_t *)malloc((MAN_WIDTH + MAN_MOVE) * MAN_HEIGHT * 3);
    int tmp = (MAN_WIDTH + MAN_MOVE) * MAN_HEIGHT;
    for (int n = 0; n < 3; n++)
    {
        for (int y = 0; y < MAN_HEIGHT; y++)
        {
            for (int x = 0; x < MAN_MOVE; x++)
                data_man[tmp * n + (MAN_WIDTH + MAN_MOVE) * y + x] = 0x00;
            for (int x = 0; x < MAN_WIDTH; x++)
                data_man[tmp * n + (MAN_WIDTH + MAN_MOVE) * y + x + MAN_MOVE] = man[(MAN_WIDTH * MAN_HEIGHT * n) + MAN_WIDTH * y + x];
        }
    }

    while (1)
    {
        char c;
        scanf(" %c", &c);
        if(c == 'q'){
            close(i2c_fd);
            break;
        }
        sleep(1);
    }

    close(i2c_fd);
    return 0;
}