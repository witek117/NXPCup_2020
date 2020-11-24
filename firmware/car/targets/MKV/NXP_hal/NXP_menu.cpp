/**
 * Copyright (c) Kolo Naukowe Elektronikow, Akademia Gorniczo-Hutnicza im. Stanislawa Staszica w Krakowie 2020
 * Authors: Arkadiusz Balys, Kamil Kasperczyk, Witold Lukasik
 *
 * NXP HALina implementation of menu
 *
*/

#define LOG_CHANNEL MENU

#include "NXP_Kitty.hpp"
#include "logger.h"

void NXP_Menu::buttonHandler() {}

void NXP_Menu::init() {
    buttons.init();
    switches.init();
    stateMachine.handle(fsm::StartMenu{});
    display.print(0);
}

bool NXP_Menu::proc(bool &systickTrigger){
    if(isMenuRunning) {
        display.update();
        if (!buttons.at(0).get()) {
            if (systickTrigger) {
                if (buttonCompare <= buttonCounter++) {
                    stateMachine.handle(fsm::ChangeParameterUp{});
                    buttonCounter = 0;
                    buttonCompare /= 1.1;
                }
                systickTrigger = false;
            }
        } else if (!buttons.at(1).get()) {
            if (systickTrigger) {
                if (buttonCompare <= buttonCounter++) {
                    stateMachine.handle(fsm::ValueDown{});
                    buttonCounter = 0;
                    buttonCompare /= 1.1;
                }
                systickTrigger = false;
            }
        } else if (!buttons.at(2).get()) {
            if (systickTrigger) {
                if (buttonCompare <= buttonCounter++) {
                    stateMachine.handle(fsm::ValueUp{});
                    buttonCounter = 0;
                    buttonCompare /= 1.1;
                }
                systickTrigger = false;
            }
        } else if (!buttons.at(3).get()) {
            stateMachine.handle(fsm::StartRace{});
        } else {buttonCompare = defaultDelay;}
    }
    return isMenuRunning;
}

namespace fsm {
    void Idle::on_entry(const StartMenu &) const {
        menu.display.print(0);
        menu.stateMachine.handle(ChangeParameterUp{});
    }

    void Idle::on_entry(const EmergencyBreak &) const {
        Kitty::kitty().servo.disable();
        Kitty::kitty().motors.block();
        menu.isMenuRunning = true;
        menu.display.print(0);
        menu.display.enable();
    }

    void Parameters::on_entry(ChangeParameterUp const&) {
        parametersCounter++;
        if((uint8_t)parametersCounter >= menu.parameters.parameters.size()){
            parametersCounter = 0;
        }
        menu.display.print(parametersCounter);
        log_debug("Current parameter: %d", parametersCounter);
    }

    void Parameters::on_entry(ValueUp const&) const {
        log_debug("Parameters::on_entry::ValueUp");
        if((uint8_t)parametersCounter < menu.parameters.parameters.size()){
            if(auto pval = get_if<uint32_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval)))); }
            if(auto pval = get_if<uint16_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<uint8_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<float*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(*(*pval), 2);}
            if(auto pval = get_if<int32_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int16_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int8_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) += menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
        }else{
            log_debug("wrong type!");
        }
    }

    void Parameters::on_entry(ValueDown const&) const {
        log_debug("Parameters::on_entry::ValueDown");
        if((uint8_t)parametersCounter < menu.parameters.parameters.size()){
            if(auto pval = get_if<uint32_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval)))); }
            if(auto pval = get_if<uint16_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<uint8_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<float*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(*(*pval), 2); log_debug("%f", *(*pval));}
            if(auto pval = get_if<int32_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int16_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int8_t*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
            if(auto pval = get_if<int*>(&menu.parameters.parameters[parametersCounter])){ (*(*pval)) -= menu.parameters.divider[parametersCounter]; menu.display.print(int((*(*pval))));}
        }else{
            log_debug("wrong type!");
        }
    }

    void Race::on_entry(const StartRace &) const {
        log_debug("Race::on_entry::StartRace");
        menu.isMenuRunning = false;
        Kitty::kitty().motors.run();
        menu.display.disable();
    }
}