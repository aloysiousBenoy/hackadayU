#include <Arduino.h>
#define led LED_BUILTIN

#define MEM32(address) (*(volatile uint32_t *)(address))

#define GPIO_OUT_CLR MEM32(SIO_BASE + 0x018)
#define GPIO_OUT_SET MEM32(SIO_BASE + 0x014)
#define GPIO_OUT_XOR MEM32(SIO_BASE + 0x01c)

#define TIMELR MEM32(TIMER_BASE + 0x0c)
#define ALARM1 MEM32(TIMER_BASE + TIMER_ALARM1_OFFSET)
#define ALARM_INTE MEM32(TIMER_BASE + TIMER_INTE_OFFSET)
#define ALARM_ARMED MEM32(TIMER_BASE + TIMER_ARMED_OFFSET)

#define NVIC_ISER MEM32(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
#define VTOR MEM32(PPB_BASE + M0PLUS_VTOR_OFFSET)

void own_delay(uint32_t milli)
{
    // uint32_t start = TIMELR;
    // while (TIMELR - start < milli * 1000)
    //     ;
    ALARM1 = TIMELR + milli * 1000;
    while ((ALARM_ARMED >> 1) & 0x01)
        ;
}
void alarm_isr(void)
{
    ALARM1 = TIMELR + (250000u);
    // toggle led
    GPIO_OUT_XOR = 1 << led;
    // clear intr bit when fired to clear latched interrupt
    // register is WC (write to clear) hence set bit to clear
    MEM32(TIMER_BASE + TIMER_INTR_OFFSET) = 1 << 1u;
}

void blink_non_blocking(uint32_t millis)
{
    Serial1.println("start");
    // set alarm1 to target time
    // clear

    // enable interrupt in timer peripheral
    // set bit #1 in INTE
    // hw_set_bits(&timer_hw->inte, 1 << 1u);
    Serial1.println(ALARM_INTE, BIN);
    ALARM_INTE |= (1 << 1u);
    Serial1.println(ALARM_INTE, BIN);

    // set ISR into vector table
    // VTOR stores start address of vector table
    // *VTOR +(16+1) words => IRQ for timer 1
    // 1 word = 4 bytes
    Serial1.println(MEM32(VTOR + (16 + TIMER_IRQ_1) * 4), HEX);
    MEM32(VTOR + (16 + TIMER_IRQ_1) * 4) = (uint32_t)alarm_isr;
    Serial1.println((uint32_t)alarm_isr, HEX);
    Serial1.println(MEM32(VTOR + (16 + TIMER_IRQ_1) * 4), HEX);

    // enable interrupt in NVIC_ISER
    // (refer to docs: writing 0 has no effect, hence just set the bit, no need to XOR/OR etc.)
    Serial1.println(NVIC_ISER, BIN);
    NVIC_ISER = 1 << TIMER_IRQ_1;
    Serial1.println(NVIC_ISER, BIN);

    // set alarm1 time
    ALARM1 = TIMELR + millis * 1000;
    Serial1.println("start");
}
// void dump_vector_table(){

//     for (int i = 1;i<16+26;i++){
//         Serial1.println(VTOR+(i*4));
//     }
// }

void setup()
{
    Serial1.begin(115200);
    delay(1000);
    Serial1.println("hello");
    pinMode(led, OUTPUT);
    blink_non_blocking(75);
}

void loop()
{
    // digitalWrite(led, HIGH);
    // alarm_isr();
    // GPIO_OUT_SET = 1 << led;
    // own_delay(50);

    // alarm_isr();
    // GPIO_OUT_CLR = 1 << led;
    // sio_hw->gpio_clr = 1 << led;
    // own_delay(500);
}
