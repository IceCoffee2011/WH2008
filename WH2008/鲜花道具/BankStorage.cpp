#include "Stdafx.h"
#include "resource.h"
#include "BankStorage.h"
#include ".\bankstorage.h"

//////////////////////////////////////////////////////////////////////////

//��ʱ��I D
#define IDI_CHARMVALUE_UPDATE_VIEW		1								//���½���
#define TIME_CHARMVALUE_UPDATE_VIEW		200								//���½���


//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CBrush CPageDlg::m_brBack;

CPageDlg::CPageDlg(UINT uIDTemplate , LPCTSTR pszTitleText, LPCTSTR pszButtonText) 
	: CDialog(uIDTemplate),
	m_nIDTemplate(uIDTemplate)
{
	m_strTitle=pszTitleText;
	m_strButtonText=pszButtonText;

	if (m_brBack.GetSafeHandle()==NULL)
	{
		m_brBack.CreateSolidBrush(RGB(229,243,255));
	}
}

CPageDlg::~CPageDlg()
{
}

BOOL CPageDlg::CreatePage( CWnd *pParent )
{
	return Create(m_nIDTemplate, pParent);
}

INT64 CPageDlg::GetDlgItemInt64(UINT nID)
{
	CString strText;
	if (GetDlgItemText(nID,strText)==0)
		return 0;
	strText.Remove(',');

	return _tstoi64(strText);
}


void CPageDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	dc.Draw3dRect(&rcClient, RGB(85,132,174),RGB(85,132,174));
}

BOOL CPageDlg::OnEraseBkgnd(CDC * pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CPageDlg::OnCtlColor( CDC * pDC, CWnd * pWnd, UINT nCtlColor )
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(CSkinDialog::m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlTXColor);
			return m_brBack;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

BOOL CPageDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CWnd *pParentWnd=GetParent();
	ASSERT(pParentWnd);

	if (pParentWnd->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		return TRUE;

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBankStorage, CSkinDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK,OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,OnBnClickedCancel)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PAGE_BUTTON1, IDC_PAGE_BUTTON4, OnClickedPageButton)
	ON_BN_CLICKED(IDC_IN_ALL, OnBnClickedInAll)
	ON_BN_CLICKED(IDC_OUT_ALL, OnBnClickedOutAll)
	ON_EN_CHANGE(IDC_IN_COUNT, OnEnChangeInCount)
	ON_EN_CHANGE(IDC_OUT_COUNT, OnEnChangeOutCount)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankStorage::CBankStorage(const BYTE bGameAction) : CSkinDialogEx(IDD_BANK_STORAGE, NULL),
	m_bGameAction(bGameAction)
	
{
	m_lGameGold=0;
	m_lStorageGold=0;
	m_pMeUserData=NULL;
	m_pIClientKernel=NULL;
	m_pClientSocket=NULL;
	m_lGameGoldTmp=0;
	m_lStorageGoldTmp=0;

	m_strInCount=TEXT("");

	m_nSelPage=0;
	m_dwOption=0;

	//�����ڴ�
	m_pPageDlg[0]=new CPageDlg(IDD_PAGE_DIALOG1,TEXT("������"),TEXT("ȷ�ϴ���"));
	m_pPageDlg[1]=new CPageDlg(IDD_PAGE_DIALOG2,TEXT("ȡ�����"),TEXT("ȷ��ȡ��"));
	m_pPageDlg[2]=new CPageDlg(IDD_PAGE_DIALOG3,TEXT("���ת��"),TEXT("ȷ��ת��"));
	m_pPageDlg[3]=new CPageDlg(IDD_PAGE_DIALOG4,TEXT("�����޸�"),TEXT("ȷ���޸�"));
}

//��������
CBankStorage::~CBankStorage()
{
	for (int i=0; i < 4; i++)
	{
		delete m_pPageDlg[i];
	}
}

//�ؼ���
void CBankStorage::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_PAGE_BUTTON1, m_btPage[0]);
	DDX_Control(pDX, IDC_PAGE_BUTTON2, m_btPage[1]);
	DDX_Control(pDX, IDC_PAGE_BUTTON3, m_btPage[2]);
	DDX_Control(pDX, IDC_PAGE_BUTTON4, m_btPage[3]);
}


