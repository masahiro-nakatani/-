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
	//現在位置の計算
	Position new_position;
	getPositionEstimation(info_.pos, r_info.rot_encoderR_val - r_info_.rot_encoderR_val
		, r_info.rot_encoderL_val - r_info_.rot_encoderL_val, TIRE_RADIUS, TIRE_DISTANCE, new_position);
	//最新のロボット情報を保持
	r_info_  = r_info;
	//位置情報を最新に更新
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

// このメソッドをコールしたときの自己位置から
// 距離dist[mm]を走行する間は、直線であると仮定し、
// その直線はdir[degree]方向を向いているとする。
// 直線のエッジをライントレースしたときの平均方向を算出し、
// 進行方向を補正する。
void Navigator::StartDirCorrection (const F32 dist, const F32 dir) {
	// 現在地を保存
	correct_pos_org_ = info_.pos;
	// 角度補正対象距離
	correct_target_dist_ = dist; // [mm]
	// 角度補正対象方向
	correct_target_dir_  = dir / 180.0 * M_PI; // [degree] ⇒ [rad]
	// 角度補正用カウンタ初期化
	correct_sum_dir_ = 0.0;
	correct_num_ = 0;
	// 角度補正中フラグをON
	is_correcting_angle = true;
}

void Navigator::CalcDirCorrection (void) {
	// 初期位置から現在地までの距離計算
	F32 dist = sqrt (
		(info_.pos.X - correct_pos_org_.X) * (info_.pos.X - correct_pos_org_.X) +
		(info_.pos.Y - correct_pos_org_.Y) * (info_.pos.Y - correct_pos_org_.Y));

	if (dist >= correct_target_dist_) {
		// 初期位置から指定距離を走行した!

		// 集計した角度の平均値を計算
		F32 dir = correct_sum_dir_ / correct_num_;

		// 角度を補正
		info_.pos.Ang = info_.pos.Ang - (dir - correct_pos_org_.Ang);
		// 位置も補正
		// 初期位置からtarget方向にdist分進んだ位置で補正
		info_.pos.X = dist * cos(-correct_target_dir_) + correct_pos_org_.X;
		info_.pos.Y = dist * sin(-correct_target_dir_) + correct_pos_org_.Y;

		// 角度補正中フラグをOFF
		is_correcting_angle = false;
	} else {
		// まだ指定の距離まで達していない
		F32 mid = (CourseInfo::colorBlack + CourseInfo::colorWhite) / 2.0;
		F32 dif = CourseInfo::colorBlack - CourseInfo::colorWhite;
		if (((mid - dif/4.0) < r_info_.light_sensor_val) && (r_info_.light_sensor_val < (mid + dif/4.0))) {
			// 黒～白の間のちょーどいい具合の色のときの角度を集計
			correct_sum_dir_ += info_.pos.Ang;
			correct_num_++;
		}
	}

}

#if 0
void Navigator::UpdateAreaInfo(){
	//現在の区間を終了しているか判断

	short curArea = info_.areaCode;
	if(curArea < CourseInfo::AREA_O_END){
		if(info_.pos.X > CourseInfo::area[curArea].endX1 && info_.pos.X < CourseInfo::area[curArea].endX2){
			if(info_.pos.Y > CourseInfo::area[curArea].endY1 && info_.pos.Y < CourseInfo::area[curArea].endY2){
				//終了区間に着たので次のエリアに遷移
				curArea++;
				info_.areaCode = curArea;
			}
		}
	}

}
#endif


	//かくどほせい
#if 0
	if(info_.pos.X < 1900 && info_.pos.X > 1880 && info_.pos.Y < 300){
		//最初のストレートで補正
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
