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
#include <pthread.h>
#include <math.h>
#include <ctype.h>

#define SSD1306_I2C_DEV 0x3C
#define S_WIDTH 128
#define S_HEIGHT 64
#define S_PAGES (S_HEIGHT / 8)
#define FONT_WIDTH 13
#define FONT_HEIGHT 1
#define SCORE_X_LOC 14
#define SCORE_Y_LOC 3
#define FONT_X_LOC 0
#define FONT_Y_LOC 0
#define MAX_MAP_X 10
#define MAX_MAP_Y 20
#define MAP_X_LOC 1
#define MAP_Y_LOC 40
#define LV_X_LOC 14
#define LV_Y_LOC 0
#define MINI_WIDTH 8
#define MINI_HEIGHT 1
#define ITEM_X_LOC 28
#define ITEM_Y_LOC 0

int map[MAX_MAP_X][MAX_MAP_Y];
int curr[4][4];
int curr_x = 0, curr_y = 0;
int curr_type, curr_rot;
int next_type = -1;
int i2c_fd;
int gpio_fd;
int init = 0;
int is_over = 1;
int is_ani = 0;
int is_name = 0;
int is_sel = 0;
int level = 1;
int speed = 3000000;
int cur_i1 = 1;
int cur_i2 = 1;
int is_combo = 0;
int key_mode = 0;
int tot_line = 0;
int score = 0;
int menu_num = 0;
uint8_t *reset;
uint8_t *frame;
char pname[4];

typedef struct rk
{
    int s;
    char *name;
} Rank;

pthread_mutex_t mutex_lock;

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

void make_string(int *list, int n, char *s)
{
    int tmp = n - 1;
    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == ' ')
            list[tmp--] = 62;
        else if (s[i] == ':')
            list[tmp--] = 63;
        else if (s[i] == '=')
            list[tmp--] = 64;
        else if (s[i] == '#')
            list[tmp--] = 65;
        else if (s[i] >= '0' && s[i] <= '9')
            list[tmp--] = s[i] - '0';
        else if (isupper(s[i]) == 0)
            list[tmp--] = s[i] - 61;
        else
            list[tmp--] = s[i] - 55;
    }
}

void update_string(int i2c_fd, char *s, int x, int y)
{
    int n = strlen(s);
    int *list = (int *)calloc(n, sizeof(int));
    make_string(list, n, s);
    int8_t *stringdata = (int8_t *)calloc(FONT_WIDTH * FONT_HEIGHT * n, sizeof(int8_t));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < FONT_WIDTH; j++)
        {
            stringdata[i * FONT_WIDTH + j] = font[FONT_WIDTH * list[i] + j];
        }
    }
    update_area(i2c_fd, stringdata, x, y, FONT_WIDTH, FONT_HEIGHT * n);
    free(list);
    free(stringdata);
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

