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
**      ofxSantaClaus.h
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

#ifndef __OFX_SANTACLAUS_H
#define __OFX_SANTACLAUS_H

/*!
    @file
    サンタクロースを操作する機能を提供するクラスを含みます。
 */

#include "ofxSATType.h"
#include "ofxSATRemoteDB.h"
#include "ofThread.h"
#include "ofxXmlSettings.h"

//! サンタクロースを操作する機能を提供するクラス
/*!
    ofxSantaClaus クラスはサンタクロースからデータを取得する機能を提供します。
    サンタクロースを操作するには、このクラスのインスタンスを作成して利用します。
 
    ofxSantaClaus クラスの代表的な使い方
    @code
    #include "ofMain.h"
    #include "ofxARTSATAPI.h"
 
    // クラスの定義
    class testApp : public ofBaseApp, private ofxSAT::Notifier {
        private:
            // ofxSantaClaus クラスのインスタンス
            ofxSantaClaus _santaclaus;
 
        public:
            void setup(void);
            void exit(void);
            void draw(void);
        private:
            // 通知を受け取るための関数
            virtual void onNotifyTLE(ofxSAT::TLERec const& tle, ofxSATTime const& time);
            virtual void onNotifyData(ofxSATTime const& time);
    };
 
    // クラスの実装
    void testApp::setup(void)
    {
        // 通知を受け取るために通知先を設定
        if (_santaclaus.setNotifier(this) == SATERROR_OK) {
            // 初期化して開始
            if (_santaclaus.setup() == SATERROR_OK) {
                cout << "Hello ARTSAT." << endl;
            }
        }
        return;
    }
 
    void testApp::exit(void)
    {
        // リソースを解放して終了
        _santaclaus.cleanup();
        return;
    }
 
    void testApp::draw(void)
    {
        ofxSATTime begin;
        ofxSATTime end;
        std::vector<ofxSATTime> available;
        double value;
        int i;
 
        // １時間前から現在の時刻までを計算
        end = ofxSATTime::currentTime();
        begin = ofxSATTime(end).subHour(1);
 
        // １時間前から現在の時刻までの間でデータを取得可能な時刻のリストを取得
        if (_santaclaus.getAvailableTime(begin, end, &available) == SATERROR_OK) {
            for (i = 0; i < available.size(); ++i) {
                // データが存在する時刻の「+X 面の太陽電池の温度」を取得
                if (_santaclaus.getSensorTemperatureSolarPX(available[i], &value) == SATERROR_OK) {
                    cout << "the temperature of solar panel +X is " << value << " degC." << endl;
                }
            }
        }
        return;
    }
 
    // TLE 情報が更新されたときに呼び出される関数
    void testApp::onNotifyTLE(ofxSAT::TLERec const& tle, ofxSATTime const& time)
    {
        // 親クラスの実装を呼び出して情報を出力（オプション）
        ofxSAT::Notifier::onNotifyTLE(tle, time);
 
        cout << "new TLE received." << endl;
        return;
    }
 
    // センサデータが更新されたときに呼び出される関数
    void testApp::onNotifyData(ofxSATTime const& time)
    {
        // 親クラスの実装を呼び出して情報を出力（オプション）
        ofxSAT::Notifier::onNotifyData(time);
 
        cout << "new telemetry data received." << endl;
        return;
    }
    @endcode
 */
class ofxSantaClaus : public ofxSATRemoteDB, private ofThread {
    public:
        /*!
            自クラスを表す型の別名です。
         */
        typedef ofxSantaClaus           self;
        /*!
            親クラスを表す型の別名です。
         */
        typedef ofxSATRemoteDB          super;
    
    public:
        /*!
            衛星がサポートしているセンサを表す定数です。
            ofxSantaClaus::SENSOR_LIMIT は具体的なセンサを表す値ではなくセンサ定数の最大値を表します。
         */
        enum SensorEnum {
            //! 衛星の内部時刻
            SENSOR_TIME_OBC             = super::SENSOR_LIMIT,
            // default
            //! センサ定数の最大値
            SENSOR_LIMIT
        };
    
    public:
        //! @cond
        struct TableRec {
            FormatEnum                  format;
            UnitEnum                    unit;
            char                        query[32];
            union {
                struct {
                    char                no[32];
                    char                yes[32];
                } boolHint;
                struct {
                    int                 lower;
                    int                 upper;
                } intHint;
                struct {
                    double              lower;
                    double              upper;
                } doubleHint;
                struct {
                    char                format[32];
                } ofxSATTimeHint;
            };
        };
        //! @endcond
    
