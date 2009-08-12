/*
 * This file is part of libmonome.
 * libmonome is copyright 2007, 2008 will light <visinin@gmail.com>
 *
 * libmonome is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *
 */

/**
 * test.c
 * basic program to test all of the output commands to the monome.
 */

#include <time.h>
#include <monome.h>

#define DEFAULT_MONOME_DEVICE "/dev/ttyUSB0"
#define PROTOCOL "series"

#define BPM 75

static void chill(int speed) {
	struct timespec rem, req = {0, ((60000 / (BPM * speed)) * 1000000)};
	nanosleep(&req, &rem);
}

void test_led_on_off(monome_t *monome) {
	unsigned int i, j;
	
	for( i = 0; i < 16; i++ )
		for( j = 0; j < 16; j++ ) {
			monome_led_on(monome, j, i);
			chill(128);
		}

	for( i = 0; i < 16; i++ )
		for( j = 0; j < 16; j++ ) {
			monome_led_off(monome, j, i);
			chill(128);
		}
}

void test_led_row(monome_t *monome) {
	unsigned int i, buf_off[2], buf_on[2];
	
	buf_off[0] = buf_off[1] = 0x00;
	buf_on[0]  = buf_on[1]  = 0xFF;
	
	for( i = 0; i < 16; i++ ) {
		monome_led_row_16(monome, i, buf_on);
		chill(16);
		monome_led_row_16(monome, i, buf_off);
	}
}

void test_led_col(monome_t *monome) {
	unsigned int i, buf_off[2], buf_on[2];
	
	buf_off[0] = buf_off[1] = 0x00;
	buf_on[0]  = buf_on[1]  = 0xFF;
	
	for( i = 0; i < 16; i++ ) {
		monome_led_col_16(monome, i, buf_on);
		chill(16);
		monome_led_col_16(monome, i, buf_off);
	}
}

void test_led_frame(monome_t *monome) {
	unsigned int pat[2], buf[8], i, j, k, l;
	
	pat[0] = 0x55;
	pat[1] = 0xAA;
	
	k = 1;
	
	for( l = 0; l < 8; l++ ) {
		for( i = 0; i < 8; i++ )
			buf[i] = pat[(i+k) & 0x1];
		
		for( j = 0; j < 4; j++ )
			monome_led_frame(monome, j, buf);
		
		k = !k;
		
		chill(8);
	}
}

void fade_out(monome_t *monome) {
	unsigned int i = 0x10;
	
	while( i-- ) {
		monome_intensity(monome, i);
		chill(16);
	}
}

int main(int argc, char **argv) {
	monome_t *monome;
	int i;
	
	if( !(monome = monome_open((argc == 2 ) ? argv[1] : DEFAULT_MONOME_DEVICE, PROTOCOL)) )
		return -1;
	
	monome_clear(monome, MONOME_CLEAR_OFF);
	
	for( i = 0; i < 2; i++ ) {
		test_led_row(monome);
		test_led_col(monome);
	}
	
	test_led_on_off(monome);
	test_led_frame(monome);
	
	chill(2);
	fade_out(monome);
	
	monome_clear(monome, MONOME_CLEAR_OFF);
	monome_intensity(monome, 0x0F);
	
	return 0;
}