void update_frame(int i2c_fd)
{
    int8_t *tmp = (int8_t *)malloc(sizeof(int8_t) * 100);
    memset(tmp, 0b11000000, 100);
    update_area(i2c_fd, tmp, 40, 0, 82, 1);
    memset(tmp, 0b00000011, 100);
    update_area(i2c_fd, tmp, 40, 6, 82, 1);
    memset(tmp, 0b11111111, 100);
    update_area(i2c_fd, tmp, 120, 1, 2, 5);
    free(tmp);
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
    if (x % 2 == 1 || x % 2 == -1)
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
            if (i >= 0 && j >= 0 && i < MAX_MAP_X && j < MAX_MAP_Y && map[i][j] == 1)
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
    update_frame(i2c_fd);
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
void reset_slow(int n)
{
    for (int i = 0; i < S_WIDTH; i += 8)
    {
        for (int j = 0; j < S_PAGES; j++)
        {
            update_area(i2c_fd, reset, i, j, 8, 1);
            usleep(n);
        }
    }
}

void print_info()
{
    int tmp = score;
    char list[6] = "00000";
    char lv[3] = "00";
    lv[0] = (level / 10) + '0';
    lv[1] = (level % 10) + '0';
    for (int i = 4; i >= 0; i--)
    {
        list[i] = tmp % 10 + '0';
        tmp /= 10;
    }
    int cnt = 0;
    update_string(i2c_fd, list, SCORE_X_LOC, SCORE_Y_LOC);
    update_string(i2c_fd, "score lv", FONT_X_LOC, FONT_Y_LOC);
    update_string(i2c_fd, lv, LV_X_LOC, LV_Y_LOC);
    uint8_t *itemdata = (uint8_t *)calloc(8 * MINI_HEIGHT * MINI_WIDTH, sizeof(uint8_t));
    int item[8] = {12, 12, 12, 12, 11, 12, 12, 10}; // tetrimino : 13~
    item[6] = cur_i1;
    item[3] = cur_i2;
    item[1] = 13 + next_type;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < MINI_WIDTH; j++)
        {
            itemdata[i * MINI_WIDTH + j] = mini[MINI_WIDTH * item[i] + j];
        }
    }
    update_area(i2c_fd, itemdata, ITEM_X_LOC, ITEM_Y_LOC, MINI_WIDTH, 8 * MINI_HEIGHT);
    free(itemdata);
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
                if (x + i >= MAX_MAP_X || y + j >= MAX_MAP_Y || x + i < 0 || y + j < 0)
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
    for (int j = 0; j < MAX_MAP_Y; j++)
    {
        int chk = 1;
        for (int i = 0; i < MAX_MAP_X; i++)
        {
            if (map[i][j] == 0)
                chk = 0;
        }
        if (chk == 1)
        {
            for (int y = j; y > 0; y--)
            {
                for (int x = 0; x < MAX_MAP_X; x++)
                {
                    map[x][y] = map[x][y - 1];
                }
            }
            cnt++;
        }
    }
    return cnt;
}

void new_block()
{
    if (next_type == -1)
        next_type = rand() % 7;
    curr_type = next_type;
    curr_rot = 0;
    next_type = rand() % 7;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            curr[i][j] = blocks[curr_type][curr_rot][i][j];
        }
    }
    curr_x = 3;
    curr_y = -1;
    if (chk_map(curr_x, curr_y) == 0)
        is_over = 1;
    else
    {
        update_curr_block(i2c_fd, curr_x, curr_y, 4, 4);
        print_info();
    }
}

void block_finish()
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
    if (line > 0)
    {
        double calc = pow(2, line);
        if (is_combo)
            calc *= 1.5;
        calc *= 1 + 0.2 * cur_i1 + 0.2 * cur_i2;
        score += (int)calc;
        print_info();
    }
    is_combo = (line > 0) ? 1 : 0;
    update_full_block(i2c_fd);
    tot_line += line;
    if (tot_line >= (level + 1) * 5)
    {
        tot_line -= (level + 1) * 5;
        level++;
        speed = (int)(speed * 0.9);
        if (level % 5 == 0)
        {
            cur_i1++;
            cur_i2++;
        }
    }
    new_block();
}

void down()
{
    if (is_over)
        return;
    if (chk_map(curr_x, curr_y + 1))
    {
        curr_y++;
        update_curr_block(i2c_fd, curr_x, curr_y - 1, 4, 5);
    }
    else
        block_finish();
}

void move(int val)
{
    if (is_over)
        return;
    if (chk_map(curr_x + val, curr_y))
    {
        curr_x += val;
        if (val > 0)
            update_curr_block(i2c_fd, curr_x - 1, curr_y, 5, 4);
        else
            update_curr_block(i2c_fd, curr_x, curr_y, 5, 4);
    }
}

void rotation(int val)
{
    if (is_over)
        return;
    int rot = curr_rot + val;
    if (rot < 0)
        rot += 4;
    else if (rot >= 4)
        rot -= 4;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            curr[i][j] = blocks[curr_type][rot][i][j];
        }
    }
    if (chk_map(curr_x, curr_y))
    {
        curr_rot = rot;
        update_curr_block(i2c_fd, curr_x, curr_y, 4, 4);
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                curr[i][j] = blocks[curr_type][curr_rot][i][j];
            }
        }
    }
}

