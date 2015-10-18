#region Copyright & License
// Copyright 2009 Takehiko YOSHIDA  (http://www.chihayafuru.jp/etrobo/)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#endregion

namespace NxtLogger
{
    using System;
    using System.Collections;
    using System.Diagnostics;
    using System.IO.Ports;
    using System.Text.RegularExpressions;

    internal class LogPort : SerialPort
    {
        /// <summary>
        /// SerialPortコンストラクタ
        /// </summary>
        public LogPort()
        {
            // シリアルポートのパラメーター設定
            //      （ポート番号は未定）
            this.BaudRate = 57600;          // 通信速度（ボーレート）
            this.Parity = Parity.None;      // パリティ
            this.DataBits = 8;              // データビット
            this.StopBits = StopBits.One;   // ストップビット
        }



        /// <summary>
        /// ソート済みのポート名の配列を取得
        /// </summary>
        /// <returns>ポート名の配列</returns>
        public static string[] GetSortedPortNames()
        {
            // ポート名一覧を取得
            string[] portNames = GetPortNames();

            // ポート名の末尾のゴミを削除
            for (int i = 0; i < portNames.Length; i++)
            {
                if (portNames[i].Length > 5)
                {
                    portNames[i] = portNames[i].Substring(0, 5);            // 文字数で強制的に切り詰め
                }
                portNames[i] = Regex.Replace(portNames[i], "[^0-9]+$", string.Empty);   // 末尾の数字以外の文字を切り詰め
            }

            // ポート名をポート番号でソート
            IComparer portNoComp = new PortNoComparer();
            Array.Sort(portNames, portNoComp);

            return portNames;
        }


        /// <summary>
        /// シリアルポート接続
        /// </summary>
        /// <param name="portName">ポート名</param>
        public void Connect(string portName)
        {
            try
            {
                // シリアルポート番号設定
                this.PortName = portName;

                // ポートのオープン制御
                this.Open();

                // 受信バッファの破棄
                this.DiscardInBuffer();

                // ハードウェアフロー制御
                this.DtrEnable = true;
                this.RtsEnable = true;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("UNEXPECTED EXCEPTION : {0}", ex.ToString());
            }
        }

        /// <summary>
        /// シリアルポート切断
        /// </summary>
        public void Disconnect()
        {
            try
            {
                // ハードウェアフロー制御
                this.RtsEnable = false;
                this.DtrEnable = false;

                // ポートのクローズ制御
                this.Close();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("UNEXPECTED EXCEPTION : {0}", ex.ToString());
            }
        }
    }
}
