#region Copyright & License
// Copyright 2009 Takehiko YOSHIDA  (http://www.chihayafuru.jp/etrobo/)
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#endregion

namespace NxtLogger
{
    using System;

    internal class LogMessege : Missions.IRobotInput
    {
        // SPP(Bluetooth)のパケット長定義
        private const ushort PacketHeaderLen = 2;
        private const ushort PacketPayloadLen = 32;
        private const ushort PacketLen = PacketHeaderLen + PacketPayloadLen;

        // 生パケット格納配列
        private readonly byte[] packetHeader = new byte[PacketHeaderLen];     // ヘッダー部
        private readonly byte[] packetPayload = new byte[PacketPayloadLen];   // ペイロード部

        private int byteNo;     // パケット先頭からの番号
        // ログデータ
        private uint  sysTick;    // システム時刻
        private sbyte   dataLeft;   // データ左
        private sbyte   dataRight;  // データ右
        private ushort  batt;       // バッテリーレベル
        private int   motorCnt0;  // モーターカウンタ０
        private int   motorCnt1;  // モーターカウンタ１
        private int   motorCnt2;  // モーターカウンタ２
        private short   sensorAdc0; // A/Dセンサー０
        private short   sensorAdc1; // A/Dセンサー１

        private int   i2C;        // I2Cセンサー

        private uint? offTick;    // 時刻オフセット（Nullable型）
        private uint  relTick;    // 相対時刻（ログ開始時刻からの相対時刻）

        // ログデータが１パケット（１行）分
        // 出来上がったことを通知するためのデリゲート
        private AppendMessegeDelegate dlg;

        /// <summary>
        /// LogMessegeコンストラクタ
        /// </summary>
        /// <param name="dlg"></param>
        public LogMessege(AppendMessegeDelegate dlg)
        {
            this.dlg = dlg;
        }



        /// <summary>
        /// SysTickアクセサ
        /// </summary>
        public uint SysTick
        {
            get
            {
                return this.sysTick;
            }
        }

        /// <summary>
        /// relTickアクセサ
        /// </summary>
        public uint RelTick
        {
            get
            {
                return this.relTick;
            }
        }

        /// <summary>
        /// DataLeftアクセサ
        /// </summary>
        public sbyte DataLeft
        {
            get
            {
                return this.dataLeft;
            }
        }

        /// <summary>
        /// DataRightアクセサ
        /// </summary>
        public sbyte DataRight
        {
            get
            {
                return this.dataRight;
            }
        }

        /// <summary>
        /// battアクセサ
        /// </summary>
        public ushort Batt
        {
            get
            {
                return this.batt;
            }
        }

        /// <summary>
        /// motorCnt0アクセサ
        /// </summary>
        public int MotorCnt0
        {
            get
            {
                return this.motorCnt0;
            }
        }

        /// <summary>
        /// motorCnt1アクセサ
        /// </summary>
        public int MotorCnt1
        {
            get
            {
                return this.motorCnt1;
            }
        }

        /// <summary>
        /// motorCnt2アクセサ
        /// </summary>
        public int MotorCnt2
        {
            get
            {
                return this.motorCnt2;
            }
        }

        /// <summary>
        /// sensorAdc0アクセサ
        /// </summary>
        public short SensorAdc0
        {
            get
            {
                return this.sensorAdc0;
            }
        }

        /// <summary>
        /// sensorAdc1アクセサ
        /// </summary>
        public short SensorAdc1
        {
            get
            {
                return this.sensorAdc1;
            }
        }

        /// <summary>
        /// X座標
        /// </summary>
        public short CoordX { get; private set; }

        /// <summary>
        /// Y座標
        /// </summary>
        public short CoordY { get; private set; }

        /// <summary>
        /// i2cアクセサ
        /// </summary>
        public int I2C
        {
            get
            {
                return this.i2C;
            }
        }



        public void Append(byte dat)
        {
            // パケットヘッダー部
            if (this.byteNo < PacketHeaderLen)
            {
                // 順送りでパケットヘッダー配列へ保存
                this.packetHeader[this.byteNo++] = dat;

                if (this.byteNo == PacketHeaderLen)
                {
                    // パケットヘッダー（パケットサイズ）のチェック
                    // NXTから送信されるパケットサイズにはヘッダの２バイト分は含まれない
                    ushort len = BitConverter.ToUInt16(this.packetHeader, 0);

                    if (len != PacketPayloadLen)
                    {
                        // パケット仕様： ヘッダー ＝ ペイロードサイズ
                        // 想定したヘッダー値でなければ１バイト分を読み捨てる
                        this.packetHeader[0] = this.packetHeader[1];
                        this.byteNo = 1;
                    }
                }
            }
            else if (this.byteNo < PacketLen)
            {
                // パケットペイロード（ヘッダーを除いた本体部）
                // 順送りでパケットペイロード配列へ保存
                this.packetPayload[this.byteNo++ - PacketHeaderLen] = dat;

                // １パケット分のデータサイズを取り出し終えたとき
                if (this.byteNo == PacketLen)
                {
                    // パケットをフィールドに変換
                    this.sysTick = BitConverter.ToUInt32(this.packetPayload, 0);
                    this.dataLeft = (sbyte)this.packetPayload[4];
                    this.dataRight = (sbyte)this.packetPayload[5];
                    this.batt = BitConverter.ToUInt16(this.packetPayload, 6);
                    this.motorCnt0 = BitConverter.ToInt32(this.packetPayload, 8);
                    this.motorCnt1 = BitConverter.ToInt32(this.packetPayload, 12);
                    this.motorCnt2 = BitConverter.ToInt32(this.packetPayload, 16);
                    this.sensorAdc0 = BitConverter.ToInt16(this.packetPayload, 20);
                    this.sensorAdc1 = BitConverter.ToInt16(this.packetPayload, 22);
                    this.CoordX = BitConverter.ToInt16(this.packetPayload, 24);
                    this.CoordY = BitConverter.ToInt16(this.packetPayload, 26);
                    this.i2C = BitConverter.ToInt32(this.packetPayload, 28);


                    // オフセット時間が未初期化(null)ならば
                    if (this.offTick == null)
                    {
                        // オフセット時間（ログ開始時のシステム時刻）セット
                        this.offTick = this.sysTick;
                    }

                    // 相対時間（ログ開始時からの時刻）計算
                    if (this.sysTick >= this.offTick)
                    {
                        // 通常の場合
                        this.relTick = this.sysTick - (uint)this.offTick;
                    }
                    else
                    {
                        // システム時刻が最大値を越えて一周した場合
                        this.relTick = this.sysTick + uint.MaxValue - (uint)this.offTick;
                    }

                    // デリゲートを介してログデータ追加メソッドを呼び出し
                    this.dlg.Invoke();

                    this.byteNo = 0;     // バイト番号を先頭に戻す
                }
            }
            else
            {
                // byteNo >= PacketLenは設計の想定外
                this.byteNo = 0;
            }
        }
    }
}
