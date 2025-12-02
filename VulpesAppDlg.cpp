#include "stdafx.h"
#include "VulpesApp.h"
#include "VulpesAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int m_sCalculateParam[48] = {
	FPS_IN,
	FPS_OUT_BASE,
	FPS_OUT_FAST,
	STAT_RESULT_AM_FAST,						//A1X	
	STAT_RESULT_AM,								//A1
	STAT_RESULT_AVG_AM,							//A4
	STAT_RESULT_AVG_ACCUM_AM,					//A2
	STAT_RESULT_MAX_ACCUM_AM,					//A3
	STAT_RESULT_AVG_AM_FAST,					//A4X
	STAT_RESULT_FREQ,							//F1
	STAT_RESULT_FREQ_FAST,						//F1X
	STAT_RESULT_FREQ_DELTA,						//F1D
	STAT_RESULT_AVG_ACCUM_FREQ,					//F2
	STAT_RESULT_MAX_ACCUM_FREQ,					//F3
	STAT_RESULT_MAX_ACCUM_FREQ_MAX,				//F4
	STAT_RESULT_FREQ_FFT,						//F5
	STAT_RESULT_FREQ_FFT_FAST,					//F5X
	STAT_RESULT_FREQ_PERIOD_TIME_AVG,			//F6
	STAT_RESULT_FREQ_PERIOD_TIME_SD,			//F7
	STAT_RESULT_FREQ_PERIOD_TIME_DELTA,			//F8
	STAT_RESULT_FREQ_PERIOD_TIME_CV,			//F9
	STAT_RESULT_MIN_ACCUM_AM_CV_VALUE_RATE,		//S1
	STAT_RESULT_MIN_ACCUM_AM_CV_NUM_RATE,		//S4
	STAT_RESULT_AVG_ACCUM_AM_CV_VALUE_RATE,		//S2
	STAT_RESULT_AVG_ACCUM_AM_CV_NUM_RATE,		//S5
	STAT_RESULT_MAX_ACCUM_AM_CV_VALUE_RATE,		//S3
	STAT_RESULT_MAX_ACCUM_AM_CV_NUM_RATE,		//S6
	STAT_RESULT_MAX_ACCUM_AM_CV_DIFFERENCE,		//S7
	STAT_RESULT_AVG_ACCUM_FREQ_RMSD,			//P1
	STAT_RESULT_AVG_ACCUM_FREQ_SCALE,			//P3
	STAT_RESULT_MAX_ACCUM_FREQ_RMSD,			//P2
	STAT_RESULT_MAX_ACCUM_FREQ_SCALE,			//P4
	STAT_RESULT_STATEMENT_CRITICAL,				//P5
	STAT_RESULT_MAX_ACCUM_AM_CV_RANGE,				//P6A
	STAT_RESULT_MAX_ACCUM_FREQ_CV_RANGE,			//P6F
	STAT_RESULT_MAX_ACCUM_AM_CV_HISTOGRAM,			//P7A
	STAT_RESULT_MAX_ACCUM_FREQ_CV_HISTOGRAM,		//P7F
	STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RMSD,		//P8A
	STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RMSD,		//P8F
	STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD,		//P16
	STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE,	//P17
	STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG,	//P18
	STAT_RESULT_CRITICAL_AVG,						//P19
	STAT_RESULT_CRITICAL_HEALTH_ONCOLOGY,			//P21
	STAT_RESULT_CRITICAL_HEALTH_PG,					//P22
	STAT_RESULT_CRITICAL_HEALTH_AVG,				//P23
	STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION,		//P26
	STAT_RESULT_CRITICAL_HEALTH_STABILITY			//P27
};


void process_var_id(LPVOID lpData)
{
	CVulpesAppDlg* pThis = (CVulpesAppDlg*)lpData;
	while (1)
	{
		pThis->ProcessVarId();
		Sleep(10);
	}
}

void CallbackEngineParamData(void* lpData, VPE_PARAM paramId1, VPE_PARAM paramId2)
{
	CVulpesAppDlg* pThis = (CVulpesAppDlg*)lpData;
	pThis->EngineParamData(paramId1, paramId2);
}

void CallbackEngineInputFrame(void* lpData, BYTE* pImg, int w, int h, double t)
{
	CVulpesAppDlg* pThis = (CVulpesAppDlg*)lpData;
	pThis->EngineInputFrame(pImg, w, h, t);
}

void CallbackVideoProcess(void* lpData, double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat)
{
	CVulpesAppDlg* pThis = (CVulpesAppDlg*)lpData;
	pThis->VideoProcess(dblSampleTime, pBuffer, lBufferSize, pFormat);
}


CVulpesAppDlg::CVulpesAppDlg(CWnd* pParent)
	: CDialogEx(IDD_VULPESAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVulpesAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_CmbCamera);
	DDX_Control(pDX, IDC_COMBO_CAMERA_SET, m_CmbCameraSet);
	DDX_Control(pDX, IDC_COMBO_VIDEO, m_CmbVideo);
}

BEGIN_MESSAGE_MAP(CVulpesAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_VIDEO, &CVulpesAppDlg::OnBtnChangeVideo)
	ON_BN_CLICKED(IDC_BUTTON_START, &CVulpesAppDlg::OnBtnStartAnalyse)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CVulpesAppDlg::OnBtnStopAnalyse)
	ON_BN_CLICKED(IDCANCEL, &CVulpesAppDlg::OnBtnClose)
END_MESSAGE_MAP()

BOOL CVulpesAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	MoveWindow(0, 0, 1600, 900, FALSE);
	Init();
	m_hThreadVarId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_var_id, this, 0, 0);
	SetThreadPriority(m_hThreadVarId, THREAD_PRIORITY_ABOVE_NORMAL);

	SetTimer(1, 40, 0);
	return TRUE;
}

void CVulpesAppDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawBK(&dc);
}

int CVulpesAppDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}
	CRect rt;
	rt.SetRect(1180, 75, 1580, 900);
	wndVideo.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, rt, this, 1, NULL);
	return 0;
}

void CVulpesAppDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	ControlLayout();
}

void CVulpesAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	ShowStatusProcess();
	ShowParamProcess();
	ShowResultProcess();
	//ShowDebugProcess();
	wndVideo.Invalidate(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}

void CVulpesAppDlg::ControlLayout()
{
	CRect rt;
	GetClientRect(&rt);

	if (wndVideo.GetSafeHwnd()) {
		wndVideo.MoveWindow(1180, 75, 400, 690, FALSE);
	}
}

void CVulpesAppDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
}


BOOL CVulpesAppDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int msg = HIWORD(wParam);
	int nid = LOWORD(wParam);
	if (msg == CBN_SELCHANGE){
		OnCmbChangeClick(nid, wParam, lParam, msg);
	}
	return CDialogEx::OnCommand(wParam, lParam);
}


void CVulpesAppDlg::OnCmbChangeClick(int nid, WPARAM wParam, LPARAM lParam, UINT msg)
{
	if (nid == IDC_COMBO_CAMERA)
	{
		OnChangeCamera();
	}
	else if (nid == IDC_COMBO_CAMERA_SET)
	{
		int nSel = m_CmbCameraSet.GetCurSel();
		m_pCamera.CmdDialog(nSel);
	}
	else if (nid == IDC_COMBO_VIDEO)
	{
		OnChangeVideo();
	}
}


void CVulpesAppDlg::OnBtnChangeVideo()
{
	m_pEngine->Pause(true);
	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	if (m_pCamera.IsVideo()) {
		m_pCamera.CmdVideoStop();
	}
	CString szFilter = _T("AVI Video Files (*.avi)|*.avi|mp4 Files (*.mp4)|*.mp4;||");
	CFileDialog fDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR)szFilter);
	fDlg.m_ofn.lpstrTitle = _T("Open Video File");
	if (fDlg.DoModal() != IDOK) {
		OnChangeCamera();
		return;
	}
	POSITION pos = fDlg.GetStartPosition();
	CString fileName;
	while (pos) {
		fileName = fDlg.GetNextPathName(pos);
		m_CmbVideo.AddString(fileName);
		m_sVideoFileList.Add(fileName);
	}
	bool bflag = m_pCamera.PlayVideo((LPCWSTR)fileName, 0, -1.0);
	if (!bflag) {
		m_sParamState.dDurationTime = 0.0;
		OnChangeCamera();
		return;
	}
	m_sParamState.dDurationTime = (m_pCamera.GetVideoDuration() * 1000);
	if (m_sParamState.dDurationTime <= 0.0) {
		OnChangeCamera();
		return;
	}
	m_pEngine->Pause(false);
}


void CVulpesAppDlg::OnBtnStartAnalyse() {

	if (m_sParamState.nAnalyse) {
		return;
	}
	m_sAnalyseMData.fStartTime =0.0f;
	m_sAnalyseMData.fCurrentTime =0.0f;
	m_sAnalyseMData.fEndTime = (float)m_sParamState.dDurationTime;
	m_sAnalyseMData.strStartTime = cutils.GetCurrentStrTimestamp();

	InitParamDataList();
	InitParamDataMList();
	nCameraCnt = 0;
	InitAnalyseData(&m_sAnalyseMData.paramAnalyseData);
	m_sAnalyseMData.paramAnalyseData.nCount = 0;
	m_sAnalyseMData.pHistFFTList.clear();
	m_sAnalyseMData.pHistFFTMaxList.clear();
	m_sAnalyseMData.pHistMList.clear();
	m_sAnalyseMData.paramAnalyseDataList.clear();
	m_sParamState.nAnalyse = 1;

	RestartEngine();
	dwStartTime = cutils.GetCurrentUnixTimestamp();
}


void CVulpesAppDlg::OnBtnStopAnalyse() {

	if (!m_sParamState.nAnalyse) {
		return;
	}
	m_sParamState.nAnalyse = 0;

	CalculateMResult();

	m_sAnalyseMData.fStartTime =0.0f;
	m_sAnalyseMData.fCurrentTime =0.0f;
	m_sAnalyseMData.fEndTime =0.0f;
	m_sAnalyseMData.strEndTime = cutils.GetCurrentStrTimestamp();
	m_sAnalyseMData.pHistFFTList.clear();
	m_sAnalyseMData.pHistFFTMaxList.clear();
	m_sAnalyseMData.pHistMList.clear();
	m_sAnalyseMData.paramAnalyseDataList.clear();
	InitAnalyseData(&m_sAnalyseMData.paramAnalyseData);
	dwStartTime = 0;
	dwCurrentTime = 0;
	m_sParamState.dEngineTime = 0.0;
}


