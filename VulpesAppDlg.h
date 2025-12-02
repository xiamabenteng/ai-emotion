#pragma once
#include "VulpesEngine.h"
#include "VulpesCamera.h"
#include "VulpesParam.h"
#include "WndVideo.h"

class CVulpesAppDlg : public CDialogEx
{
public:
	CVulpesAppDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AIEMOTION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCmbChangeClick(int nid, WPARAM wParam, LPARAM lParam, UINT msg);
	afx_msg void OnBtnChangeVideo();
	afx_msg void OnChangeCamera();
	afx_msg void OnChangeVideo();
	afx_msg void OnBtnClose();
	afx_msg void OnBtnStartAnalyse();
	afx_msg void OnBtnStopAnalyse();
	DECLARE_MESSAGE_MAP()

public:
	void DrawBK(CDC* pDC);
	void DrawVideo(CDC* pDC, int nid, CRect rt);
	void ControlLayout();

public:
	void Init();
	void InitEmotion();
	void InitCamera();
	void InitEngine();
	void InitEngineParam();
	void SetParam();
	void InitParamData(PARAM_DATA* pParamData);
	void InitParamDataList();
	void InitParamDataMList();
	void InitCorrelationCoeff(PARAM_DATA* pParamData);
	void RestartEngine();

public:
	void EngineInputFrame(BYTE* pImg, int w, int h, double t);
	void EngineParamData(VPE_PARAM paramId1, VPE_PARAM paramId2);
	void VideoProcess(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat);
	void ProcessVarId();
	void AnalyseProcess();
	void ShowStatusProcess();
	void ShowParamProcess();
	void ShowResultProcess();
	void ShowDebugProcess();

public:
	void InitAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData);
	void AddParamAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData, float* pfBaseHistogramData, float* pfFastHistogramData, float fBaseFps, float fFastFps);
	void CalculateParamAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData);
	void AddHistMData();
	void CalculatepHistMData();
	void CalculateHealthData();
	void BaseCalHealthExtraversionStability(double* p26, double* p27);
	void MaxCalHealthExtraversionStability(double* p26, double* p27);
	void CalculateMResult();
	void Int2Float(float *pf, int *pn, int nCnt);

public:
	CVulpesEngine* m_pEngine;
	CVulpesCamera m_pCamera;
	PARAM_STATE m_sParamState;
	CComboBox m_CmbCamera;
	CComboBox m_CmbCameraSet;
	CComboBox m_CmbVideo;
	CArray <CString, CString&>m_sVideoFileList;
	CDC memDC;
	CBitmap memBmp;
	CCriticalSection m_csResult;
	CWndVideo wndVideo;
	CImage m_ImageSRC0;
	CImage m_ImageSRCA;
	CImage m_ImageSRCB;
	HANDLE m_hThreadVarId;

	CArray <PARAM_DATA, PARAM_DATA&> m_sParamDataList;
	CArray <PARAM_DATA, PARAM_DATA&> m_sParamDataMList;
	CArray <VPE_PARAM, VPE_PARAM&>m_sParamIdList;
	ANALYSE_M_DATA m_sAnalyseMData;
	CString m_strVideoFile;

	int nCameraCnt;
	DWORD64 dwCurrentTime;
	DWORD64 dwStartTime;
};