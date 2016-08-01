// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2013-08-24
//

#ifndef DATETIME_H
#define DATETIME_H
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <sys/time.h>

#include <iostream>

#define MICROSECONDS_PER_SECOND 1000000
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

class TimeTick
{
public:
    time_t m_sec;   // 秒数,取值范围[0x8000000000000000,0x7FFFFFFFFFFFFFFF]
    int m_usec;     // 微秒数,取值范围(-1000000,1000000),正负与time相同
public:

    TimeTick(time_t tm, int us);
    TimeTick(const TimeTick &tt);
    TimeTick();
    static TimeTick Now();
    void Normalize();
    bool IsNormalized()const;

    std::string ToString() const;

    int GetDays()const;
    int GetHours()const;
    int GetMinutes()const;
    int GetSeconds()const;
    int GetMilliSeconds()const;
    int GetMicroSeconds()const;

    double GetTotalDays()const;
    double GetTotalHours()const;
    double GetTotalMinutes()const;
    double GetTotalSeconds()const;
    double GetTotalMilliSeconds()const;
    double GetTotalMicroSeconds()const;

    TimeTick operator+(const TimeTick &tt)const;
    TimeTick &operator+=(const TimeTick &tt);
    TimeTick operator-(const TimeTick &tt)const;
    TimeTick &operator-=(const TimeTick &tt);
    TimeTick operator-()const;

    bool operator>(const TimeTick &tt)const;
    bool operator<(const TimeTick &tt)const;
    bool operator>=(const TimeTick &tt)const;
    bool operator<=(const TimeTick &tt)const;
    bool operator==(const TimeTick &tt)const;
    bool operator!=(const TimeTick &tt)const;
};

class TimeCounter
{
public:
    void Reset();
    void Pause() const;

    std::string ToString() const;
    double GetTotalDays()const;
    double GetTotalHours()const;
    double GetTotalMinutes()const;
    double GetTotalSeconds()const;
    double GetTotalMilliSeconds()const;
    double GetTotalMicroSeconds()const;

private:
    mutable TimeTick m_start;
    mutable TimeTick m_last;
    mutable TimeTick m_span;
};

class DateTime
{
public:
    DateTime();
    DateTime(time_t timestamp);
    std::string ToString(const std::string& format = "") const;
    static DateTime Now();

    friend std::ostream& operator << (std::ostream& os, const DateTime& dt)
    {
        os << dt.ToString();
        return os;
    }

    int GetYear() {
        return m_tm.tm_year+1900;
    }

    int GetMonth(){
        return m_tm.tm_mon+1;
    }

    int GetDay() {
        return m_tm.tm_mday;
    }

    int GetHour() {
        return m_tm.tm_hour;
    }

    int GetMinute() {
        return m_tm.tm_min;
    }

    int GetSecond() {
        return m_tm.tm_sec;
    }

    int GetWeekday() {
        return m_tm.tm_wday;
    }
private:
    time_t m_sec;
    struct tm m_tm;

};

#endif // DATETIME_H
