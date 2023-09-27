/*
 * Copyright (c) Alex2772, https://github.com/alex2772
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <cstdint>
#include "adc.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "config.h"
#include "app.h"
#include <glm/glm.hpp>

extern "C" ADC_HandleTypeDef hadc1;


namespace {

    unsigned gAdcFiringTick = 0;

    struct {
        std::int32_t coil = 0;
        std::int32_t bat = 0;
        std::int32_t shunt = 0;
    } gAdcRaw;

    struct {
        float coil = 0;
        float bat = 0;
        float shunt = 0;
    } gAdc;

    struct {
        float batteryVoltage = 0.f;
        float current = 0.f;
        float coilResistance = 0.f;
        int batteryLevel;
    } gProcessedData;


    unsigned gShutter = config::SHUTTER_BOOT;

    bool isAdcDataReady() {
        return app::fireMosfet() > 0 && gAdcFiringTick > (gShutter * 10);
    }
}

/**
  * @brief  Regular conversion half DMA transfer callback in non blocking mode
  * @param  hadc pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

    using ivec = glm::vec<sizeof(gAdcRaw) / sizeof(gAdcRaw.coil), decltype(gAdcRaw.coil)>;
    using fvec = glm::vec<sizeof(gAdc) / sizeof(gAdc.coil), decltype(gAdc.coil)>;
    auto& processed = reinterpret_cast<fvec&>(gAdc);
    auto& unprocessed = reinterpret_cast<ivec&>(gAdcRaw);

    processed += (fvec(unprocessed) - processed) * 0.1f;

    if (app::fireMosfet() > 0) {
        gAdcFiringTick += 1;
    } else {
        gAdcFiringTick = 0;
    }


    gProcessedData.batteryVoltage = adc::batteryVoltage();

#if AT_DUDKA_REV >= 2
    gProcessedData.current = glm::max(config::calibrated::SHUNT_CURRENT(gAdc.shunt - gAdc.coil), 0.f);
    gProcessedData.coilResistance = glm::max((gProcessedData.batteryVoltage - adc::shuntVoltage()) / gProcessedData.current , 0.f);
#else
    gProcessedData.current = glm::max(config::calibrated::SHUNT_CURRENT(gProcessedData.batteryVoltage- adc::shuntVoltage()), 0.f);
    gProcessedData.coilResistance = glm::max((adc::shuntVoltage() - adc::coilVoltage()) / gProcessedData.current , 0.f);
#endif
}

void adc::init() {
    auto r = HAL_ADC_Start_DMA(&hadc1, reinterpret_cast<uint32_t*>(&gAdcRaw), sizeof(gAdcRaw) / sizeof(gAdcRaw.coil));
    assert(r == HAL_OK);
}

float adc::batteryVoltage() {
    return glm::max(0.f, config::calibrated::BATTERY_VOLTAGE(gAdc.bat));
}

float adc::shuntVoltage() {
    return glm::max(0.f, config::calibrated::SHUNT_VOLTAGE(gAdc.shunt));
}

float adc::coilVoltage() {
    return glm::max(0.f, config::calibrated::COIL_VOLTAGE(gAdc.coil));
}

float adc::current() {
    return gProcessedData.current;
}

std::optional<float> adc::coilResistance() {
    if (gProcessedData.coilResistance < 2.f && isAdcDataReady()) { // inf check
        return gProcessedData.coilResistance;
    }
    return std::nullopt;
}

adc::CalibrationData adc::calibrationData() {
    return { int(gAdc.bat), int(gAdc.shunt), int(gAdc.coil) };
}

void adc::setShutter(unsigned s) {
    gShutter = s;
}