void drop()
{
    if (is_over)
        return;
    int tmp = curr_y;
    while (chk_map(curr_x, curr_y + 1))
    {
        curr_y++;
    }
    update_curr_block(i2c_fd, curr_x, tmp, 4, 4 + (curr_y - tmp));
    usleep(100000);
    block_finish();
}

void item_1() // 아래 4줄 삭제
{
    if (cur_i1 > 0)
    {
        cur_i1--;
        print_info();
        is_ani = 1;
        for (int i = 0; i < MAX_MAP_X; i++)
        {
            for (int j = MAX_MAP_Y - 4; j < MAX_MAP_Y; j++)
            {
                map[i][j] = 0;
            }
            update_curr_block(i2c_fd, i, MAX_MAP_Y - 4, 1, 4);
            usleep(100000);
        }
        for (int i = 0; i < MAX_MAP_X; i++)
        {
            for (int j = MAX_MAP_Y - 1; j >= 4; j--)
            {
                map[i][j] = map[i][j - 4];
            }
        }
        update_curr_block(i2c_fd, 0, 0, MAX_MAP_X, MAX_MAP_Y);
        is_ani = 0;
    }
}

void item_2() // 왼쪽 아래 정렬
{
    if (cur_i2 > 0)
    {
        cur_i2--;
        print_info();
        is_ani = 1;
        for (int i = MAX_MAP_Y - 1; i >= 0; i--)
        {
            int cnt = 0;
            for (int j = 0; j < MAX_MAP_X; j++)
            {
                if (map[j][i] == 1)
                    cnt++;
            }
            for (int j = MAX_MAP_X - 1; j >= 0; j--)
            {
                if (cnt > 0)
                {
                    map[j][i] = 1;
                    cnt--;
                }
                else
                    map[j][i] = 0;
            }
            update_curr_block(i2c_fd, 0, i, MAX_MAP_X, 1);
            usleep(30000);
        }
        for (int i = MAX_MAP_X - 1; i>=0; i--)
        {
            int cnt = 0;
            for (int j = 0; j < MAX_MAP_Y; j++)
            {
                if (map[i][j] == 1)
                    cnt++;
            }
            for (int j = MAX_MAP_Y - 1; j >= 0; j--)
            {
                if (cnt > 0)
                {
                    map[i][j] = 1;
                    cnt--;
                }
                else
                    map[i][j] = 0;
            }
            update_curr_block(i2c_fd, i, 0, 1, MAX_MAP_Y);
            usleep(30000);
        }
        is_ani = 0;
    }
}

void intro()
{
    is_ani = 1;
    int8_t *introdata = (int8_t *)calloc(S_PAGES * S_WIDTH, sizeof(int8_t));
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int x = 0; x < S_PAGES; x++)
            {
                for (int y = 0; y < S_WIDTH; y++)
                {
                    if (x <= j || (x >= S_PAGES - j - 1))
                    {
                        if (y < (i + 1) * 16)
                        {
                            introdata[x * S_WIDTH + y] = intro_img[x * S_WIDTH + y];
                            continue;
                        }
                    }
                }
            }
            update_full(i2c_fd, introdata);
            usleep(50000);
        }
    }
    free(introdata);
    reset_slow(30000);
    is_ani = 0;
}

void menu_up()
{
    if (!is_sel)
    {
        if (menu_num > 0)
        {
            update_string(i2c_fd, " ", (menu_num + 1) * 20, 6);
            menu_num--;
            update_string(i2c_fd, "#", (menu_num + 1) * 20, 6);
        }
    }
}

void menu_down()
{
    if (!is_sel)
    {
        if (menu_num < 2)
        {
            update_string(i2c_fd, " ", (menu_num + 1) * 20, 6);
            menu_num++;
            update_string(i2c_fd, "#", (menu_num + 1) * 20, 6);
        }
    }
}

void menu_select()
{
    is_sel = 1;
}

void menu_back()
{
    is_sel = 0;
}

