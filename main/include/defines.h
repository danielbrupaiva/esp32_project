#pragma once
#define DEBUG 1
// digital outputs
#define DIGITAL_OUTPUT_0 GPIO_NUM_26
#define DIGITAL_OUTPUT_1 GPIO_NUM_27
#define GPIO_OUTPUT_PIN_SEL ((1ULL << DIGITAL_OUTPUT_0) | (1ULL << DIGITAL_OUTPUT_1))

// digital inputs
#define DIGITAL_INPUT_0 GPIO_NUM_35
#define DIGITAL_INPUT_1 GPIO_NUM_34
#define GPIO_INPUT_PIN_SEL ((1ULL << DIGITAL_INPUT_0) | (1ULL << DIGITAL_INPUT_1))

// analog inputs
#define ADC1_CH_0 ADC1_CHANNEL_5