void CVulpesAppDlg::AnalyseProcess() {

	if (!m_sParamState.nAnalyse) {
		return;
	}
	PARAM_DATA *pParamData;
	for (int i = 0; i < m_sParamDataMList.GetSize(); i++) {
		pParamData = &m_sParamDataMList[i];
		pParamData->value = m_sParamDataList[pParamData->paramId].value;
		pParamData->MinMax = m_sParamDataList[pParamData->paramId].MinMax;
		pParamData->vi = m_sParamDataList[pParamData->paramId].vi;
		pParamData->s = m_sParamDataList[pParamData->paramId].s;
	}
}


void CVulpesAppDlg::OnChangeVideo()
{
	m_pEngine->Pause(true);

	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	if (m_pCamera.IsVideo()) {
		m_pCamera.CmdVideoStop();
	}

	if (m_sVideoFileList.IsEmpty()) {
		return;
	}
	int nSel = m_CmbVideo.GetCurSel();
	CString fileName = m_sVideoFileList[nSel];
	if (fileName.IsEmpty()) {
		return;
	}
	bool bflag = m_pCamera.PlayVideo((LPCWSTR)fileName, 0, -1.0);
	if (!bflag) {
		m_sParamState.dDurationTime = 0.0;
		OnChangeCamera();
		return;
	}
	m_sParamState.dDurationTime = (m_pCamera.GetVideoDuration() * 1000);
	if (m_sParamState.dDurationTime <= 0.0) {
		OnChangeCamera();
		return;
	}
	m_pEngine->Pause(false);
}


void CVulpesAppDlg::OnChangeCamera()
{
	int nsel = m_CmbCamera.GetCurSel();
	if (m_pCamera.IsVideo()) {
		m_pCamera.CmdVideoStop();
	}
	if (nsel < 0) {
		if (m_pCamera.CanCmdPreviewStop()) {
			m_pCamera.CmdPreviewStop();
		}
		return;
	}
	m_pEngine->Pause(true);
	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	m_pCamera.CmdDeviceVideo(nsel);
	if (m_pCamera.CanCmdPreview()) {
		m_pCamera.CmdPreview();
	}
	m_pEngine->Pause(false);
	m_pEngine->Reset();
}


void CVulpesAppDlg::OnBtnClose()
{
	::SuspendThread(m_hThreadVarId);

	if (m_pEngine != nullptr) {
		m_pEngine->Pause(true);
		m_pEngine->Stop(true);
	}
	delete m_pEngine;
	m_pEngine = nullptr;

	if (m_pCamera.IsVideo()) {
		m_pCamera.CmdVideoStop();
	}
	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	CDialogEx::OnCancel();
}


void CVulpesAppDlg::DrawBK(CDC* pDC) {

	CBitmap *m_memOldBmp;
	CRect rt;
	GetClientRect(&rt);
	if (memDC.GetSafeHdc() == NULL) {
		memDC.CreateCompatibleDC(pDC);
		memBmp.CreateCompatibleBitmap(pDC, rt.Width(), rt.Height());
		m_memOldBmp = (CBitmap *)memDC.SelectObject(&memBmp);
	}
	pDC->SetBkMode(TRANSPARENT);
	memDC.SetBkColor(RGB(240, 240, 240));
	memDC.ExtTextOut(0, 0, ETO_OPAQUE, &rt, _T(""), NULL);
	pDC->BitBlt(0, 0, rt.Width(), rt.Height(), &memDC, 0, 0, SRCCOPY);
}


void CVulpesAppDlg::DrawVideo(CDC* pDC, int paramId, CRect rt)
{
	CString str;
	if (paramId == IMAGE_RESULT_SRC)
	{
		if (!m_ImageSRC0.IsNull())
		{
			m_ImageSRC0.Draw(pDC->m_hDC, rt);
			str = L"SRC";
		}
	}
	else if (paramId == IMAGE_RESULT_AM)
	{
		if (!m_ImageSRCA.IsNull())
		{
			m_ImageSRCA.Draw(pDC->m_hDC, rt);
			str = L"SRCA";
		}
	}
	else if (paramId == IMAGE_RESULT_FREQ)
	{
		if (!m_ImageSRCB.IsNull())
		{
			m_ImageSRCB.Draw(pDC->m_hDC, rt);
			str = L"SRCB";
		}
	}
	CString str1;
	str1.Format(L" [%.2d:%.2d]", int(m_sParamState.dEngineTime) / 60, int(m_sParamState.dEngineTime) % 60);
	str += str1;
	pDC->SetTextColor(RGB(0, 255, 0));
	pDC->TextOutW(rt.left, rt.top + 4, str);
}


void CVulpesAppDlg::Init() {

	m_sParamState.dCameraTime = 0.0;
	m_sParamState.dDurationTime = 0.0;
	m_sParamState.dEngineTime = 0.0;
	m_sParamState.dSpeedRate = 10.0;
	m_sParamState.nBpp = 0;
	m_sParamState.nHeight = 64;
	m_sParamState.nMode = 0;
	m_sParamState.nWidth = 64;
	dwCurrentTime = 0;
	dwStartTime = 0;
	nCameraCnt = 0;

	InitEmotion();
	InitEngine();
	InitCamera();
	int n0 = (int)IMAGE_RESULT_SRC;
	int n1 = (int)IMAGE_RESULT_AM;
	int n2 = (int)IMAGE_RESULT_FREQ;
	wndVideo.videoList.Add(n0);
	wndVideo.videoList.Add(n1);
	wndVideo.videoList.Add(n2);
	wndVideo.lpData = this;
}


void CVulpesAppDlg::InitEmotion() {

	m_sParamDataList.RemoveAll();
	m_sParamDataMList.RemoveAll();

	PARAM_DATA paramData;
	for (int i = 0; i < PARAM_CNT; i++) {
		paramData.paramId = (VPE_PARAM)i;
		InitParamData(&paramData);
		m_sParamDataList.Add(paramData);
	}

	paramData.strName = L"FpsIn";
	paramData.paramId = FPS_IN;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"FpsOutFast";
	paramData.paramId = FPS_OUT_FAST;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"FpsOutBase";
	paramData.paramId = FPS_OUT_BASE;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumHistogramCv";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumRangeCv";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_CV_RANGE;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"FreqFFTFast";
	paramData.paramId = STAT_RESULT_FREQ_FFT_FAST;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalAvg";
	paramData.paramId = STAT_RESULT_CRITICAL_AVG;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumFreqHistogramSD";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumAmHistogramDiff";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumHistogramRMSD";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"MaxAccumFreqAmHistogramAvg";
	paramData.paramId = STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"FreqPeriodTimeAvg";
	paramData.paramId = STAT_RESULT_FREQ_PERIOD_TIME_AVG;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"FreqPeriodTimeCv";
	paramData.paramId = STAT_RESULT_FREQ_PERIOD_TIME_CV;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalHealthOncology";
	paramData.paramId = STAT_RESULT_CRITICAL_HEALTH_ONCOLOGY;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalHealthPG";
	paramData.paramId = STAT_RESULT_CRITICAL_HEALTH_PG;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalHealthAvg";
	paramData.paramId = STAT_RESULT_CRITICAL_HEALTH_AVG;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalHealthExtraversion";
	paramData.paramId = STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION;
	m_sParamDataMList.Add(paramData);

	paramData.strName = L"CriticalHealthStability";
	paramData.paramId = STAT_RESULT_CRITICAL_HEALTH_STABILITY;
	m_sParamDataMList.Add(paramData);
	InitParamDataMList();
}


void CVulpesAppDlg::InitCamera() {
	m_pCamera.SetInplaceVideo(2);
	m_pCamera.SetRegistryKey(_T("SOFTWARE\\VulpesEngine\\VulpesApp\\"));
	m_pCamera.SetRegistryKeyGroup(_T("Video0"));
	m_pCamera.SetCallbackData(this);
	m_pCamera.SetCallbackOnVideoPlay(CallbackVideoProcess);

	m_CmbCameraSet.AddString(_T("Set Camera Filter"));
	m_CmbCameraSet.AddString(_T("Set Camera Format"));
	m_CmbCameraSet.SetCurSel(0);

	if (!m_pCamera.Create(NULL, _T("Video"), WS_CHILD, CRect(329, 90, 649, 330), this, 0)) {
		return;
	}
	int devCount = m_pCamera.GetDeviceVideoCount();
	if (devCount < 1)
	{
		return;
	}
	m_pCamera.ShowWindow(SW_HIDE);
	m_pCamera.CmdDeviceVideo(0);
	CString str;
	for (int i = 0; i < devCount; i++) {
		str = (CString)m_pCamera.GetDevNameVideoW(i);
		m_CmbCamera.AddString(str);
	}
	m_CmbCamera.SetCurSel(0);
	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	m_pCamera.CmdDeviceVideo(0);
	if (m_pCamera.CanCmdPreview()) {
		m_pCamera.CmdPreview();
	}
}


void CVulpesAppDlg::InitEngine() {
	m_pEngine = new CVulpesEngine(-1);
	m_pEngine->SetCallbackOnParamData(CallbackEngineParamData, this);
	m_pEngine->SetCallbackOnAddFrameImage(CallbackEngineInputFrame, this);
	m_pEngine->Start();
	InitEngineParam();
	SetParam();
}


