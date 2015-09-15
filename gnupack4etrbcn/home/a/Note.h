#ifndef _INCLUDE_NOTE_H_
#define _INCLUDE_NOTE_H_

/**
 * @file Note.h
 * @brief ���K�̒�`
 */

/*
 * ������(just intonation )�̉��K�\
 */
#define HALT       0   //!< ��~
#define LOOP    0xff   //!< ���[�v

#define REST    0x02   //!< �x��
#define O1C     0x11   //!< �h     33 Hz
#define O1CS    0x12   //!<        35 Hz
#define O1D     0x13   //!< ��     37 Hz
#define O1DS    0x14   //!<        39 Hz
#define O1E     0x15   //!< �~     41 Hz
#define O1F     0x16   //!< �t�@   44 Hz
#define O1FS    0x17   //!<        46 Hz
#define O1G     0x18   //!< �\     50 Hz
#define O1GS    0x19   //!<        53 Hz
#define O1A     0x1a   //!< ��     55 Hz
#define O1AS    0x1b   //!<        59 Hz
#define O1B     0x1c   //!< �V     62 Hz
#define O2C     0x21   //!< �h     66 Hz
#define O2CS    0x22   //!<        70 Hz
#define O2D     0x23   //!< ��     74 Hz
#define O2DS    0x24   //!<        78 Hz
#define O2E     0x25   //!< �~     83 Hz
#define O2F     0x26   //!< �t�@   88 Hz
#define O2FS    0x27   //!<        93 Hz
#define O2G     0x28   //!< �\     99 Hz
#define O2GS    0x29   //!<       106 Hz
#define O2A     0x2a   //!< ��    110 Hz
#define O2AS    0x2b   //!<       119 Hz
#define O2B     0x2c   //!< �V    124 Hz
#define O3C     0x31   //!< �h    132 Hz
#define O3CS    0x32   //!<       141 Hz
#define O3D     0x33   //!< ��    149 Hz
#define O3DS    0x34   //!<       156 Hz
#define O3E     0x35   //!< �~    165 Hz
#define O3F     0x36   //!< �t�@  176 Hz
#define O3FS    0x37   //!<       186 Hz
#define O3G     0x38   //!< �\    198 Hz
#define O3GS    0x39   //!<       211 Hz
#define O3A     0x3a   //!< ��    220 Hz
#define O3AS    0x3b   //!<       238 Hz
#define O3B     0x3c   //!< �V    248 Hz
#define O4C     0x41   //!< �h    264 Hz
#define O4CS    0x42   //!<       282 Hz
#define O4D     0x43   //!< ��    297 Hz
#define O4DS    0x44   //!<       312 Hz
#define O4E     0x45   //!< �~    330 Hz
#define O4F     0x46   //!< �t�@  352 Hz
#define O4FS    0x47   //!<       371 Hz
#define O4G     0x48   //!< �\    396 Hz
#define O4GS    0x49   //!<       422 Hz
#define O4A     0x4a   //!< ��    440 Hz
#define O4AS    0x4b   //!<       475 Hz
#define O4B     0x4c   //!< �V    495 Hz
#define O5C     0x51   //!< �h    528 Hz
#define O5CS    0x52   //!<       563 Hz
#define O5D     0x53   //!< ��    594 Hz
#define O5DS    0x54   //!<       624 Hz
#define O5E     0x55   //!< �~    660 Hz
#define O5F     0x56   //!< �t�@  704 Hz
#define O5FS    0x57   //!<       743 Hz
#define O5G     0x58   //!< �\    792 Hz
#define O5GS    0x59   //!<       845 Hz
#define O5A     0x5a   //!< ��    880 Hz
#define O5AS    0x5b   //!<       950 Hz
#define O5B     0x5c   //!< �V    990 Hz
#define O6C     0x61   //!< �h   1056 Hz
#define O6CS    0x62   //!<      1126 Hz
#define O6D     0x63   //!< ��   1188 Hz
#define O6DS    0x64   //!<      1248 Hz
#define O6E     0x65   //!< �~   1320 Hz
#define O6F     0x66   //!< �t�@ 1408 Hz
#define O6FS    0x67   //!<      1485 Hz
#define O6G     0x68   //!< �\   1584 Hz
#define O6GS    0x69   //!<      1690 Hz
#define O6A     0x6a   //!< ��   1760 Hz
#define O6AS    0x6b   //!<      1901 Hz
#define O6B     0x6c   //!< �V   1980 Hz
#define O7C     0x71   //!< �h   2112 Hz
#define O7CS    0x72   //!<      2253 Hz
#define O7D     0x73   //!< ��   2376 Hz
#define O7DS    0x74   //!<      2496 Hz
#define O7E     0x75   //!< �~   2640 Hz
#define O7F     0x76   //!< �t�@ 2816 Hz
#define O7FS    0x77   //!<      2970 Hz
#define O7G     0x78   //!< �\   3168 Hz
#define O7GS    0x79   //!<      3379 Hz
#define O7A     0x7a   //!< ��   3520 Hz
#define O7AS    0x7b   //!<      3802 Hz
#define O7B     0x7c   //!< �V   3960 Hz
#define O8C     0x81   //!< �h   4224 Hz
#define O8CS    0x82   //!<      4506 Hz
#define O8D     0x83   //!< ��   4752 Hz
#define O8DS    0x84   //!<      4992 Hz
#define O8E     0x85   //!< �~   5280 Hz
#define O8F     0x86   //!< �t�@ 5632 Hz
#define O8FS    0x87   //!<      5940 Hz
#define O8G     0x88   //!< �\   6336 Hz
#define O8GS    0x89   //!<      6758 Hz
#define O8A     0x8a   //!< ��   7040 Hz
#define O8AS    0x8b   //!<      7603 Hz
#define O8B     0x8c   //!< �V   7920 Hz

/**
 * 8�I�N�^�[�u�̉��K
 * ���ꂩ��A�S���K�̉������B
 */
unsigned short notes[13] = {
       0,   //!< padding
    4224,   //!< �h
    4506,   //!<
    4752,   //!< ��
    4992,   //!<
    5280,   //!< �~
    5632,   //!< �t�@
    5940,   //!<
    6336,   //!< �\_
    6758,   //!<
    7040,   //!< ��
    7603,   //!<
    7920    //!< �V
};

/* 4������ = 480Tick �Ƃ��Đݒ�          */
#define L1_DOT   255   //!<       �S���� 12
#define L1       192   //!<       �S���� 11
#define L2_DOT   144   //!< �t�_ 2������ 10
#define L2        96   //!<      2������  9
#define L4_DOT    72   //!< �t�_ 4������  8
#define L4        48   //!<      4������  7
#define L8_DOT    36   //!< �t�_ 8������  6
#define L8        24   //!<      8������  5
#define L16_DOT   18   //!< �t�_16������  4
#define L16       12   //!<     16������  3
#define L32_DOT    9   //!< �t�_32������  2
#define L32        6   //!<     32������  1

#endif // _INCLUDE_NOTE_H_
