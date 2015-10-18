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
    using System.Diagnostics;
    using System.IO;
    using System.IO.Ports;
    using System.Windows.Forms;

    // デリゲート宣言
    public delegate void AppendMessegeDelegate();
    
    public partial class Form1 : Form
    {
        
        // デリゲート
        private readonly AppendMessegeDelegate dlgAppendMessege;
        
        // ストップウォッチ（経過時間計測）
        private readonly Stopwatch myStopwatch = new Stopwatch();

        // シリアルポート
        private LogPort port;

        // ログデータ
        private LogMessege log;

        // ログファイル名
        private string logFileName;

        // テキストボックスへの書込み遅延バッファー
        private string bufTextBox;

        /// <summary>
        /// Form1コンストラクタ
        /// </summary>
        public Form1()
        {
            // コンポーネントの初期化
            // （コンポーネント利用に必須のデフォルト処理）
            this.InitializeComponent();

            // デリゲートメソッド追加登録
            this.dlgAppendMessege = this.AppendTextBox;    // 画面(TextBox)の更新
            this.dlgAppendMessege += this.AppendLogFile;   // ログファイル(*.csv)の更新
        }

        // デリゲート宣言
        public delegate void DlgLogOutput(byte[] mes);

        /// <summary>
        /// シリアルポート名をシステムより取得しコンボボックスに反映
        /// </summary>
        private void PortNoLoad()
        {
            // ソート済みのポート名一覧を取得
            string[] portNames = LogPort.GetSortedPortNames();

            // 取得したシリアル・ポート名をコンボボックスにセット
            this.portNoBox.Items.Clear();
            foreach (string item in portNames)
            {
                this.portNoBox.Items.Add(item);
            }

            if (portNames.Length > 0)
            {
                // コンボボックスのデフォルト選択をセット
                this.portNoBox.Text = portNames[0];
            }
            else
            {
                // CONNECTボタンを無効状態
                this.chkConnect.Enabled = false;

                // コンボボックスを無効状態
                this.portNoBox.Enabled = false;
            }
        }


        /// <summary>
        /// （メインスレッドの）ログデータ受信
        /// </summary>
        /// <param name="mes">データ</param>
        private void MessegeReceive(byte[] mes)
        {
            foreach (var t in mes)
            {
                this.log.Append(t);
            }

            System.Threading.Thread.Sleep(150);
        }

        ////////////////////////
        // イベントハンドラー //
        ////////////////////////

        /// <summary>
        /// フォームロード時の処理
        /// </summary>
        /// <param name="sender">センダー</param>
        /// <param name="e">イベント引数</param>
        private void Form1Load(object sender, EventArgs e)
        {
            // シリアルポート番号をメニューにセット
            this.PortNoLoad();

            // ポートインスタンス生成
            this.port = new LogPort();

            // シリアルポート受信イベントハンドラー登録
            this.port.DataReceived += this.SerialPortDataReceived;
        }



        /// <summary>
        /// フォームクローズ時の処理
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1FormClosed(object sender, FormClosedEventArgs e)
        {
            // シリアルポートインスタンス破棄
            if (this.port != null)  this.port.Dispose();

            // アプリケーション終了処理
            Application.Exit();
        }



        /// <summary>
        /// QUITボタン押下時の処理
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BtnQuitClick(object sender, EventArgs e)
        {
            // 自分自身のフォームを閉じる
            this.Close();
        }



        

        /// <summary>
        /// テキストボックスへログデータを追加
        /// </summary>
        private void AppendTextBox()
        {
            this.bufTextBox
               += Convert.ToString(this.log.RelTick).PadLeft(6, ' ') + ","
                + Convert.ToString(this.log.DataLeft).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.DataRight).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.Batt).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.MotorCnt0).PadLeft(5, ' ') + ","
                + Convert.ToString(this.log.MotorCnt1).PadLeft(5, ' ') + ","
                + Convert.ToString(this.log.MotorCnt2).PadLeft(5, ' ') + ","
                + Convert.ToString(this.log.SensorAdc0).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.SensorAdc1).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.CoordX).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.CoordY).PadLeft(4, ' ') + ","
                + Convert.ToString(this.log.I2C).PadLeft(4, ' ')
                + "\r\n";


            // テキストボックスへの書込み（AppendText）を頻繁に繰り返すと
            // 実行速度が低下するためバッファーを介して書込みをまとめて行う。

            // ストップウォッチ停止
           this. myStopwatch.Stop();

            // 更新時間が一定時間内であればスキップ
           if (this.myStopwatch.ElapsedMilliseconds > 20)
            {
                // テキストボックスへ追記
                this.boxDataView.AppendText(this.bufTextBox);

                // 書込みバッファーをクリア
                this.bufTextBox = string.Empty;

                // ストップウォッチをリセット
                this.myStopwatch.Reset();
            }

            // ストップウオッチを（再）スタート
           this.myStopwatch.Start();
        }

        /// <summary>
        /// ログファイルへログデータを追加
        /// </summary>
        private void AppendLogFile()
        {
            string rec
                = Convert.ToString(this.log.RelTick) + ","
                + Convert.ToString(this.log.DataLeft) + ","
                + Convert.ToString(this.log.DataRight) + ","
                + Convert.ToString(this.log.Batt) + ","
                + Convert.ToString(this.log.MotorCnt0) + ","
                + Convert.ToString(this.log.MotorCnt1) + ","
                + Convert.ToString(this.log.MotorCnt2) + ","
                + Convert.ToString(this.log.SensorAdc0) + ","
                + Convert.ToString(this.log.SensorAdc1) + ","
                + Convert.ToString(this.log.CoordX) + ","
                + Convert.ToString(this.log.CoordY) + ","
                + Convert.ToString(this.log.I2C) + "\r\n";


            using (StreamWriter sw = new StreamWriter(new FileStream(this.logFileName, FileMode.Append)))
            {
                try
                {
                    // ファイルへ追記
                    sw.Write(rec);
                }
                catch (Exception ex)
                {
                    // フォームにエラー表示
                    this.textLogFile.Text = "ERROR";

                    Debug.WriteLine("FILE WRITE ERROR : {0}", ex.ToString());
                }
            }
        }

        /// <summary>
        /// シリアルポート受信イベントハンドラ
        /// </summary>
        /// <param name="sender">センダー</param>
        /// <param name="e">イベント引数</param>
        private void SerialPortDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            DlgLogOutput dlgByteOut = this.MessegeReceive;

            // データ受信バッファ
            byte[] buf = new byte[this.port.BytesToRead];

            if (buf.Length > 0)
            {
                try
                {
                    // シリアルポートより受信
                    this.port.Read(buf, 0, buf.Length);

                    // 受信データをメインスレッドへ
                    this.BeginInvoke(dlgByteOut, buf);
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("UNEXPECTED EXCEPTION : {0}", ex.ToString());
                }
            }
        }



        /// <summary>
        /// １秒間隔のタイマーハンドラー
        /// </summary>
        /// <param name="sender">sender</param>
        /// <param name="e">event</param>
        private void Timer1Tick(object sender, EventArgs e)
        {
            // ポート状態監視
            if (this.port != null && this.port.IsOpen)
            {
                // ハードウェアフロー制御端子(CTS:Clear To Send)監視
                if (this.port.CtsHolding)
                {
                    // ラベル文字をアクティブ表示
                    this.labelCTS.ForeColor = System.Drawing.SystemColors.ControlText;
                }
                else
                {
                    // ラベル文字を非アクティブ表示
                    this.labelCTS.ForeColor = System.Drawing.SystemColors.ControlDark;
                }

                // ハードウェアフロー制御端子(DSR:Data Set Ready)監視
                if (this.port.DsrHolding)
                {
                    // ラベル文字をアクティブ表示
                    this.labelDSR.ForeColor = System.Drawing.SystemColors.ControlText;
                }
                else
                {
                    // ラベル文字を非アクティブ表示
                    this.labelDSR.ForeColor = System.Drawing.SystemColors.ControlDark;
                }

                if (this.port.CtsHolding != true || this.port.DsrHolding != true)
                {
                    // CONNECTボタンをアンチェック状態
                    this.chkConnect.Checked = false;
                }
            }
            else
            {
                // ラベル文字を非アクティブ表示
                this.labelCTS.ForeColor = System.Drawing.SystemColors.ControlDark;
                this.labelDSR.ForeColor = System.Drawing.SystemColors.ControlDark;
            }
        }


        /// <summary>
        /// CONNECTボタン（チェックボックス）押下時の処理
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ChkConnectCheckedChanged(object sender, EventArgs e)
        {
            if (this.chkConnect.Checked)
            {
                DateTime timeNow = DateTime.Now;    //  現在日時の取得

                // シリアルポート番号設定
                this.port.Connect(this.portNoBox.Text);

                // COMポート番号選択COMBO BOXを無効化
                this.portNoBox.Enabled = false;

                // 日時よりログファイル名作成
                this.logFileName = timeNow.ToString("yyyyMMdd_HHmmss") + ".csv";

                // フォームにログファイル名を表示
                this.textLogFile.Text = this.logFileName;

                // ログファイル(*.csv)の一行目にタイトル挿入
                using (StreamWriter sw = new StreamWriter(new FileStream(this.logFileName, FileMode.Append)))
                {
                    try
                    {
                        // ファイルへ追記
                        sw.WriteLine("Time,Data1,Data2,Battery,Motor Rev A,Motor Rev B,Motor Rev C,ADC S1,ADC S2,CRD X,CRD Y,I2C");
                    }
                    catch (Exception ex)
                    {
                        // フォームにエラー表示
                        this.textLogFile.Text = "ERROR";

                        Debug.WriteLine("FILE WRITE ERROR : {0}", ex.ToString());
                    }
                }

                // ログメッセージ作成
                this.log = new LogMessege(this.dlgAppendMessege);
            }
            else
            {
                // ポートの切断処理
                this.port.Disconnect();

                // COMポート番号選択COMBO BOXを有効化
                this.portNoBox.Enabled = true;
            }
        }

    }
}
