// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2013-08-24
//

#include "datetime.h"

// *************** TimeTick *************************
TimeTick::TimeTick(time_t tm, int us)
{
    m_sec = tm;
    m_usec = us;
    Normalize();
}

TimeTick::TimeTick(const TimeTick& tt)
{
    m_sec = tt.m_sec;
    m_usec = tt.m_usec;
    Normalize();
}

TimeTick::TimeTick()
{
    struct timeval tvNow;
    gettimeofday(&tvNow, NULL);
    m_sec = tvNow.tv_sec;
    m_usec = tvNow.tv_usec;
    Normalize();
}

TimeTick TimeTick::Now()
{
    TimeTick tt;
    return tt;
}

std::string TimeTick::ToString() const
{
    char buf[30];
    snprintf(buf, 30, "%02d:%02d:%02d::%04d:%04d",
             GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds(), GetMicroSeconds());
    return buf;
}

void TimeTick::Normalize()
{
    if (!IsNormalized())
    {
        if (m_sec >= 0)
        {
            if (m_usec >= 0)
            {
                m_sec += m_usec / MICROSECONDS_PER_SECOND;
                m_usec = m_usec % MICROSECONDS_PER_SECOND;
            }
            else
            {
                m_sec -= (-m_usec) / MICROSECONDS_PER_SECOND;
                m_usec = -((-m_usec) % MICROSECONDS_PER_SECOND);

                if (m_sec > 0 && m_usec < 0)
                {
                    --m_sec;
                    m_usec = MICROSECONDS_PER_SECOND + m_usec;
                }
            }
        }
        else
        {
            if (m_usec >= 0)
            {
                m_sec += m_usec / MICROSECONDS_PER_SECOND;
                m_usec = m_usec % MICROSECONDS_PER_SECOND;

                if (m_sec < 0 && m_usec > 0)
                {
                    ++m_sec;
                    m_usec = m_usec - MICROSECONDS_PER_SECOND;
                }
            }
            else
            {
                m_sec -= (-m_usec) / MICROSECONDS_PER_SECOND;
                m_usec = -((-m_usec) % MICROSECONDS_PER_SECOND);
            }
        }
    }
}

bool TimeTick::IsNormalized() const
{
    return (m_sec >= 0 && m_usec >= 0 && m_usec < MICROSECONDS_PER_SECOND) ||
        (m_sec <= 0 && m_usec <= 0 && m_usec > -MICROSECONDS_PER_SECOND);
}

int TimeTick::GetDays()const
{
    return (int)(m_sec / SECONDS_PER_DAY);
}

int TimeTick::GetHours() const
{
    return (m_sec % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
}

int TimeTick::GetMinutes() const
{
    return int(m_sec % SECONDS_PER_HOUR) / 60;
}

int TimeTick::GetSeconds() const
{
    return (int)(m_sec % 60);
}

int TimeTick::GetMilliSeconds() const
{
    return m_usec / 1000;
}

int TimeTick::GetMicroSeconds() const
{
        return m_usec % 1000;
}

double TimeTick::GetTotalDays() const
{
    return ((double)m_sec + (double)m_usec / MICROSECONDS_PER_SECOND) /
        SECONDS_PER_DAY;
}

double TimeTick::GetTotalHours() const
{
    return ((double)m_sec + (double)m_usec / MICROSECONDS_PER_SECOND) /
        SECONDS_PER_HOUR;
}

double TimeTick::GetTotalMinutes() const
{
    return ((double)m_sec + (double)m_usec / MICROSECONDS_PER_SECOND) / 60;
}

double TimeTick::GetTotalSeconds() const
{
    return (double)m_sec + (double)m_usec / MICROSECONDS_PER_SECOND;
}

double TimeTick::GetTotalMilliSeconds() const
{
    return (double)m_sec * 1000 + (double)m_usec / 1000;
}

double TimeTick::GetTotalMicroSeconds() const
{
    return (double)m_sec * MICROSECONDS_PER_SECOND + (double)m_usec;
}


TimeTick TimeTick::operator+(const TimeTick& tt) const
{
    TimeTick tick;
    tick.m_sec = this->m_sec + tt.m_sec;
    tick.m_usec = this->m_usec + tt.m_usec;
    tick.Normalize();
    return tick;
}

TimeTick TimeTick::operator-(const TimeTick& tt) const
{
    TimeTick tick;
    tick.m_sec = this->m_sec - tt.m_sec;
    tick.m_usec = this->m_usec - tt.m_usec;
    tick.Normalize();
    return tick;
}

TimeTick& TimeTick::operator+=(const TimeTick& tt)
{
    this->m_sec += tt.m_sec;
    this->m_usec += tt.m_usec;
    this->Normalize();
    return *this;
}

TimeTick& TimeTick::operator-=(const TimeTick& tt)
{
    this->m_sec -= tt.m_sec;
    this->m_usec -= tt.m_usec;
    this->Normalize();
    return *this;
}

TimeTick TimeTick::operator-() const
{
    TimeTick tick;
    tick.m_sec = -this->m_sec;
    tick.m_usec = -this->m_usec;
    return tick;
}

bool TimeTick::operator>(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec > tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec > tt.m_usec);
}

bool TimeTick::operator<(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec < tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec < tt.m_usec);
}

bool TimeTick::operator>=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec >= tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec >= tt.m_usec);
}

bool TimeTick::operator<=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec <= tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec <= tt.m_usec);
}

bool TimeTick::operator==(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec == tt.m_sec && this->m_usec == tt.m_usec;
}

bool TimeTick::operator!=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec != tt.m_sec && this->m_usec != tt.m_usec;
}

// *************** TimeCounter *************************
void TimeCounter::Reset()
{
    m_start = TimeTick::Now();
    m_last = TimeTick::Now();
}

void TimeCounter::Pause() const
{
    m_last = TimeTick::Now();
    m_span = m_last - m_start;
}

std::string TimeCounter::ToString() const
{
    Pause();
    return m_span.ToString();
}

double TimeCounter::GetTotalDays() const
{
    return m_span.GetTotalDays();
}

double TimeCounter::GetTotalHours() const
{
    return m_span.GetTotalHours();
}

double TimeCounter::GetTotalMinutes() const
{
    return m_span.GetTotalMinutes();
}

double TimeCounter::GetTotalSeconds() const
{
    return m_span.GetTotalSeconds();
}

double TimeCounter::GetTotalMilliSeconds() const
{
    return m_span.GetTotalMicroSeconds();
}

double TimeCounter::GetTotalMicroSeconds() const
{
    return m_span.GetTotalMicroSeconds();
}

// *************** DateTime *************************
DateTime::DateTime(time_t timestamp)
{
    m_sec = timestamp;
    localtime_r(&m_sec, &m_tm);
}
DateTime::DateTime()
{
    time(&m_sec);
    //gmtime_r(&m_sec, &m_tm);
    localtime_r(&m_sec, &m_tm);
}

DateTime DateTime::Now()
{
    DateTime now;
    return now;
}

std::string DateTime::ToString(const std::string& format) const
{
    char buf[512];
    if (format.empty()) {
        snprintf(buf, 512, "%04d-%02d-%02d %02d:%02d:%02d",
                 1900+m_tm.tm_year,
                 m_tm.tm_mon+1,
                 m_tm.tm_mday,
                 m_tm.tm_hour,
                 m_tm.tm_min,
                 m_tm.tm_sec
                );
    } else {
        strftime(buf, 512, format.c_str(), &m_tm);
    }
    return buf;
}
