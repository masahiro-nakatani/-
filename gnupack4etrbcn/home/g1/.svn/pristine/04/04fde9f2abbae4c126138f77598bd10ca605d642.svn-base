
/**
 * @file PositionEstimation.cpp
 * @brief 位置推定処理関数の定義
 */

#include "PositionEstimation.h"
#include "Debug.h"

/** 
 * 前回値と、モーターの角速度、タイヤ半径、タイヤから中心までの距離から現在の位置を推定する。
 *
 * @note AngR,AngLは角速度の前回値との差。単位は角度らしい。 
 *       diff計算時にラジアンに変換している。*M_PI/180がそれ。
 *
 * @param[in]  Pre  前回値
 * @param[in]  AngR 右角速度（前回との差）[°]→単純に、エンコーダ値の前回との差を入れればOKです。
 * @param[in]  AngL 左角速度（前回との差）[°]
 * @param[in]  r    タイヤ半径[mm]
 * @param[in]  d    タイヤから中心までの距離[mm]（タイヤ間距離の半分）
 * @param[out] Now  現在値
 * @return     なし
 *
 */
void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now)
{
	//差分を計算し、前回値に足す
	Now.X = Pre.X + 0.5 * r * ( AngL + AngR ) * M_PI / (F64)180 * cos( Pre.Ang );
	Now.Y = Pre.Y - 0.5 * r * ( AngL + AngR ) * M_PI / (F64)180 * sin( Pre.Ang );
	Now.Ang = Pre.Ang - 0.5 * r * ( AngL - AngR ) * M_PI / (F64)180 / d;
}
