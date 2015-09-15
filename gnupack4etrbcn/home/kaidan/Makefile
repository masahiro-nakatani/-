#
# make all [options..]
#
# options:
#  BS_TARGET:=(in|out) # 省略するとinになります
#
# example:
#  $ make all BS_TARGET=out  #アウトコースの場合
#
#  ※注意 BS_TARGETを変更して違うオブジェクトを作成する際は、
#         一度「make clean」を実行してください。

ifeq ($(BS_TARGET),out)
USER_DEF = OUT_COURSE
else
USER_DEF = IN_COURSE
endif

all:
	@echo "Build completed $(USER_DEF)"

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
	Utils.cpp \
	CourseInfo.cpp \
	Robot.cpp \
	Navigator.cpp \
	Driver.cpp \
	Mission.cpp \
	PositionEstimation.cpp \
	MarkerCheck.cpp \
	Technic.cpp \
	Vec.cpp \
	PIDController.cpp

# SGのパス
TOPPERS_OSEK_ROOT_SG = "${shell cygpath -w $(HOME)/nxtOSEK/toppers_osek}"

# TOPPERS/ATK1(OSEK)設定ファイル
TOPPERS_OSEK_OIL_SOURCE = Main.oil

# 下記のマクロは変更しないでください
O_PATH ?= build

# makefile for C++(.cpp) build
include $(NXTOSEK_ROOT)/ecrobot/ecrobot++.mak