//��ʼ������
BOOL CBankStorage::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	UpdateData(FALSE);

	SetWindowText(TEXT("����"));

	HINSTANCE hInstance=GetModuleHandle(PROPERTY_MODULE_DLL_NAME);

	for (int i = 0; i < 4; i++)
	{
		m_pPageDlg[i]->CreatePage(this);
		m_btPage[i].SetButtonImage(IDB_TAB_BUTTON,hInstance,false);
		m_btPage[i].SetTextColor(RGB(0,0,0));
	}

	CRect rcClient;
	GetClientRect(&rcClient);
	m_rcPage.SetRect(rcClient.left+15, rcClient.top+71, rcClient.right-15, rcClient.bottom-50);

	m_pPageDlg[m_nSelPage]->MoveWindow(&m_rcPage);
	m_pPageDlg[m_nSelPage]->ShowWindow(SW_SHOW);
	m_btPage[m_nSelPage].SetButtonImage(IDB_TAB_BUTTON_EX,hInstance,false);
	m_btPage[m_nSelPage].SetTextColor(RGB(0,73,146));

	//��ʱ����
	SetTimer(IDI_CHARMVALUE_UPDATE_VIEW,TIME_CHARMVALUE_UPDATE_VIEW,NULL);

	return TRUE;
}

//��������
BOOL CBankStorage::CreateBankWindow(CWnd *pParent, UINT nSelPage, DWORD dwOption)
{
	ASSERT(pParent!=NULL);
	ASSERT(nSelPage<4);
	m_nSelPage=nSelPage;
	m_dwOption=dwOption;
	return Create(IDD_BANK_STORAGE, pParent);
}

//���½���
void CBankStorage::UpdateBank()
{
	UpdateData(TRUE);

	ASSERT(m_pMeUserData!=NULL);
	if(m_pMeUserData==NULL)return;

	//������Ϣ
	m_lGameGold=((m_pMeUserData->lGameGold<0)?0:m_pMeUserData->lGameGold);
	m_lStorageGold=m_pMeUserData->lInsureScore;

	CString static strGameGold=TEXT(""), strStorageGold=TEXT(""),strTmpNumber1,strTmpNumber2;
	strTmpNumber1.Empty();
	strTmpNumber2.Empty();
	strGameGold.Empty();
	strStorageGold.Empty();
	if(m_lGameGold==0) strGameGold=TEXT("0");
	int nNumberCount=0;
	int count=0;
	//if(m_lGameGold/1000==0) return;
	while(m_lGameGold>0)
	{
		strTmpNumber1.Format(TEXT("%I64d"),m_lGameGold%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if(nNumberCount==3)
		{
			count++;
			if(count==1)
			{
				strTmpNumber2 +=strGameGold;
			}
			else
			{
				strTmpNumber2 += (TEXT(",") +strGameGold);
			}
			strGameGold=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		m_lGameGold/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		if(strGameGold.IsEmpty()==FALSE)
			strTmpNumber2 += (TEXT(",") +strGameGold);
		strGameGold=strTmpNumber2;
	}

	strTmpNumber1.Empty();
	strTmpNumber2.Empty();

	if(m_lStorageGold==0) strStorageGold=TEXT("0");
	nNumberCount=0;
	count=0;
	//if(m_lStorageGold/1000==0) return;
	while(m_lStorageGold>0)
	{
		strTmpNumber1.Format(TEXT("%I64d"),m_lStorageGold%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if(nNumberCount==3)
		{
			count++;
			if(count==1)
			{
				strTmpNumber2 +=strStorageGold;
			}
			else
			{
				strTmpNumber2 += (TEXT(",") +strStorageGold);
			}
			strStorageGold=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		m_lStorageGold/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		if(strStorageGold.IsEmpty()==FALSE)
			strTmpNumber2 += (TEXT(",") +strStorageGold);
		strStorageGold=strTmpNumber2;
	}

	if (m_nSelPage<3)
	{
		//CString strText;
		//strText.Format(TEXT("%I64d"), m_lStorageGold);
		m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_BANK_GOLD, strStorageGold);

		//strText.Format(TEXT("%I64d"), m_lGameGold);
		m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_GAME_GOLD, strGameGold);
	}

	m_lGameGold=((m_pMeUserData->lGameGold<0)?0:m_pMeUserData->lGameGold);
	m_lStorageGold=m_pMeUserData->lInsureScore;

	UpdateData(FALSE);
}

//���ð�ť
void CBankStorage::SetWaiting(BOOL bWait)
{
	if (bWait)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		AfxGetApp()->BeginWaitCursor();
		
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		AfxGetApp()->EndWaitCursor();

		CPageDlg *pSelPage=m_pPageDlg[m_nSelPage];

		switch(m_nSelPage)
		{
		case 0:
			{
				pSelPage->SetDlgItemText(IDC_IN_COUNT,TEXT(""));
				pSelPage->GetDlgItem(IDC_IN_COUNT)->EnableWindow(TRUE);
				((CButton*)pSelPage->GetDlgItem(IDC_IN_ALL))->SetCheck(0);
				break;
			}
		case 1:
			{
				//pSelPage->SetDlgItemText(IDC_OUT_COUNT,TEXT(""));
				pSelPage->SetDlgItemText(IDC_BANK_PASSWORD,TEXT(""));
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(TRUE);
				((CButton*)pSelPage->GetDlgItem(IDC_OUT_ALL))->SetCheck(0);
				break;
			}
		case 2:
			{
				//pSelPage->SetDlgItemText(IDC_USER_ID,TEXT(""));
				//pSelPage->SetDlgItemText(IDC_OUT_COUNT,TEXT(""));
				pSelPage->SetDlgItemText(IDC_BANK_PASSWORD,TEXT(""));
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(TRUE);
				((CButton*)pSelPage->GetDlgItem(IDC_OUT_ALL))->SetCheck(0);
				break;
			}
		case 3:
			{
				pSelPage->SetDlgItemText(IDC_BANK_PASSWORD,TEXT(""));
				pSelPage->SetDlgItemText(IDC_NEW_PASSWORD,TEXT(""));
				pSelPage->SetDlgItemText(IDC_CONFIRM_PASSWORD,TEXT(""));
				break;
			}
		}
	}
}

//�����Ϣ
void CBankStorage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSkinDialogEx::OnLButtonDown(nFlags, point);
}

