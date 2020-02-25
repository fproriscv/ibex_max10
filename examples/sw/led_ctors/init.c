#include <stdint.h>
//#include "platform.h"
#include "ctor_led.c"

extern void __attribute__ ((exception_handler)) 
__attribute__ ((visibility ("default"))) default_exc_handler();

extern void __attribute__ ((exception_handler)) 
__attribute__ ((visibility ("default"))) reset_handler();

extern uint32_t __attribute__ ((constructor)) 
__attribute__ ((visibility ("default"))) ctor_led();

extern void* _stack;
extern void _init();

extern int main(int argc, char** argv);

//__attribute__((__section__(".ctors")))
//void (* const gpio_ctors[])(void)=
//{
//  ctor_led,
//};
/*(* const g_constructors[])(void) = 
* {
*  (void (*) (void)) (&_stack),  // the initial stack
*  reset_handler,                // the reset handler
*  constructor_led               // the led constructor   
* };
*/

// Not sure if two-stage assembly wrapping is needed for handlers.
// https://wiki.osdev.org/Interrupt_Service_Routines
void default_exc_handler(){
  asm(
      "jal x0, default_exc_handler \n"
      );
}

//void __attribute__ ((section(".vectors"))) reset_handler(){
void reset_handler(){

  /* Note: the following routines are transferred from crt0.S */
  asm ( // not sure if the volatile keyword is needed here
        //"reset_handler: \n" This label seems redundant.
        /* set all registers to zero */
        "mv  x1, x0 \n"
        "mv  x2, x1 \n"
        "mv  x3, x1 \n"
        "mv  x4, x1 \n"
        "mv  x5, x1 \n"
        "mv  x6, x1 \n"
        "mv  x7, x1 \n"
        "mv  x8, x1 \n"
        "mv  x9, x1 \n"
        "mv x10, x1 \n"
        "mv x11, x1 \n"
        "mv x12, x1 \n"
        "mv x13, x1 \n"
        "mv x14, x1 \n"
        "mv x15, x1 \n"
        "mv x16, x1 \n"
        "mv x17, x1 \n"
        "mv x18, x1 \n"
        "mv x19, x1 \n"
        "mv x20, x1 \n"
        "mv x21, x1 \n"
        "mv x22, x1 \n"
        "mv x23, x1 \n"
        "mv x24, x1 \n"
        "mv x25, x1 \n"
        "mv x26, x1 \n"
        "mv x27, x1 \n"
        "mv x28, x1 \n"
        "mv x29, x1 \n"
        "mv x30, x1 \n"
        "mv x31, x1 \n"
      );
    // Set the vector table pointer.
  //  asm(
  //   "la x1, 0x00200000 \n"
  //    "la x2, gpio_ctors \n"
  //    "sw x1, 0(x2) \n"
  //    );
}

void __attribute__((__section__(".vectors"))) interrupts(){
  asm(
      //".section .vectors, 'ax' \n"
      //".option norvc; \n"

      // external interrupts are handled by the same callback
      // until compiler supports IRQ routines
      ".org 0x00 \n"
      ".rept 31 \n"
      "nop \n"
      ".endr \n"
      "jal x0, default_exc_handler \n"

      // reset vector
      ".org 0x80 \n"
      "jal x0, reset_handler \n"

      // illegal instruction exception
      ".org 0x84 \n"
      "jal x0, default_exc_handler \n"

      // ecall handler
      ".org 0x88 \n"
      "jal x0, default_exc_handler \n"
      );
}

void _init(){

  //default_exc_handler();
  reset_handler();
  /* Note: the following asm is transferred from crt0.S */
  asm (  /* stack initialization */
        "la   x2, _stack_start \n"

      "_start: \n"
      ".global _start \n"

        /* clear BSS */
        "la x26, _bss_start \n"
        "la x27, _bss_end \n"

        "bge x26, x27, zero_loop_end \n"

      "zero_loop: \n"
        "sw x0, 0(x26) \n"
        "addi x26, x26, 4 \n"
        "ble x26, x27, zero_loop \n"
      "zero_loop_end: \n"
      );

  // Call the application.
  //main(0, 0);
  asm(
      "main_entry: \n"
  /* jump to main program entry point (argc = argv = 0) */
        "addi x10, x0, 0 \n"
        "addi x11, x0, 0 \n"
        "jal x1, main \n"
     );
  interrupts();
}

