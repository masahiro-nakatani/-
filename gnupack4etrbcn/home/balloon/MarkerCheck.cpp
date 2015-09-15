
/**
 * @file MarkerCheck.cpp
 * @brief マーカー(灰色)検知処理
 */

#include "MarkerCheck.h"

/**
 * ロボットがマーカー上にいるならば 1 を返す
 *
 * @param[in] bright 輝度値
 * @retval 1 マーカー上にいる
 * @retval 0 マーカー上にいない
 *
 * @note IsMarkerは 20ms 周期でCallされる
 * @note マーカーの検知は走行スピード50で調整しています。
 * @note マーカー出口を検知します。
 */
bool MarkerCheck::IsMarker(S16 bright)
{
	int result = false;
    F32 normalizedLightValue = (1.0 - (-1.0)) /
            (CourseInfo::colorBlack - CourseInfo::colorWhite) *
            (bright - CourseInfo::colorWhite) + (-1.0);
	if(normalizedLightValue >= -0.28)
	{
		result = true;
	}

	return result;
}

