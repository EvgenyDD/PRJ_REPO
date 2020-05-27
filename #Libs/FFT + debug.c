
  int32_t mul256(int32_t x) { return x << 8; }
  int32_t mul240(int32_t x) { return (x << 8) - (x << 4); }
  int32_t mul208(int32_t x) { return (x << 7) + (x << 6) + (x << 4); }
  int32_t mul176(int32_t x) { return (x << 7) + (x << 5) + (x << 4); }
  int32_t mul144(int32_t x) { return (x << 7) + (x << 4); }
  int32_t mul96(int32_t x)  { return (x << 6) + (x << 5); }
  int32_t mul48(int32_t x)  { return (x << 5) + (x << 4); }
  
  typedef int32_t (*fmul32)(int32_t);
  const fmul32 fmulVec[4] = { mul96, mul176, mul240, mul256 };
  
  // calculate FFT[10] for 32 samples
  uint8_t fft10() {
    int32_t a = 0;
    for (uint8_t i = 0; i < 4; ++i) {
      a += fmulVec[i](sampleAccA[i + 1]);
    }


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Пара слов про отладку
При возникновении ошибок работы ядра ARM вызывается исключение HardFault. Контекст, характеризующий параметры ошибки, сохраняется в стеке и в регистрах ядра Cortex. При отладке программы мы будем нередко попадать в HardFault, поэтому, чтобы упростить восприятие отладочной информации, обязательно нужно преобразовать эту самую информацию в удобный вид. Для этого переопределяем обработчик прерывания HardFault_Handler

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile
    (
        "TST LR, #4             \n"
        "ITE EQ                 \n"
        "MRSEQ R0, MSP          \n"
        "MRSNE R0, PSP          \n"
        "B hard_fault_handler_c \n"
    );
}


и пишем функцию hard_fault_handler_c (см. модуль exceptions.c):

void hard_fault_handler_c (uint32_t * hardfault_args)
{
    volatile uint32_t stacked_r0 __attribute__((unused));
    volatile uint32_t stacked_r1 __attribute__((unused));
    volatile uint32_t stacked_r2 __attribute__((unused));
    volatile uint32_t stacked_r3 __attribute__((unused));
    volatile uint32_t stacked_r12 __attribute__((unused));
    volatile uint32_t stacked_lr __attribute__((unused));
    volatile uint32_t stacked_pc __attribute__((unused));
    volatile uint32_t stacked_psr __attribute__((unused));

    // Bus Fault Address Register
    volatile uint32_t BFAR __attribute__((unused));
    // Configurable Fault Status Register Consists of MMSR, BFSR and UFSR
    volatile uint32_t CFSR __attribute__((unused));  
    // Hard Fault Status Register
    volatile uint32_t HFSR __attribute__((unused));     
    // Debug Fault Status Register
    volatile uint32_t DFSR __attribute__((unused));     
    // Auxiliary Fault Status Register
    volatile uint32_t AFSR __attribute__((unused));     
    // MemManage Fault Address Register
    volatile uint32_t MMAR __attribute__((unused));   

    volatile uint32_t SCB_SHCSR __attribute__((unused));

    stacked_r0 = hardfault_args[0];
    stacked_r1 = hardfault_args[1];
    stacked_r2 = hardfault_args[2];
    stacked_r3 = hardfault_args[3];
    stacked_r12 = hardfault_args[4];
    stacked_lr = hardfault_args[5];
    stacked_pc = hardfault_args[6];
    stacked_psr = hardfault_args[7];

    BFAR = (*((volatile unsigned long *)(0xE000ED38)));
    CFSR = (*((volatile unsigned long *)(0xE000ED28)));
    HFSR = (*((volatile unsigned long *)(0xE000ED2C)));
    DFSR = (*((volatile unsigned long *)(0xE000ED30)));
    AFSR = (*((volatile unsigned long *)(0xE000ED3C)));
    MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    SCB_SHCSR = SCB->SHCSR;

    DBG_HALT(0);
    while (1);
}


Теперь при возникновении исключения все отладочные параметры будут помещены в отладочные переменные, которые вполне комфортно можно просмотреть в отладчике IDE. При наличии интерфейса dbgu можно вывести соответствующее сообщение в терминальное окно.

Более подробно про анализ исключений следует почитать этот аппноут и документацию на официальном сайте ARM.

Также рекомендую к применению макрос

#define DBG_HALT(numb) \
    do { \
        if (CoreDebug->DHCSR & 1) \
        { \
            __asm( "BKPT %0\n"::"M"(numb) ); \
        } \
    } while(0)


Он позволяет остановить выполнение программы и передать управление отладчику, если микроконтроллер запущен из-под отладчика. Использование данного макроса в обработчиках ошибок (например, в функции assert_failed) позволяет максимально быстро реагировать на возникающие неполадки программы. И, что очень важно, не тратятся дефицитные аппаратные брейкпоинты Кортекса. Значение numb ни к чему не обязывает, это просто маркер — число, которое может быть считано отладчиком для дальнейшего сопоставления брейкпоинта с исходниками.