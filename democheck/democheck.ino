#include<tm4c123gh6pm.h>
void PWMsig(void);

void wait(void)
{
    GPTMTAMATCHR=0x1E84800; // Match register value loaded for 2 sec delay
    GPTMCTL=0x1; //Timer enabled
}

int setup(void)
{
    GPTMCFG=0x0; //Configuration Register
    GPTMTAMR=0x1; //Setting One-Shot mode
    GPTMTAR=0x0; //Setting count-up mode
    //Match Value given for 2 sec delay
    GPIODIRF=0x0;//PF0 configured as input
    GPIODENF=0x1;//PF configured as digital io
    int x, mask=0x1;
        RCGC0=0x00100000;
        GPIOAFSEL=0x00010000; //enable alternate function (pwm) on PB6
        GPIOPCTL=0x100;//assign the PWM signals to the PB6(pin 1)
        PWM0CTL=0x00000000;
        PWM0GENA=0x8C;
        PWM0GENB=0x80C;//for count-down mode with immediate updates
        PWM0LOAD=0x18F;//period loaded for a 25-KHz frequency
        PWM0CMPA=0x12B;//pulse width of the MnPWM0 pin for 25% duty cycle
        PWM0CMPB=0x63;//pulse width of the MnPWM0 pin for 75% duty cycle

  while(1)
  {
      wait();
      x=GPIOPINF&mask;
      if(x==1)
          PWMsig();//Start pump motor (PWM output)
      else
          PWMENABLE=0x0;//disable PWM output
  }
  
  }
  return 0;
}

void PWMsig(void)
{

    PWM0CTL=0x1;//Start the timer in PWM generator
    PWMENABLE=0x3;//Enable PWM outputs
}




