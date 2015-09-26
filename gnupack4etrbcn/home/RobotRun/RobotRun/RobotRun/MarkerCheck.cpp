#include "MarkerCheck.h"
#include "CourseInfo.h"
#include "Navigator.h"

#define GRAY_DIFF 6
#define STRAIGHT 0.05		// ���i����l

bool MarkerCheck::is_straight()
{
	F32 sumAng = 0.0;
	bool ret = FALSE;

	// 200ms�O�̊p�x�������������Z
	// angLog[0] �� 200ms�O�̊p�x
	// angLog[1] �� 180ms�O�̊p�x
	//   :
	// angLog[4] �� 120ms�O�̊p�x
	sumAng += fabs(angLog[0] - angLog[1]);
	sumAng += fabs(angLog[1] - angLog[2]);
	sumAng += fabs(angLog[2] - angLog[3]);
//	sumAng += fabs(angLog[3] - angLog[4]);
//	sumAng += fabs(angLog[4] - angLog[5]);
//	sumAng += fabs(angLog[5] - angLog[6]);
//	sumAng += fabs(angLog[6] - angLog[7]);
//	sumAng += fabs(angLog[7] - angLog[8]);
//	sumAng += fabs(angLog[8] - angLog[9]);

	if(sumAng <= STRAIGHT)
	{
		ret = TRUE;
	}
	return ret;
}

int MarkerCheck::IsMarker(S16 bright)
{
	// 20ms������IsMarker��Call�����
	int result = 0;
	int i;
	Position nowPos = Navigator::Instance().GetNowPos();
	F32 ang = nowPos.Ang;
	S16 sumBright = 0;

	// 200ms���̋P�x��ۑ�
	for(i = 0; i < (BRTLOGNUM - 1); i++)
	{
		brightLog[i] = brightLog[i+1];
	}
	brightLog[i] = bright;

	// 200ms���̊p�x��ۑ�
	for(i = 0; i < (ANGLOGNUM - 1); i++)
	{
		angLog[i] = angLog[i+1];
	}
	angLog[i] = ang;

//	sumBright += (brightLog[0] - brightLog[1]);
//	sumBright += (brightLog[1] - brightLog[2]);
//	sumBright += (brightLog[2] - brightLog[3]);
//	sumBright += (brightLog[3] - brightLog[4]);
	sumBright += (brightLog[4] - brightLog[5]);
	sumBright += (brightLog[5] - brightLog[6]);
	sumBright += (brightLog[6] - brightLog[7]);
	sumBright += (brightLog[7] - brightLog[8]);
	sumBright += (brightLog[8] - brightLog[9]);
	if((sumBright > 25) && (sumBright < 35))
	{
		if(is_straight())
		{
			result = 1;
		}
	}

	return result;
}