void CVulpesAppDlg::InitEngineParam() {
	
	m_pEngine->SetParamValueNx(CORRELATION_X, 155);
	m_pEngine->SetParamValueNx(CORRELATION_Y, 0);

	m_pEngine->SetParamValueNx(FACE_DETECT_ENABLE, 1);
	m_pEngine->SetParamValueStr(FACE_DETECT_CONFIG_FILE, _T("ocv\\frontalface_alt2.xml"));
	m_pEngine->SetParamValueNx(FACE_DETECT_DRAW, 2);
	m_pEngine->SetParamValueNx(FACE_DETECT_QUALITY, 3);
	m_pEngine->SetParamValueNx(FACE_DETECT_QUALITY_SIZE_MIN, 64);
	m_pEngine->SetParamValueNx(FACE_DETECT_QUALITY_SIZE_MAX, 640);
	m_pEngine->SetParamValueFx(FACE_DETECT_SCALE, 2.0f);

	m_pEngine->SetParamValueFx(FILTER_AM, 16.0f);
	m_pEngine->SetParamValueNx(FILTER_HF_THRESHOLD, 10);
	m_pEngine->SetParamValueNx(FILTER_LF_THRESHOLD, 0);

	m_pEngine->SetParamValueNx(FILTER_CONTOUR, 0);
	m_pEngine->SetParamValueNx(FILTER_CONTOUR_MAX_SPEED, 0);
	m_pEngine->SetParamValueNx(FILTER_STRETCH, 1);
	m_pEngine->SetParamValueNx(FILTER_CROP, 0);
	m_pEngine->SetParamValueNx(FILTER_CROP_POSITION, 0);

	m_pEngine->SetParamValueFx(FILTER_SPACE, 4.0f);
	m_pEngine->SetParamValueFx(FILTER_EXTEND_THRESHOLD, 8.0f);
	m_pEngine->SetParamValueFx(FILTER_EXTEND_FAST_THRESHOLD, 8.0f);

	m_pEngine->SetParamValueNx(FILTER_DISABLE_AM, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_FREQ, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_FAST, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_MAX_ACCUM, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_AVG_ACCUM, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_MIN_ACCUM, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_FFT, 0);
	m_pEngine->SetParamValueNx(FILTER_DISABLE_ENTROPY, 0);

	m_pEngine->SetParamValueNx(FILTER_DISTORTION, 0);
	m_pEngine->SetParamValueNx(FILTER_PERIOD_THRESHOLD, 100);
	m_pEngine->SetParamValueNx(FILTER_FPS_DIVISION_SET, 0);
	m_pEngine->SetParamValueNx(FILTER_HISTNW, 20);

	m_pEngine->SetParamValueNx(FILTER_MOTION_SET, 0);
	m_pEngine->SetParamValueNx(FILTER_MOTION_INTERGRATE_AVG_ACCUM, 1);
	m_pEngine->SetParamValueFx(FILTER_MOTION_LEVEL, 0.1f);

	m_pEngine->SetParamValueNx(FILTER_SKIP_FRAME, 20);
	m_pEngine->SetParamValueNx(FILTER_SINGLE_POINTS, 1);
	m_pEngine->SetParamValueNx(FILTER_STAB_X, 0);
	m_pEngine->SetParamValueNx(FILTER_STAB_Y, 0);

	m_pEngine->SetParamValueFx(FPS_PERIOD, 2.0f);
	m_pEngine->SetParamValueFx(FPS_MAX_BASE, 5.0f);
	m_pEngine->SetParamValueFx(FPS_MAX_FAST, 0.0f);

	m_pEngine->SetParamValueFx(FILTER_GAIN_FACTOR, 1.0f);

	m_pEngine->SetParamValueNx(CRITICAL_MODE_FAST_LEVEL, 150);
	m_pEngine->SetParamValueFx(CRITICAL_MODE_FAST_AREA_RATE, 5.0f);

	m_pEngine->SetParamValueNx(MODE_IMAGE_RESULT, 256);
	m_pEngine->SetParamValueNx(MODE_BASE_CONTOUR_AROUD, 771);
	m_pEngine->SetParamValueNx(MODE_FAST_CONTOUR_AROUD, 1);
	m_pEngine->SetParamValueNx(MODE_FAST_ENABLE, 1);
	m_pEngine->SetParamValueNx(MODE_FAST_FACE_ENABLE, 1);
	m_pEngine->SetParamValueNx(MODE_IMAGE_COLOR, 1);


	m_pEngine->SetParamValueNx(MAX_ACCUM_FRAME_SET, 100);
	m_pEngine->SetParamValueNx(AVG_ACCUM_FRAME_SET, 10);
	m_pEngine->SetParamValueNx(MIN_ACCUM_FRAME_SET, 2);

	m_pEngine->SetParamValueFx(CRITICAL_MAX_ACCUM_CV_RANGE_LEVEL, 0.8f);
	m_pEngine->SetParamValueFx(CRITICAL_MAX_ACCUM_CV_HISTOGRAM_LEVEL, 0.8f);
	m_pEngine->SetParamValueFx(CRITICAL_FREQ_FFT_FAST_LEVEL, 0.8f);

	m_pEngine->SetParamValueNx(STAT_CFG_SIN, 1);
	m_pEngine->SetParamValueNx(STAT_CFG_HISTFN, 0);

	m_pEngine->SetParamValueFx(STAT_RESULT_CRITICAL, 0.65f);
	m_pEngine->SetParamValueFx(CRITICAL_LEVEL, 0.65f);
	m_pEngine->SetParamValueFx(CRITICAL_LEVEL_RANGE, 0.0f);
	m_pEngine->SetParamValueFx(CRITICAL_LEVEL_GRADE, 0.0f);

	m_pEngine->SetParamValueNx(STATEMENT_ENABLE, 0);
	m_pEngine->SetParamValueFx(STATEMENT_LTH, 0.25f);
	m_pEngine->SetParamValueNx(STATEMENT_MODE, 0);
	m_pEngine->SetParamValueNx(STATEMENT_MODE_CMP, 0);
	m_pEngine->SetParamValueNx(STATEMENT_MODE_OPPOSITE, 0);
	m_pEngine->SetParamValueFx(STATEMENT_PERIOD, 10.0f);
	m_pEngine->SetParamValueFx(STATEMENT_PERIOD_BASE, 3.0f);

	m_pEngine->SetParamValueFx(FILTER_BRIGHT_THRESHOLD, 2.0f);
	m_pEngine->SetParamValueFx(FILTER_GAIN_FACTOR, 1.0f);
}


void CVulpesAppDlg::SetParam() {

	for (int i = 0; i < 48; i++) {
		m_pEngine->SetStatCalculateStatus(m_sCalculateParam[i], 1);
		m_pEngine->SetStatRate1(m_sCalculateParam[i], 1.0f);
		m_pEngine->SetStatRate2(m_sCalculateParam[i], 2.0f);
	}
}


void CVulpesAppDlg::InitParamData(PARAM_DATA* pParamData) {
	pParamData->vi =0.0f;
	pParamData->s =0.0f;
	pParamData->fVector.clear();
	pParamData->fAvg =0.0f;
	pParamData->fSum =0.0f;
	pParamData->fSqSum =0.0f;
	pParamData->nCnt = 0;
	pParamData->fv1 =0.0f;
	pParamData->value.fx =0.0f;
	pParamData->value.fy =0.0f;
	pParamData->value.nx = 0;
	pParamData->value.ny = 0;
	memset(&pParamData->level, 0, sizeof(PARAM_LEVEL_VALUE));
	memset(&pParamData->MinMax, 0, sizeof(PARAM_MINMAX_VALUE));

	switch (pParamData->paramId)
	{
	case STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM:
		pParamData->MinMax.fBaseMin = 0.2f;
		pParamData->MinMax.fBaseMax = 0.5f;
		break;
	case STAT_RESULT_MAX_ACCUM_CV_RANGE:
		pParamData->MinMax.fBaseMin = 0.2f;
		pParamData->MinMax.fBaseMax = 0.4f;
		break;
	case STAT_RESULT_FREQ_FFT_FAST:
		pParamData->MinMax.fBaseMin = 0.15f;
		pParamData->MinMax.fBaseMax = 0.4f;
		break;
	case STAT_RESULT_CRITICAL_AVG:
		pParamData->MinMax.fBaseMin = 0.2f;
		pParamData->MinMax.fBaseMax = 0.5f;
		break;
	case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD:
		pParamData->MinMax.fBaseMin = 0.5f;
		pParamData->MinMax.fBaseMax = 1.f;
		break;
	case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE:
		pParamData->MinMax.fBaseMin = 0.4f;
		pParamData->MinMax.fBaseMax = 1.f;
		break;
	case STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD:
		pParamData->MinMax.fBaseMin = 0.1f;
		pParamData->MinMax.fBaseMax = 0.5f;
		break;
	case STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG:
		pParamData->MinMax.fBaseMin = 0.5f;
		pParamData->MinMax.fBaseMax = 1.f;
		break;
	case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
		pParamData->MinMax.fBaseMin = 0.1f;
		pParamData->MinMax.fBaseMax = 0.25f;
		break;
	case STAT_RESULT_FREQ_PERIOD_TIME_CV:
		pParamData->MinMax.fBaseMin = 0.1f;
		pParamData->MinMax.fBaseMax = 0.5f;
		break;
	default:
		pParamData->MinMax.fBaseMin =0.0f;
		pParamData->MinMax.fBaseMax = 1.f;
		break;
	}
}


void CVulpesAppDlg::InitParamDataList() {

	PARAM_DATA* pParamData;
	for (int i = 0; i < PARAM_CNT; i++) {
		pParamData = &m_sParamDataList[i];
		InitParamData(pParamData);

	}
}


void CVulpesAppDlg::InitParamDataMList() {

	PARAM_DATA* pParamData;
	for (int i = 0; i < m_sParamDataMList.GetSize(); i++) {
		pParamData = &m_sParamDataMList[i];
		InitParamData(pParamData);
	}
}


void CVulpesAppDlg::InitCorrelationCoeff(PARAM_DATA *pParamData) {

	switch (pParamData->paramId)
	{
	case STAT_RESULT_FREQ_FFT_FAST:
		pParamData->dCorrFactorA = 0.222286537;
		pParamData->dCorrFactorB = 0.064000556;
		pParamData->dCorrFactorA1 = 0.07256828;
		pParamData->dCorrFactorB1 = 0.207140855;
		break;
	case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
		pParamData->dCorrFactorA = 0.143071942;
		pParamData->dCorrFactorB = -0.117075724;
		pParamData->dCorrFactorA1 = 0.055159379;
		pParamData->dCorrFactorB1 = 0.130475537;
		break;
	case STAT_RESULT_FREQ_PERIOD_TIME_CV:
		pParamData->dCorrFactorA = 0.233518154;
		pParamData->dCorrFactorB = -0.233518154;
		pParamData->dCorrFactorA1 = 0.001383325;
		pParamData->dCorrFactorB1 = 0.216128199;
		break;
	case STAT_RESULT_MAX_ACCUM_CV_RANGE:
		pParamData->dCorrFactorA = 0.111074881;
		pParamData->dCorrFactorB = 0.408202633;
		pParamData->dCorrFactorA1 = 0.464193807;
		pParamData->dCorrFactorB1 = 0.107167843;
		break;
	case STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM:
		pParamData->dCorrFactorA = 0.16384858;
		pParamData->dCorrFactorB = 0.356177494;
		pParamData->dCorrFactorA1 = 0.49588927;
		pParamData->dCorrFactorB1 = 0.14992276;
		break;
	case STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD:
		pParamData->dCorrFactorA = 0.199925451;
		pParamData->dCorrFactorB = 0.485254329;
		pParamData->dCorrFactorA1 = 0.578140296;
		pParamData->dCorrFactorB1 = 0.18766579;
		break;
	case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD:
		pParamData->dCorrFactorA = 0.102699659;
		pParamData->dCorrFactorB = 0.096152052;
		pParamData->dCorrFactorA1 = 0.386264528;
		pParamData->dCorrFactorB1 = 0.087050062;
		break;
	case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE:
		pParamData->dCorrFactorA = 0.107334468;
		pParamData->dCorrFactorB = 0.358474328;
		pParamData->dCorrFactorA1 = 0.513174117;
		pParamData->dCorrFactorB1 = 0.096152052;
		break;
	case STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG:
		pParamData->dCorrFactorA = 0.084944415;
		pParamData->dCorrFactorB = 0.23521761;
		pParamData->dCorrFactorA1 = 0.49234087;
		pParamData->dCorrFactorB1 = 0.07256828;
		break;
	case STAT_RESULT_CRITICAL_AVG:
		pParamData->dCorrFactorA = 0.119272209;
		pParamData->dCorrFactorB = 0.348611343;
		pParamData->dCorrFactorA1 = 0.386264528;
		pParamData->dCorrFactorB1 = 0.087050062;
		break;
	default:
		pParamData->dCorrFactorA = 0.0;
		pParamData->dCorrFactorB = 0.0;
		pParamData->dCorrFactorA1 = 0.0;
		pParamData->dCorrFactorB1 = 0.0;
		break;
	}
}


