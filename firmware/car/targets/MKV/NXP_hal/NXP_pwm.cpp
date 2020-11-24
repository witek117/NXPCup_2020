/**
 * Copyright (c) Kolo Naukowe Elektronikow, Akademia Gorniczo-Hutnicza im. Stanislawa Staszica w Krakowie 2020
 * Authors: Arkadiusz Balys, Kamil Kasperczyk, Witold Lukasik
 *
 * NXP HALina implementation of pwm driver
 *
 */
#define LOG_CHANNEL MOTOR

#include <limits>
#include <bits/algorithmfwd.h>
#include "fsl_pwm.h"
#include "NXP_pwm.hpp"
#include "logger.h"

void NXP_PWM::setDutyCycle(float dutyCycle){
    dutyCycle = std::clamp(dutyCycle, 0.0f, 1.0f);
    currentValue = dutyCycle;
    setRawPeriod((uint16_t)(dutyCycle * modulo), channel);
}

float NXP_PWM::getDutyCycle(){
    return currentValue;
}

void NXP_PWM::init() {
    ftm->MODE = (FTM_MODE_FAULTM(0x00) | FTM_MODE_WPDIS_MASK);
        ftm->CONTROLS[channel].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
        port.setMux();

    ftm->SC = FTM_SC_CLKS(0x01); // System clock
    uint32_t period = CLOCK_GetFreq(kCLOCK_FastPeriphClk) / frequency;

    for (uint8_t i = 0; i < uint8_t(sizeof(dividers)); i++) {
        uint32_t mod = 0;
        mod = period / dividers[i];
        if (mod < std::numeric_limits<uint16_t>::max()) {
            dividerIndex = i;
            modulo = mod;
            break;
        }
    }

    if (ftm == FTM0) {
        SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
    } else if (ftm == FTM1) {
        SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
    } else if (ftm == FTM2) {
        SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
    } else if (ftm == FTM3) {
        SIM->SCGC6 |= SIM_SCGC6_FTM3_MASK;
    }
    ftm->SC |= FTM_SC_PS(dividerIndex);
    ftm->CNT = 0;
    ftm->MOD = FTM_MOD_MOD(modulo);

    setDutyCycle(0.0f);

}
