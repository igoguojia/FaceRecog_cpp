
// MFCApplication1Dlg.h: 头文件
//

#pragma once

#include<opencv.hpp>
//#include<string>
#include < vector> 
using namespace std;
// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	struct vectorFace
	{		
		string Label;
		string sFolder;
		string Name;
	};
	vector<vectorFace> FaceList;
public:
	afx_msg void OnBnClickedBegin();
	int fi = 1;
//	afx_msg void OnBnClickedSelect();
	CString csvPath1;
	afx_msg void OnBnClickedTrain();
//	virtual void PostNcDestroy();
	CvCapture* pCapture = NULL;
	IplImage* pFrame = NULL;

	virtual void OnCancel();
	BOOL Cancel = FALSE;
//	afx_msg void OnBnClickedRecog();
	void OnBnClickedRecog();
	CListBox Information_Box;

//	BOOL Recog = FALSE;
//	afx_msg void OnBnClickedTakephoto();
	char tempjpg[50]="";
	time_t rawtime=0;
	struct tm *info=NULL;
	char buffer[30] ="";
	int nameOrder = 0;
	int sFolderOrder = 0;
	string namestr;

	afx_msg void OnBnClickedAddFace();
	//void TakePhoto();
	BOOL Enabletakephoto = FALSE;
	string dir;
};
