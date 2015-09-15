
/**
 * @file MarkerCheck.cpp
 * @brief �}�[�J�[(�D�F)���m����
 */

#include "MarkerCheck.h"

/**
 * ���{�b�g���}�[�J�[��ɂ���Ȃ�� 1 ��Ԃ�
 *
 * @param[in] bright �P�x�l
 * @retval 1 �}�[�J�[��ɂ���
 * @retval 0 �}�[�J�[��ɂ��Ȃ�
 *
 * @note IsMarker�� 20ms ������Call�����
 * @note �}�[�J�[�̌��m�͑��s�X�s�[�h50�Œ������Ă��܂��B
 * @note �}�[�J�[�o�������m���܂��B
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