void CVulpesAppDlg::RestartEngine() {

	m_pEngine->Pause(1);

	if (m_pCamera.CanCmdPreviewStop()) {
		m_pCamera.CmdPreviewStop();
	}
	if (m_pCamera.IsVideo()) {
		m_pCamera.CmdVideoStop();
	}

	float fSpeedRate = (float)m_sParamState.dSpeedRate / 10.f;

	m_pCamera.SetVideoPlaySpeed(fSpeedRate);
	m_pCamera.GetFrameRate(m_sParamState.nSpeedFps, m_sParamState.dSpeedFps);

	m_pEngine->SetFpsMultiple(fSpeedRate);
	m_pEngine->SetFpsSpeed((float)m_sParamState.dSpeedFps);
	for (int i = 0; i < 48; i++) {
		m_pEngine->SetStatCalculateStatus(m_sCalculateParam[i], 1);
		m_pEngine->SetStatRate1(m_sCalculateParam[i], 1.0f);
		m_pEngine->SetStatRate1(m_sCalculateParam[i], 2.0f);
	}

	m_pEngine->Reset();

	int nsel = m_CmbCamera.GetCurSel();
	m_pCamera.CmdDeviceVideo(nsel);

	if (m_pCamera.CanCmdPreview()) {
		m_pCamera.CmdPreview();
	}
	m_pEngine->Pause(0);
}


void CVulpesAppDlg::VideoProcess(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat)
{
	m_sParamState.dCameraTime = dblSampleTime;
	nCameraCnt++;
	BITMAPINFOHEADER *pih = (BITMAPINFOHEADER*)pFormat;
	int w = pih->biWidth;
	int h = pih->biHeight;
	int bpp = pih->biBitCount;
	if (w != m_sParamState.nWidth || h != m_sParamState.nHeight || bpp != m_sParamState.nBpp)
	{
		if (m_pEngine != nullptr) {
			m_pEngine->SetResultImageData(IMAGE_RESULT_SRC, cutils.ImageByte(m_ImageSRC0, w, h, 32), w, h);
			m_pEngine->SetResultImageData(IMAGE_RESULT_AM, cutils.ImageByte(m_ImageSRCA, w, h, 32), w, h);
			m_pEngine->SetResultImageData(IMAGE_RESULT_FREQ, cutils.ImageByte(m_ImageSRCB, w, h, 32), w, h);
		}
		m_sParamState.nWidth = w;
		m_sParamState.nHeight = h;
		m_sParamState.nBpp = bpp;
	}
	if (m_pEngine != nullptr) {
		m_pEngine->AddFrameImage(pBuffer, w, h, bpp, dblSampleTime);
	}
}


void CVulpesAppDlg::EngineInputFrame(BYTE* pImg, int w, int h, double t)
{
	if (!m_pEngine) {
		return;
	}
	m_sParamState.dEngineTime = t;
	if (m_sParamState.nAnalyse) {
		m_sAnalyseMData.fCurrentTime = (float)t;
		AnalyseProcess();
	}
	
}


void CVulpesAppDlg::EngineParamData(VPE_PARAM paramId1, VPE_PARAM paramId2) {

	if (!m_pEngine) {
		return;
	}
	PARAM_DATA* pParamData = &m_sParamDataList[paramId1];
	pParamData->value = m_pEngine->GetParamValue(paramId1);
	if (m_sParamState.nAnalyse) {
		switch (paramId1)
		{
		case FPS_IN:
		case FPS_OUT_FAST:
		case FPS_OUT_BASE:
		case STAT_RESULT_MAX_ACCUM_CV_RANGE:
		case STAT_RESULT_MAX_ACCUM_AM_CV_RANGE:
		case STAT_RESULT_MAX_ACCUM_FREQ_CV_RANGE:
		case STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM:
		case STAT_RESULT_MAX_ACCUM_AM_CV_HISTOGRAM:
		case STAT_RESULT_MAX_ACCUM_FREQ_CV_HISTOGRAM:
		case STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD:
		case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RMSD:
		case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RMSD:
		case STAT_RESULT_FREQ_FFT_FAST:
		case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD:
		case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE:
		case STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG:
		case STAT_RESULT_CRITICAL_AVG:
		case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
		case STAT_RESULT_FREQ_PERIOD_TIME_CV:
			//AddData
			if (_isnan(pParamData->value.fx) || pParamData->value.fx ==0.0f) {
				break;
			}
			pParamData->fVector.push_back(pParamData->value.fx);
			pParamData->fSum += pParamData->value.fx;
			pParamData->nCnt += 1;

			if (pParamData->MinMax.fCalculateMin ==0.0f) {
				pParamData->MinMax.fCalculateMin = pParamData->value.fx;
			}
			pParamData->fAvg = pParamData->fSum / pParamData->nCnt;
			pParamData->MinMax.fCalculateMin = min(pParamData->value.fx, pParamData->MinMax.fCalculateMin);
			pParamData->MinMax.fCalculateMax = max(pParamData->value.fx, pParamData->MinMax.fCalculateMax);

			//CalF9,P7,P8,P6
			if (paramId1 == STAT_RESULT_FREQ_PERIOD_TIME_AVG) {
				m_csResult.Lock();
				m_sParamIdList.Add(paramId1);
				m_csResult.Unlock();
			}
			if (m_sParamState.nMode) {
				if (paramId1 == STAT_RESULT_MAX_ACCUM_FREQ_CV_RANGE) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
				if (paramId1 == STAT_RESULT_MAX_ACCUM_FREQ_CV_HISTOGRAM) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
				if (paramId1 == STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RMSD) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
			}
			else {
				if (paramId1 == STAT_RESULT_MAX_ACCUM_AM_CV_RANGE) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
				if (paramId1 == STAT_RESULT_MAX_ACCUM_AM_CV_HISTOGRAM) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
				if (paramId1 == STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RMSD) {
					m_csResult.Lock();
					m_sParamIdList.Add(paramId1);
					m_csResult.Unlock();
				}
			}
			break;
		default:
			break;
		}

		if (paramId1 == FRAME_BASE) {
			m_csResult.Lock();
			m_sParamIdList.Add(paramId1);
			m_csResult.Unlock();
		}
	}
}


void CVulpesAppDlg::ProcessVarId() {
	PARAM_DATA* pParamData;
	VPE_PARAM paramId, paramId1;
	while (1) {
		while (m_sParamIdList.GetSize() > 0) {
			paramId = PARAM_CNT;
			paramId1 = m_sParamIdList.GetAt(0);
			pParamData = &m_sParamDataList[paramId1];
			m_csResult.Lock();
			m_sParamIdList.RemoveAt(0);
			m_csResult.Unlock();
			switch (paramId1)
			{
			case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
				m_csResult.Lock();
				if ((int)pParamData->fVector.size() > 1) {
					pParamData->fSqSum =0.0f;
					pParamData->fAvg = pParamData->fSum / pParamData->nCnt;
					for (int i = 0; i < (int)pParamData->fVector.size(); i++) {
						pParamData->fSqSum += (pParamData->fVector.at(i) - pParamData->fAvg)*(pParamData->fVector.at(i) - pParamData->fAvg);
					}
					pParamData->s = sqrt(pParamData->fSqSum / pParamData->nCnt);
					m_pEngine->SetParamValueFx(STAT_RESULT_FREQ_PERIOD_TIME_CV, pParamData->s * 10.f);
				}
				m_csResult.Unlock();
				break;
			case FRAME_BASE:
				AddHistMData();
				break;
			default:
				break;
			}
			if (m_sParamState.nMode) {
				switch (paramId1) {
				case STAT_RESULT_MAX_ACCUM_FREQ_CV_RANGE:
					paramId = STAT_RESULT_MAX_ACCUM_CV_RANGE;
					break;
				case STAT_RESULT_MAX_ACCUM_FREQ_CV_HISTOGRAM:
					paramId = STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM;
					break;
				case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RMSD:
					paramId = STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD;
					break;
				default:
					break;
				}
			}
			else {
				switch (paramId1) {
				case STAT_RESULT_MAX_ACCUM_AM_CV_RANGE:
					paramId = STAT_RESULT_MAX_ACCUM_CV_RANGE;
					break;
				case STAT_RESULT_MAX_ACCUM_AM_CV_HISTOGRAM:
					paramId = STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM;
					break;
				case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RMSD:
					paramId = STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD;
					break;
				default:
					break;
				}
			}
			if (paramId < PARAM_CNT) {
				m_pEngine->SetParamValueFx(paramId, pParamData->value.fx);
			}
		}
		Sleep(1);
	}
}


