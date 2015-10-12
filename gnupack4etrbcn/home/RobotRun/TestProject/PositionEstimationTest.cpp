#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>	//sin cos 計算で必要
#include "PositionEstimation.h"

namespace
{
	

	TEST(position_estimation_1, position_estimation_test)
	{
		Position pos;
		pos.Ang = 0;
		pos.X=0;
		pos.Y=0;


		for(auto i =0; i < 5000; ++i)
		{
			Position pre = pos;
			getPositionEstimation(pre,2,1,180,1000,pos);
		}
		//角度は5piになるはず
		ASSERT_EQ(static_cast<int>(5*M_PI*1000*0.5), static_cast<int>(pos.Ang*1000));
	}

	TEST(position_estimation_2, position_estimation_test)
	{
		Position pos;
		pos.Ang = 0;
		pos.X=0;
		pos.Y=0;


		for(auto i =0; i < 4000; ++i)
		{
			Position pre = pos;
			getPositionEstimation(pre,1,0,180,1000,pos);
		}
		//2pi進むため、原点に戻る
		ASSERT_EQ(0,static_cast<int>(pos.X*1000));
		ASSERT_EQ(0,static_cast<int>(pos.Y*1000));
	}

	TEST(position_estimation_3, position_estimation_test)
	{
		Position pos;
		pos.Ang = 0;
		pos.X=0;
		pos.Y=0;


		for(auto i =0; i < 2000; ++i)
		{
			Position pre = pos;
			getPositionEstimation(pre,1,0,180,1000,pos);
		}

		ASSERT_EQ(0,static_cast<int>(pos.X*1000));
		ASSERT_EQ(0,static_cast<int>(pos.Y*1000));
	}

	TEST(position_estimation_4, position_estimation_test)
	{
		Position pos;
		pos.Ang = 0;
		pos.X=0;
		pos.Y=0;


		for(auto i =0; i < 200; ++i)
		{
			Position pre = pos;
			getPositionEstimation(pre,1,1,180.0/M_PI,1000,pos);
		}

		ASSERT_EQ(0,static_cast<int>(pos.X));
		ASSERT_EQ(200,static_cast<int>(pos.Y));
	}
}