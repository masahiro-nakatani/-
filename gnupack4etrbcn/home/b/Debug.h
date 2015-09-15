#ifndef _INCLUDE_DEBUG_H_
#define _INCLUDE_DEBUG_H_

/**
 * @file Debug.h
 * @brief デバッグ用 LCD出力マクロ
 */

#define DEBUG //!< この定数が定義されている場合のみデバッグ出力する

#include "Robot.h"

#ifdef DEBUG // デバッグモードのときはLCD出力

  #define DEBUG_PRINT1(F, X)                  \
      Robot::Instance().GetLCD().cursor(0,5); \
      Robot::Instance().GetLCD().putf(F, X);  \
      Robot::Instance().GetLCD().disp();
   
  #define DEBUG_PRINT2(F, X, Y)                   \
      Robot::Instance().GetLCD().cursor(0,5);     \
      Robot::Instance().GetLCD().putf(F, X, Y);   \
      Robot::Instance().GetLCD().disp();
   
  #define DEBUG_PRINT3(F, X, Y, Z)                    \
      Robot::Instance().GetLCD().cursor(0,5);         \
      Robot::Instance().GetLCD().putf(F, X, Y, Z);    \
      Robot::Instance().GetLCD().disp();

#else // デバッグモードでなければ何もしない

  #define DEBUG_PRINT1(F, X) ;
  #define DEBUG_PRINT2(F, X, Y) ;
  #define DEBUG_PRINT3(F, X, Y, Z) ;

#endif

#endif
