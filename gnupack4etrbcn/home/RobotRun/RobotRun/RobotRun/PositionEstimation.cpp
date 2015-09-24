#include "PositionEstimation.h"
#include "debug.h"

//------------------------------------------------------------------------------
// 関数名：getPositionEstimation
// 概要：
//	前回値と、モーターの角速度、タイヤ半径、タイヤから中心までの距離から
//	現在の位置を推定する。
// 戻り値：
//	struct Position Now		：現在値
// 引数：
//	struct Position& Pre	：前回値
//	double AngR				：右角速度（前回との差）[°]→単純に、エンコーダ値の前回との差を入れればOKです。
//	double AngL				：左角速度（前回との差）[°]
//	double r				：タイヤ半径[mm]
//	double d				：タイヤから中心までの距離[mm]（タイヤ間距離の半分）
// その他：
//		AngR,AngLは角速度の前回値との差。単位は角度らしい。
//		→diff計算時にラジアンに変換している。*M_PI/180がそれ。
// http://d.hatena.ne.jp/teammaru/20120107/1325962481
// 変更履歴：
//	2010/07/15	新規作成	foo
//------------------------------------------------------------------------------
void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now)
{
	//差分を計算し、前回値に足す
	F64 advanceL = r*2*M_PI * (AngL/360.0);
	F64 advanceR = r*2*M_PI * (AngR/360.0);
	F64 theta = (advanceR-advanceL) / d; // 角度(rad)=孤/車軸長
	F64 half_theta = theta * 0.5;
	F64 nxt_advanced = (advanceL+advanceR) * 0.5;
	
	Now.Ang =  Pre.Ang + half_theta;
	Now.X = Pre.X + nxt_advanced * sin( half_theta );
	Now.Y = Pre.Y + nxt_advanced * cos( half_theta );
	/*Now.Ang = Pre.Ang - 0.5 * r * ( AngR - AngL ) * M_PI / 180.0 / d;
	Now.X = Pre.X + 0.5 * r * ( AngL + AngR ) * M_PI / 180.0 * sin( Now.Ang );
	Now.Y = Pre.Y + 0.5 * r * ( AngL + AngR ) * M_PI / 180.0 * cos( Now.Ang );*/
	
	Robot::Instance().GetLCD().clear();
	Robot::Instance().GetLCD().putf("d", static_cast<int>(AngL));
	Robot::Instance().GetLCD().disp();
}
