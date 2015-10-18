namespace NxtLogger.Missions
{
    using System;

    public interface IRobotInput
    {
        /// <summary>
        /// SysTickアクセサ
        /// </summary>
        uint SysTick
        {
            get;
        }

        /// <summary>
        /// relTickアクセサ
        /// </summary>
        uint RelTick
        {
            get;
        }

        /// <summary>
        /// DataLeftアクセサ
        /// </summary>
        sbyte DataLeft
        {
            get;
        }

        /// <summary>
        /// DataRightアクセサ
        /// </summary>
        sbyte DataRight
        {
            get;
        }

        /// <summary>
        /// battアクセサ
        /// </summary>
        ushort Batt
        {
            get;
        }

        /// <summary>
        /// しっぽモーターアクセサ
        /// </summary>
        int MotorCnt0
        {
            get;
        }

        /// <summary>
        /// 左モーターアクセサ
        /// </summary>
        int MotorCnt1
        {
            get;
        }

        /// <summary>
        /// 右モーターアクセサ
        /// </summary>
        int MotorCnt2
        {
            get;
        }

        /// <summary>
        /// 光センサアクセサ
        /// </summary>
        short SensorAdc0
        {
            get;
        }

        /// <summary>
        /// ジャイロセンサアクセサ
        /// </summary>
        short SensorAdc1
        {
            get;
        }

        /// <summary>
        /// ジャイロセンサアクセサ
        /// </summary>
        short CoordX
        {
            get;
        }

        /// <summary>
        /// ジャイロセンサアクセサ
        /// </summary>
        short CoordY
        {
            get;
        }

        /// <summary>
        /// i2cアクセサ
        /// </summary>
        int I2C
        {
            get;
        }

    }
}
