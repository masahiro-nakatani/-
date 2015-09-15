#include "Navigator.h"
#include "CourseInfo.h"
#include "Debug.h"

Navigator* Navigator::theInstance_ = 0 ;

Navigator& Navigator::Instance(void){
    if ( !theInstance_ ){
        theInstance_ = new Navigator();
    }
    return *theInstance_;

}

Navigator::Navigator(){
	Init();
}

Navigator::~Navigator(void){
}

void Navigator::Init(){
	CourseInfo::getStartPos(info_.pos.X, info_.pos.Y, info_.pos.Ang);
	info_.areaCode = 0;
	r_info_ = Robot::Instance().GetInfo();
	is_correcting_angle = false;
}

NavInfo Navigator::GetInfo(const RobotInfo& r_info){
	//���݈ʒu�̌v�Z
	Position new_position;
	getPositionEstimation(info_.pos, r_info.rot_encoderR_val - r_info_.rot_encoderR_val
		, r_info.rot_encoderL_val - r_info_.rot_encoderL_val, TIRE_RADIUS, TIRE_DISTANCE, new_position);
	//�ŐV�̃��{�b�g����ێ�
	r_info_  = r_info;
	//�ʒu�����ŐV�ɍX�V
	info_.pos = new_position;

	if (is_correcting_angle) {
		CalcDirCorrection ();
	}

    return info_;
}

void Navigator::SetInfo (F64 x, F64 y, F64 ang) {
	info_.pos.X = x;
	info_.pos.Y = y;
	info_.pos.Ang = ang;
}

Position Navigator::Getnow_positionition (void) {
	return info_.pos;
}


void  Navigator::ResetEncoder()
{
	r_info_.rot_encoderR_val = 0;	r_info_.rot_encoderL_val = 0;
}

// ���̃��\�b�h���R�[�������Ƃ��̎��Ȉʒu����
// ����dist[mm]�𑖍s����Ԃ́A�����ł���Ɖ��肵�A
// ���̒�����dir[degree]�����������Ă���Ƃ���B
// �����̃G�b�W�����C���g���[�X�����Ƃ��̕��ϕ������Z�o���A
// �i�s������␳����B
void Navigator::StartDirCorrection (const F32 dist, const F32 dir) {
	// ���ݒn��ۑ�
	correct_pos_org_ = info_.pos;
	// �p�x�␳�Ώۋ���
	correct_target_dist_ = dist; // [mm]
	// �p�x�␳�Ώە���
	correct_target_dir_  = dir / 180.0 * M_PI; // [degree] �� [rad]
	// �p�x�␳�p�J�E���^������
	correct_sum_dir_ = 0.0;
	correct_num_ = 0;
	// �p�x�␳���t���O��ON
	is_correcting_angle = true;
}

void Navigator::CalcDirCorrection (void) {
	// �����ʒu���猻�ݒn�܂ł̋����v�Z
	F32 dist = sqrt (
		(info_.pos.X - correct_pos_org_.X) * (info_.pos.X - correct_pos_org_.X) +
		(info_.pos.Y - correct_pos_org_.Y) * (info_.pos.Y - correct_pos_org_.Y));

	if (dist >= correct_target_dist_) {
		// �����ʒu����w�苗���𑖍s����!

		// �W�v�����p�x�̕��ϒl���v�Z
		F32 dir = correct_sum_dir_ / correct_num_;

		// �p�x��␳
		info_.pos.Ang = info_.pos.Ang - (dir - correct_pos_org_.Ang);
		// �ʒu���␳
		// �����ʒu����target������dist���i�񂾈ʒu�ŕ␳
		info_.pos.X = dist * cos(-correct_target_dir_) + correct_pos_org_.X;
		info_.pos.Y = dist * sin(-correct_target_dir_) + correct_pos_org_.Y;

		// �p�x�␳���t���O��OFF
		is_correcting_angle = false;
	} else {
		// �܂��w��̋����܂ŒB���Ă��Ȃ�
		F32 mid = (CourseInfo::colorBlack + CourseInfo::colorWhite) / 2.0;
		F32 dif = CourseInfo::colorBlack - CourseInfo::colorWhite;
		if (((mid - dif/4.0) < r_info_.light_sensor_val) && (r_info_.light_sensor_val < (mid + dif/4.0))) {
			// ���`���̊Ԃ̂���[�ǂ�����̐F�̂Ƃ��̊p�x���W�v
			correct_sum_dir_ += info_.pos.Ang;
			correct_num_++;
		}
	}

}

#if 0
void Navigator::UpdateAreaInfo(){
	//���݂̋�Ԃ��I�����Ă��邩���f

	short curArea = info_.areaCode;
	if(curArea < CourseInfo::AREA_O_END){
		if(info_.pos.X > CourseInfo::area[curArea].endX1 && info_.pos.X < CourseInfo::area[curArea].endX2){
			if(info_.pos.Y > CourseInfo::area[curArea].endY1 && info_.pos.Y < CourseInfo::area[curArea].endY2){
				//�I����Ԃɒ����̂Ŏ��̃G���A�ɑJ��
				curArea++;
				info_.areaCode = curArea;
			}
		}
	}

}
#endif


	//�����ǂق���
#if 0
	if(info_.pos.X < 1900 && info_.pos.X > 1880 && info_.pos.Y < 300){
		//�ŏ��̃X�g���[�g�ŕ␳
		isAdjustingAng = true;
		timer_ = clock_.now();
	}
	if(isAdjustingAng){
		if(info_.pos.X < 1370 && info_.pos.X > 1350 && (info_.pos.Y < 250)){
			isAdjustingAng = false;
			info_.pos.Ang = info_.pos.Ang - (angSum_ / sumNum_) + CourseInfo::startPosAngOut;
			angSum_ = 0;
			sumNum_ = 0;
		}else{
			angSum_ += info_.pos.Ang;
			sumNum_++;
		}
	}
#endif
