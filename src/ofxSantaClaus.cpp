/*
**      ARTSAT Project
**
**      Original Copyright (C) 2011 - 2012 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     openFrameworks
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 4.2 (LLVM compiler 3.0)
**
**      ofxSantaClaus.cpp
**
**      ------------------------------------------------------------------------
**
**      THE SOURCE CODE AND THE BINARY OF ARTSAT API IS DISTRIBUTED UNDER THE MIT LICENSE WRITTEN BELOW.
**      THE LICENSE OF ANY DATA GOTTEN THROUGH ARTSAT API FOLLOWS PROVIDED LICENSE ON EACH SATELLITE.
**
**      ARTSAT API のソースコードとバイナリは下記に示す MIT ライセンスの元で配布されます。
**      ARTSAT API を通して取得できるデータのライセンスに関してはデータの提供元となる各衛星の定めるライセンスに準じます。
**
**      The MIT License (MIT)
**
**      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
**      associated documentation files (the "Software"), to deal in the Software without restriction,
**      including without limitation the rights to use, copy, modify, merge, publish, distribute,
**      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
**      furnished to do so, subject to the following conditions:
**      The above copyright notice and this permission notice shall be included in all copies or
**      substantial portions of the Software.
**      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
**      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
**      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
**      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
**      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
**      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
**      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
**      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
**      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
**      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
**      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
**      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/

#include "ofxSantaClaus.h"

#define SAT_ID                  ("norad://SantaClaus")
#define URL_QUERY               ("http://www.noradsanta.org/")
#define DEFAULT_UPDATE          (5)
#define DEFAULT_RETRY           (60)
#define THREAD_SLEEP            (100)

static  ofxSantaClaus::TableRec const
                                ofxsantaclaus_table[] = {
    {// SENSOR_TIME_OBC
        .format                 = ofxSantaClaus::FORMAT_INT,
        .unit                   = ofxSantaClaus::UNIT_NONE,
        .query                  = "",
        .intHint                = {
            INT_MIN + 1,
            INT_MAX
        }
    }
};

/*public */ofxSantaClaus::ofxSantaClaus(void)
{
    _update.set(DEFAULT_UPDATE);
    _retry.set(DEFAULT_RETRY);
    setID(SAT_ID);
}

/*public virtual */ofxSantaClaus::~ofxSantaClaus(void)
{
    cleanup();
}

/*public */ofxSATError ofxSantaClaus::setUpdateInterval(ofxSATTimeDiff const& param)
{
    ofxSATError error(SATERROR_OK);
    
    if (!isValid()) {
        _update = param;
    }
    else {
        error = SATERROR_INVALID_STATE;
    }
    return error;
}