//�����Ϣ
void CBankStorage::OnMouseMove(UINT nFlags, CPoint point)
{
	CSkinDialogEx::OnMouseMove(nFlags, point);
}

//������Ϣ
BOOL CBankStorage::PreTranslateMessage(MSG* pMsg)
{
	//������ĸ
	//if(GetFocus()==GetDlgItem(IDC_IN_COUNT)&& pMsg->message==WM_CHAR)
	//{
	//	BYTE bMesValue = (BYTE)(pMsg->wParam);
	//	BYTE bTemp = bMesValue-'0';
	//	if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK) return TRUE;
	//}

	////���˻س�
	//if(pMsg->message==WM_KEYDOWN)
	//{
	//	BYTE bMesValue = (BYTE)(pMsg->wParam);
	//	if(bMesValue==VK_RETURN) return TRUE;
	//}


	//if((WM_CHAR == pMsg->message) && (pMsg->hwnd == GetDlgItem(IDC_IN_COUNT)->GetSafeHwnd()))
	//{
	//	if(pMsg->wParam >=_T('0') && pMsg->wParam <= _T('9'))
	//	{
	//		CString strText(_T(""));
	//		GetDlgItemText(IDC_IN_COUNT, strText);
	//		CString strLeft(_T(""));
	//		CString strRight(_T(""));

	//		if(strText.GetLength() >= 3)
	//		{
	//			int nFlag = strText.ReverseFind(_T(','));
	//			if(-1 != nFlag)
	//			{
	//				strLeft = strText.Left(nFlag + 1);
	//				strRight = strText.Mid(nFlag + 1);
	//				if(0 == strRight.GetLength()%3)
	//				{
	//					strRight += _T(",");
	//				}
	//				strText = strLeft + strRight;
	//			}
	//			else
	//			{
	//				strText += _T(",");
	//			}
	//			SetDlgItemText(IDC_IN_COUNT, strText);
	//			((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(strText.GetLength(), -1);
	//		}
	//	}
	//}

	return __super::PreTranslateMessage(pMsg);
}

