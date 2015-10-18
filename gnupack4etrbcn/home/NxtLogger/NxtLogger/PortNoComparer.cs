#region Copyright & License
//
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
//
#endregion

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NxtLogger
{
    /// <summary>
    /// ポート番号比較クラス
    //      Array.Sortを使ったソートのためにIComparerを実装
    /// </summary>
    public class PortNoComparer : IComparer
    {
        /// <summary>
        /// 比較メソッド
        /// </summary>
        /// <param name="x">比較対象１</param>
        /// <param name="y">比較対象２</param>
        /// <returns>比較対象１－比較対象２</returns>
        public int Compare(object x, object y)
        {
            String str1 = (String)x;
            String str2 = (String)y;

            // 先頭の３文字("COM")を削除
            str1 = Convert.ToString(str1).Remove(0, 3);
            str2 = Convert.ToString(str2).Remove(0, 3);

            // ポート番号を数値として取得
            Int16 no1;
            Int16 no2;
            try
            {
                // 文字列を整数値へ変換
                no1 = Convert.ToInt16(str1);
            }
            catch
            {
                // 整数値への変換に失敗したら最大値を代入
                no1 = Int16.MaxValue;
            }

            try
            {
                // 文字列を整数値へ変換
                no2 = Convert.ToInt16(str2);
            }
            catch
            {
                // 整数値への変換に失敗したら最大値を代入
                no2 = Int16.MaxValue;
            }

            return (no1 - no2);
        }
    }
}
