#include "system.h"

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();

extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

//用于存放irq对应的处理函数指针
void *irq_routines[16]={
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

void irq_install_handler(int irq, void (*handler)(struct regs *r)){
    irq_routines[irq]=handler;
}

void irq_uninstall_handler(int irq){
    irq_routines[irq]=0;
}

//由于irq 0-7 在idt上的默认映射为8-15，但是我们已经用了 故将0-15重新映射到32-47
//主PCI命令寄存器的地址 0x20
//从PCI命令寄存器的地址 0xA0
void irq_remap(void){
    //初始化控制字 ICW1
    outportb(0x20, 0x11);
    outportb(0xA0,0x11);
    //ICW2
    //将IRQ 0-7路由到INT 20h-27h
    outportb(0x21,0x20);
    //将IRQ 8-15路由到INT 28h-2Fh 
    outportb(0xA1,0x28);

    //ICW3
    outportb(0x21,0x04);
    outportb(0xA1,0x02);

    //ICW4
    outportb(0x21,0x01);
    outportb(0xA1,0x01);

    //*启用IRQ0（计时器）和IRQ1（键盘）
    outportb(0x21,0x0);
    outportb(0xA1,0x0);

    //puts("8259 init ok \n");
}

//将IRQ安装到idt中

void irq_install(){
    irq_remap();

    idt_set_gate(32,(unsigned)irq0,0x08,0x8E);
    idt_set_gate(33,(unsigned)irq1,0x08,0x8E);
    idt_set_gate(34,(unsigned)irq2,0x08,0x8E);
    idt_set_gate(35,(unsigned)irq3,0x08,0x8E);
    idt_set_gate(36,(unsigned)irq4,0x08,0x8E);
    idt_set_gate(37,(unsigned)irq5,0x08,0x8E);
    idt_set_gate(38,(unsigned)irq6,0x08,0x8E);
    idt_set_gate(39,(unsigned)irq7,0x08,0x8E);

    idt_set_gate(40,(unsigned)irq8,0x08,0x8E);
    idt_set_gate(41,(unsigned)irq9,0x08,0x8E);
    idt_set_gate(42,(unsigned)irq10,0x08,0x8E);
    idt_set_gate(43,(unsigned)irq11,0x08,0x8E);
    idt_set_gate(44,(unsigned)irq12,0x08,0x8E);
    idt_set_gate(45,(unsigned)irq13,0x08,0x8E);
    idt_set_gate(46,(unsigned)irq14,0x08,0x8E);
    idt_set_gate(47,(unsigned)irq15,0x08,0x8E);

}

void irq_handler(struct regs *r){
    void(*handler)(struct regs *r);

    handler = irq_routines[r->int_no-32];
    if(handler){
        handler(r);
    }
    //如果是从PIC处理的中断,需要转发
    if(r->int_no>=40){
        outportb(0xA0,0x20);
    }

    //发送EOI，提示中断结束
    outportb(0x20, 0x20);
}