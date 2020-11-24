/**
 * Copyright (c) Kolo Naukowe Elektronikow, Akademia Gorniczo-Hutnicza im. Stanislawa Staszica w Krakowie 2020
 * Authors: Arkadiusz Balys, Kamil Kasperczyk, Witold Lukasik
 *
 *
 *
 */

#include "track_lines_detector.hpp"

#define LOG_CHANNEL TRACK

#include "logger.h"

void TrackLinesDetector::detect(int16_t const* cameraData){
    if(cameraData != nullptr){
        findLine(LineType::LEFT, cameraData);
        findLine(LineType::RIGHT, cameraData);

        // make sure than found lines are not outside of the window (too close to the camera window edge)
        if(leftLine.leftBorderIndex < lineSearchingMargin){
            leftLine.isDetected = false;
        }
        if(rightLine.rightBorderIndex > cameraDataSize-lineSearchingMargin){
            rightLine.isDetected = false;
        }
    }
}

void TrackLinesDetector::findLine(LineType lineType, int16_t const* data){
    Line* line;
    if(data != nullptr) {
        switch (lineType) {
            case LineType::LEFT:
                line = &leftLine;
                break;
            case LineType::RIGHT:
                line = &rightLine;
                break;
            default:
                break;
        }

        for(auto i = line->leftBorderIndex; i < line->rightBorderIndex; i++){
            if(data[i] != 0) {
                line->isDetected = true;
                line->centerIndex = i;
                lineSearchingWindow = standardLineSearchingWindow;
                return;
            }else{
                line->isDetected = false;
            }
        }
    }
}