void CVulpesAppDlg::ShowStatusProcess()
{
	CString str;

	if (m_sParamState.nAnalyse) {
		dwCurrentTime = cutils.GetCurrentUnixTimestamp();
	}

	//播放帧
	str.Format(_T("%d"), nCameraCnt);
	GetDlgItem(IDC_EDIT_CMAERAN)->SetWindowTextW(str);

	//已处理标准帧 输入帧
	str.Format(_T("%d|%d"), m_sParamDataList[FRAME_BASE].value.nx, m_sParamDataList[FRAME_IN].value.nx);
	GetDlgItem(IDC_EDIT_NFRAME)->SetWindowTextW(str);

	//最大处理标准帧 最大处理快速帧
	str.Format(_T("%.3f|%.3f"), m_sParamDataList[FPS_MAX_BASE].value.fx, m_sParamDataList[FPS_MAX_FAST].value.fx);
	GetDlgItem(IDC_EDIT_NFRAMEX)->SetWindowTextW(str);

	//帧丢弃
	str.Format(_T("%d|%.3f%s"), m_sParamDataList[FRAME_DROP].value.nx, m_sParamDataList[FRAME_DROP].value.nx * 100.0f / (nCameraCnt + 1),_T("%"));
	GetDlgItem(IDC_EDIT_DROP)->SetWindowTextW(str);

	//系统耗时
	DWORD ddt = (DWORD)(dwCurrentTime - dwStartTime) / 1000;
	str.Format(_T("%.2d:%.2d.%.3d"), ddt / 60, ddt % 60, (dwCurrentTime - dwStartTime) % 1000);
	GetDlgItem(IDC_EDIT_CONSUME)->SetWindowTextW(str);
	//处理加速
	str.Format(_T("%.1f"), (float)m_sParamState.dSpeedRate / 10.f);
	GetDlgItem(IDC_EDIT_SPEED)->SetWindowTextW(str);

	//视频时长
	str.Format(_T("%.2d:%.2d.%.3d"), (int)m_sParamState.dDurationTime / 60, (int)m_sParamState.dDurationTime % 60, (int)(m_sParamState.dDurationTime * 1000.0) % 1000);
	GetDlgItem(IDC_EDIT_DURATION)->SetWindowTextW(str);
	//摄像头播放时长
	str.Format(_T("%.2d:%.2d.%.3d"), (int)m_sParamState.dCameraTime / 60, (int)m_sParamState.dCameraTime % 60, (int)(m_sParamState.dCameraTime * 1000.0) % 1000);
	GetDlgItem(IDC_EDIT_CAMERA)->SetWindowTextW(str);
	//引擎分析时长
	str.Format(_T("%.2d:%.2d.%.3d"), (int)m_sParamState.dEngineTime / 60, (int)m_sParamState.dEngineTime % 60, (int)(m_sParamState.dEngineTime * 1000.0) % 1000);
	GetDlgItem(IDC_EDIT_ENGINE)->SetWindowTextW(str);
}


void CVulpesAppDlg::ShowParamProcess() {
	CString str = _T("");
	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AM_FAST].value.fx);
	GetDlgItem(IDC_EDIT_A1X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AM].value.fx);
	GetDlgItem(IDC_EDIT_A1)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_AM].value.fx);
	GetDlgItem(IDC_EDIT_A4)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_AM].value.fx);
	GetDlgItem(IDC_EDIT_A2)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM].value.fx);
	GetDlgItem(IDC_EDIT_A3)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_AM_FAST].value.fx);
	GetDlgItem(IDC_EDIT_A4X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ].value.fx);
	GetDlgItem(IDC_EDIT_F1)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_FAST].value.fx);
	GetDlgItem(IDC_EDIT_F1X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_DELTA].value.fx);
	GetDlgItem(IDC_EDIT_F1D)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_FREQ].value.fx);
	GetDlgItem(IDC_EDIT_F2)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ].value.fx);
	GetDlgItem(IDC_EDIT_F3)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_MAX].value.fx);
	GetDlgItem(IDC_EDIT_F4)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_FFT].value.fx);
	GetDlgItem(IDC_EDIT_F5)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_FFT_FAST].value.fx);
	GetDlgItem(IDC_EDIT_F5X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_PERIOD_TIME_AVG].value.fx);
	GetDlgItem(IDC_EDIT_F6)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_PERIOD_TIME_SD].value.fx);
	GetDlgItem(IDC_EDIT_F7)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_PERIOD_TIME_DELTA].value.fx);
	GetDlgItem(IDC_EDIT_F8)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FREQ_PERIOD_TIME_CV].value.fx);
	GetDlgItem(IDC_EDIT_F9)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MIN_ACCUM_AM_CV_VALUE_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S1)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MIN_ACCUM_AM_CV_NUM_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S4)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_AM_CV_VALUE_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S2)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_AM_CV_NUM_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S5)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_CV_VALUE_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S3)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_CV_NUM_RATE].value.fx);
	GetDlgItem(IDC_EDIT_S6)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_CV_DIFFERENCE].value.fx);
	GetDlgItem(IDC_EDIT_S7)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_FREQ_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P1)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AVG_ACCUM_FREQ_SCALE].value.fx);
	GetDlgItem(IDC_EDIT_P3)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P2)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_SCALE].value.fx);
	GetDlgItem(IDC_EDIT_P4)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_STATEMENT_CRITICAL].value.fx);
	GetDlgItem(IDC_EDIT_P5)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_CV_RANGE].value.fx);
	GetDlgItem(IDC_EDIT_P6)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_CV_RANGE].value.fx);
	GetDlgItem(IDC_EDIT_P6A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_CV_RANGE].value.fx);
	GetDlgItem(IDC_EDIT_P6F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM].value.fx);
	GetDlgItem(IDC_EDIT_P7)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_CV_HISTOGRAM].value.fx);
	GetDlgItem(IDC_EDIT_P7A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_CV_HISTOGRAM].value.fx);
	GetDlgItem(IDC_EDIT_P7F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P8)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P8A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P8F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_HISTOGRAM_LOG_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P9)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_LOG_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P9A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_LOG_RMSD].value.fx);
	GetDlgItem(IDC_EDIT_P9F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_HISTOGRAM_ENTROPY].value.fx);
	GetDlgItem(IDC_EDIT_P10)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_ENTROPY].value.fx);
	GetDlgItem(IDC_EDIT_P10A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_ENTROPY].value.fx);
	GetDlgItem(IDC_EDIT_P10F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_HISTOGRAM_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P11)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P11A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P11F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_HISTOGRAM_LOG_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P12)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_LOG_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P12A)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_LOG_RSD].value.fx);
	GetDlgItem(IDC_EDIT_P12F)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_HIST_CRITICAL_VALUE_RATE].value.fx);
	GetDlgItem(IDC_EDIT_P13)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_HIST_CRITICAL_CNT_RATE].value.fx);
	GetDlgItem(IDC_EDIT_P14)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_HIST_ALL_CNT_RATE].value.fx);
	GetDlgItem(IDC_EDIT_P15)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD].value.fx);
	GetDlgItem(IDC_EDIT_P16)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE].value.fx);
	GetDlgItem(IDC_EDIT_P17)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG].value.fx);
	GetDlgItem(IDC_EDIT_P18)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_AVG].value.fx);
	GetDlgItem(IDC_EDIT_P19)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_STATEMENT_QUANTITY_INDICATOR].value.fx);
	GetDlgItem(IDC_EDIT_P20)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_ONCOLOGY].value.fx);
	GetDlgItem(IDC_EDIT_P21)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_PG].value.fx);
	GetDlgItem(IDC_EDIT_P22)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_AVG].value.fx);
	GetDlgItem(IDC_EDIT_P23)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_AM_HISTOGRAM_DIFFERENCE_NORMALIZE].value.fx);
	GetDlgItem(IDC_EDIT_P24)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_NORMALIZE].value.fx);
	GetDlgItem(IDC_EDIT_P25)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION].value.fx);
	GetDlgItem(IDC_EDIT_P26)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_STABILITY].value.fx);
	GetDlgItem(IDC_EDIT_P27)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_MAX_ACCUM_FREQ_STATISFY].value.fx);
	GetDlgItem(IDC_EDIT_P28)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_DEPRESSION].value.fx);
	GetDlgItem(IDC_EDIT_P29)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_CRITICAL_HEALTH_HAPPINESS].value.fx);
	GetDlgItem(IDC_EDIT_P30)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_SIN].value.fx);
	GetDlgItem(IDC_EDIT_SIN)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FAST_PIX_MIDDLE_CV].value.fx);
	GetDlgItem(IDC_EDIT_C1)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FAST_PIX_MIDDLE_CV_DELTA].value.fx);
	GetDlgItem(IDC_EDIT_C1X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FAST_PIX_BOTH_CV].value.fx);
	GetDlgItem(IDC_EDIT_C2)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FAST_PIX_BOTH_CV_DELTA].value.fx);
	GetDlgItem(IDC_EDIT_C2X)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[STAT_RESULT_FAST_PIX_CV].value.fx);
	GetDlgItem(IDC_EDIT_C3)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[FPS_IN].value.fx);
	GetDlgItem(IDC_EDIT_FPS_IN)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[FPS_OUT_FAST].value.fx);
	GetDlgItem(IDC_EDIT_FPS_FAST)->SetWindowTextW(str);

	str.Format(_T("%0.6f"), m_sParamDataList[FPS_OUT_BASE].value.fx);
	GetDlgItem(IDC_EDIT_FPS_BASE)->SetWindowTextW(str);

}


void CVulpesAppDlg::ShowResultProcess() {

	CString str;
	PARAM_DATA *pParamData;

	float m =0.0f, s =0.0f, vi =0.0f, bMin =0.0f, bMax =0.0f, cMin =0.0f, cMax =0.0f;
	float fMul = 100.f;

	for (int i = 0; i < m_sParamDataMList.GetSize(); i++) {
		pParamData = &m_sParamDataMList[i];
		m = pParamData->value.fx*fMul;
		s = pParamData->s*fMul;
		vi = pParamData->vi*fMul;
		bMin = pParamData->MinMax.fBaseMin*fMul;
		bMax = pParamData->MinMax.fBaseMax*fMul;
		cMin = pParamData->MinMax.fCalculateMin*fMul;
		cMax = pParamData->MinMax.fCalculateMax*fMul;
		if (i == 0 || i == 1 || i == 2) {
			m /= fMul;
			s /= fMul;
			vi /= fMul;
			cMin /= fMul;
			cMax /= fMul;
		}

		str.Format(_T("%0.3f  %0.3f  %0.3f  %0.3f  %0.3f  %0.f  %0.f"), m, s, vi, cMin, cMax, bMin, bMax);

		switch (i)
		{
		case 0:
			GetDlgItem(IDC_EDIT_FPSIN)->SetWindowTextW(str);
			break;
		case 1:
			GetDlgItem(IDC_EDIT_FPSOUTF)->SetWindowTextW(str);
			break;
		case 2:
			GetDlgItem(IDC_EDIT_FPSOUTR)->SetWindowTextW(str);
			break;
		case 3:
			GetDlgItem(IDC_EDIT_AGGRESSION)->SetWindowTextW(str);
			break;
		case 4:
			GetDlgItem(IDC_EDIT_STRESS)->SetWindowTextW(str);
			break;
		case 5:
			GetDlgItem(IDC_EDIT_TENSION)->SetWindowTextW(str);
			break;
		case 6:
			GetDlgItem(IDC_EDIT_SUSPECT)->SetWindowTextW(str);
			break;
		case 7:
			GetDlgItem(IDC_EDIT_BALANCE)->SetWindowTextW(str);
			break;
		case 8:
			GetDlgItem(IDC_EDIT_CHARM)->SetWindowTextW(str);
			break;
		case 9:
			GetDlgItem(IDC_EDIT_ENERGY)->SetWindowTextW(str);
			break;
		case 10:
			GetDlgItem(IDC_EDIT_REGULATION)->SetWindowTextW(str);
			break;
		case 11:
			GetDlgItem(IDC_EDIT_INHIBITION)->SetWindowTextW(str);
			break;
		case 12:
			GetDlgItem(IDC_EDIT_NEUROTICISM)->SetWindowTextW(str);
			break;
		case 13:
			GetDlgItem(IDC_EDIT_HEALTH)->SetWindowTextW(str);
			break;
		case 14:
			GetDlgItem(IDC_EDIT_HEALTH1)->SetWindowTextW(str);
			break;
		case 15:
			GetDlgItem(IDC_EDIT_VI)->SetWindowTextW(str);
			break;
		case 16:
			GetDlgItem(IDC_EDIT_EXTROVERSION)->SetWindowTextW(str);
			break;
		case 17:
			GetDlgItem(IDC_EDIT_STABILITY)->SetWindowTextW(str);
			break;
		default:
			break;
		}
	}
}