void score_board()
{

    update_full(i2c_fd, reset);
    if (access("rank.txt", F_OK) == -1)
    {
        update_string(i2c_fd, "NO", 50, 3);
        update_string(i2c_fd, "DATA", 70, 2);
    }
    else
    {
        FILE *fd = fopen("rank.txt", "r");
        int cnt = 0;
        int tmp = fscanf(fd, "%d", &tmp);
        while (cnt < 9)
        {
            int score;
            char name[4];
            fscanf(fd, "%s %d", name, &score);
            if (score == 0)
            {
                break;
            }
            char tmp[6] = "";
            char list[6] = "";
            int index = 0;
            while (score > 0)
            {
                tmp[index++] = score % 10 + '0';
                score /= 10;
            }
            int i = 0;
            index--;
            while(index>=0)
            {
                list[i++] = tmp[index--];
            }
            update_string(i2c_fd, name, cnt * 14, 5);
            update_string(i2c_fd, list, cnt * 14, 0);
            cnt++;
        }
        fclose(fd);
    }
    is_ani = 0;
    while (is_sel)
    {
    }
    is_ani = 1;
}

void main_menu()
{
    is_ani = 1;
    key_mode = 0;
    menu_num = 0;
    is_sel = 0;
    update_full(i2c_fd, reset);
    update_string(i2c_fd, "Start", 20, 1);
    update_string(i2c_fd, "Score", 40, 1);
    update_string(i2c_fd, " Exit", 60, 1);
    update_string(i2c_fd, "==Keys==", 90, 0);
    update_string(i2c_fd, "U D S B ", 108, 0);
    update_string(i2c_fd, "#", 20, 6);
    is_ani = 0;
    while (!is_sel)
    {
    }
    is_ani = 1;
    if (menu_num == 1)
    {
        score_board();
        main_menu();
        return;
    }
    else if (menu_num == 2)
    {
        update_full(i2c_fd, reset);
        update_string(i2c_fd, "THANKS", 40, 1);
        update_string(i2c_fd, "FOR", 60, 3);
        update_string(i2c_fd, "PLAYING", 80, 1);
        usleep(2000000);
        update_full(i2c_fd, reset);
        close(i2c_fd);
        exit(0);
    }
    is_ani = 0;
}

void gameover()
{
    is_ani = 1;
    printf("Game_Over\n");
    memset(map, 0, sizeof(map));
    reset_slow(5000);
    usleep(500000);
    update_string(i2c_fd, "GAME", 50, 2);
    update_string(i2c_fd, "OVER", 70, 2);
    usleep(2000000);
    is_ani = 0;
}

int comp(const void *a, const void *b)
{
    Rank *x = (Rank *)a;
    Rank *y = (Rank *)b;
    if (x->s > y->s)
        return -1;
    else if (x->s == y->s)
        return 0;
    else
        return 1;
}
void add_rank()
{
    update_full(i2c_fd, reset);
    update_string(i2c_fd, "Your", 20, 4);
    update_string(i2c_fd, "Name:", 35, 3);
    update_string(i2c_fd, "==Keys==", 75, 0);
    update_string(i2c_fd, "RESET OK", 90, 0);
    update_string(i2c_fd, " ABCDEF ", 108, 0);
    key_mode = 2;
    is_name = 0;
    strcpy(pname, "");
    is_ani = 0;
    while (!is_name)
    {
        char tmp[4];
        strcpy(tmp, pname);
        int len = strlen(tmp);
        for (int i = 0; i < 3 - len; i++)
        {
            strcat(tmp, " ");
        }
        update_string(i2c_fd, tmp, 35, 0);
        usleep(100000);
    }
    key_mode = 0;
    is_ani = 1;
}
void rank()
{
    is_ani = 1;
    if(score == 0){
        update_full(i2c_fd, reset);
        is_ani = 0;
        return;
    }
    if (access("rank.txt", F_OK) == -1)
    {
        creat("rank.txt", 0644);
    }
    FILE *fd = fopen("rank.txt", "r+");
    Rank *r = (Rank *)malloc(sizeof(Rank) * 9);
    for (int i = 0; i < 9; i++)
    {
        r[i].s = 0;
        r[i].name = (char *)malloc(sizeof(char) * 4);
        strcpy(r[i].name, "XXX");
    }
    int min = 0;
    fscanf(fd, "%d", &min);
    for (int i = 0; i < 9; i++)
    {
        fscanf(fd, "%s %d", r[i].name, &r[i].s);
    }
    fflush(fd);
    fseek(fd, 0, SEEK_SET);
    if (min < score)
    {
        add_rank();
        r[8].s = score;
        strcpy(r[8].name, pname);
        qsort(r, 9, sizeof(Rank), comp);
        min = r[8].s;
    }
    fprintf(fd, "%d ", min);
    for (int i = 0; i < 9; i++)
    {
        fprintf(fd, "%s %d ", r[i].name, r[i].s);
    }
    fclose(fd);
    free(r);
    update_full(i2c_fd, reset);
    is_ani = 0;
}

