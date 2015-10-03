#
# make all [options..]
#
# options:
#  BS_TARGET: ICHIRO_IN
#             ICHIRO_OUT
#             JIRO_IN
#             JIRO_OUT
#  ※注意 BS_TARGETを変更して違うオブジェクトを作成する際は、
#         一度「make clean」を実行してください。
#

ifndef BS_TARGET
BS_TARGET = ICHIRO_IN
endif

ifeq ($(BS_TARGET), ICHIRO_IN)
USER_DEF = ICHIRO IN_COURSE
else
ifeq ($(BS_TARGET), ICHIRO_OUT)
USER_DEF = ICHIRO OUT_COURSE
else
ifeq ($(BS_TARGET), JIRO_IN)
USER_DEF = JIRO IN_COURSE
else
ifeq ($(BS_TARGET), JIRO_OUT)
USER_DEF = JIRO OUT_COURSE
endif
endif
endif
endif

# nxtOSEKルートディレクトリ
NXTOSEK_ROOT = ${HOME}/nxtOSEK

# ターゲット実行形式ファイル名
TARGET = BSNro

BUILD_MODE = RXE_ONLY

# インクルードパス
USER_INC_PATH= $(NXTOSEK_ROOT)/ecrobot/nxtway_gs_balancer

# ライブラリ
USER_LIB = nxtway_gs_balancer

# Cソースファイル
TARGET_SOURCES = balancer_param.c

# C++(.cpp)ソースファイル
TARGET_CPP_SOURCES = \
	utils.cpp \
	CourseInfo.cpp \
	Robot.cpp \
	Navigator.cpp \
	Driver.cpp \
	Mission.cpp \
	PositionEstimation.cpp \
	MarkerCheck.cpp \
	Technic.cpp \
	Vec.cpp \
	TechBezierTrace.cpp \
	PIDController.cpp


# SGのパス
TOPPERS_OSEK_ROOT_SG = "${shell cygpath -w $(HOME)/nxtOSEK/toppers_osek}"

# TOPPERS/ATK1(OSEK)設定ファイル
TOPPERS_OSEK_OIL_SOURCE = Main.oil

# 下記のマクロは変更しないでください
O_PATH ?= build

# makefile for C++(.cpp) build
include $(NXTOSEK_ROOT)/ecrobot/ecrobot++.mak