    private:
        mutable ofMutex                 _mutexCrawl;
        mutable bool                    _crawl;
        mutable bool                    _immediate;
                ofxSATTimeDiff          _update;
                ofxSATTimeDiff          _retry;
    
    public:
        /*!
            コンストラクタです。
            初期化に必要な最低限の処理をおこないます。
         */
        explicit                        ofxSantaClaus                   (void);
        /*!
            デストラクタです。
            管理しているリソースを解放します。
         */
        virtual                         ~ofxSantaClaus                  (void);
        /*!
            自動アップデートの間隔を設定します。
            デフォルト値は５秒です。
            @param [in] param 設定する時間間隔
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
         */
                ofxSATError             setUpdateInterval               (ofxSATTimeDiff const& param);
        /*!
            自動アップデートの間隔を取得します。
            @return 現在の時間間隔
         */
                ofxSATTimeDiff const&   getUpdateInterval               (void) const;
        /*!
            エラー発生時の自動リトライの間隔を設定します。
            デフォルト値は６０秒です。
            @param [in] param 設定する時間間隔
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
         */
                ofxSATError             setRetryInterval                (ofxSATTimeDiff const& param);
        /*!
            エラー発生時の自動リトライの間隔を取得します。
            @return 現在の時間間隔
         */
                ofxSATTimeDiff const&   getRetryInterval                (void) const;
        /*!
            センサの種類に応じた書式を取得します。
            @param [in] sensor ofxSantaClaus::SensorEnum 型のセンサ定数
            @param [out] result 取得結果
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
         */
        virtual ofxSATError             getSensorFormat                 (SensorType sensor, FormatEnum* result) const;
        /*!
            センサの種類に応じた単位を取得します。
            @param [in] sensor ofxSantaClaus::SensorEnum 型のセンサ定数
            @param [out] result 取得結果
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
         */
        virtual ofxSATError             getSensorUnit                   (SensorType sensor, UnitEnum* result) const;
        /*!
            センサの種類に応じた最小値を取得します。
            @param [in] sensor ofxSantaClaus::SensorEnum 型のセンサ定数
            @param [out] result 取得結果
            @param [in] size result に指定した変数のバイト数
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
            @retval ::SATERROR_NO_RESULT データが存在しない
            @retval ::SATERROR_FAILED 内部エラー
         */
        virtual ofxSATError             getSensorMinimum                (SensorType sensor, void* result, int size) const;
        /*!
            センサの種類に応じた最大値を取得します。
            @param [in] sensor ofxSantaClaus::SensorEnum 型のセンサ定数
            @param [out] result 取得結果
            @param [in] size result に指定した変数のバイト数
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
            @retval ::SATERROR_NO_RESULT データが存在しない
            @retval ::SATERROR_FAILED 内部エラー
         */
        virtual ofxSATError             getSensorMaximum                (SensorType sensor, void* result, int size) const;
        /*!
            センサの種類に応じた指定された時刻のデータを取得します。
            データ自体を取得する必要がないときは result 引数に NULL を、
            シミュレーションか否かを取得する必要がないときには simulation 引数に NULL を指定することができます。
         
            使い方１：データとシミュレーション結果を取得するとき
            @code
            ofxSATTime time;
            double value;
            bool simulation;
            ofxSATError error;
         
            time = ofxSATTime::currentTime();
            error = _santaclaus.getSensorData(ofxSantaClaus::SENSOR_POWER_SOLARPX, time, &value, sizeof(value), &simulation);
            if (error == SATERROR_OK) {
                cout << "value = " << value << endl;
            }
            else {
                cout << "error = " << error.print() << endl;
            }
            @endcode
            使い方２：データのみを取得するとき
            @code
            ofxSATTime time;
            double value;
            ofxSATError error;
         
            time = ofxSATTime::currentTime();
            error = _santaclaus.getSensorData(ofxSantaClaus::SENSOR_POWER_SOLARPX, time, &value, sizeof(value), NULL);
            if (error == SATERROR_OK) {
                cout << "value = " << value << endl;
            }
            else {
                cout << "error = " << error.print() << endl;
            }
            @endcode
            @param [in] sensor ofxSantaClaus::SensorEnum 型のセンサ定数
            @param [in] time データを取得する時刻
            @param [out] result 取得結果
                @arg NULL : 結果を返さない
                @arg その他
            @param [in] size result に指定した変数のバイト数
            @param [out] simulation 取得結果はシミュレーション
                @arg NULL : 結果を返さない
                @arg その他
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
            @retval ::SATERROR_NETWORK_DOWN ネットワークが未接続
            @retval ::SATERROR_NETWORK_CODE HTTP ステータスが不正
            @retval ::SATERROR_INVALID_FORMAT データのフォーマットが不正
            @retval ::SATERROR_NO_RESULT データが存在しない
            @retval ::SATERROR_FAILED 内部エラー
         */
        virtual ofxSATError             getSensorData                   (SensorType sensor, ofxSATTime const& time, void* result, int size, bool* simulation = NULL) const;
        /*!
            指定された時刻の「衛星の内部時刻」を取得します。
            データ自体を取得する必要がないときは result 引数に NULL を、
            シミュレーションか否かを取得する必要がないときには simulation 引数に NULL を指定することができます。
            @param [in] time データを取得する時刻
            @param [out] result 取得結果
                @arg NULL : 結果を返さない
                @arg その他
            @param [out] simulation 取得結果はシミュレーション
                @arg NULL : 結果を返さない
                @arg その他
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
            @retval ::SATERROR_INVALID_PARAM パラメータが不正
            @retval ::SATERROR_NETWORK_DOWN ネットワークが未接続
            @retval ::SATERROR_NETWORK_CODE HTTP ステータスが不正
            @retval ::SATERROR_INVALID_FORMAT データのフォーマットが不正
            @retval ::SATERROR_NO_RESULT データが存在しない
            @retval ::SATERROR_FAILED 内部エラー
         */
                ofxSATError             getSensorTimeOBC                (ofxSATTime const& time, int* result, bool* simulation = NULL) const;
        /*!
            データの取得やさまざまな機能を利用できるように初期化をおこないます。
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
         */
        virtual ofxSATError             setup                           (void);
        /*!
            データの取得やさまざまな機能を利用するために確保したリソースを解放します。
            @return なし
         */
        virtual void                    cleanup                         (void);
        /*!
            希望する時刻のデータを更新するように要求します。
            @param [in] time 希望する時刻
            @return エラー情報
            @retval ::SATERROR_OK 正常終了
            @retval ::SATERROR_INVALID_STATE 呼び出すタイミングが不正
         */
        virtual ofxSATError             update                          (ofxSATTime const& time = ofxSATTime::currentTime());
    private:
        virtual void                    threadedFunction                (void);
                TableRec const*         getTableBySensor                (SensorType sensor) const;
                ofxSATError             receiveLatest                   (time_t* result);
                ofxSATError             receiveSpan                     (time_t begin, time_t end, time_t* result);
                ofxSATError             fetchXML                        (std::string const& query, ofxXmlSettings* result) const;
                void                    waitRetry                       (void);
                void                    pushCrawl                       (bool immediate) const;
                bool                    popCrawl                        (void) const;
                bool                    remainsCrawl                    (void) const;
                bool                    isImmediate                     (void) const;
        static  std::string             convertToUNIXTime               (ofxSATTime const& time);
        static  std::string             convertToUNIXTime               (time_t time);
        static  std::string             convertToUNIXTime               (ofxSATTime const& begin, ofxSATTime const& end);
        static  std::string             convertToUNIXTime               (time_t begin, time_t end);
        static  ofxSATError             convertUNIXTimeTo               (std::string const& string, ofxSATTime* result);
        static  ofxSATError             convertUNIXTimeTo               (std::string const& string, time_t* result);
    private:
                                        ofxSantaClaus                   (ofxSantaClaus const&);
                ofxSantaClaus&          operator=                       (ofxSantaClaus const&);
};

/*public */inline ofxSATTimeDiff const& ofxSantaClaus::getUpdateInterval(void) const
{
    return _update;
}

/*public */inline ofxSATTimeDiff const& ofxSantaClaus::getRetryInterval(void) const
{
    return _retry;
}

/*public */inline ofxSATError ofxSantaClaus::getSensorTimeOBC(ofxSATTime const& time, int* result, bool* simulation) const
{
    return getSensorData(SENSOR_TIME_OBC, time, result, sizeof(*result), simulation);
}

#endif
