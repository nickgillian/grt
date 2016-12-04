/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GRT_TIMESTAMP_HEADER
#define GRT_TIMESTAMP_HEADER

#include "Timer.h"
#include "ErrorLog.h"
#include "WarningLog.h"

GRT_BEGIN_NAMESPACE

class TimeStamp{
public:
    TimeStamp(unsigned int year=0,unsigned int month=0,unsigned int day=0,unsigned int hour=0,unsigned int minute=0,unsigned int second=0,unsigned int millisecond=0){
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        this->millisecond = millisecond;
        
        errorLog.setKey("[ERROR TimeStamp]");
        warningLog.setKey("[WARNING TimeStamp]");
    }
    
    TimeStamp(const TimeStamp &rhs){
        *this = rhs;
    }
    
    TimeStamp(const std::string &timeStampAsString){
        this->year = 0;
        this->month = 0;
        this->day = 0;
        this->hour = 0;
        this->minute = 0;
        this->second = 0;
        this->millisecond = 0;
        setTimeStampFromString( timeStampAsString );
        
        errorLog.setKey("[ERROR TimeStamp]");
        warningLog.setKey("[WARNING TimeStamp]");
    }
    
    ~TimeStamp(){
        
    }
    
    TimeStamp& operator=(const TimeStamp &rhs){
        if( this != &rhs ){
            this->year = rhs.year;
            this->month = rhs.month;
            this->day = rhs.day;
            this->hour = rhs.hour;
            this->minute = rhs.minute;
            this->second = rhs.second;
            this->millisecond = rhs.millisecond;
            this->errorLog = rhs.errorLog;
            this->warningLog = rhs.warningLog;
        }
        return *this;
    }
    
    bool operator>(const TimeStamp &rhs) const{
        if( this->getTimeInMilliseconds() > rhs.getTimeInMilliseconds() ){
            return true;
        }
        return false;
    }
    
    bool operator>=(const TimeStamp &rhs) const{
        if( this->getTimeInMilliseconds() >= rhs.getTimeInMilliseconds() ){
            return true;
        }
        return false;
    }
    
    bool operator<(const TimeStamp &rhs) const{
        if( this->getTimeInMilliseconds() < rhs.getTimeInMilliseconds() ){
            return true;
        }
        return false;
    }
    
    bool operator<=(const TimeStamp &rhs) const{
        if( this->getTimeInMilliseconds() <= rhs.getTimeInMilliseconds() ){
            return true;
        }
        return false;
    }
    
    TimeStamp operator+(const TimeStamp &rhs){
        TimeStamp ts;
        ts.year = this->year + rhs.year;
        ts.month = this->month + rhs.month;
        ts.day = this->day + rhs.day;
        ts.hour = this->hour + rhs.hour;
        ts.minute = this->minute + rhs.minute;
        ts.second = this->second + rhs.second;
        ts.millisecond = this->millisecond + rhs.millisecond;
        
        if( ts.millisecond >= 1000 ){
            ts.millisecond = ts.millisecond % 1000;
            ts.second++;
        }
        
        if( ts.second >= 60 ){
            ts.second = ts.second % 60;
            ts.minute++;
        }
        
        if( ts.minute >= 60 ){
            ts.minute = ts.minute % 60;
            ts.hour++;
        }
        
        if( ts.hour >= 24 ){
            ts.hour = ts.hour % 24;
            ts.day++;
        }
        
        unsigned int numDaysInMonth = 0;
        if( ts.month > 12 ){
            numDaysInMonth = getNumDaysInMonth( ts.month % 12 );
        }else numDaysInMonth = getNumDaysInMonth( ts.month );
        
        if( ts.day >= numDaysInMonth ){
            ts.day = ts.day - numDaysInMonth;
            ts.month++;
        }
        
        if( ts.month > 12 ){
            ts.month = ts.month % 12;
            ts.year++;
        }
        
        return ts;
    }
    
