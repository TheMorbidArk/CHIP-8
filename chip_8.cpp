
#include "chip_8.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

// CHIP-8 字体
unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

/**
 * 初始化CHIP-8系统
 */
void chip_8::init() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Clear display
    for (unsigned char &i: gfx)
        i = 0;

    // Clear stack
    for (unsigned short &i: stack)
        i = 0;

    for (int i = 0; i < 16; ++i)
        key[i] = V[i] = 0;

    // Clear memory
    for (unsigned char &i: memory)
        i = 0;


    // Load fontset
    for (int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    // Clear screen once
    drawFlag = true;

    //重置随机数种子
    srand(time(nullptr));
}

/**
 * CHIP-8 解释器模拟周期
 */
void chip_8::emulateCycle() {
    //获取操作码
    opcode = memory[pc] << 8 | memory[pc + 1];
    //解析操作码
    switch (opcode & 0xF000) {
        case 0x0000: // 00NN 系列操作码
            switch (opcode & 0x00FF) {
                case 0x00E0: // 0x00E0: 清除屏幕
                    for (unsigned char &i: gfx)
                        i = 0x0;
                    drawFlag = true;

                    pc += 2;
                    break;

                case 0x00EE: // 0x00EE: 从子程序返回
                    --sp;            // 16层堆栈，下移堆栈指针，以防止覆盖
                    pc = stack[sp];    // 将堆栈中存储的返回地址放进程序计数器
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }

        case 0x1000: // 1NNN: 跳转到地址 NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000: // 2NNN: 在 NNN 调用子程序
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000: // 3XNN: 如果 VX 等于 NN，则跳过下一条指令
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        case 0x4000: // 0x4XNN: 如果 VX 不等于 NN，则跳过下一条指令
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        case 0x5000: // 0x5XY0: 如果 VX 等于 VY，则跳过下一条指令
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        case 0x6000: // 0x6VNN: 将 VX 设置为 NN
            V[(opcode & 0xF00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        case 0x7000: // 0x7XNN: 将 NN 添加到 VX (进位标志不变)
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        case 0x8000: // 8NNN 系列操作码
            switch (opcode & 0x000F) {
                case 0x0000: // 0x8XY0: 将 VX 设置为 VY 的值
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0001: // 0x8XY1: 将 VX 设置为 ( VX | VY ) 的值
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0002: // 0x8XY2: 将 VX 设置为 ( VX & VY ) 的值
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0003: // 0x8XY3: 将 VX 设置为 ( VX ^ VY ) 的值
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0004: // 0x8XY4: 将 VY 添加到 VX , VF 有进位时设为 1 ,没有进位时设为 0
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0005: // 0x8XY5: 将 VY 减去 VX , VF 有进位时设为 1 ,没有进位时设为 0
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0006: // 0x8XY6: 将 VX 的最高有效位存储在 VF 中,然后将 VX 向左移动 1
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                case 0x0007: // 0x8XY7: 将 VX 设置为 VY 减去 VX , VF 有借位时设为 0,没有借位时设为 1
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])    // VY-VX
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x000E: // 0x8XYE: 将 VX 的最高有效位存储在 VF 中,然后将 VX 向左移动 1
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
            break;
        case 0x9000: // 0x9XY0: 如果 VX 不等于 VY，则跳过下一条指令
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        case 0xA000: // ANNN: 将 I 设置为地址 NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000: // BNNN: 跳转到地址 NNN + V0
            pc = (opcode & 0x0FFF) + V[0];
            break;

        case 0xC000: // CXNN: 将 VX 设置为对随机数(0~255)和 NN 进行按位和运算的结果
            V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
            pc += 2;
            break;

        case 0xD000: // DXYN: 在坐标 (VX, VY) 处绘制一个宽度为 8 像素，高度为 N 像素的精灵
            /**
             * 从内存位置 I 开始，每行 8 个像素被读取为位编码
             * I 值在执行该指令后不会改变
             * 如果任何屏幕像素在绘制精灵时从设置翻转为未设置，则 VF 设置为 1
             * 如果没有发生，则设置为 0
             */
        {
            // X , Y , H
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;

            unsigned short pixel;   //8位像素编码

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) { // 判断 pixel 的值是否符合要求
                        if (gfx[(x + xline + ((y + yline) * 64))] == 1) { // 检查(x,y)处的像素是否为 1
                            V[0xF] = 1;
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;    //更新屏幕
            pc += 2;
        }
            break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: // EX9E: 如果按下存储在 VX 中的键,则跳过下一条指令
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;

                case 0x00A1: // EXA1: 如果未按下 VX 中存储的键,则跳过下一条指令
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // FX07: 将 VX 设置为延迟计时器的值
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;

                case 0x000A: { // FX0A: 等待按键,然后存储在 VX 中
                    bool keyPress = false;

                    for (int i = 0; i < 16; ++i) {
                        if (key[i] != 0) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            keyPress = true;
                        }
                    }

                    // 如果没有收到一个按键,跳过这个循环,然后再试一次
                    if (!keyPress)
                        return;

                    pc += 2;
                }
                    break;

                case 0x0015: // FX15: 将延迟计时器设置为 VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0018: // FX18: 将声音计时器设置为 VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x001E: // FX1E: I += VX
                    if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)    // (I+VX>0xFFF) 时 VF 设置为 1 ,否则为 0
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0029: // FX29: 将 I 设置为 VX 中角色的精灵位置
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                case 0x0033: // FX33: 存储 VX 的二进制编码十进制表示，最高有效位在 I 中的地址的三位数字，中间位在 I 加 1，最低有效位在 I 加 2
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055: // FX55: 将 V0 到 VX（包括 VX）存储在内存中，从地址 I 开始。对于每个写入的值，从 I 的偏移量增加 1，但 I 本身保持不变
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];

                    // 指令执行结束后, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                case 0x0065: // FX65: 用内存中的值从 V0 填充到 VX（包括 VX），从地址 I 开始。对于每个读取的值，从 I 的偏移量增加 1，但 I 本身保持不变
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        V[i] = memory[I + i];

                    // 指令执行结束后, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    // 更新计时器
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

/**
 * 在终端测试CHIP-8模拟器功能是否正常
 */
void chip_8::debugRender() {
    // Draw in Terminal
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (gfx[(y * 64) + x] == 0)
                printf("O");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * 加载ROM文件
 * @param filename -> ROM包名
 * @return bool -> 是否加载完成
 */
bool chip_8::loadApplication(const char *filename) {
    init();
    printf("Loading: %s\n", filename);

    // 读取文件
    FILE *pFile = fopen(filename, "rb");
    if (pFile == nullptr) {
        fputs("File error", stderr);
        return false;
    }

    // 查看文件大小
    fseek(pFile, 0, SEEK_END);    // 将文件指针移至文件末尾
    long lSize = ftell(pFile);  // 读取文件指针位置
    rewind(pFile);  // 重置文件指针位置
    printf("Filesize: %d\n", (int) lSize);

    // 分配内存 -> buffer
    char *buffer = (char *) malloc(sizeof(char) * lSize);
    if (buffer == nullptr) {
        fputs("Memory error", stderr);
        return false;
    }

    // 将文件数据 -> buffer:size_t
    size_t result = fread(buffer, 1, lSize, pFile);
    if (result != lSize) {
        fputs("Reading error", stderr);
        return false;
    }

    // buffer -> memory[0x200-0xFFF]
    if ((4096 - 512) > lSize) {
        for (int i = 0; i < lSize; ++i)
            memory[i + 512] = buffer[i];
    } else
        printf("Error: ROM too big for memory");

    // 释放内存 pFile buffer
    fclose(pFile);
    free(buffer);

    return true;
}