void CVulpesAppDlg::ShowDebugProcess() {

	//debug
	CString cstr;
	cstr.Format(_T("A1X:%0.3f|%0.3f|%0.3f"), m_sParamDataList[STAT_RESULT_AM_FAST].value.fx, (float)m_sParamState.dEngineTime, m_sParamDataList[FPS_OUT_FAST].value.fx);
	string str = CW2A(cstr);
	LOG4CPLUS_INFO(theApp.vulpesLog, str.c_str());
}


void CVulpesAppDlg::InitAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData) {
	paramAnalyseData->fBaseDeltaScale =0.0f;
	paramAnalyseData->fFastDeltaScale =0.0f;
	paramAnalyseData->fStartTime =0.0f;
	paramAnalyseData->fBaseFps =0.0f;
	paramAnalyseData->fFastFps =0.0f;
	paramAnalyseData->fBaseMScale =0.0f;
	paramAnalyseData->fFastMScale =0.0f;
	paramAnalyseData->fBaseMulitScale =0.0f;
	paramAnalyseData->fFastMulitScale =0.0f;
	paramAnalyseData->nCount = 0;
	paramAnalyseData->fBaseRMSDScale =0.0f;
	paramAnalyseData->fFastRMSDScale =0.0f;
	paramAnalyseData->fBaseMaxValue =0.0f;
	paramAnalyseData->fFastMaxValue =0.0f;
	paramAnalyseData->fBaseMinValue = 255.f;
	paramAnalyseData->fFastMinValue = 255.f;
	paramAnalyseData->nBaseMaxIndex =0;
	paramAnalyseData->nFastMaxIndex =0;
	paramAnalyseData->nBaseMinIndex =0;
	paramAnalyseData->nFastMinIndex =0;

	memset(paramAnalyseData->fBaseHistogramData, 0, sizeof(float) * 256);
	memset(paramAnalyseData->fFastHistogramData, 0, sizeof(float) * 256);
}


void CVulpesAppDlg::AddParamAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData, float* fBaseHistogramData, float* fFastHistogramData, float fBaseFps, float fFastFps) {

	for (int i = 0; i < 256; i++) {
		paramAnalyseData->fBaseHistogramData[i] += fBaseHistogramData[i];
		paramAnalyseData->fFastHistogramData[i] += fFastHistogramData[i];
	}
	paramAnalyseData->fBaseFps += fBaseFps;
	paramAnalyseData->fFastFps += fFastFps;

	paramAnalyseData->nCount++;
}


void CVulpesAppDlg::CalculateParamAnalyseData(PARAM_ANALYSE_DATA* paramAnalyseData) {

	int nBaseMaxIndex = 0, nFastMaxIndex = 0, nBaseMinIndex = 0, nFastMinIndex = 0;
	float fBaseMaxValue = 0.0f, fFastMaxValue = 0.0f, fBaseMinValue = 255.0f, fFastMinValue = 255.0f;
	float fBaseHistogramSum = 0.0f, fFastHistogramSum = 0.0f;
	float fBaseMScale = 0.0f, fFastMScale = 0.0f;
	float fBaseMulitScale = 0.0f, fFastMulitScale = 0.0f;
	float fBaseDeltaScale = 0.0f, fFastDeltaScale = 0.0f;

	for (int i = 0; i < 255; i++) {

		paramAnalyseData->fBaseHistogramData[i] /= (float)paramAnalyseData->nCount;
		paramAnalyseData->fFastHistogramData[i] /= (float)paramAnalyseData->nCount;

		if (paramAnalyseData->fBaseHistogramData[i] > fBaseMaxValue) {
			fBaseMaxValue = paramAnalyseData->fBaseHistogramData[i];
			nBaseMaxIndex = i;
		}
		if (paramAnalyseData->fFastHistogramData[i] > fFastMaxValue) {
			fFastMaxValue = paramAnalyseData->fFastHistogramData[i];
			nFastMaxIndex = i;
		}

		if (paramAnalyseData->fBaseHistogramData[i] < fBaseMinValue) {
			fBaseMinValue = paramAnalyseData->fBaseHistogramData[i];
			nBaseMinIndex = i;
		}
		if (paramAnalyseData->fFastHistogramData[i] < fFastMinValue) {
			fFastMinValue = paramAnalyseData->fFastHistogramData[i];
			nFastMinIndex = i;
		}
	}

	paramAnalyseData->fBaseMaxValue = fBaseMaxValue;
	paramAnalyseData->fBaseMinValue = fBaseMinValue;
	paramAnalyseData->nBaseMaxIndex = nBaseMaxIndex;
	paramAnalyseData->nBaseMinIndex = nBaseMinIndex;

	paramAnalyseData->fFastMaxValue = fFastMaxValue;
	paramAnalyseData->fFastMinValue = fFastMinValue;
	paramAnalyseData->nFastMaxIndex = nFastMaxIndex;
	paramAnalyseData->nFastMinIndex = nFastMinIndex;

	paramAnalyseData->fBaseFps /= (float)paramAnalyseData->nCount;
	paramAnalyseData->fFastFps /= (float)paramAnalyseData->nCount;

	float fBaseScale = paramAnalyseData->fBaseFps / 255.f;
	float fFastScale = paramAnalyseData->fFastFps / 255.f;

	for (int i = 0; i < 255; i++) {
		fBaseHistogramSum += paramAnalyseData->fBaseHistogramData[i];
		fFastHistogramSum += paramAnalyseData->fFastHistogramData[i];
	}
	for (int i = 0; i < 255; i++) {
		fBaseMulitScale += i * paramAnalyseData->fBaseHistogramData[i];// / fBaseHistogramSum;
		fFastMulitScale += i * paramAnalyseData->fFastHistogramData[i];// / fFastHistogramSum;

		fBaseDeltaScale += (i * i) * paramAnalyseData->fBaseHistogramData[i];// / fBaseHistogramSum;
		fFastDeltaScale += (i * i) * paramAnalyseData->fFastHistogramData[i];// / fFastHistogramSum;
	}


	if (fBaseHistogramSum !=0.0f) {
		fBaseMulitScale /= (fBaseHistogramSum * 255.f);
		fBaseMScale = fBaseMulitScale * paramAnalyseData->fBaseFps;
		fBaseDeltaScale = (fBaseDeltaScale / fBaseHistogramSum)  * fBaseScale * fBaseScale - fBaseMScale * fBaseMScale;
	}
	else
	{
		fBaseMScale =0.0f;
		fBaseMulitScale =0.0f;
		fBaseDeltaScale =0.0f;
	}

	if (fFastHistogramSum !=0.0f)
	{
		fFastMulitScale /= (fFastHistogramSum * 255.f);
		fFastMScale = fFastMulitScale * paramAnalyseData->fFastFps;
		fFastDeltaScale = fFastDeltaScale * fFastScale * fFastScale / fFastHistogramSum - fFastMScale * fFastMScale;
	}
	else
	{
		fFastMScale =0.0f;
		fFastMulitScale =0.0f;
		fFastDeltaScale =0.0f;
	}

	if (fBaseDeltaScale <0.0f) fBaseDeltaScale =0.0f;
	if (fFastDeltaScale <0.0f) fFastDeltaScale =0.0f;

	paramAnalyseData->fBaseMScale = fBaseMScale;
	paramAnalyseData->fFastMScale = fFastMScale;

	paramAnalyseData->fBaseMulitScale = fBaseMulitScale;
	paramAnalyseData->fFastMulitScale = fFastMulitScale;

	paramAnalyseData->fBaseDeltaScale = fBaseDeltaScale;
	paramAnalyseData->fFastDeltaScale = fFastDeltaScale;

	paramAnalyseData->fBaseRMSDScale = sqrt(fBaseDeltaScale);
	paramAnalyseData->fFastRMSDScale = sqrt(fFastDeltaScale);
}