    TimeStamp& operator+=(const TimeStamp &rhs){
        if( this != &rhs ){
            this->year += rhs.year;
            this->month += rhs.month;
            this->day += rhs.day;
            this->hour += rhs.hour;
            this->minute += rhs.minute;
            this->second += rhs.second;
            this->millisecond += rhs.millisecond;
            
            if( this->millisecond >= 1000 ){
                this->millisecond = this->millisecond % 1000;
                this->second++;
            }
            
            if( this->second >= 60 ){
                this->second = this->second % 60;
                this->minute++;
            }
            
            if( this->minute >= 60 ){
                this->minute = this->minute % 60;
                this->hour++;
            }
            
            if( this->hour >= 24 ){
                this->hour = this->hour % 24;
                this->day++;
            }
            
            unsigned int numDaysInMonth = 0;
            if( this->month > 12 ){
                numDaysInMonth = getNumDaysInMonth( this->month % 12 );
            }else numDaysInMonth = getNumDaysInMonth( this->month );
            
            if( this->day >= numDaysInMonth ){
                this->day = this->day - numDaysInMonth;
                this->month++;
            }
            
            if( this->month > 12 ){
                this->month = this->month % 12;
                this->year++;
            }
        }
        return *this;
    }
    
    TimeStamp operator-(const TimeStamp &rhs){
        
        int year = (int)this->year - rhs.year;
        int month = (int)this->month - rhs.month;
        int day = (int)this->day - rhs.day;
        int hour = (int)this->hour - rhs.hour;
        int minute = (int)this->minute - rhs.minute;
        int second = (int)this->second - rhs.second;
        int millisecond = (int)this->millisecond - rhs.millisecond;
        
        if( millisecond < 0 ){
            millisecond = this->millisecond + 1000 - rhs.millisecond;
            second--;
        }
        
        if( second < 0 ){
            second = this->second + 60 - rhs.second;
            minute--;
        }
        
        if( minute < 0 ){
            minute = this->minute + 60 - rhs.minute;
            hour--;
        }
        
        if( hour < 0 ){
            hour = this->hour + 24 - rhs.hour;
            day--;
        }
        
        if( day <= 0 ){
            int numDaysInMonth = 0;
            if( month > 1 ){
                numDaysInMonth = getNumDaysInMonth( month - 1 );
            }else numDaysInMonth = getNumDaysInMonth( 12 - month );
            
            day = numDaysInMonth - day;
            month--;
        }
        
        if( month <= 0 ){
            month = 12 - month;
            year--;
        }
        
        TimeStamp ts;
        ts.year = year;
        ts.month = month;
        ts.day = day;
        ts.hour = hour;
        ts.minute = minute;
        ts.second = second;
        ts.millisecond = millisecond;
        
        return ts;
    }
    
    TimeStamp& operator-=(const TimeStamp &rhs){
        if( this != &rhs ){
            
            int year = (int)this->year - rhs.year;
            int month = (int)this->month - rhs.month;
            int day = (int)this->day - rhs.day;
            int hour = (int)this->hour - rhs.hour;
            int minute = (int)this->minute - rhs.minute;
            int second = (int)this->second - rhs.second;
            int millisecond = (int)this->millisecond - rhs.millisecond;
            
            if( millisecond < 0 ){
                millisecond = this->millisecond + 1000 - rhs.millisecond;
                second--;
            }
            
            if( second < 0 ){
                second = this->second + 60 - rhs.second;
                minute--;
            }
            
            if( minute < 0 ){
                minute = this->minute + 60 - rhs.minute;
                hour--;
            }
            
            if( hour < 0 ){
                hour = this->hour + 24 - rhs.hour;
                day--;
            }
            
            if( day <= 0 ){
                int numDaysInMonth = 0;
                if( month > 1 ){
                    numDaysInMonth = getNumDaysInMonth( month - 1 );
                }else numDaysInMonth = getNumDaysInMonth( 12 - month );
                
                day = numDaysInMonth - day;
                month--;
            }
            
            if( month <= 0 ){
                month = 12 - month;
                year--;
            }
            
            this->year = year;
            this->month = month;
            this->day = day;
            this->hour = hour;
            this->minute = minute;
            this->second = second;
            this->millisecond = millisecond;
        }
        return *this;
    }
    