void *detect_irq(void *arg)
{
    int *buf = (int *)calloc(10, sizeof(int));
    char cur;
    while (1)
    {
        if (read(gpio_fd, buf, 1) > 0)
        {
            cur = 0;
            if (buf[0] != 0 && is_ani == 0)
                cur = buf[0];
            if (key_mode == 1)
            {
                switch (cur)
                {
                case '0':
                    pthread_mutex_lock(&mutex_lock);
                    item_1();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '1':
                    pthread_mutex_lock(&mutex_lock);
                    item_2();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '2':
                    pthread_mutex_lock(&mutex_lock);
                    rotation(-1);
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '3':
                    pthread_mutex_lock(&mutex_lock);
                    rotation(1);
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '4':
                    pthread_mutex_lock(&mutex_lock);
                    drop();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '5':
                    pthread_mutex_lock(&mutex_lock);
                    down();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '6':
                    pthread_mutex_lock(&mutex_lock);
                    move(1);
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '7':
                    pthread_mutex_lock(&mutex_lock);
                    move(-1);
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                }
            }
            else if (key_mode == 2)
            {
                switch (cur)
                {
                case '0':
                    strcpy(pname, "");
                    break;
                case '1':
                    is_name = 1;
                    break;
                case '2':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "E");
                    }
                    break;
                case '3':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "F");
                    }
                    break;
                case '4':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "A");
                    }
                    break;
                case '5':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "C");
                    }
                    break;
                case '6':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "B");
                    }
                    break;
                case '7':
                    if (strlen(pname) < 3)
                    {
                        strcat(pname, "D");
                    }
                    break;
                }
            }
            else if (key_mode == 0)
            {
                switch (cur)
                {
                case '4':
                    pthread_mutex_lock(&mutex_lock);
                    menu_up();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '5':
                    pthread_mutex_lock(&mutex_lock);
                    menu_select();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '6':
                    pthread_mutex_lock(&mutex_lock);
                    menu_down();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                case '7':
                    pthread_mutex_lock(&mutex_lock);
                    menu_back();
                    pthread_mutex_unlock(&mutex_lock);
                    break;
                }
            }
        }
        usleep(10000);
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
    gpio_fd = open("/dev/rpigpio", O_RDWR);
    if (gpio_fd < 0)
    {
        printf("err opening gpio\n");
        return -1;
    }
    write(gpio_fd, "r", 1);
    ssd1306_Init(i2c_fd);
    pthread_mutex_init(&mutex_lock, NULL);
    pthread_t th;
    pthread_create(&th, NULL, detect_irq, NULL);
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
    update_full(i2c_fd, reset);
    intro();
    while (1)
    {
        main_menu();
        srand((unsigned int)time(NULL));
        is_over = 0;
        cur_i1 = 1;
        cur_i2 = 1;
        level = 1;
        tot_line = 0;
        score = 0;
        speed = 3000000;
        next_type = -1;
        key_mode = 1;
        update_full(i2c_fd, frame);
        print_info();
        printf("New Game Start!\n");
        new_block();
        while (!is_over)
        {
            usleep(speed);
            pthread_mutex_lock(&mutex_lock);
            down();
            pthread_mutex_unlock(&mutex_lock);
        }
        pthread_mutex_lock(&mutex_lock);
        gameover();
        pthread_mutex_unlock(&mutex_lock);
        rank();
    }
    close(i2c_fd);
    return 0;
}