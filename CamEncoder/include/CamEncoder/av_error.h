/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#pragma once

#include "av_ffmpeg.h"
#include <string>

static std::string av_error_to_string(int errnum)
{
    char x[1024] = {0};
    int ret = av_strerror(errnum, x, 1023);
    if (ret < 0)
        throw std::runtime_error("av_strerror failed");
    return x;
}

static std::string av_timestamp_to_string(int64_t ts)
{
    char x[AV_TS_MAX_STRING_SIZE] = {0};
    av_ts_make_string(x, ts);
    return x;
}

static std::string av_timestamp_to_timestring(int64_t ts, AVRational *tb)
{
    char x[AV_TS_MAX_STRING_SIZE] = {0};
    av_ts_make_time_string(x, ts, tb);
    return x;
}