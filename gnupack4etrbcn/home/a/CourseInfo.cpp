#include "CourseInfo.h"

/**
 * @file CourseInfo.cpp
 * @brief �R�[�X���̒�`
 */

S16 CourseInfo::colorBlack = 0; //!< �R�[�X�̍��l�B�L�����u���[�V�������[�V�����Őݒ肳���B
S16 CourseInfo::colorWhite = 0; //!< �R�[�X�̔��l�B�L�����u���[�V�������[�V�����Őݒ肳���B
S16 CourseInfo::colorGray  = 0; //!< @todo ���g�p

// �S�̃R�[�X�}�̗΃G���A����̍��W��x=0,y=0�Ƃ��A�����@(�P��:mm)�Ƃ���
// ET2013�C���R�[�X�̃p�����[�^
F32 CourseInfo::startPosXIn = 5108;
F32 CourseInfo::startPosYIn = 506;
F32 CourseInfo::startPosAngIn = 3.14159;

// ET2013�A�E�g�R�[�X�̃p�����[�^
F32 CourseInfo::startPosXOut = 4314;
F32 CourseInfo::startPosYOut = 256;
F32 CourseInfo::startPosAngOut = 3.14159;

CourseInfo::CourseType CourseInfo::type = CourseInfo::InCourse;

/**
 * ���{�b�g�̏����ʒu��Ԃ�
 * @param[out] x
 * @param[out] y
 * @param[out] ang
 */
void CourseInfo::getStartPos(F64 &x, F64 &y, F64 &ang){
	if (CourseInfo::type == CourseInfo::InCourse) {
		x = startPosXIn;
		y = startPosYIn;
		ang = startPosAngIn;
	} else {
		x = startPosXOut;
		y = startPosYOut;
		ang = startPosAngOut;
	}
}


