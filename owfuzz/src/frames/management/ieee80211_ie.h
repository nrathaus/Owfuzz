/**************************************************************************
* Copyright (C) 2020-2021 by Hongjian Cao <haimohk@gmail.com>
* *
* This file is part of owfuzz.
* *
* Owfuzz is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* *
* Owfuzz is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* *
* You should have received a copy of the GNU General Public License
* along with owfuzz.  If not, see <https://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef IEEE80211_IE_H
#define IEEE80211_IE_H
#include <inttypes.h>

#define IE_0_SSID 0   
#define IE_1_SUPPORTED_RATES_AND_BSS_MEMBERSHIP_SELECTORS 1
#define IE_2_RESERVED 2
#define IE_3_DSSS_PARAMETER_SET 3
#define IE_4_CF_PARAMETER_SET 4
#define IE_5_TIM 5
#define IE_6_IBSS_PARAMETER_SET 6
#define IE_7_COUNTRY 7
#define IE_8_RESERVED 8
#define IE_9_RESERVED 9
#define IE_10_REQUEST 10
#define IE_11_BSS_LOAD 11
#define IE_12_EDCA_PARAMETER_SET 12
#define IE_13_TSPEC 13
#define IE_14_TCLAS 14
#define IE_15_SCHEDULE 15
#define IE_16_CHALLENGE_TEXT 16
#define IE_17_RESERVED 17
#define IE_18_RESERVED 18
#define IE_19_RESERVED 19
#define IE_20_RESERVED 20
#define IE_21_RESERVED 21
#define IE_22_RESERVED 22
#define IE_23_RESERVED 23
#define IE_24_RESERVED 24
#define IE_25_RESERVED 25
#define IE_26_RESERVED 26
#define IE_27_RESERVED 27
#define IE_28_RESERVED 28
#define IE_29_RESERVED 29
#define IE_30_RESERVED 30
#define IE_31_RESERVED 31
#define IE_32_POWER_CONSTRAINT 32
#define IE_33_POWER_CAPABILITY 33
#define IE_34_TPC_REQUEST 34
#define IE_35_TPC_REPORT 35
#define IE_36_SUPPORTED_CHANNELS 36
#define IE_37_CHANNEL_SWITCH_ANNOUNCEMENT 37
#define IE_38_MEASUREMENT_REQUEST 38
#define IE_39_MEASUREMENT_REPORT 39
#define IE_40_QUIET 40
#define IE_41_IBSS_DFS 41
#define IE_42_ERP 42
#define IE_43_TS_DELAY 43
#define IE_44_TCLAS_PROCESSING 44
#define IE_45_HT_CAPABILITIES 45
#define IE_46_QOS_CAPABILITY 46
#define IE_47_RESERVED 47
#define IE_48_RSN 48
#define IE_49_RESERVED 49
#define IE_50_EXTENDED_SUPPORTED_RATES_AND_BSS_MEMBERSHIP_SELECTORS 50
#define IE_51_AP_CHANNEL_REPORT 51
#define IE_52_NEIGHBOR_REPORT 52
#define IE_53_RCPI 53
#define IE_54_MOBILITY_DOMAIN 54
#define IE_55_FAST_BSS_TRANSITION 55
#define IE_56_TIMEOUT_INTERVAL 56
#define IE_57_RIC_DATA 57
#define IE_58_DSE_REGISTERED_LOCATION 58
#define IE_59_SUPPORTED_OPERATING_CLASSES 59
#define IE_60_EXTENDED_CHANNEL_SWITCH_ANNOUNCEMENT 60
#define IE_61_HT_OPERATION 61
#define IE_62_SECONDARY_CHANNEL_OFFSET 62
#define IE_63_BSS_AVERAGE_ACCESS_DELAY 63
#define IE_64_ANTENNA 64
#define IE_65_RSNI 65
#define IE_66_MEASUREMENT_PILOT_TRANSMISSION 66
#define IE_67_BSS_AVAILABLE_ADMISSION_CAPACITY 67
#define IE_68_BSS_AC_ACCESS_DELAY 68
#define IE_69_TIME_ADVERTISEMENT 69
#define IE_70_RM_ENABLED_CAPABILITIES 70
#define IE_71_MULTIPLE_BSSID 71
#define IE_72_20_40_BSS_COEXISTENCE 72
#define IE_73_20_40_BSS_INTOLERANT_CHANNEL_REPORT 73
#define IE_74_OVERLAPPING_BSS_SCAN_PARAMETERS 74
#define IE_75_RIC_DESCRIPTOR 75
#define IE_76_MANAGEMENT_MIC 76
#define IE_78_EVENT_REQUEST 78
#define IE_79_EVENT_REPORT 79
#define IE_80_DIAGNOSTIC_REQUEST 80
#define IE_81_DIAGNOSTIC_REPORT 81
#define IE_82_LOCATION_PARAMETERS 82
#define IE_83_NONTRANSMITTED_BSSID_CAPABILITY 83
#define IE_84_SSID_LIST 84
#define IE_85_MULTIPLE_BSSID_INDEX 85
#define IE_86_FMS_DESCRIPTOR 86
#define IE_87_FMS_REQUEST 87
#define IE_88_FMS_RESPONSE 88
#define IE_89_QOS_TRAFFIC_CAPABILITY 89
#define IE_90_BSS_MAX_IDLE_PERIOD 90
#define IE_91_TFS_REQUEST 91
#define IE_92_TFS_RESPONSE 92
#define IE_93_WNM_SLEEP_MODE 93
#define IE_94_TIM_BROADCAST_REQUEST 94
#define IE_95_TIM_BROADCAST_RESPONSE 95
#define IE_96_COLLOCATED_INTERFERENCE_REPORT 96
#define IE_97_CHANNEL_USAGE 97
#define IE_98_TIME_ZONE 98
#define IE_99_DMS_REQUEST 99
#define IE_100_DMS_RESPONSE 100
#define IE_101_LINK_IDENTIFIER 101
#define IE_102_WAKEUP_SCHEDULE 102
#define IE_104_CHANNEL_SWITCH_TIMING 104
#define IE_105_PTI_CONTROL 105
#define IE_106_TPU_BUFFER_STATUS 106
#define IE_107_INTERWORKING 107
#define IE_108_ADVERTISEMENT_PROTOCOL 108
#define IE_109_EXPEDITED_BANDWIDTH_REQUEST 109
#define IE_110_QOS_MAP 110
#define IE_111_ROAMING_CONSORTIUM 111
#define IE_112_EMERGENCY_ALERT_IDENTIFIER 112
#define IE_113_MESH_CONFIGURATION 113
#define IE_114_MESH_ID 114
#define IE_115_MESH_LINK_METRIC_REPORT 115
#define IE_116_CONGESTION_NOTIFICATION 116
#define IE_117_MESH_PEERING_MANAGEMENT 117
#define IE_118_MESH_CHANNEL_SWITCH_PARAMETERS 118
#define IE_119_MESH_AWAKE_WINDOW 119
#define IE_120_BEACON_TIMING 120
#define IE_121_MCCAOP_SETUP_REQUEST 121
#define IE_122_MCCAOP_SETUP_REPLY 122
#define IE_123_MCCAOP_ADVERTISEMENT 123
#define IE_124_MCCAOP_TEARDOWN 124
#define IE_125_GANN 125
#define IE_126_RANN 126
#define IE_127_EXTENDED_CAPABILITIES 127
#define IE_128_RESERVED 128
#define IE_129_RESERVED 129
#define IE_130_PREQ 130
#define IE_131_PREP 131
#define IE_132_PERR 132
#define IE_133_RESERVED 133
#define IE_134_RESERVED 134
#define IE_135_RESERVED 135
#define IE_136_RESERVED 136
#define IE_137_PXU 137
#define IE_138_PXUC 138
#define IE_139_AUTHENTICATED_MESH_PEERING_EXCHANGE 139
#define IE_140_MIC 140
#define IE_141_DESTINATION_URI 141
#define IE_142_U_APSD 142
#define IE_143_DMG_WAKEUP_SCHEDULE 143
#define IE_144_EXTENDED_SCHEDULE 144
#define IE_145_STA_AVAILABILITY 145
#define IE_146_DMG_TSPEC 146
#define IE_147_NEXT_DMG_ATI 147
#define IE_148_DMG_CAPABILITIES 148
#define IE_149_RESERVED 149
#define IE_150_RESERVED 150
#define IE_151_DMG_OPERATION 151
#define IE_152_DMG_BSS_PARAMETER_CHANGE 152
#define IE_153_DMG_BEAM_REFINEMENT 153
#define IE_154_CHANNEL_MEASUREMENT_FEEDBACK 154
#define IE_155_RESERVED 155
#define IE_156_RESERVED 156
#define IE_157_AWAKE_WINDOW 157
#define IE_158_MULTI_BAND 158
#define IE_159_ADDBA_EXTENSION 159
#define IE_160_NEXTPCP_LIST 160
#define IE_161_PCP_HANDOVER 161
#define IE_162_DMG_LINK_MARGIN 162
#define IE_163_SWITCHING_STREAM 163
#define IE_164_SESSION_TRANSITION 164
#define IE_165_DYNAMIC_TONE_PAIRING_REPORT 165
#define IE_166_CLUSTER_REPORT 166
#define IE_167_RELAY_CAPABILITIES 167
#define IE_168_RELAY_TRANSFER_PARAMETER 168
#define IE_169_BEAMLINK_MAINTENANCE 169
#define IE_170_MULTIPLE_MAC_SUBLAYERS 170
#define IE_171_U_PID 171
#define IE_172_DMG_LINK_ADAPTATION_ACKNOWLEDGMENT 172
#define IE_173_RESERVED 173
#define IE_174_MCCAOP_ADVERTISEMENT_OVERVIEW 174
#define IE_175_QUIET_PERIOD_REQUEST 175
#define IE_176_RESERVED 176
#define IE_177_QUIET_PERIOD_RESPONSE 177
#define IE_178_RESERVED 178
#define IE_179_RESERVED 179
#define IE_180_RESERVED 180
#define IE_181_QMF_POLICY 181
#define IE_182_ECAPC_POLICY 182
#define IE_183_CLUSTER_TIME_OFFSET 183
#define IE_184_INTRA_ACCESS_CATEGORY_PRIORITY 184
#define IE_185_SCS_DESCRIPTOR 185
#define IE_186_QLOAD_REPORT 186
#define IE_187_HCCA_TXOP_UPDATE_COUNT 187
#define IE_188_HIGHER_LAYER_STREAM_ID 188
#define IE_189_GCR_GROUP_ADDRESS 189
#define IE_190_ANTENNA_SECTOR_ID_PATTERN 190
#define IE_191_VHT_CAPABILITIES 191
#define IE_192_VHT_OPERATION 192
#define IE_193_EXTENDED_BSS_LOAD 193
#define IE_194_WIDE_BANDWIDTH_CHANNEL_SWITCH 194
#define IE_195_TRANSMIT_POWER_ENVELOPE 195
#define IE_196_CHANNEL_SWITCH_WRAPPER 196
#define IE_197_AID 197
#define IE_198_QUIET_CHANNEL 198
#define IE_199_OPERATING_MODE_NOTIFICATION 199
#define IE_200_UPSIM 200
#define IE_201_REDUCED_NEIGHBOR_REPORT 201
#define IE_202_TVHT_OPERATION 202
#define IE_203_RESERVED 203
#define IE_204_DEVICE_LOCATION 204
#define IE_205_WHITE_SPACE_MAP 205
#define IE_206_FINE_TIMING_MEASUREMENT_PARAMETERS 206
#define IE_207_RESERVED 207
#define IE_208_RESERVED 208
#define IE_209_RESERVED 209
#define IE_210_RESERVED 210
#define IE_211_RESERVED 211
#define IE_212_RESERVED 212
#define IE_213_RESERVED 213
#define IE_214_RESERVED 214
#define IE_215_RESERVED 215
#define IE_216_RESERVED 216
#define IE_217_RESERVED 217
#define IE_218_RESERVED 218
#define IE_219_RESERVED 219
#define IE_220_RESERVED 220
#define IE_221_VENDOR_SPECIFIC 221
#define IE_222_RESERVED 222
#define IE_223_RESERVED 223
#define IE_224_RESERVED 224
#define IE_225_RESERVED 225
#define IE_226_RESERVED 226
#define IE_227_RESERVED 227
#define IE_228_RESERVED 228
#define IE_229_RESERVED 229
#define IE_230_RESERVED 230
#define IE_231_RESERVED 231
#define IE_232_RESERVED 232
#define IE_233_RESERVED 233
#define IE_234_RESERVED 234
#define IE_235_RESERVED 235
#define IE_236_RESERVED 236
#define IE_237_RESERVED 237
#define IE_238_RESERVED 238
#define IE_239_RESERVED 239
#define IE_240_RESERVED 240
#define IE_241_RESERVED 241
#define IE_242_RESERVED 242
#define IE_243_RESERVED 243
#define IE_244_RESERVED 244
#define IE_245_RESERVED 245
#define IE_246_RESERVED 246
#define IE_247_RESERVED 247
#define IE_248_RESERVED 248
#define IE_249_RESERVED 249
#define IE_250_RESERVED 250
#define IE_251_RESERVED 251
#define IE_252_RESERVED 252
#define IE_253_RESERVED 253
#define IE_254_RESERVED 254
#define IE_255_EXTENSIONS 255




typedef enum _FUZZING_TYPE
{
	NOT_PRESENT = 1,
	REPEATED,
	ALL_BITS_ZERO,
	MIN_SUB_1,
	MIN,
	MIN_ADD_1,
	RANDOM_VALUE,
	SPECIFIC_VALUE,
	MAX_SUB_1,
	MAX,
	MAX_ADD_1,
	ALL_BITS_ONE,
	//LEN_256,
	FUZZING_END
}FUZZING_TYPE;


typedef enum _FUZZING_VALUE_TYPE
{
	VALUE_ALL_BITS_ZERO = 1,
	VALUE_RANDOM,
	VALUE__ALL_BITS_ONE,
	VALUE_A,
	FUZZING_VALUE_END
}FUZZING_VALUE_TYPE;

#define IE_MAX_LENGTH ((255+2)*2) + 100 

struct ie_common_data
{
	uint8_t id;
	uint8_t length;
	uint8_t data[255];

}__attribute__((packed));


struct ie_data
{
	uint16_t length;
	uint8_t data[1024];
	
}__attribute__((packed));



#endif
