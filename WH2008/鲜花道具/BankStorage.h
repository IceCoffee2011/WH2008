#ifndef BANKER_STORAGE_HEAD_FILE
#define BANKER_STORAGE_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

class CPageDlg : public CDialog
{
protected:
	CString m_strTitle;
	CString m_strButtonText;
	const UINT m_nIDTemplate;

	static CBrush m_brBack;

public:
	CPageDlg(UINT uIDTemplate, LPCTSTR pszTitleText, LPCTSTR pszButtonText);
	virtual ~CPageDlg();

protected:
	virtual void OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); };
	virtual void OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); };

public:
	virtual BOOL CreatePage(CWnd *pParent);
	virtual LPCTSTR GetTitleText() { return m_strTitle; }
	virtual LPCTSTR GetButtonText() { return m_strButtonText; }
	INT64 GetDlgItemInt64(UINT nID);

public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	//�ػ�����
	afx_msg void OnPaint();
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//��ɫ��Ϣ
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#define ALLOW_SAVE_PAGE				0x00000001L
#define ALLOW_DRAW_PAGE		        0x00000002L
#define ALLOW_TRANSFER_PAGE			0x00000004L
#define ALLOW_SECURITY_PAGE		    0x00000008L
#define ALLOW_ALL_PAGE				(ALLOW_SAVE_PAGE|ALLOW_DRAW_PAGE|ALLOW_TRANSFER_PAGE|ALLOW_SECURITY_PAGE)


//�һ���� 
class PROPERTY_MODULE_CLASS CBankStorage : public CSkinDialogEx
{
	//��������
public:
	tagUserData						*m_pMeUserData;					//�û���Ϣ
	LONGLONG						m_lGameGold;					//������Ŀ
	LONGLONG						m_lStorageGold;					//�洢��Ŀ
	const BYTE						m_bGameAction;
	LONGLONG						m_lGameGoldTmp;
	LONGLONG						m_lStorageGoldTmp;

	CString							m_strInCount;


	//�ؼ�����
protected:
	CSkinButton						m_btOK;							//ȷ����ť
	CSkinButton						m_btCancel;						//ȡ����ť
	CSkinButton                     m_btPage[4];

protected:
	CPageDlg                        * m_pPageDlg[4];
	UINT                            m_nSelPage;
	CRect                           m_rcPage;
	DWORD							m_dwOption;

	//�ӿڱ���
protected:	
	IClientKernel					* m_pIClientKernel;				//�ں˽ӿ�
	ITCPSocket						* m_pClientSocket;				//����ӿ�

	//��������
public:
	//���캯��
	CBankStorage(const BYTE bGameAction); 
	//��������
	virtual ~CBankStorage();

	//��������
public:
	//��������
	BOOL CreateBankWindow(CWnd *pParent, UINT nSelPage, DWORD dwOption=ALLOW_ALL_PAGE);
	//���½���
	void UpdateBank();
	//���ð�ť
	void SetWaiting(BOOL bWait);
	//�������
	void SetSendInfo(IClientKernel *pClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserDatas);
	//������Ϣ
	void SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

	//���غ���
protected:
	//�ؼ���
	virtual void DoDataExchange(CDataExchange* pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����ť
	afx_msg void OnBnClickedOk();
	//ȡ����ť
	afx_msg void OnBnClickedCancel();
	//�����Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//�����Ϣ
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//������Ϣ
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//������Ϣ
	//afx_msg void OnEnChangeInCount();
	
	afx_msg void OnClickedPageButton(UINT nCtrlID);
	
	afx_msg void OnBnClickedInAll();
	afx_msg void OnBnClickedOutAll();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeInCount();
	afx_msg void OnEnChangeOutCount();
};

//////////////////////////////////////////////////////////////////////////
#endif