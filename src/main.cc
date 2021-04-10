/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"
#include "synopsys_wei_gpio.h"
#include "hx_drv_tflm.h"
#include "stdio.h"

/*
hx_drv_gpio_config_t hal_led_r;
hx_drv_gpio_config_t hal_led_g;
*/
hx_drv_gpio_config_t hal_gpio_0;
hx_drv_gpio_config_t hal_gpio_1;
hx_drv_gpio_config_t hal_gpio_2;
hx_drv_gpio_config_t hal_led_r;
hx_drv_gpio_config_t hal_led_g;


volatile void delay_ms(uint32_t ms_input);
//void GPIO_INIT(void);
volatile void led_toggle(uint32_t hunmsec_cnt);

char string_buf[100] = "test\n";
#define accel_scale 10

typedef struct
{
	uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;


// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
int main(int argc, char* argv[]) 
{
    uint32_t msec_x100 = 0;
    int32_t int_buf;
	accel_type accel_x, accel_y, accel_z, accel_s;
	uint32_t msec_cnt = 0;
	uint32_t sec_cnt = 0;

    hx_drv_uart_initial(UART_BR_115200);
	if (hx_drv_accelerometer_initial() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Accelerometer Initialize Fail\n");
    setup();
	
	//GPIO_INIT();
    //3.1.1 print T03
    hx_drv_uart_print("T03\n\n");
    while (true) 
    {
		hx_drv_led_on(HX_DRV_LED_GREEN);
        
		uint32_t available_count = 0;
		float x, y, z;
        float gravity_g=0;
		available_count = hx_drv_accelerometer_available_count();		
		for (int i = 0; i < available_count; i++) 
		{
			hx_drv_accelerometer_receive(&x, &y, &z);
		}

        gravity_g = x*x + y*y + z*z;
		int_buf = x * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_x.symbol = '-';
		}
		else 
		{
			accel_x.symbol = '+';
		}
		accel_x.int_part = int_buf / accel_scale;
		accel_x.frac_part = int_buf % accel_scale;


		int_buf = y * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_y.symbol = '-';
		}
		else 
		{
			accel_y.symbol = '+';
		}
        y = int_buf;
		accel_y.int_part = int_buf / accel_scale;
		accel_y.frac_part = int_buf % accel_scale;


		int_buf = z * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_z.symbol = '-';
		}
		else 
		{
			accel_z.symbol = '+';
		}
		accel_z.int_part = int_buf / accel_scale;
		accel_z.frac_part = int_buf % accel_scale;

        int_buf = gravity_g * accel_scale;
        uint32_t gravity_g_int = int_buf / accel_scale;
        uint32_t gravity_g_frac  = int_buf % accel_scale;

		sprintf(string_buf, "X:%c%1ld.%1ld | Y:%c%1ld.%1ld | Z:%c%1ld.%1ld | S:%2ld.%1ld\n", 
				accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
				accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
				accel_z.symbol, accel_z.int_part, accel_z.frac_part,
                gravity_g_int, gravity_g_frac);
		hx_drv_uart_print(string_buf);

		if(gravity_g_int > 3){
			for(int i=0; i<20; i++)
			{
				loop(i);
				delay_ms(500);
			}
			
		}


		//led toggle function 0.1s
		

        msec_x100 = msec_x100 + 1;
        delay_ms(100);
        msec_cnt = msec_cnt + 100;
        sec_cnt = sec_cnt + (msec_cnt / 1000);
        msec_cnt = msec_cnt % 1000;
    }
 }
volatile void delay_ms(uint32_t ms_input)
{
    volatile uint32_t i = 0;
    volatile uint32_t j = 0;

    for(i = 0; i < ms_input; i++)
        for(j = 0; j < 40000; j++);
}

/*
void GPIO_INIT(void)
{
  
  if(hal_gpio_init(&hal_led_r, HX_DRV_LED_RED, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO_LED_RED Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO_LED_RED Initialized: Error\n");

  if(hal_gpio_init(&hal_led_g, HX_DRV_LED_GREEN, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO_LED_GREEN Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO_LED_GREEN Initialized: Error\n");
}
*/
