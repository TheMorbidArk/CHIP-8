
#ifndef CHIP_8_CHIP_8_H
#define CHIP_8_CHIP_8_H

class chip_8 {
public:
    bool drawFlag;

    void emulateCycle();
    void debugRender();
    bool loadApplication(const char * filename);


    //CHIP-8
    unsigned char  gfx[64 * 32];	// Chip 8 的图形是黑白的，屏幕共有 2048 个像素
    unsigned char  key[16];         // Chip 8 基于 HEX 的键盘（0x0-0xF）


private:
    unsigned short pc;				// 程序计数器
    unsigned short opcode;			// 指向当前操作码
    unsigned short I;				// 索引寄存器,与涉及内存操作的多个操作码一起使用
    unsigned short sp;				// 栈指针


    /** CPU 寄存器：
     * Chip 8 有 15 个 8 位通用寄存器
     * 分别命名为 V0、V1 到 VE
     * 第 16 个寄存器用于“进位标志”
     */
    unsigned char  V[16];			// V-寄存器 (V0-VF)

    /** CHIP-8 系统栈
     * CHIP-8 系统有 16 层堆栈
     */
    unsigned short stack[16];		// 栈 (16 levels)

    /** 系统内存映射 memory[4096] 4K
     * 0x000-0x1FF -芯片8解释器(包含emu中的字体集)
     * 0x050-0x0A0 -用于内置4x5像素的字体集(0-F)
     * 0x200-0xFFF -程序ROM和工作RAM
     */
    unsigned char  memory[4096];	// 内存 (size = 4k)


    /** CHIP-8 定时器寄存器
     * 两个以 60 Hz 计数的定时器寄存器
     * 当设置高于零时，它们将倒计时至零
     * 每当声音计时器达到零时，系统的蜂鸣器就会响起
     */
    unsigned char  delay_timer;		// 延滞定时器
    unsigned char  sound_timer;		// 声音计时器


    void init();

};


#endif //CHIP_8_CHIP_8_H
