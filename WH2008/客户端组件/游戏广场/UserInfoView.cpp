#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "UserInfoView.h"

//////////////////////////////////////////////////////////////////////////

//��ɫ����
#define CR_TEXT						RGB(255,255,255)					//������ɫ
#define CR_BACKGROUP				RGB(255,232,160)					//������ɫ


#define IDC_BT_EXCHANGE_LOVELINESS	100									//�һ�����
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserInfoView, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BT_EXCHANGE_LOVELINESS,OnBnClickedExchangeLoveliness)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CUserInfoView::CUserInfoView() : CDialog(IDD_USER_INFO)
{
	//״̬����
	m_nTitleHeight=0;
	m_bShowInfo=false;
	m_bCreateFlag=false;

	return;
}

//��������
CUserInfoView::~CUserInfoView()
{
}

//�ؼ���
VOID CUserInfoView::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CUserInfoView::OnInitDialog()
{
	__super::OnInitDialog();

	//������Դ
	UpdateSkinResource();

	//��ȡ��С
	CImageHandle HandleTitleM(&m_ImageTitleM);
	m_nTitleHeight=m_ImageTitleM.GetHeight();

	//�ƶ�����
	SetWindowPos(NULL,0,0,0,m_nTitleHeight,SWP_NOMOVE|SWP_NOZORDER);

	//���ñ���
	m_bCreateFlag=true;

	return TRUE;
}

//����߶�
UINT CUserInfoView::GetTitleHeight()
{
	return m_nTitleHeight;
}

//������Ϣ
VOID CUserInfoView::UpdateUserInfo()
{
	//���½���
	Invalidate();

	return;
}

//��ʾ��Ϣ
VOID CUserInfoView::ShowUserInfo(bool bShowInfo)
{
	//״̬�ж�
	if (m_bShowInfo==bShowInfo) return;

	//���ñ���
	m_bShowInfo=bShowInfo;

	//���½���
	Invalidate();

	return;
}

//������Դ
bool CUserInfoView::UpdateSkinResource()
{
	//��ȡ��Դ
	tagControlViewImage & ControlViewImage=g_GlobalUnits.m_ControlViewImage;
	HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();

	m_ImageTitleL.SetLoadInfo(ControlViewImage.uUserInfoL,hInstance);
	m_ImageTitleM.SetLoadInfo(ControlViewImage.uUserInfoM,hInstance);
	m_ImageTitleR.SetLoadInfo(ControlViewImage.uUserInfoR,hInstance);

	//������ť
	if (NULL==m_btExchangeLoveliness.m_hWnd) m_btExchangeLoveliness.Create(TEXT(""),WS_CHILD,CRect(0,0,0,0),this,IDC_BT_EXCHANGE_LOVELINESS);
	m_btExchangeLoveliness.SetButtonImage(IDB_BT_BUY_MEMBER_VALUE,AfxGetInstanceHandle(),false);

	return true;
}

//�滭����
BOOL CUserInfoView::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),24);

	//���� DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(pDC);
	BufferDC.SelectObject(ImageBuffer);

	//������Դ
	CImageHandle HandleTitleL(&m_ImageTitleL);
	CImageHandle HandleTitleM(&m_ImageTitleM);
	CImageHandle HandleTitleR(&m_ImageTitleR);

	//�滭����
	m_ImageTitleL.BitBlt(BufferDC,0,0);

	//����м�
	for (INT nXPos=m_ImageTitleL.GetWidth();nXPos<rcClient.Width();nXPos+=m_ImageTitleM.GetWidth())
	{
		m_ImageTitleM.BitBlt(BufferDC,nXPos,0);
	}

	//�滭����
	m_ImageTitleR.BitBlt(BufferDC,rcClient.Width()-m_ImageTitleR.GetWidth()-6,0);

	//���� DC
	BufferDC.SetTextColor(RGB(255,255,255));
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//�û���Ϣ
	if (m_bShowInfo==true)
	{
		//��������
		tagGlobalUserData & GlobalUserInfo=g_GlobalUnits.GetGolbalUserData();

		//�û�ͷ��
		//g_GlobalUnits.m_UserFaceRes->DrawNormalFace(&BufferDC,10,12,GlobalUserInfo.wFaceID,GlobalUserInfo.dwUserID,GlobalUserInfo.dwCustomFaceVer);

		//����λ��
		TCHAR szBuffer[255]=TEXT("");
		CRect rcUserID(200,30,rcClient.Width()-20,17);
		CRect rcAccounts(100,82,rcClient.Width()-20,27);
		CRect rcUnderWrite(100,108,rcClient.Width()-20,50);

		//�û��ʺ�
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("�û��ʺţ�%s [ %ld ]"),GlobalUserInfo.szAccounts,GlobalUserInfo.dwGameID);
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GlobalUserInfo.dwGameID);

        HFONT   hf; 
		LOGFONT   lf; 
		lf.lfHeight=10; 
		lf.lfWidth=10; 
		lf.lfEscapement=1;
		lf.lfItalic=0;
		lf.lfUnderline=0;
		lf.lfStrikeOut=0;
		lf.lfCharSet=1;
		hf=CreateFontIndirect(&lf); 
		SelectObject(BufferDC,hf); 

        BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcUserID,DT_VCENTER|DT_WORD_ELLIPSIS|DT_SINGLELINE);
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("�û��ʺţ�%s"),GlobalUserInfo.szAccounts);
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcAccounts,DT_VCENTER|DT_WORD_ELLIPSIS|DT_SINGLELINE);

		//��Ա��Ϣ
		LPCTSTR pszMemberOrder[]={TEXT("�ǻ�Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա")};
		if (GlobalUserInfo.cbMember<CountArray(pszMemberOrder))
		{
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("��Ա�ȼ���%s"),pszMemberOrder[GlobalUserInfo.cbMember]);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcUnderWrite,DT_VCENTER|DT_WORD_ELLIPSIS|DT_SINGLELINE);
		}

		//�ƶ���ť
		CRect reButton;
		m_btExchangeLoveliness.ShowWindow(SW_SHOW);
		m_btExchangeLoveliness.GetWindowRect(reButton);
		m_btExchangeLoveliness.MoveWindow(rcClient.Width()-reButton.Width()-30,rcClient.Height()-reButton.Height()-10,reButton.Width(),reButton.Height());
	}

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();

	return TRUE;
}

//�����һ�
void CUserInfoView::OnBnClickedExchangeLoveliness()
{
	ShellExecute(NULL,TEXT("open"),TEXT("http://www.52e8.com/BuyMember.asp"),NULL,NULL,SW_SHOWDEFAULT);
}
//////////////////////////////////////////////////////////////////////////

