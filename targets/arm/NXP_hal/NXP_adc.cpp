/**
 * Copyright (c) Kolo Naukowe Elektronikow, Akademia Gorniczo-Hutnicza im. Stanislawa Staszica w Krakowie 2020
 * Authors: Arkadiusz Balys, Kamil Kasperczyk, Witold Lukasik
 *
 * NXP HALina implementation of adc
 *
 */

#include <drivers/fsl_hsadc.h>
#include "NXP_adc.hpp"

volatile bool isCalibFinished = false;
static volatile uint32_t hsadc0CurrentValue = 0;
static volatile uint32_t hsadc1CurrentValue = 0;

void NXP_ADC::init() {
    hsadc_config_t  hsadcConfig;
    hsadc_converter_config_t  converterConfig;
    hsadc_sample_config_t sampleConfig;

    hsadcConfig.enableSimultaneousMode = false;

    sampleConfig.channelNumber = channel;
    sampleConfig.channel67MuxNumber = mux;
    sampleConfig.enableDifferentialPair = enableDifferentialPair;

    HSADC_GetDefaultConfig(&hsadcConfig);
    HSADC_GetDefaultConverterConfig(&converterConfig);
    HSADC_SetConverterConfig(adc, (_hsadc_converter_id)converterType, &converterConfig);

    HSADC_Init(adc, &hsadcConfig);

    HSADC_EnableConverter(adc, (_hsadc_converter_id)converterType, true);
    HSADC_EnableConverterPower(adc, (_hsadc_converter_id)converterType, true);
    while (( kHSADC_ConverterBPowerDownFlag) == ((kHSADC_ConverterBPowerDownFlag) & HSADC_GetStatusFlags(adc))){;}

    for (uint8_t i = 0; i < 16; i++) {
        HSADC_SetSampleConfig(adc, i, &sampleConfig);
        HSADC_EnableSample(adc, HSADC_SAMPLE_MASK(i), true);
    }

    enableInterrupts();
}

int32_t NXP_ADC::getValue(){
    if(adc == HSADC0){
        currentValue = hsadc0CurrentValue;
    }else if(adc == HSADC1){
        currentValue = hsadc1CurrentValue;
    }
    return currentValue;
}

void  NXP_ADC::startConversion() {
    HSADC_DoSoftwareTriggerConverter(adc, (_hsadc_converter_id)converterType);
}

void NXP_ADC::autoCalibration() {
    HSADC_DoAutoCalibration(adc, ( kHSADC_ConverterB),(kHSADC_CalibrationModeSingleEnded));
    uint16_t interruptType = 0;
    if(converterType == Converter::CONVERTER_A){
        interruptType = kHSADC_ConverterAEndOfCalibrationInterruptEnable;
    } else if(converterType == Converter::CONVERTER_B){
        interruptType = kHSADC_ConverterBEndOfCalibrationInterruptEnable;
    }
    HSADC_EnableInterrupts(adc, interruptType);
    while ((kHSADC_ConverterAEndOfCalibrationFlag) != ((kHSADC_ConverterAEndOfCalibrationFlag) & HSADC_GetStatusFlags(adc))){;}
    while (!isCalibFinished);
    HSADC_DisableInterrupts(adc, interruptType);
}

void NXP_ADC::enableInterrupts() {
    uint16_t interruptType = 0;
    if(converterType == Converter::CONVERTER_A){
        interruptType = kHSADC_ConverterAEndOfScanInterruptEnable;
    } else if(converterType == Converter::CONVERTER_B){
        interruptType = kHSADC_ConverterBEndOfScanInterruptEnable;
    }

    HSADC_EnableInterrupts(adc, interruptType);

    if(adc == HSADC0) {
        if (converterType == Converter::CONVERTER_A) {
            NVIC_ClearPendingIRQ(HSADC0_CCA_IRQn);
            NVIC_EnableIRQ(HSADC0_CCA_IRQn);
        } else if (converterType == Converter::CONVERTER_B) {
            NVIC_ClearPendingIRQ(HSADC0_CCB_IRQn);
            NVIC_EnableIRQ(HSADC0_CCB_IRQn);
        }
    }else if(adc == HSADC1){
        if(converterType == Converter::CONVERTER_A) {
            NVIC_ClearPendingIRQ(HSADC1_CCA_IRQn);
            NVIC_EnableIRQ(HSADC1_CCA_IRQn);
        }else if(converterType == Converter::CONVERTER_B){
            NVIC_ClearPendingIRQ(HSADC1_CCB_IRQn);
            NVIC_EnableIRQ(HSADC1_CCB_IRQn);
        }
    }
}

void NXP_ADC::disableInterrupts() {
    uint16_t interruptType = 0;
    if(converterType == Converter::CONVERTER_A){
        interruptType = kHSADC_ConverterAEndOfScanInterruptEnable;
    } else if(converterType == Converter::CONVERTER_B){
        interruptType = kHSADC_ConverterBEndOfScanInterruptEnable;
    }

    HSADC_DisableInterrupts(adc, interruptType);

    if(adc == HSADC0) {
        if (converterType == Converter::CONVERTER_A) {
            NVIC_ClearPendingIRQ(HSADC0_CCA_IRQn);
            NVIC_DisableIRQ(HSADC0_CCA_IRQn);
        } else if (converterType == Converter::CONVERTER_B) {
            NVIC_ClearPendingIRQ(HSADC0_CCB_IRQn);
            NVIC_DisableIRQ(HSADC0_CCB_IRQn);
        }
    }else if (adc == HSADC1) {
        if (converterType == Converter::CONVERTER_A) {
            NVIC_ClearPendingIRQ(HSADC1_CCA_IRQn);
            NVIC_DisableIRQ(HSADC1_CCA_IRQn);
        } else if (converterType == Converter::CONVERTER_B) {
            NVIC_ClearPendingIRQ(HSADC1_CCB_IRQn);
            NVIC_DisableIRQ(HSADC1_CCB_IRQn);
        }

    }
}

extern "C" {
void HSADC_ERR_IRQHandler() {
    HSADC0->STAT |= HSADC_STAT_EOCALIA_MASK;
    isCalibFinished = true;
}

void HSADC0_CCB_IRQHandler() {
    HSADC0->STAT |= HSADC_STAT_EOSIA_MASK;
    HSADC0->STAT |= HSADC_STAT_EOSIB_MASK;

    for (auto i = 0; i < 16; i++) {
        hsadc0CurrentValue += (HSADC_GetSampleResultValue(HSADC0, i) >> 3U) & 0xFFF;
    }
}

void HSADC1_CCA_IRQHandler(){
    HSADC1->STAT |= HSADC_STAT_EOSIA_MASK;
    HSADC1->STAT |= HSADC_STAT_EOSIB_MASK;

    for (auto i = 0; i < 16; i++) {
        hsadc0CurrentValue += (HSADC_GetSampleResultValue(HSADC1, i) >> 3U) & 0xFFF;
    }
}
}