void CVulpesAppDlg::AddHistMData() {
	float fBaseFps =0.0f;
	float fFastFps =0.0f;
	float fBaseHistogramData[256];
	float fFastHistogramData[256];
	CString strBaseParamId, strFastParamId;
	int nBaseId, nFastId;
	int nBaseHistogramData[256], nFastHistogramData[256];

	memset(nBaseHistogramData, 0, sizeof(int) << 8);
	memset(nFastHistogramData, 0, sizeof(int) << 8);

	nBaseId = HISTOGRAM_MAX_ACCUM_AM_AVG;
	nFastId = HISTOGRAM_MAX_ACCUM_AM_FAST;

	strBaseParamId = _T("MaxAccumHistogramAmAvg");
	strFastParamId = _T("MaxAccumHistogramAmFast");

	if (m_sParamState.nMode) {

		nBaseId = HISTOGRAM_MAX_ACCUM_FREQ_AVG;
		nFastId = HISTOGRAM_MAX_ACCUM_FREQ_FAST;
		strBaseParamId = _T("MaxAccumHistogramFreqAvg");
		strFastParamId = _T("MaxAccumHistogramFreqFast");
	}

	m_pEngine->GetHistogramStat(nBaseId, nBaseHistogramData, &fBaseFps);
	m_pEngine->GetHistogramStat(nFastId, nFastHistogramData, &fFastFps);

	Int2Float(fBaseHistogramData, nBaseHistogramData, 256);
	Int2Float(fFastHistogramData, nFastHistogramData, 256);

	PARAM_ANALYSE_DATA paramAnalyseData;
	InitAnalyseData(&paramAnalyseData);
	paramAnalyseData.nParamId1 = nBaseId;
	paramAnalyseData.nParamId2 = nFastId;
	paramAnalyseData.strParamId1 = strBaseParamId;
	paramAnalyseData.strParamId2 = strFastParamId;
	AddParamAnalyseData(&paramAnalyseData, fBaseHistogramData, fFastHistogramData, fBaseFps, fFastFps);
	paramAnalyseData.fStartTime = (float)m_sParamState.dEngineTime;
	m_sAnalyseMData.paramAnalyseDataList.push_back(paramAnalyseData);

	float fThreshold = 30.f;
	float fDeltaTime =0.0f;
	float fLastTime = (float)m_sParamState.dEngineTime;
	int i = 0;
	int nOldCnt = m_sAnalyseMData.paramAnalyseDataList.size();
	do
	{
		fDeltaTime = fLastTime - m_sAnalyseMData.paramAnalyseDataList.at(i).fStartTime;
		if (fDeltaTime <= fThreshold) {
			break;
		}
		i++;
	} while (1);
	if (i > 0) {
		m_sAnalyseMData.paramAnalyseDataList.erase(m_sAnalyseMData.paramAnalyseDataList.begin(), m_sAnalyseMData.paramAnalyseDataList.begin() + i);
	}
}


void CVulpesAppDlg::CalculatepHistMData() {

	int nParamAnalyseDataCnt = m_sAnalyseMData.paramAnalyseDataList.size();

	if (nParamAnalyseDataCnt < 1) {
		return;
	}
	float fThreshold = 10.f;
	float fFirstTime = m_sAnalyseMData.paramAnalyseDataList.at(0).fStartTime;
	float fLastTime = m_sAnalyseMData.paramAnalyseDataList.at(nParamAnalyseDataCnt - 1).fStartTime;
	float fDeltaTime = fLastTime - fFirstTime;

	do {
		if (fDeltaTime > fThreshold * 3.0f) {
			break;
		}
		fThreshold -= 1.0f;
	} while (fThreshold >0.0f);

	if (fThreshold < 1.0f) {
		fThreshold = 1.0f;
	}

	PARAM_ANALYSE_DATA* paramAnalyseData;
	PARAM_ANALYSE_DATA paramAnalyseDataMSum;
	PARAM_ANALYSE_DATA paramAnalyseDataM;
	HIST_FFT_DATA pHistFFTData;

	m_sAnalyseMData.pHistFFTList.clear();
	m_sAnalyseMData.pHistFFTMaxList.clear();

	InitAnalyseData(&paramAnalyseDataMSum);

	for (int i = 0; i < (int)m_sAnalyseMData.paramAnalyseDataList.size(); i++) {

		paramAnalyseData = &m_sAnalyseMData.paramAnalyseDataList.at(i);
		AddParamAnalyseData(&paramAnalyseDataMSum, paramAnalyseData->fBaseHistogramData, paramAnalyseData->fFastHistogramData, paramAnalyseData->fBaseFps, paramAnalyseData->fFastFps);

		fDeltaTime = fabs(paramAnalyseData->fStartTime - fFirstTime);
		if (fDeltaTime < fThreshold) {
			continue;
		}

		InitAnalyseData(&paramAnalyseDataM);

		for (int j = i; j < (int)m_sAnalyseMData.paramAnalyseDataList.size(); j++) {
			fDeltaTime = fabs(m_sAnalyseMData.paramAnalyseDataList.at(j).fStartTime - paramAnalyseData->fStartTime + 0.001f);
			if (fDeltaTime >= fThreshold) {
				break;
			}
			AddParamAnalyseData(&paramAnalyseDataM, m_sAnalyseMData.paramAnalyseDataList.at(j).fBaseHistogramData, m_sAnalyseMData.paramAnalyseDataList.at(j).fFastHistogramData, m_sAnalyseMData.paramAnalyseDataList.at(j).fBaseFps, m_sAnalyseMData.paramAnalyseDataList.at(j).fFastFps);
		}

		if (paramAnalyseDataM.nCount > 0) {
			CalculateParamAnalyseData(&paramAnalyseDataM);
			m_sAnalyseMData.pHistMList.push_back(paramAnalyseDataM);
		}
	}

	int npHistMCnt = (int)m_sAnalyseMData.pHistMList.size();

	if (npHistMCnt >= 1) {
		int nHalfCnt = (int)(npHistMCnt - 1) >> 1;
		for (int i = 0; i < npHistMCnt; i++) {
			pHistFFTData.fBaseDeltaScale = m_sAnalyseMData.pHistMList.at(i).fBaseDeltaScale;
			pHistFFTData.fFastDeltaScale = m_sAnalyseMData.pHistMList.at(i).fFastDeltaScale;
			pHistFFTData.fBaseMScale = m_sAnalyseMData.pHistMList.at(i).fBaseMScale;
			pHistFFTData.fFastMScale = m_sAnalyseMData.pHistMList.at(i).fFastMScale;
			pHistFFTData.fBaseMaxValue = m_sAnalyseMData.pHistMList.at(i).fBaseMaxValue;
			pHistFFTData.fFastMaxValue = m_sAnalyseMData.pHistMList.at(i).fFastMaxValue;
			pHistFFTData.fBaseMulitScale = m_sAnalyseMData.pHistMList.at(i).fBaseMulitScale;
			pHistFFTData.fFastMulitScale = m_sAnalyseMData.pHistMList.at(i).fFastMulitScale;
			pHistFFTData.nBaseMaxIndex = m_sAnalyseMData.pHistMList.at(i).nBaseMaxIndex;
			pHistFFTData.nFastMaxIndex = m_sAnalyseMData.pHistMList.at(i).nFastMaxIndex;
			pHistFFTData.fBaseRMSDScale = m_sAnalyseMData.pHistMList.at(i).fBaseRMSDScale;
			pHistFFTData.fFastRMSDScale = m_sAnalyseMData.pHistMList.at(i).fFastRMSDScale;
			pHistFFTData.fBaseFps = m_sAnalyseMData.pHistMList.at(i).fBaseFps;
			pHistFFTData.fFastFps = m_sAnalyseMData.pHistMList.at(i).fFastFps;
			m_sAnalyseMData.pHistFFTList.push_back(pHistFFTData);

			if (nHalfCnt == i) {
				m_sAnalyseMData.paramAnalyseData.fBaseDeltaScale = m_sAnalyseMData.pHistMList.at(i).fBaseDeltaScale;
				m_sAnalyseMData.paramAnalyseData.fFastDeltaScale = m_sAnalyseMData.pHistMList.at(i).fFastDeltaScale;
				m_sAnalyseMData.paramAnalyseData.fBaseFps = m_sAnalyseMData.pHistMList.at(i).fBaseFps;
				m_sAnalyseMData.paramAnalyseData.fFastFps = m_sAnalyseMData.pHistMList.at(i).fFastFps;
				m_sAnalyseMData.paramAnalyseData.fBaseMScale = m_sAnalyseMData.pHistMList.at(i).fBaseMScale;
				m_sAnalyseMData.paramAnalyseData.fFastMScale = m_sAnalyseMData.pHistMList.at(i).fFastMScale;
				m_sAnalyseMData.paramAnalyseData.fBaseMaxValue = m_sAnalyseMData.pHistMList.at(i).fBaseMaxValue;
				m_sAnalyseMData.paramAnalyseData.fFastMaxValue = m_sAnalyseMData.pHistMList.at(i).fFastMinValue;
				m_sAnalyseMData.paramAnalyseData.fBaseMinValue = m_sAnalyseMData.pHistMList.at(i).fBaseMinValue;
				m_sAnalyseMData.paramAnalyseData.fFastMinValue = m_sAnalyseMData.pHistMList.at(i).fFastMinValue;
				m_sAnalyseMData.paramAnalyseData.fBaseMulitScale = m_sAnalyseMData.pHistMList.at(i).fBaseMulitScale;
				m_sAnalyseMData.paramAnalyseData.fFastMulitScale = m_sAnalyseMData.pHistMList.at(i).fFastMulitScale;
				m_sAnalyseMData.paramAnalyseData.nBaseMaxIndex = m_sAnalyseMData.pHistMList.at(i).nBaseMaxIndex;
				m_sAnalyseMData.paramAnalyseData.nFastMaxIndex = m_sAnalyseMData.pHistMList.at(i).nFastMaxIndex;
				m_sAnalyseMData.paramAnalyseData.nBaseMinIndex = m_sAnalyseMData.pHistMList.at(i).nBaseMinIndex;
				m_sAnalyseMData.paramAnalyseData.nFastMinIndex = m_sAnalyseMData.pHistMList.at(i).nFastMinIndex;
				m_sAnalyseMData.paramAnalyseData.fBaseRMSDScale = m_sAnalyseMData.pHistMList.at(i).fBaseRMSDScale;
				m_sAnalyseMData.paramAnalyseData.fFastRMSDScale = m_sAnalyseMData.pHistMList.at(i).fFastRMSDScale;

				if (nParamAnalyseDataCnt > 0) {
					for (int j = 0; j < 256; j++) {
						m_sAnalyseMData.paramAnalyseData.fBaseHistogramData[j] = m_sAnalyseMData.pHistMList.at(i).fBaseHistogramData[j] / nParamAnalyseDataCnt;
						m_sAnalyseMData.paramAnalyseData.fFastHistogramData[j] = m_sAnalyseMData.pHistMList.at(i).fFastHistogramData[j] / nParamAnalyseDataCnt;
					}
				}
				m_sAnalyseMData.pHistFFTMaxList.push_back(pHistFFTData);

			}
		}
	}
	else {
		CalculateParamAnalyseData(&paramAnalyseDataMSum);
		pHistFFTData.fBaseDeltaScale = paramAnalyseDataMSum.fBaseDeltaScale;
		pHistFFTData.fFastDeltaScale = paramAnalyseDataMSum.fFastDeltaScale;
		pHistFFTData.fBaseMScale = paramAnalyseDataMSum.fBaseMScale;
		pHistFFTData.fFastMScale = paramAnalyseDataMSum.fFastMScale;
		pHistFFTData.fBaseMaxValue = paramAnalyseDataMSum.fBaseMaxValue;
		pHistFFTData.fFastMaxValue = paramAnalyseDataMSum.fFastMaxValue;
		pHistFFTData.fBaseMulitScale = paramAnalyseDataMSum.fBaseMulitScale;
		pHistFFTData.fFastMulitScale = paramAnalyseDataMSum.fFastMulitScale;
		pHistFFTData.nBaseMaxIndex = paramAnalyseDataMSum.nBaseMaxIndex;
		pHistFFTData.nFastMaxIndex = paramAnalyseDataMSum.nFastMaxIndex;
		pHistFFTData.fBaseRMSDScale = paramAnalyseDataMSum.fBaseRMSDScale;
		pHistFFTData.fFastRMSDScale = paramAnalyseDataMSum.fFastRMSDScale;
		pHistFFTData.fBaseFps = paramAnalyseDataMSum.fBaseFps;
		pHistFFTData.fFastFps = paramAnalyseDataMSum.fFastFps;
		m_sAnalyseMData.pHistFFTMaxList.push_back(pHistFFTData);
	}
}