/*public */ofxSATError ofxSantaClaus::setRetryInterval(ofxSATTimeDiff const& param)
{
    ofxSATError error(SATERROR_OK);
    
    if (!isValid()) {
        _retry = param;
    }
    else {
        error = SATERROR_INVALID_STATE;
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::getSensorFormat(SensorType sensor, FormatEnum* result) const
{
    TableRec const* table;
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::getSensorFormat(sensor, result)) == SATERROR_NO_SUPPORT) {
        error = SATERROR_OK;
        if ((table = getTableBySensor(sensor)) != NULL) {
            *result = table->format;
        }
        else {
            error = SATERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::getSensorUnit(SensorType sensor, UnitEnum* result) const
{
    TableRec const* table;
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::getSensorUnit(sensor, result)) == SATERROR_NO_SUPPORT) {
        error = SATERROR_OK;
        if ((table = getTableBySensor(sensor)) != NULL) {
            *result = table->unit;
        }
        else {
            error = SATERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::getSensorMinimum(SensorType sensor, void* result, int size) const
{
    TableRec const* table;
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::getSensorMinimum(sensor, result, size)) == SATERROR_NO_SUPPORT) {
        error = SATERROR_OK;
        if ((table = getTableBySensor(sensor)) != NULL) {
            if ((error = checkSize(table->format, size)) == SATERROR_OK) {
                switch (table->format) {
                    case FORMAT_BOOL:
                        *static_cast<bool*>(result) = false;
                        break;
                    case FORMAT_INT:
                        *static_cast<int*>(result) = table->intHint.lower;
                        break;
                    case FORMAT_DOUBLE:
                        *static_cast<double*>(result) = table->doubleHint.lower;
                        break;
                    case FORMAT_STRING:
                    case FORMAT_OFXSATTIME:
                        error = SATERROR_NO_RESULT;
                        break;
                    default:
                        error = SATERROR_FAILED;
                        break;
                }
            }
        }
        else {
            error = SATERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::getSensorMaximum(SensorType sensor, void* result, int size) const
{
    TableRec const* table;
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::getSensorMaximum(sensor, result, size)) == SATERROR_NO_SUPPORT) {
        error = SATERROR_OK;
        if ((table = getTableBySensor(sensor)) != NULL) {
            if ((error = checkSize(table->format, size)) == SATERROR_OK) {
                switch (table->format) {
                    case FORMAT_BOOL:
                        *static_cast<bool*>(result) = true;
                        break;
                    case FORMAT_INT:
                        *static_cast<int*>(result) = table->intHint.upper;
                        break;
                    case FORMAT_DOUBLE:
                        *static_cast<double*>(result) = table->doubleHint.upper;
                        break;
                    case FORMAT_STRING:
                    case FORMAT_OFXSATTIME:
                        error = SATERROR_NO_RESULT;
                        break;
                    default:
                        error = SATERROR_FAILED;
                        break;
                }
            }
        }
        else {
            error = SATERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::getSensorData(SensorType sensor, ofxSATTime const& time, void* result, int size, bool* simulation) const
{
    TableRec const* table;
    ofxXmlSettings xml;
    int tempInt;
    double tempDouble;
    std::string tempString;
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::getSensorData(sensor, time, result, size, simulation)) == SATERROR_NO_SUPPORT) {
        error = SATERROR_OK;
        if ((table = getTableBySensor(sensor)) != NULL) {
            if ((error = fetchXML("getSensorData?&sensor=" + std::string(table->query) + convertToUNIXTime(time), &xml)) == SATERROR_OK) {
                if (result != NULL) {
                    if ((error = checkSize(table->format, size)) == SATERROR_OK) {
                        switch (table->format) {
                            case FORMAT_BOOL:
                                if ((tempInt = xml.getValue("data", INT_MIN)) != INT_MIN) {
                                    *static_cast<bool*>(result) = tempInt;
                                }
                                else {
                                    error = SATERROR_NO_RESULT;
                                }
                                break;
                            case FORMAT_INT:
                                if ((tempInt = xml.getValue("data", INT_MIN)) != INT_MIN) {
                                    *static_cast<int*>(result) = tempInt;
                                }
                                else {
                                    error = SATERROR_NO_RESULT;
                                }
                                break;
                            case FORMAT_DOUBLE:
                                if ((tempDouble = xml.getValue("data", NAN)) != NAN) {
                                    *static_cast<double*>(result) = tempDouble;
                                }
                                else {
                                    error = SATERROR_NO_RESULT;
                                }
                                break;
                            case FORMAT_STRING:
                                tempString = xml.getValue("data", "");
                                if (!tempString.empty()) {
                                    *static_cast<std::string*>(result) = tempString;
                                }
                                else {
                                    error = SATERROR_NO_RESULT;
                                }
                                break;
                            case FORMAT_OFXSATTIME:
                                tempString = xml.getValue("data", "");
                                if (!tempString.empty()) {
                                    error = convertUNIXTimeTo(tempString, static_cast<ofxSATTime*>(result));
                                }
                                else {
                                    error = SATERROR_NO_RESULT;
                                }
                                break;
                            default:
                                error = SATERROR_FAILED;
                                break;
                        }
                    }
                }
                if (error == SATERROR_OK) {
                    if (simulation != NULL) {
                        *simulation = true;
                    }
                }
            }
        }
        else {
            error = SATERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public virtual */ofxSATError ofxSantaClaus::setup(void)
{
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::setup()) == SATERROR_OK) {
        pushCrawl(true);
        startThread();
        if (error != SATERROR_OK) {
            super::cleanup();
        }
    }
    return error;
}

/*public virtual */void ofxSantaClaus::cleanup(void)
{
    waitForThread();
    super::cleanup();
    return;
}

/*public virtual */ofxSATError ofxSantaClaus::update(ofxSATTime const& time)
{
    ofxSATError error(SATERROR_OK);
    
    if ((error = super::update(time)) == SATERROR_OK) {
        pushCrawl(true);
    }
    return error;
}

/*private virtual */void ofxSantaClaus::threadedFunction(void)
{
    ofxSATTime last;
    ofxSATTime now;
    time_t time;
    time_t temp;
    ofxSATError error(SATERROR_OK);
    
    last = ofxSATTime::currentTime();
    time = last.asTime_t();
    while (isThreadRunning()) {
        if (isValid()) {
            now = ofxSATTime::currentTime();
            if (isImmediate() || error == SATERROR_OK) {
                if (popCrawl()) {
                    if ((error = receiveLatest(&temp)) == SATERROR_OK) {
                        error = receiveSpan(time, temp, &time);
                    }
                    if (!remainsCrawl() || error != SATERROR_OK) {
                        last = ofxSATTime::currentTime();
                        notifyFinish(error);
                    }
                }
                else if (now - last >= _update) {
                    pushCrawl(false);
                }
                else {
                    ofSleepMillis(THREAD_SLEEP);
                }
            }
            else if (now - last >= _retry) {
                error = SATERROR_OK;
            }
            else {
                ofSleepMillis(THREAD_SLEEP);
            }
        }
        else {
            ofSleepMillis(THREAD_SLEEP);
        }
    }
    return;
}

/*private */ofxSantaClaus::TableRec const* ofxSantaClaus::getTableBySensor(SensorType sensor) const
{
    TableRec const* result(NULL);
    
    if (0 <= sensor && sensor < SENSOR_LIMIT) {
        result = &ofxsantaclaus_table[sensor];
    }
    return result;
}

/*private */ofxSATError ofxSantaClaus::receiveLatest(time_t* result)
{
    ofxXmlSettings xml;
    bool fetch;
    time_t time;
    ofxSATError error(SATERROR_OK);
    
    if (result != NULL) {
        for (fetch = false; !fetch && isThreadRunning() && !isImmediate() && error == SATERROR_OK; ) {
            error = fetchXML("getAvailableTime?&limit=1", &xml);
            switch (error) {
                case SATERROR_OK:
                    if (xml.pushTag("time")) {
                        if ((time = xml.getValue("time", 0)) != 0) {
                            *result = time;
                            fetch = true;
                            ofSleepMillis(THREAD_SLEEP);
                        }
                        else {
                            error = SATERROR_INVALID_FORMAT;
                        }
                        xml.popTag();
                    }
                    else {
                        error = SATERROR_INVALID_FORMAT;
                    }
                    break;
                case SATERROR_NETWORK_DOWN:
                case SATERROR_NETWORK_CODE:
                    error = SATERROR_OK;
                    waitRetry();
                    break;
                default:
                    break;
            }
        }
    }
    else {
        error = SATERROR_INVALID_PARAM;
    }
    return error;
}

/*private */ofxSATError ofxSantaClaus::receiveSpan(time_t begin, time_t end, time_t* result)
{
    ofxXmlSettings xml;
    bool fetch;
    time_t time;
    int count;
    int i;
    ofxSATError error(SATERROR_OK);
    
    if (result != NULL) {
        *result = begin;
        if (begin < end) {
            for (fetch = false; !fetch && isThreadRunning() && !isImmediate() && error == SATERROR_OK; ) {
                error = fetchXML("getAvailableTime?&limit=32768" + convertToUNIXTime(begin + 1, end + 1), &xml);
                switch (error) {
                    case SATERROR_OK:
                        if (xml.pushTag("time")) {
                            count = xml.getNumTags("time");
                            for (i = 0; i < count; ++i) {
                                if ((time = xml.getValue("time", 0, i)) != 0) {
                                    *result = time;
                                    notifyData(ofxSATTime(time));
                                }
                            }
                            fetch = true;
                            ofSleepMillis(THREAD_SLEEP);
                            xml.popTag();
                        }
                        else {
                            error = SATERROR_INVALID_FORMAT;
                        }
                        break;
                    case SATERROR_NETWORK_DOWN:
                    case SATERROR_NETWORK_CODE:
                        error = SATERROR_OK;
                        waitRetry();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else {
        error = SATERROR_INVALID_PARAM;
    }
    return error;
}

/*private */ofxSATError ofxSantaClaus::fetchXML(std::string const& query, ofxXmlSettings* result) const
{
    ofHttpResponse response;
    ofxSATError error(SATERROR_OK);
    
    if (result != NULL) {
        response = ofLoadURL(URL_QUERY + query + "&format=xml");
        if (response.status >= 0) {
            switch (response.status) {
                case 200:
                    if (result->loadFromBuffer(response.data)) {
                        if (result->pushTag("artsat")) {
                            break;
                        }
                    }
                    error = SATERROR_FAILED;
                    break;
                case 404:
                    if (result->loadFromBuffer(response.data)) {
                        if (result->pushTag("artsat")) {
                            error = SATERROR_NO_RESULT;
                            break;
                        }
                    }
                default:
                    error = SATERROR_NETWORK_CODE;
                    break;
            }
        }
        else {
            error = SATERROR_NETWORK_DOWN;
        }
        switch (error) {
            case SATERROR_OK:
            case SATERROR_NO_RESULT:
                break;
            default:
                ofLogError() << "ofxSantaClaus::fetch(" << getID() << ") [" << response.status << " : " << response.error << "]";
                break;
        }
    }
    else {
        error = SATERROR_INVALID_PARAM;
    }
    return error;
}

/*private */void ofxSantaClaus::waitRetry(void)
{
    ofxSATTime last;
    ofxSATTime now;
    
    last = ofxSATTime::currentTime();
    while (isThreadRunning() && !isImmediate()) {
        now = ofxSATTime::currentTime();
        if (now - last >= _retry) {
            break;
        }
        else {
            ofSleepMillis(THREAD_SLEEP);
        }
    }
    return;
}

/*private */void ofxSantaClaus::pushCrawl(bool immediate) const
{
    _mutexCrawl.lock();
    _crawl = true;
    _immediate = immediate;
    _mutexCrawl.unlock();
    return;
}

/*private */bool ofxSantaClaus::popCrawl(void) const
{
    bool result(false);
    
    _mutexCrawl.lock();
    if (_crawl) {
        _crawl = false;
        result = true;
    }
    _immediate = false;
    _mutexCrawl.unlock();
    return result;
}

/*private */bool ofxSantaClaus::remainsCrawl(void) const
{
    bool result(false);
    
    _mutexCrawl.lock();
    if (_crawl) {
        result = true;
    }
    _mutexCrawl.unlock();
    return result;
}

/*private */bool ofxSantaClaus::isImmediate(void) const
{
    bool result(false);
    
    _mutexCrawl.lock();
    if (_immediate) {
        result = true;
    }
    _mutexCrawl.unlock();
    return result;
}

/*private static */std::string ofxSantaClaus::convertToUNIXTime(ofxSATTime const& time)
{
    return convertToUNIXTime(time.asTime_t());
}

/*private static */std::string ofxSantaClaus::convertToUNIXTime(time_t time)
{
    char result[64] = "\0";
    
    snprintf(result, sizeof(result), "&time=%ld", time);
    return result;
}

/*private static */std::string ofxSantaClaus::convertToUNIXTime(ofxSATTime const& begin, ofxSATTime const& end)
{
    return convertToUNIXTime(begin.asTime_t(), end.asTime_t());
}

/*private static */std::string ofxSantaClaus::convertToUNIXTime(time_t begin, time_t end)
{
    char result[64] = "\0";
    
    snprintf(result, sizeof(result), "&begin=%ld&end=%ld", begin, end);
    return result;
}

/*private static */ofxSATError ofxSantaClaus::convertUNIXTimeTo(std::string const& string, ofxSATTime* result)
{
    time_t time;
    ofxSATError error(SATERROR_OK);
    
    if (result != NULL) {
        if ((error = convertUNIXTimeTo(string, &time)) == SATERROR_OK) {
            result->set(time);
        }
    }
    else {
        error = SATERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */ofxSATError ofxSantaClaus::convertUNIXTimeTo(std::string const& string, time_t* result)
{
    time_t time;
    ofxSATError error(SATERROR_OK);
    
    if (result != NULL) {
        if (sscanf(string.c_str(), "%ld", &time) > 0) {
            *result = time;
        }
        else {
            error = SATERROR_INVALID_FORMAT;
        }
    }
    else {
        error = SATERROR_INVALID_PARAM;
    }
    return error;
}
