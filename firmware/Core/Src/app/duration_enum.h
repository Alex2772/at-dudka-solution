// /*
//  * Copyright (c) Alex2772, https://github.com/alex2772
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  *

#pragma once

#include <chrono>

enum class SleepDuration {
    kNEVER,
    k30S,
    k1MIN,
    k3MIN,
    k5MIN,
    k10MIN,
    k30MIN,
};

template<>
struct enum_traits<SleepDuration> {
    static constexpr SleepDuration values[] = {
            SleepDuration::kNEVER,
            SleepDuration::k30S,
            SleepDuration::k1MIN,
            SleepDuration::k3MIN,
            SleepDuration::k5MIN,
            SleepDuration::k10MIN,
            SleepDuration::k30MIN,
    };
    static const char* name(SleepDuration value) {
        switch (value) {
            case SleepDuration::kNEVER: return "OFF";
            case SleepDuration::k30S: return "30s";
            case SleepDuration::k1MIN: return "1min";
            case SleepDuration::k3MIN: return "3min";
            case SleepDuration::k5MIN: return "5min";
            case SleepDuration::k10MIN:  return "10min";
            case SleepDuration::k30MIN:  return "30min";
        }
    }

    static std::chrono::milliseconds duration(SleepDuration value) {
        using namespace std::chrono_literals;
        switch (value) {
            case SleepDuration::kNEVER:  return 0s;
            case SleepDuration::k30S:    return 30s;
            case SleepDuration::k1MIN:   return 1min;
            case SleepDuration::k3MIN:   return 3min;
            case SleepDuration::k5MIN:   return 5min;
            case SleepDuration::k10MIN:  return 10min;
            case SleepDuration::k30MIN:  return 30min;
        }
    }
};


enum class CooldownDuration {
    k30S,
    k1MIN,
    k3MIN,
    k5MIN,
    k10MIN,
    k30MIN,
    k1HOUR,
    k2HOUR,
    k3HOUR,
    k4HOUR,
    k8HOUR,
    k12HOUR,
    k1D,
};

template<>
struct enum_traits<CooldownDuration> {
    static constexpr CooldownDuration values[] = {
            CooldownDuration::k30S,
            CooldownDuration::k1MIN,
            CooldownDuration::k3MIN,
            CooldownDuration::k5MIN,
            CooldownDuration::k10MIN,
            CooldownDuration::k30MIN,
            CooldownDuration::k1HOUR,
            CooldownDuration::k2HOUR,
            CooldownDuration::k3HOUR,
            CooldownDuration::k4HOUR,
            CooldownDuration::k8HOUR,
            CooldownDuration::k12HOUR,
            CooldownDuration::k1D,
    };
    static const char* name(CooldownDuration value) {
        switch (value) {
            case CooldownDuration::k30S: return "30s";
            case CooldownDuration::k1MIN: return "1min";
            case CooldownDuration::k3MIN: return "3min";
            case CooldownDuration::k5MIN: return "5min";
            case CooldownDuration::k10MIN:  return "10min";
            case CooldownDuration::k30MIN:  return "30min";
            case CooldownDuration::k1HOUR:  return "1h";
            case CooldownDuration::k2HOUR:  return "2h";
            case CooldownDuration::k3HOUR:  return "3h";
            case CooldownDuration::k4HOUR:  return "4h";
            case CooldownDuration::k8HOUR:  return "8h";
            case CooldownDuration::k12HOUR: return "12h";
            case CooldownDuration::k1D:     return "1d";
        }
    }

    static std::chrono::milliseconds duration(CooldownDuration value) {
        using namespace std::chrono_literals;
        switch (value) {
            case CooldownDuration::k30S:    return 30s;
            case CooldownDuration::k1MIN:   return 1min;
            case CooldownDuration::k3MIN:   return 3min;
            case CooldownDuration::k5MIN:   return 5min;
            case CooldownDuration::k10MIN:  return 10min;
            case CooldownDuration::k30MIN:  return 30min;
            case CooldownDuration::k1HOUR:  return 1h;
            case CooldownDuration::k2HOUR:  return 2h;
            case CooldownDuration::k3HOUR:  return 3h;
            case CooldownDuration::k4HOUR:  return 4h;
            case CooldownDuration::k8HOUR:  return 8h;
            case CooldownDuration::k12HOUR: return 12h;
            case CooldownDuration::k1D:     return 24h;
        }
    }
};
