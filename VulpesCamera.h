#pragma once
#include <Mmreg.h>
#include <atlimage.h>

#ifdef VULPESCAMERA_EXPORTS
#define VULPESCAMERA_API __declspec(dllexport)
#else
#define VULPESCAMERA_API __declspec(dllimport)
#endif


class VULPESCAMERA_API CVulpesCamera
{
public:
	enum VIDEOTYPE
	{
		VIDEOTYPE_RGB1,
		VIDEOTYPE_RGB4,
		VIDEOTYPE_RGB8,
		VIDEOTYPE_RGB555,
		VIDEOTYPE_RGB565,
		VIDEOTYPE_RGB24,
		VIDEOTYPE_RGB32,
		VIDEOTYPE_AYUV,
		VIDEOTYPE_UYVY,
		VIDEOTYPE_Y411,
		VIDEOTYPE_Y41P,
		VIDEOTYPE_Y211,
		VIDEOTYPE_YUY2,
		VIDEOTYPE_YVYU,
		VIDEOTYPE_YUYV
	};

	enum VIDEO_STANDATRD
	{
		VIDEO_STANDATRD_None = 0x00000000,
		VIDEO_STANDATRD_NTSC_M = 0x00000001,
		VIDEO_STANDATRD_NTSC_M_J = 0x00000002,
		VIDEO_STANDATRD_NTSC_433 = 0x00000004,
		VIDEO_STANDATRD_PAL_B = 0x00000010,
		VIDEO_STANDATRD_PAL_D = 0x00000020,
		VIDEO_STANDATRD_PAL_H = 0x00000080,
		VIDEO_STANDATRD_PAL_I = 0x00000100,
		VIDEO_STANDATRD_PAL_M = 0x00000200,
		VIDEO_STANDATRD_PAL_N = 0x00000400,
		VIDEO_STANDATRD_PAL_60 = 0x00000800,
		VIDEO_STANDATRD_SECAM_B = 0x00001000,
		VIDEO_STANDATRD_SECAM_D = 0x00002000,
		VIDEO_STANDATRD_SECAM_G = 0x00004000,
		VIDEO_STANDATRD_SECAM_H = 0x00008000,
		VIDEO_STANDATRD_SECAM_K = 0x00010000,
		VIDEO_STANDATRD_SECAM_K1 = 0x00020000,
		VIDEO_STANDATRD_SECAM_L = 0x00040000,
		VIDEO_STANDATRD_SECAM_L1 = 0x00080000,
		VIDEO_STANDATRD_PAL_N_COMBO = 0x00100000
	};

public:
	CVulpesCamera();
	virtual ~CVulpesCamera();


public:

	BOOL Create(LPCWSTR lpszClassName, LPCWSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	bool GetCaptureCC();
	LPCWSTR GetCaptureFile();
	WORD GetCaptureFileAllocSize();
	HWND GetWindow(void);
	void GetFrameRate(int &nUse, double &rate);
	void GetTimeLimit(int &nUse, DWORD &limit);


	BOOL ShowWindow(int nCmdShow);
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);


	void SetRegistryKey(LPCWSTR key);
	void SetRegistryKeyGroup(LPCWSTR key);
	void SetTimeLimit(int nUse, DWORD dwLimit);
	void SetFrameRate(int bUse, double dRate);

	BOOL SetCaptureFileAllocSize(WORD size);
	BOOL SetCaptureFile(LPCWSTR file);

	BOOL SaveCaptureFile(LPCWSTR tachDstFile);

	void SetCaptureCC(bool cap);

	void CmdCapture(bool bPaused = false);
	bool CanCmdCapture(void);


	void CmdPreview(void);
	void CmdPreviewStop(void);
	bool CanCmdPreview(void);
	bool CanCmdPreviewStop(void);

	void CmdCaptureStop(int nMode = 0);
	bool CanCmdCaptureStop(void);

	bool CanCmdVideoStop(void);
	void CmdVideoStop(void);

	void ChooseAudioFormat(void);
	bool CanChooseAudioFormat(void);
	bool CanSetCaptureAudio(void);
	bool CanSetCaptureCC(void);
	bool CanSetFrameRate(void);
	bool CanSetTimeLimit(void);
	bool CanSetCaptureFileAllocSize(void);
	bool CanSetCaptureFile(void);
	bool CanSaveCaptureFile(void);
	bool CanCmdMasterNo(void);
	bool CanCmdMasterAudio(void);

	bool CanCmdMasterVideo(void);

	bool CanRTSP(void);

	bool IsCapturing(void);
	bool IsPreviewing(void);

	void CmdDeviceVideo(int n);
	int GetDeviceVideo(void);
	int GetDeviceVideoCount(void);
	bool IsDeviceVideo(int n);
	bool CanDeviceVideo(void);

	int GetDeviceAudio(void);
	int GetDeviceAudioCount(void);
	bool IsDeviceAudio(int n);
	bool CanDeviceAudio(void);

	bool CanCmdDialog(int n);
	void CmdDialog(int n);

	bool IsVideo(void);
	void Stop(void);
	UINT64 GetFrameCount(void);

	LPCWSTR GetDialogNameVideoW(int n);
	LPCWSTR GetDevNameVideoW(int n);

	bool IsActive(void);
	bool CanPlayAVI(void);
	bool PlayVideo(LPCWSTR file, LPCWSTR fileOut = 0, double tStart = -1);
	bool RestartVideo(double tStart = 0);
	double	GetVideoDuration();
	LPCWSTR GetVideoFile();

	void SetCallbackData(void* data);
	void SetCallbackOnVideoPlay(void * pFn);
	void SetCallbackAudio(void * pFn);
	void SetNotifyWindow(HWND hWndNotify);
	double GetCurrentTimeMS(void);
	LPCWSTR GetCurrentFile(void);
	int GetInplaceVideo(void);
	void SetInplaceVideo(int v);
	bool SetBPP(int bpp);
	int GetBPP(void);
	void SetDefFrameRate(double fr);
	double GetDefFrameRate(void);

	int GetVideoType(void);
	bool PlayIP(LPCWSTR host, int port, LPCWSTR user, LPCWSTR pwd, LPCWSTR url, float fps, bool bStop = false, LPCWSTR ptz = 0, bool bRTSP = false);
	float GetFpsIP(void);
	float GetFpsIPBase(void);
	bool IsIP();

	int GetRotate();
	void SetRotate(int nRotate);
	bool CanCmdCaptureIS(void);

	float GetAVIPlaySpeed() const;
	void SetVideoPlaySpeed(float speed);

	double GetAudioDelay();
	void SetAudioDelay(double);
};