#define _USE_MATH_DEFINES
#include <math.h>	//sin cos �v�Z�ŕK�v
#include "PositionEstimation.h"


//------------------------------------------------------------------------------
// �֐����FgetPositionEstimation
// �T�v�F
//	�O��l�ƁA���[�^�[�̊p���x�A�^�C�����a�A�^�C�����璆�S�܂ł̋�������
//	���݂̈ʒu�𐄒肷��B
// �߂�l�F
//	struct Position Now		�F���ݒl
// �����F
//	struct Position& Pre	�F�O��l
//	double AngR				�F�E�p���x�i�O��Ƃ̍��j[��]���P���ɁA�G���R�[�_�l�̑O��Ƃ̍��������OK�ł��B
//	double AngL				�F���p���x�i�O��Ƃ̍��j[��]
//	double r				�F�^�C�����a[mm]
//	double d				�F�^�C�����璆�S�܂ł̋���[mm]�i�^�C���ԋ����̔����j
// ���̑��F
//		AngR,AngL�͊p���x�̑O��l�Ƃ̍��B�P�ʂ͊p�x�炵���B
//		��diff�v�Z���Ƀ��W�A���ɕϊ����Ă���B*M_PI/180������B
// http://d.hatena.ne.jp/teammaru/20120107/1325962481
// �ύX�����F
//	2010/07/15	�V�K�쐬	foo
//------------------------------------------------------------------------------
void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now)
{
	//�������v�Z���A�O��l�ɑ���
	F64 advanceL = r * AngL * M_PI / 180.0;
	F64 advanceR = r * AngR * M_PI / 180.0;
	F64 theta = (advanceR-advanceL) / d; // �p�x(rad)=��/�Ԏ���
	F64 half_theta = theta * 0.5;
	F64 nxt_advanced = (advanceL+advanceR) * 0.5;
	
	Now.Ang =  Pre.Ang + half_theta;
	Now.X = Pre.X + nxt_advanced * sin( Now.Ang );
	Now.Y = Pre.Y + nxt_advanced * cos( Now.Ang );
	
}