//ʱ����Ϣ
void CBankStorage::OnTimer(UINT nIDEvent)
{
	//ʱ����Ϣ
	switch(nIDEvent)
	{
	case IDI_CHARMVALUE_UPDATE_VIEW:		//���½���
		{
			//���½���
			UpdateBank();
			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

void CBankStorage::OnClickedPageButton(UINT nCtrlID)
{
	int nPage=nCtrlID-IDC_PAGE_BUTTON1;
	ASSERT(nPage>=0 && nPage<4);

	if (nPage!=m_nSelPage)
	{
		HINSTANCE hInstance=GetModuleHandle(PROPERTY_MODULE_DLL_NAME);
		m_pPageDlg[nPage]->MoveWindow(&m_rcPage);
		m_pPageDlg[nPage]->ShowWindow(SW_SHOW);
		m_btPage[nPage].SetButtonImage(IDB_TAB_BUTTON_EX,hInstance,false);
		m_btPage[nPage].SetTextColor(RGB(0,73,146));

		m_pPageDlg[m_nSelPage]->ShowWindow(SW_HIDE);
		m_btPage[m_nSelPage].SetButtonImage(IDB_TAB_BUTTON,hInstance,false);
		m_btPage[m_nSelPage].SetTextColor(RGB(0,0,0));
		m_nSelPage=nPage;
	}
}

//������Ϣ
void CBankStorage::SetSendInfo(IClientKernel *pClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserData)
{
	ASSERT((pClientKernel==NULL && pClientSocket!=NULL)||(pClientKernel!=NULL && pClientSocket==NULL));

	//������Ϣ
	m_pIClientKernel = pClientKernel;
	m_pClientSocket= pClientSocket;
	m_pMeUserData= pMeUserData;

	return;
}

//������Ϣ
void CBankStorage::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	ASSERT(m_pIClientKernel!=NULL || m_pClientSocket!=NULL);

	//������Ϣ
	if(m_pClientSocket!=NULL)m_pClientSocket->SendData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);
	else m_pIClientKernel->SendSocketData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);

	return ;
}

//void CBankStorage::OnEnChangeInCount()
//{
//	if(m_nSelPage==0)
//	{
//		CString strPageIn;
//		int strPutNumber=0;
//		int insertCount=0;
//		strPutNumber=m_pPageDlg[m_nSelPage]->GetDlgItemText(IDC_IN_COUNT,strPageIn);
//		if(strPutNumber>3)
//		{
//			if(strPutNumber%3==0) insertCount=strPutNumber/3-1;
//			else insertCount=strPutNumber/3;
//			for(int i=0;i<insertCount;i++) strPageIn.Insert(i*3,',');
//		}
//		m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_IN_COUNT,strPageIn);
//	}
//}

//ȫ������
void CBankStorage::OnBnClickedInAll()
{
	if (m_nSelPage==0)
	{
		CButton *pCheckBox=(CButton*)m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_IN_ALL);
		ASSERT(pCheckBox);

		if (pCheckBox->GetCheck()==BST_CHECKED)
		{
			//SetDlgItemText���ô��ڻ�Ի���ӵ�еı�����ǿؼ��ı���
			CString strText;
			strText.Format(TEXT("%I64d"), m_lGameGold);
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_IN_COUNT, strText);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_IN_COUNT)->EnableWindow(FALSE);
		}
		else
		{
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_IN_COUNT, TEXT(""));
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_IN_COUNT)->EnableWindow(TRUE);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_IN_COUNT)->SetFocus();
		}
	}
}

//ȫ��ȡ��/ת��
void CBankStorage::OnBnClickedOutAll()
{
	if (m_nSelPage==1)
	{
		CButton *pCheckBox=(CButton*)m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_ALL);
		ASSERT(pCheckBox);

		if (pCheckBox->GetCheck()==BST_CHECKED)
		{
			CString strText;
			strText.Format(TEXT("%I64d"), m_lStorageGold);
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_OUT_COUNT, strText);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(FALSE);
		}
		else
		{
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_OUT_COUNT, TEXT(""));
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(TRUE);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->SetFocus();
		}
	}
	else if (m_nSelPage==2)
	{
		CButton *pCheckBox=(CButton*)m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_ALL);
		ASSERT(pCheckBox);

		if (pCheckBox->GetCheck()==BST_CHECKED)
		{
			CString strText;
			strText.Format(TEXT("%I64d"), m_lStorageGold);
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_OUT_COUNT, strText);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(FALSE);
		}
		else
		{
			m_pPageDlg[m_nSelPage]->SetDlgItemText(IDC_OUT_COUNT, TEXT(""));
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->EnableWindow(TRUE);
			m_pPageDlg[m_nSelPage]->GetDlgItem(IDC_OUT_COUNT)->SetFocus();
		}
	}
}

