/**
 * Author: Liam Mulshine
 * 
 * Main code for embedded heart rate monitor project.
 *
 */

/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>


/*********************
 *	
 *	Pin configuration 
 *
 *	create MOD_PIN, and configure it as an output, default low
 *
 *
 */
// create modulation pin, portA pin 8 (for testing purposes.. change to PA14 for embedded HR monitor) 
#define MOD_PIN IOPORT_CREATE_PIN (IOPORT_PORTA,14)

void configure_pins (void)
{
	// see http://asf.atmel.com/docs/3.0.1/xmega.drivers.dma.unit_tests.xmega_a3bu_xplained/html/ioport_quickstart_use_case_1.html for setting entire ports
	
	// needed to configure ioport pins
	ioport_init();
	
	// set MOD_PIN as output, default low
	ioport_set_pin_dir(MOD_PIN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(MOD_PIN,0);
}

/****************************
 *
 *	GCLK Initialization
 *
 *	Configure GCLK_GENERATOR_1 for use by TC3 to generate the modulation frequency
 *
 */
void configure_gclk(void)
{
	// create gclk struct and get default parameters
	struct system_gclk_gen_config modultn_clk;
	system_gclk_gen_get_config_defaults(&modultn_clk);
	
	// set modulation frequency to 32,000/(division factor)
	modultn_clk.source_clock = SYSTEM_CLOCK_SOURCE_OSC32K;
	modultn_clk.division_factor = 128;
	system_gclk_gen_set_config(GCLK_GENERATOR_1,&modultn_clk);
	
	system_gclk_gen_enable(GCLK_GENERATOR_1);	
}

/*********************
 *
 *  Timer counter initialization
 *	
 *	Timer counter is used to generate modulation signal
 *
 */ 

// create timer counter module struct
struct tc_module modultn_counter;

// timer counter callback: toggles modulation pin
void tc1_callback(struct tc_module *const modultn_counter)
{

	ioport_toggle_pin_level(MOD_PIN);
	
}

// timer counter configuration function
void configure_tc (void)
{
	struct tc_config tc_config_struct;
	tc_get_config_defaults(&tc_config_struct);
	
	tc_config_struct.counter_size = TC_COUNTER_SIZE_8BIT;
	tc_config_struct.clock_source = GCLK_GENERATOR_1;
	tc_config_struct.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	tc_config_struct.counter_8_bit.period = 0xFF;
	tc_config_struct.counter_8_bit.compare_capture_channel[0] = 10;
	 
	// initialize and enable tc instance on TC3 hardware with previously defined settings
	tc_init(&modultn_counter,TC3,&tc_config_struct);
	tc_enable(&modultn_counter);
}
void configure_tc_callbacks(void)
{
	tc_register_callback(&modultn_counter,tc1_callback,TC_CALLBACK_CC_CHANNEL0);
	
	tc_enable_callback(&modultn_counter,TC_CALLBACK_CC_CHANNEL0);
}


int main (void)
{
	// Initialize system and necessary peripherals
	system_init();
	configure_gclk();
	configure_pins();
	configure_tc(); 
	configure_tc_callbacks();
	while(true)
	{
	}
}