    unsigned long long getTimeInMilliseconds() const{
        
        unsigned long long secondInMs = 1000;
        unsigned long long minuteInMs = 60*secondInMs;
        unsigned long long hourInMs = 60*minuteInMs;
        unsigned long long dayInMs = 24*hourInMs;
        
        unsigned long long firstJan2014InMS = 1388534400000; //Number of milliseconds since Jan 01 1970 on 01/01/2014 at 00:00:00
        unsigned long long yearTime = year == 2014 ? firstJan2014InMS : 0;
        unsigned long long monthTime = 0;
        unsigned long long dayTime = day == 1 ? 0 : (day-1) * dayInMs;
        unsigned long long hourTime = hour == 0 ? 0 : (hour-1) * hourInMs;
        unsigned long long minuteTime = minute == 0 ? 0 : (minute-1) * minuteInMs;
        unsigned long long secondTime = second == 0 ? 0 : (second-1) * secondInMs;
        
        unsigned long long janInMs = 31*dayInMs;
        unsigned long long febInMs = 29*dayInMs + janInMs;
        unsigned long long marchInMs = 31*dayInMs + febInMs;
        unsigned long long aprilInMs = 30*dayInMs + marchInMs;
        unsigned long long mayInMs = 31*dayInMs + aprilInMs;
        unsigned long long juneInMs = 30*dayInMs + mayInMs;
        unsigned long long julyInMs = 31*dayInMs + juneInMs;
        unsigned long long augInMs = 31*dayInMs + julyInMs;
        unsigned long long sepInMs = 31*dayInMs + augInMs;
        unsigned long long octInMs = 31*dayInMs + sepInMs;
        unsigned long long novInMs = 30*dayInMs + octInMs;
        
        switch( month ){
            case 1:
                monthTime = 0;
                break;
            case 2:
                monthTime = janInMs;
                break;
            case 3:
                monthTime = febInMs;
                break;
            case 4:
                monthTime = marchInMs;
                break;
            case 5:
                monthTime = aprilInMs;
                break;
            case 6:
                monthTime = mayInMs;
                break;
            case 7:
                monthTime = juneInMs;
                break;
            case 8:
                monthTime = julyInMs;
                break;
            case 9:
                monthTime = augInMs;
                break;
            case 10:
                monthTime = sepInMs;
                break;
            case 11:
                monthTime = octInMs;
                break;
            case 12:
                monthTime = novInMs;
                break;
        }
        
        return (yearTime + monthTime + dayTime + hourTime + minuteTime + secondTime + millisecond);
    }
    
    unsigned int getDayTimeInMilliseconds() const{
        
        unsigned int secondInMs = 1000;
        unsigned int minuteInMs = 60*secondInMs;
        unsigned int hourInMs = 60*minuteInMs;
        unsigned int hourTime = hour * hourInMs;
        unsigned int minuteTime = minute * minuteInMs;
        unsigned int secondTime = second * secondInMs;
        return (hourTime + minuteTime + secondTime + millisecond);
    }
    
    bool setTimeStampAsNow(){
#if defined(__GRT_OSX_BUILD__) || defined(__GRT_LINUX_BUILD__)
        
        //Get the date and time
        time_t tim = time(NULL);
        tm *now = localtime( &tim );
        
        if( now == NULL ) return false;
        
        //Get the millisecon time
        struct timeval nowTimeval;
        gettimeofday( &nowTimeval, NULL );
        
        year = (unsigned int)now->tm_year + 1900;
        month = (unsigned int)now->tm_mon + 1;
        day = (unsigned int)now->tm_mday;
        hour = (unsigned int)now->tm_hour;
        minute = (unsigned int)now->tm_min;
        second = (unsigned int)now->tm_sec;
        millisecond = (unsigned int)nowTimeval.tv_usec/1000;
        
        return true;
#endif
#ifdef __GRT_WINDOWS_BUILD__
        SYSTEMTIME systemTime;
        GetSystemTime(&systemTime);
        year = (unsigned int)systemTime.wYear;
        month = (unsigned int)systemTime.wMonth;
        day = (unsigned int)systemTime.wDay;
        hour = (unsigned int)systemTime.wHour;
        minute = (unsigned int)systemTime.wMinute;
        second = (unsigned int)systemTime.wSecond;
        millisecond = (unsigned int)systemTime.wMilliseconds;
        return true;
#endif
        warningLog << "setTimeStampAsNow() - Failed to get time stamp value! Unknown OS!" << std::endl;
        return false;
    }
    