//��ȡ��ť
void CBankStorage::OnBnClickedOk()
{
	CPageDlg *pSelPage=m_pPageDlg[m_nSelPage];
	ASSERT(pSelPage!=NULL);

	INT64 lGoldCount=0;

	switch( m_nSelPage )
	{
	case 0: //������
		{
			lGoldCount=pSelPage->GetDlgItemInt64(IDC_IN_COUNT);

			if (lGoldCount <= 0 || lGoldCount > m_lGameGold)
			{
				CString strMessage;
				if(m_lGameGold>0)
					strMessage.Format(TEXT("������Ľ��ֵ������1��%I64d֮��"), m_lGameGold);
				else 
					strMessage.Format(TEXT("��ĵ�ǰ�����ĿΪ0,���ܽ��д洢����!"));

				ShowInformationEx(strMessage,0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				//GetDlgItem������öԻ��������������ָ���ؼ����Ӵ��ڵ�ָ��
				//SetFocus����Ҫ��õ����뽹�㡣���뽹�㽫�������м������붨��������ڡ�
				//ԭ��ӵ�����뽹����κδ��ڶ���ʧȥ����SetFocus��Ա������ʧȥ���뽹��Ĵ��ڷ���һ��WM_KILLFOCUS��Ϣ��
				//����������뽹��Ĵ��ڷ���һ��WM_SETFOCUS��Ϣ����������ô��ڻ����ĸ����ڡ�
				//�����ǰ�����Ǽ���ģ����ǲ��������뽹�㣨����ζ�ţ�û�д��ھ������뽹�㣩�����κΰ��µļ���������WM_SYSCHAR��WM_SYSKEYDOWN��WM_SYSKEYUP��Ϣ��
				pSelPage->GetDlgItem(IDC_IN_COUNT)->SetFocus();
				((CEdit*)pSelPage->GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
				return;
			}
			break;
		}
	case 1: //ȡ�����
		{
			lGoldCount=pSelPage->GetDlgItemInt64(IDC_OUT_COUNT);
			if (lGoldCount <= 0 || lGoldCount > m_lStorageGold)
			{
				CString strMessage;
				if(m_lStorageGold>0)
					strMessage.Format(TEXT("������Ľ��ֵ������1��%I64d֮��"),m_lStorageGold);
				else 
					strMessage.Format(TEXT("��Ĵ洢�����ĿΪ0,���ܽ�����ȡ����!"));

				ShowInformationEx(strMessage,0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->SetFocus();
				((CEdit*)pSelPage->GetDlgItem(IDC_OUT_COUNT))->SetSel(0,-1);
				return;
			}
			break;
		}
	case 2: //ת�����
		{
			lGoldCount=pSelPage->GetDlgItemInt64(IDC_OUT_COUNT);
			DWORD dwTagGameID=pSelPage->GetDlgItemInt(IDC_USER_ID);

			if (dwTagGameID==0)
			{
				ShowInformationEx(TEXT("�����û�ID����ȷ"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_USER_ID)->SetFocus();
				return;
			}

			if (dwTagGameID==m_pMeUserData->dwGameID)
			{
				ShowInformationEx(TEXT("�����û�ID���������ѣ�����������"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_USER_ID)->SetFocus();
				return;
			}

			if (lGoldCount <= 0 || lGoldCount > m_lStorageGold)
			{
				CString strMessage;
				if (m_lStorageGold>0)
					strMessage.Format(TEXT("������Ľ��ֵ������1��%I64d֮��"),m_lStorageGold);
				else 
					strMessage.Format(TEXT("��Ĵ洢�����ĿΪ0,���ܽ���ת�ʲ���!"));
				ShowInformationEx(strMessage,0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->SetFocus();
				((CEdit*)pSelPage->GetDlgItem(IDC_OUT_COUNT))->SetSel(0,-1);
				return;
			}

			if ((m_lStorageGold-lGoldCount) < 2000)
			{
				ShowInformationEx(TEXT("�������б������ٱ��� 2000 ��ң�����ȫ��ת����"),0,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				((CEdit*)pSelPage->GetDlgItem(IDC_OUT_COUNT))->SetSel(0,-1);
				return;
			}
			break;
		}
	case 3: //��������
		{
			TCHAR szOldPassword[PASS_LEN]=TEXT("");
			TCHAR szPassword1[PASS_LEN]=TEXT("");
			TCHAR szPassword2[PASS_LEN]=TEXT("");

			pSelPage->GetDlgItemText(IDC_BANK_PASSWORD,szOldPassword,CountArray(szOldPassword));
			pSelPage->GetDlgItemText(IDC_NEW_PASSWORD,szPassword1,CountArray(szPassword1));
			pSelPage->GetDlgItemText(IDC_CONFIRM_PASSWORD,szPassword2,CountArray(szPassword2));

			if (szPassword1[0]==0)
			{
				ShowInformationEx(TEXT("�����벻��Ϊ�գ���������д"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_NEW_PASSWORD)->SetFocus();
				return;
			}

			if (lstrlen(szPassword1) < 6)
			{
				ShowInformationEx(TEXT("Ϊ���������밲ȫ�����볤�ȱ�����5λ����"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_NEW_PASSWORD)->SetFocus();
				return;
			}

			if (szPassword2[0]==0)
			{
				ShowInformationEx(TEXT("ȷ�����벻��Ϊ�գ���������д"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_CONFIRM_PASSWORD)->SetFocus();
				return;
			}

			if (lstrlen(szPassword2) < 6)
			{
				ShowInformationEx(TEXT("Ϊ���������밲ȫ�����볤�ȱ�����5λ����"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_CONFIRM_PASSWORD)->SetFocus();
				return;
			}

			if (strcmp(szPassword1,szPassword2)!=0)
			{
				ShowInformationEx(TEXT("��������������벻һ�£���������д"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_CONFIRM_PASSWORD)->SetFocus();
				return;
			}

			if (strcmp(szOldPassword,szPassword1)==0)
			{
				ShowInformationEx(TEXT("�������������;�����ͬ����������д"),0 ,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ"));
				pSelPage->GetDlgItem(IDC_NEW_PASSWORD)->SetFocus();
				return;
			}
			break;
		}
	}

	//����Ч��
	TCHAR szPassword[PASS_LEN]=TEXT("");
	
	if (m_nSelPage > 0)
	{
		pSelPage->GetDlgItemText(IDC_BANK_PASSWORD, szPassword, CountArray(szPassword));

		if (szPassword[0]==0)
		{
			ShowInformationEx(TEXT("�������벻��Ϊ�գ��������������룡"),0,MB_ICONINFORMATION,TEXT("ϵͳ��Ϣ")) ;
			pSelPage->GetDlgItem(IDC_BANK_PASSWORD)->SetWindowText("");
			pSelPage->GetDlgItem(IDC_BANK_PASSWORD)->SetFocus();
			return;
		}

		TCHAR szTempPassword[PASS_LEN]=TEXT("");
		CopyMemory(szTempPassword,szPassword,sizeof(szTempPassword));
		CMD5Encrypt::EncryptData(szTempPassword,szPassword);
	}


	switch( m_nSelPage )
	{
	case 0: //������
		{
			//������Ϣ
			CMD_GF_BankStorage BankStorage;
			ZeroMemory(&BankStorage, sizeof(BankStorage));
			BankStorage.lStorageValue = lGoldCount;
			BankStorage.cbGameAction=m_bGameAction;
			CopyMemory(BankStorage.szPassword,szPassword,sizeof(BankStorage.szPassword));
			SendData(MDM_GF_BANK, SUB_GF_BANK_STORAGE, &BankStorage, sizeof(BankStorage));
			break;
		}
	case 1: //ȡ�����
		{
			//������Ϣ
			CMD_GF_BankGet BankGetGold;
			ZeroMemory(&BankGetGold, sizeof(BankGetGold));
			BankGetGold.lGetValue = lGoldCount;
			BankGetGold.cbGameAction=m_bGameAction;
			CopyMemory(BankGetGold.szPassword,szPassword,sizeof(BankGetGold.szPassword));
			SendData(MDM_GF_BANK, SUB_GF_BANK_GET, &BankGetGold, sizeof(BankGetGold));
			break;
		}
	case 2: //ת����
		{
			CString static strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
			strTmpNumber1.Empty();
			strTmpNumber2.Empty();
			strNumber.Empty();
			if(lGoldCount==0) strNumber=TEXT("0");
			int nNumberCount=0;
			int count=0;
			INT64 lTmpNumber=lGoldCount;
			while(lGoldCount>0)
			{
				strTmpNumber1.Format(TEXT("%I64d"),lGoldCount%10);
				nNumberCount++;
				strTmpNumber2 = strTmpNumber1+strTmpNumber2;

				if(nNumberCount==3)
				{
					count++;
					if(count==1)
					{
						strTmpNumber2 +=strNumber;
					}
					else
					{
						strTmpNumber2 += (TEXT(",") +strNumber);
					}
					strNumber=strTmpNumber2;
					nNumberCount=0;
					strTmpNumber2=TEXT("");
				}
				lGoldCount/=10;
			}

			if (strTmpNumber2.IsEmpty()==FALSE)
			{
				strTmpNumber2 += (TEXT(",") +strNumber);
				strNumber=strTmpNumber2;
			}

			DWORD dwTagGameID=pSelPage->GetDlgItemInt(IDC_USER_ID);

			INT64 lRevenue=lTmpNumber * 0.01;
			TCHAR szMessage[512]=TEXT("");
			sprintf(szMessage, TEXT("���Ƿ�ȷ��Ҫת %s ��ң���˰ %I64d ��ң����û�IDΪ��%d�ݵ��û���"),
				strNumber,lRevenue,dwTagGameID);
			
			if (ShowInformationEx(szMessage,0,MB_YESNO,TEXT("ת��ȷ��"))!=IDYES) return;
	
			CMD_GF_BankTransfer BankTransfer;
			ZeroMemory(&BankTransfer,sizeof(BankTransfer));
			BankTransfer.dwTagGameID=dwTagGameID;
			BankTransfer.lTransferValue=lTmpNumber;
			BankTransfer.cbGameAction=m_bGameAction;
			CopyMemory(BankTransfer.szPassword,szPassword,sizeof(BankTransfer.szPassword));
			SendData(MDM_GF_BANK, SUB_GF_BANK_TRANSFER, &BankTransfer, sizeof(BankTransfer));

			break;
		}
	case 3: //�޸�����
		{
			TCHAR szNewPassword[PASS_LEN]=TEXT("");
			TCHAR szEncrypt[PASS_LEN]=TEXT("");
			pSelPage->GetDlgItemText(IDC_NEW_PASSWORD, szNewPassword, CountArray(szNewPassword));

			CMD_GF_BankSecurity BankSecurity;
			ZeroMemory(&BankSecurity,sizeof(BankSecurity));

			BankSecurity.cbGameAction=m_bGameAction;
			CMD5Encrypt::EncryptData(szNewPassword,szEncrypt);
			CopyMemory(BankSecurity.szPassword,szPassword,sizeof(BankSecurity.szPassword));
			CopyMemory(BankSecurity.szNewPassword,szEncrypt,sizeof(BankSecurity.szNewPassword));

			SendData(MDM_GF_BANK, SUB_GF_BANK_SECURITY, &BankSecurity, sizeof(BankSecurity));
			break;
		}
	}

	SetWaiting(TRUE);
}

//ȡ����ť
void CBankStorage::OnBnClickedCancel()
{
	KillTimer(IDI_CHARMVALUE_UPDATE_VIEW);
	DestroyWindow();
}

void CBankStorage::OnEnChangeInCount()
{
	CPageDlg *pSelPage=m_pPageDlg[m_nSelPage];
	ASSERT(pSelPage!=NULL);

	//CString strText, strLeft, strRight;
	//pSelPage->GetDlgItem(IDC_IN_COUNT)->GetWindowText(strText);
	//int len = strText.GetLength();
	//if ( len > 3)
	//{
	//	int nFind = strText.ReverseFind(',');
	//	
	//	if ( nFind == -1 )
	//	{
	//		strText.Insert(3,',');
	//		pSelPage->GetDlgItem(IDC_IN_COUNT)->SetWindowText(strText);
	//		((CEdit*)pSelPage->GetDlgItem(IDC_IN_COUNT))->SetSel(strText.GetLength(),-1);
	//	}
	//	else
	//	{
	//		strLeft = strText.Left(nFind+1);//������
	//		strRight = strText.Right(strText.GetLength()-nFind-1);
	//		if ( strRight.GetLength() > 3 )
	//		{
	//			strRight.Insert(3,',');

	//			strText = strLeft + strRight;
	//			pSelPage->GetDlgItem(IDC_IN_COUNT)->SetWindowText(strText);
	//			((CEdit*)pSelPage->GetDlgItem(IDC_IN_COUNT))->SetSel(strText.GetLength(),-1);
	//		}
	//	}
	//}

	CString strText,strTmpText;
	strText = TEXT("");
	INT64 lGold;
	lGold = pSelPage->GetDlgItemInt64(IDC_IN_COUNT);
	int NumCount=0;
	int lenCount=0;
	if(lGold==0) strText=TEXT("");
	if(lGold>0)
	{
		strTmpText.Format(TEXT("%I64d"),lGold);
		if(strTmpText.GetLength() > 3)
		{
			for(int i=0;i<strTmpText.GetLength();i++)
			{
				if(i%4==0)
				{
					strTmpText.Insert(strTmpText.GetLength()-i,',');
				}
			}
		}
		int nFind = strTmpText.ReverseFind(',');
		if(-1==nFind)
		{
			strText=strTmpText;
		}
		else
		{
			strText = strTmpText.Left(strTmpText.ReverseFind(','));
		}
		pSelPage->GetDlgItem(IDC_IN_COUNT)->SetWindowText(strText);
		((CEdit*)pSelPage->GetDlgItem(IDC_IN_COUNT))->SetSel(strText.GetLength(),-1);
	}
}

void CBankStorage::OnEnChangeOutCount()
{
	CPageDlg *pSelPage=m_pPageDlg[m_nSelPage];
	ASSERT(pSelPage!=NULL);

	switch(m_nSelPage)
	{
	case 1:
		{
			CString strText,strTmpText;
			strText = TEXT("");
			INT64 lGold;
			lGold = pSelPage->GetDlgItemInt64(IDC_OUT_COUNT);
			int NumCount=0;
			int lenCount=0;
			if(lGold==0) strText=TEXT("");
			if(lGold>0)
			{
				strTmpText.Format(TEXT("%I64d"),lGold);
				if(strTmpText.GetLength() > 3)
				{
					for(int i=0;i<strTmpText.GetLength();i++)
					{
						if(i%4==0)
						{
							strTmpText.Insert(strTmpText.GetLength()-i,',');
						}
					}
				}
				int nFind = strTmpText.ReverseFind(',');
				if(-1==nFind)
				{
					strText=strTmpText;
				}
				else
				{
					strText = strTmpText.Left(strTmpText.ReverseFind(','));
				}
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->SetWindowText(strText);
				((CEdit*)pSelPage->GetDlgItem(IDC_OUT_COUNT))->SetSel(strText.GetLength(),-1);
			}
			break;
		}
	case 2:
		{
			CString strText,strTmpText;
			strText = TEXT("");
			INT64 lGold;
			lGold = pSelPage->GetDlgItemInt64(IDC_OUT_COUNT);
			int NumCount=0;
			int lenCount=0;
			if(lGold==0) strText=TEXT("");
			if(lGold>0)
			{
				strTmpText.Format(TEXT("%I64d"),lGold);
				if(strTmpText.GetLength() > 3)
				{
					for(int i=0;i<strTmpText.GetLength();i++)
					{
						if(i%4==0)
						{
							strTmpText.Insert(strTmpText.GetLength()-i,',');
						}
					}
				}
				int nFind = strTmpText.ReverseFind(',');
				if(-1==nFind)
				{
					strText=strTmpText;
				}
				else
				{
					strText = strTmpText.Left(strTmpText.ReverseFind(','));
				}
				pSelPage->GetDlgItem(IDC_OUT_COUNT)->SetWindowText(strText);
				((CEdit*)pSelPage->GetDlgItem(IDC_OUT_COUNT))->SetSel(strText.GetLength(),-1);
			}
			break;
		}
	}
}
