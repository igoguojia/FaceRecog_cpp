#pragma once


// CFaceID 对话框

class CFaceID : public CDialogEx
{
	DECLARE_DYNAMIC(CFaceID)

public:
	CFaceID(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFaceID();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FaceID };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit FIDName;
	CString FIDstr="";
	virtual void OnOK();
};