    bool setTimeStampFromString(const std::string &timeString){
        
        if( timeString == "NOW" || timeString == "now" ){
            return setTimeStampAsNow();
        }
        
        //Find all the _
        std::vector< std::string > s;
        std::string tempString;
        for(unsigned int i=0; i<timeString.length(); i++ ){
            if( timeString[i] == '_' || timeString[i] == '\n' || timeString[i] == '\r'  ){
                s.push_back( tempString );
                tempString = "";
            }else tempString += timeString[i];
        }
        
        if( tempString.size() > 0 ) s.push_back( tempString );
        
        if( s.size() != 7 ){
            warningLog << "WARNING: Failed to set timestamp from string. Incorrect size! Size: " << s.size() << std::endl;
            return false;
        }
        
        year = grt_from_str< unsigned int >( s[0] );
        month = grt_from_str <unsigned int >( s[1]  );
        day = grt_from_str< unsigned int >( s[2] );
        hour = grt_from_str< unsigned int >( s[3]  );
        minute = grt_from_str< unsigned int >( s[4] );
        second = grt_from_str< unsigned int >( s[5]  );
        millisecond = grt_from_str< unsigned int >( s[6]  );
        
        return true;
    }
    
    std::string getTimeStampAsString( const bool includeDate = true ) const {
        std::string timeString = "";
        if( includeDate ){
            timeString = grt_to_str< unsigned int >(year);
            timeString += grt_to_str("_");
            timeString += grt_to_str< unsigned int >(month);
            timeString += grt_to_str("_");
            timeString += grt_to_str< unsigned int >(day);
            timeString += grt_to_str("_");
        }
        timeString += grt_to_str< unsigned int >(hour);
        timeString += grt_to_str("_");
        timeString += grt_to_str< unsigned int >(minute);
        timeString += grt_to_str("_");
        timeString += grt_to_str< unsigned int >(second);
        timeString += grt_to_str("_");
        timeString += grt_to_str< unsigned int >(millisecond);
        return timeString;
    }
    
    std::string getTimeStampAsJSONString() const {
        std::string timeString = "{";
        timeString += grt_to_str("\"year\":");
        timeString += grt_to_str< unsigned int >(year);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"month\":");
        timeString += grt_to_str< unsigned int >(month);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"day\":");
        timeString += grt_to_str< unsigned int >(day);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"hour\":");
        timeString += grt_to_str< unsigned int >(hour);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"minute\":");
        timeString += grt_to_str< unsigned int >(minute);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"second\":");
        timeString += grt_to_str< unsigned int >(second);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"millisecond\":");
        timeString += grt_to_str< unsigned int >(millisecond);
        timeString += grt_to_str(",");
        timeString += grt_to_str("\"timeInMS\":");
        timeString += grt_to_str< unsigned int >(
            static_cast<unsigned int>(
                getTimeInMilliseconds()
            )
        );
        timeString += grt_to_str("}");
        return timeString;
    }
    
    std::string getTimeAsISOString() const {
        std::string s = "";
        s += grt_to_str< unsigned int >(year);
        s += grt_to_str("-");
        s += pad( grt_to_str< unsigned int >(month) );
        s += grt_to_str("-");
        s += pad( grt_to_str< unsigned int >(day) );
        s += grt_to_str("T");
        s += pad( grt_to_str< unsigned int >( hour ) );
        s += grt_to_str(":");
        s += pad( grt_to_str< unsigned int >( minute ) );
        s += grt_to_str(":");
        s += pad( grt_to_str< unsigned int >( second ) );
        s += grt_to_str(".");
        s += grt_to_str< unsigned int >( millisecond );
        s += grt_to_str("Z");
        return s;
    }
    
    unsigned int getNumDaysInMonth( const unsigned int month ){
        switch( month ){
            case 1://Jan
                return 31;
                break;
            case 2: //Feb
                return 29; //Leap Year?????
                break;
            case 3: //March
                return 31;
                break;
            case 4: //April
                return 30;
                break;
            case 5: //May
                return 31;
                break;
            case 6: //June
                return 30;
                break;
            case 7: //July
                return 31;
                break;
            case 8: //August
                return 31;
                break;
            case 9: //September
                return 31;
                break;
            case 10: //October
                return 31;
                break;
            case 11: //November
                return 30;
                break;
            case 12: //December
                return 31;
                break;
        }
        warningLog << "getNumDaysInMonth(const unsigned int month) - Bad month parameter: " << month << std::endl;
        return 0;
    }
    
    std::string pad(const std::string &s) const {
        if( s.length() != 1 ) return s;
        return ( "0" + s );
    }
    
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
    unsigned int millisecond;
    ErrorLog errorLog;
    WarningLog warningLog;
    
};

GRT_END_NAMESPACE

#endif //GRT_TIMESTAMP_HEADER
