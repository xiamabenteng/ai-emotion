#pragma once

typedef struct tagPARAM_MINMAX_VALUE
{
	float fBaseMax;
	float fBaseMin;
	float fCalculateMax;
	float fCalculateMin;
} PARAM_MINMAX_VALUE;

typedef struct tagPARAM_LEVEL_VALUE
{
	int nRateBase;
	float fStatLevel;
	float fStatLeveldef;
	float fStatRate;
	float fStatRate1;
	float fStatRate2;
	float fStatRate1v;
	float fStatRate2v;
	float fStatRate1def;
	float fStatRate2def;

} PARAM_LEVEL_VALUE;

typedef struct tagPARAM_DATA {
	float fSum;
	float fSqSum;
	float fAvg;
	int nCnt;
	float fv1;
	float s;
	float vi;
	double dCorrFactorA;
	double dCorrFactorB;
	double dCorrFactorA1;
	double dCorrFactorB1;
	VPE_PARAM_VALUE value;
	PARAM_MINMAX_VALUE MinMax;
	PARAM_LEVEL_VALUE level;
	VPE_PARAM paramId;
	vector<float> fVector;
	CString strName;
} PARAM_DATA;

typedef struct tagPARAM_STATE {
	int nMode;
	int nWidth;
	int nHeight;
	int nBpp;
	int nAnalyse;
	int nSpeedFps;
	double dDurationTime;
	double dCameraTime;
	double dEngineTime;
	double dSpeedRate;
	double dSpeedFps;
}PARAM_STATE;

typedef struct tagPARAM_ANALYSE_DATA
{
	float fStartTime;
	float fBaseHistogramData[256];
	float fFastHistogramData[256];
	float fBaseFps;
	float fFastFps;
	float fBaseMaxValue;
	float fBaseMinValue;
	float fFastMaxValue;
	float fFastMinValue;
	float fBaseMScale;
	float fFastMScale;
	float fBaseRMSDScale;
	float fFastRMSDScale;
	float fBaseDeltaScale;
	float fFastDeltaScale;
	float fBaseMulitScale;
	float fFastMulitScale;
	int nCount;
	int nBaseMaxIndex;
	int nBaseMinIndex;
	int nFastMaxIndex;
	int nFastMinIndex;

	int nParamId1;
	int nParamId2;
	CString strParamId1;
	CString strParamId2;
} PARAM_ANALYSE_DATA;

typedef struct tagVALUE_FFT_DATA
{
	float fBaseMScale;
	float fFastMScale;
	float fBaseMulitScale;
	float fFastMulitScale;
	float fBaseRMSDScale;
	float fFastRMSDScale;
	float fBaseFps;
	float fFastFps;
	float fBaseDeltaScale;
	float fFastDeltaScale;
	float fBaseMaxValue;
	float fFastMaxValue;
	int nBaseMaxIndex;
	int nFastMaxIndex;
} HIST_FFT_DATA;

typedef struct tagANALYSE_M_DATA {
	float fStartTime;
	float fCurrentTime;
	float fEndTime;
	deque <HIST_FFT_DATA> pHistFFTList;
	deque <HIST_FFT_DATA> pHistFFTMaxList;
	deque <PARAM_ANALYSE_DATA> pHistMList;
	deque <PARAM_ANALYSE_DATA> paramAnalyseDataList;
	PARAM_ANALYSE_DATA paramAnalyseData;
	CString strStartTime;
	CString strEndTime;
}ANALYSE_M_DATA;