void CVulpesAppDlg::CalculateHealthData() {

	double dHealthOncology = 0.0;
	double dHealthPG = 0.0;
	double dHealthExtraversion = 0.0;
	double dP27 = 0.0;
	double dHealthExtraversionMax = 0.0;
	double dP27Max = 0.0;
	float fHealthAvg =0.0f;

	BaseCalHealthExtraversionStability(&dHealthExtraversion, &dP27);
	MaxCalHealthExtraversionStability(&dHealthExtraversionMax, &dP27Max);

	dHealthExtraversion = (dHealthExtraversion + dHealthExtraversionMax) / 2.0;
	dP27 = (dP27 + dP27Max) / 2.0;

	PARAM_DATA* pParamData;

	for (int i = 0; i < m_sParamDataMList.GetSize(); i++) {

		pParamData = &m_sParamDataMList[i];
		InitCorrelationCoeff(pParamData);

		switch (pParamData->paramId)
		{
		case STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_CV_RANGE:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_FREQ_FFT_FAST:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_CRITICAL_AVG:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_FREQ_PERIOD_TIME_CV:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_ONCOLOGY:
			if (dHealthOncology > 1.0) {
				dHealthOncology = 1.0;
			}
			pParamData->value.fx = (float)dHealthOncology;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_PG:
			if (dHealthPG > 1.0) {
				dHealthPG = 1.0;
			}
			pParamData->value.fx = (float)dHealthPG;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_AVG:
			pParamData->value.fx = fHealthAvg / 10.f;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION:
			pParamData->value.fx = (float)dHealthExtraversion;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_STABILITY:
			pParamData->value.fx = (float)dP27;
			break;
		default:
			break;
		}
	}
}


void CVulpesAppDlg::BaseCalHealthExtraversionStability(double* pdValue1, double* pdValue2) {

	HIST_FFT_DATA* pHistFFTData;

	float fBaseFps = 0.0f, fBaseMScale = 0.0f, fVal = 0.0f, fSumM = 0.0f, fSumVal = 0.0f;
	int nValidCnt = 0;

	int npHistFFTCnt = (int)m_sAnalyseMData.pHistFFTList.size();

	for (int i = 0; i < npHistFFTCnt; i += 2) {

		pHistFFTData = &m_sAnalyseMData.pHistFFTList.at(i);
		fBaseFps = pHistFFTData->fBaseFps;
		if (fBaseFps <=0.0f) {
			continue;
		}
		fBaseMScale = pHistFFTData->fBaseMScale;
		if (fBaseMScale <=0.0f) {
			continue;
		}
		fBaseMScale /= fBaseFps;
		fVal = 1.f - pHistFFTData->fBaseRMSDScale * 5.f / fBaseFps;
		if (fVal > 0.0) {
			if (fVal > 1.0f) {
				fVal = 1.0f;
			}
		}
		else {
			fVal = 0.0f;
		}
		nValidCnt++;
		fSumM += fBaseMScale;
		fSumVal += fVal;
	}

	fVal =0.0f;
	fBaseMScale =0.0f;
	if (nValidCnt) {
		fVal = fSumVal / (nValidCnt * 0.8f);
		fBaseMScale = fSumM / (nValidCnt * 0.8f);
		if (fVal >0.0f) {
			if (fVal > 1.0f) {
				fVal = 1.0f;
			}
		}
		if (fBaseMScale >0.0f) {
			if (fBaseMScale > 1.0f) {
				fBaseMScale = 1.0f;
			}
		}
		fBaseMScale = 1.f - fBaseMScale;
	}
	*pdValue1 = fVal;
	*pdValue2 = fBaseMScale;
}


void CVulpesAppDlg::MaxCalHealthExtraversionStability(double* pdValue1, double* pdValue2) {

	HIST_FFT_DATA* pHistFFTData;

	float fBaseFps = 0.0f, fBaseMScale = 0.0f, fVal = 0.0f, fSumM = 0.0f, fSumVal = 0.0f;
	int nValidCnt = 0;

	int npHistFFTCnt = (int)m_sAnalyseMData.pHistFFTMaxList.size();

	for (int i = 0; i < npHistFFTCnt; i += 2) {

		pHistFFTData = &m_sAnalyseMData.pHistFFTMaxList.at(i);
		fBaseFps = pHistFFTData->fBaseFps;
		if (fBaseFps <=0.0f) {
			continue;
		}
		fBaseMScale = pHistFFTData->fBaseMScale;
		if (fBaseMScale <=0.0f) {
			continue;
		}
		fBaseMScale = pHistFFTData->nBaseMaxIndex * 1.f / 255.f;
		fVal = 1.f - pHistFFTData->fBaseRMSDScale * 5.f / fBaseFps;

		if (fVal >0.0f) {
			if (fVal > 1.0f) {
				fVal = 1.0f;
			}
		}
		else {
			fVal =0.0f;
		}
		nValidCnt++;
		fSumM += fBaseMScale;
		fSumVal += fVal;
	}

	fVal =0.0f;
	fBaseMScale =0.0f;
	if (nValidCnt) {
		fVal = fSumVal / (nValidCnt * 0.8f);
		fBaseMScale = fSumM / (nValidCnt * 0.8f);
		if (fVal >0.0f) {
			if (fVal > 1.0f) {
				fVal = 1.0f;
			}
		}
		if (fBaseMScale >0.0f) {
			if (fBaseMScale > 1.0f) {
				fBaseMScale = 1.0f;
			}
		}
		fBaseMScale = 1.0f - fBaseMScale;
	}
	*pdValue1 = fVal;
	*pdValue2 = fBaseMScale;
}


void CVulpesAppDlg::CalculateMResult() {

	double dHealthOncology = 0.0;
	double dHealthPG = 0.0;
	double dHealthExtraversion = 0.0;
	double dP27 = 0.0;
	double dHealthExtraversionMax = 0.0;
	double dP27Max = 0.0;
	float fHealthAvg =0.0f;
	PARAM_DATA *pParamData;

	CalculatepHistMData();

	for (int i = 0; i < m_sParamDataMList.GetSize(); i++) {
		pParamData = &m_sParamDataMList[i];
		pParamData->value = m_sParamDataList[pParamData->paramId].value;
		pParamData->MinMax = m_sParamDataList[pParamData->paramId].MinMax;
		pParamData->nCnt = m_sParamDataList[pParamData->paramId].nCnt;
		pParamData->fSum = m_sParamDataList[pParamData->paramId].fSum;
		pParamData->fAvg = m_sParamDataList[pParamData->paramId].fAvg;
		pParamData->fSqSum =0.0f;
		if (pParamData->nCnt > 1) {
			for (int i = 0; i < (int)m_sParamDataList[pParamData->paramId].fVector.size(); i++) {
				pParamData->fSqSum += (m_sParamDataList[pParamData->paramId].fVector.at(i) - pParamData->fAvg)*(m_sParamDataList[pParamData->paramId].fVector.at(i) - pParamData->fAvg);
			}
			pParamData->s = (float)sqrt(pParamData->fSqSum / pParamData->nCnt);
		}
		if (pParamData->value.fx !=0.0f) {
			pParamData->vi = pParamData->s / pParamData->value.fx;
		}

		if (pParamData->paramId == STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION) {
			BaseCalHealthExtraversionStability(&dHealthExtraversion, &dP27);
			MaxCalHealthExtraversionStability(&dHealthExtraversionMax, &dP27Max);
			dHealthExtraversion = (dHealthExtraversion + dHealthExtraversionMax) / 2.0;
			dP27 = (dP27 + dP27Max) / 2.0;
		}

		InitCorrelationCoeff(pParamData);

		switch (pParamData->paramId)
		{
		case STAT_RESULT_CRITICAL_HEALTH_EXTRAVERSION:
			pParamData->value.fx = (float)dHealthExtraversion;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_STABILITY:
			pParamData->value.fx = (float)dP27;
			break;
		case STAT_RESULT_MAX_ACCUM_CV_HISTOGRAM:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_CV_RANGE:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_FREQ_FFT_FAST:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_CRITICAL_AVG:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_MAX_ACCUM_FREQ_HISTOGRAM_SD:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_AM_HISTOGRAM_DIFFERENCE:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_HISTOGRAM_RMSD:
			fHealthAvg += pParamData->vi;
			dHealthOncology += (pParamData->vi - pParamData->dCorrFactorA) * pParamData->dCorrFactorB;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_MAX_ACCUM_FREQ_AM_HISTOGRAM_AVG:
			fHealthAvg += pParamData->vi;
			dHealthPG += (pParamData->vi - pParamData->dCorrFactorA1) * pParamData->dCorrFactorB1;
			break;
		case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_FREQ_PERIOD_TIME_CV:
			fHealthAvg += pParamData->vi;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_ONCOLOGY:
			if (dHealthOncology > 1.0) {
				dHealthOncology = 1.0;
			}
			pParamData->value.fx = (float)dHealthOncology;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_PG:
			if (dHealthPG > 1.0) {
				dHealthPG = 1.0;
			}
			pParamData->value.fx = (float)dHealthPG;
			break;
		case STAT_RESULT_CRITICAL_HEALTH_AVG:
			pParamData->value.fx = fHealthAvg / 10.f;
			break;
		default:
			break;
		}
	}
}


void CVulpesAppDlg::Int2Float(float *pf, int *pn, int nCnt) {

	for (int i = 0; i < nCnt; i++) {
		pf[i] = (float)pn[i];
	}
}