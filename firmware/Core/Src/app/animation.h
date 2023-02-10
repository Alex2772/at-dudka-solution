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

#include <glm/glm.hpp>
#include "glm/ext/scalar_constants.hpp"

namespace animation {
    inline float linear(float v) {
        return v;
    }

    inline float in(float v) {
        return glm::sin(v * glm::pi<float>() / 2.f);
    }
}

template<int frames, int from, int to, float(*curve)(float) = animation::linear>
class ValueAnimator {
public:

    float nextFrame() {
        auto x = float(mCurrentFrame) / float(frames);
        mCurrentFrame += mPlayDirection;
        if (mCurrentFrame < 0) {
            mCurrentFrame = 0;
            pause();
        } else if (mCurrentFrame > frames) {
            mCurrentFrame = frames;
            pause();
        }

        return curve(x) * (to - from) + from;
    }

    void play() {
        mPlayDirection = 1;
    }

    void playReverse() {
        mPlayDirection = -1;
    }

    void pause() {
        mPlayDirection = 0;
    }

    bool isPaused() {
        return mPlayDirection == 0;
    }

    int currentFrame() {
        return mPlayDirection;
    }

private:
    int mCurrentFrame = 0;
    int mPlayDirection = 0;
};
