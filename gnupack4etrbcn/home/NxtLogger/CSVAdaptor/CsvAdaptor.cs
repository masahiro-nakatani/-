namespace CSVAdaptor
{
    using System;
    using System.Collections.Generic;

    using Microsoft.VisualBasic.FileIO;

    /// <summary>
    /// The CSV adaptor.
    /// </summary>
    public static class CsvAdaptor
    {
        /// <summary>
        /// CSVファイルを読み込みます
        /// </summary>
        /// <param name="csvFileName">
        /// CSVパス
        /// </param>
        /// <returns>
        /// CSVファイル内の行データの配列
        /// </returns>
        /// <exception cref="ApplicationException">
        /// </exception>
        public static IEnumerable<IEnumerable<string>> ReadCsv(string csvFileName)
        {
            var csvRecords = new List<IEnumerable<string>>();

            // Shift JISで読み込む
            var tfp = new TextFieldParser(
                csvFileName,
                System.Text.Encoding.GetEncoding(932))
            {
                TextFieldType = FieldType.Delimited,
                Delimiters = new[] {","},
                HasFieldsEnclosedInQuotes = true,
                TrimWhiteSpace = true
            };

            // フィールドが文字で区切られているとする
            // デフォルトでDelimitedなので、必要なし
            // 区切り文字を,とする
            // フィールドを"で囲み、改行文字、区切り文字を含めることができるか
            // デフォルトでtrueなので、必要なし
            // フィールドの前後からスペースを削除する
            // デフォルトでtrueなので、必要なし
            try
            {
                while (!tfp.EndOfData)
                {
                    // フィールドを読み込む
                    var fields = tfp.ReadFields();

                    // 保存
                    csvRecords.Add(fields);
                }
            }
            catch (MalformedLineException ex)
            {
                throw new ApplicationException("Line " + ex.Message + " is invalid.  Skipping");
            }
            finally
            {
                // 後始末
                tfp.Close();
            }

            return csvRecords;
        }

        /// <summary>
        /// The write CSV.
        /// </summary>
        /// <param name="data">
        /// The data.
        /// </param>
        /// <param name="csvPath">
        /// The CSV path.
        /// </param>
        public static void WriteCsv(IEnumerable<IEnumerable<string>> data, string csvPath)
        {
            // CSVファイルに書き込むときに使うEncoding
            System.Text.Encoding enc =
                System.Text.Encoding.GetEncoding("Shift_JIS");

            // 書き込むファイルを開く
            using (var sr = new System.IO.StreamWriter(csvPath, false, enc))
            {
                foreach (var row in data)
                {
                    var isFirstCol = true;
                    foreach (var col in row)
                    {
                        if (!isFirstCol)
                        {
                            sr.Write(',');
                        }

                        isFirstCol = false;

                        // フィールドの取得
                        // "で囲む
                        var field = EncloseDoubleQuotesIfNeed(col);
                        
                        // フィールドを書き込む
                        sr.Write(field);
                    }

                    sr.Write("\r\n");
                }
            }
        }

        /// <summary>
        /// 必要ならば、文字列をダブルクォートで囲む
        /// </summary>
        private static string EncloseDoubleQuotesIfNeed(string field)
        {
            if (NeedEncloseDoubleQuotes(field))
            {
                return EncloseDoubleQuotes(field);
            }
            return field;
        }

        /// <summary>
        /// 文字列をダブルクォートで囲む
        /// </summary>
        private static string EncloseDoubleQuotes(string field)
        {
            if (field.IndexOf('"') > -1)
            {
                // "を""とする
                field = field.Replace("\"", "\"\"");
            }
            return "\"" + field + "\"";
        }

        /// <summary>
        /// 文字列をダブルクォートで囲む必要があるか調べる
        /// </summary>
        private static bool NeedEncloseDoubleQuotes(string field)
        {
            return field.IndexOf('"') > -1 ||
                field.IndexOf(',') > -1 ||
                field.IndexOf('\r') > -1 ||
                field.IndexOf('\n') > -1 ||
                field.StartsWith(" ") ||
                field.StartsWith("\t") ||
                field.EndsWith(" ") ||
                field.EndsWith("\t");
        }
    }
}
