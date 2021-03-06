#include "precomp_dboclient.h"
#include "CharMakePartGui.h"

// core
#include "NtlDebug.h"
#include "NtlMath.h"
#include "NtlStringUtil.h"

// share
#include "NtlCharacter.h"

// presentation
#include "NtlPLGuiManager.h"

// dbo
#include "DboEvent.h"
#include "DboEventGenerator.h"
#include "DisplayStringManager.h"
#include "CharStageState.h"
#include "CharCreateGui.h"
#include "DboGlobal.h"
#include "AlarmManager.h"
#include "NtlSobProxy.h"

CCharMakePartGui::CCharMakePartGui(const RwChar* pName)
:CNtlPLGui(pName)
{
	for( RwUInt8 i = 0 ; i < dNUM_LEFT_RIGHT_BUTTONS ; ++i )
		m_aptAttributeDisplay[i] = NULL;
}

CCharMakePartGui::~CCharMakePartGui()
{

}

RwBool CCharMakePartGui::Create(CCharCreateGuiGui* pCreateGui)
{
	NTL_FUNCTION( "CCharMakePartGui::Create" );

	if(!CNtlPLGui::Create("", "gui\\CharMakePart.srf", "gui\\CharMakePart.frm"))
		NTL_RETURN(FALSE);

	CNtlPLGui::CreateComponents(CNtlPLGuiManager::GetInstance()->GetGuiManager());

	m_pThis = (gui::CDialog*)GetComponent("dlgMain");
	
		

	CRectangle rect;

	// Create GUI의 포인터를 보관
	m_pCreateGui = pCreateGui;

	// 배경 상단
	m_Background_UP.SetType(CWindowby3::WT_VERTICAL);
	m_Background_UP.SetSurface(0, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundLeft_UP" ));
	m_Background_UP.SetSurface(1, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundCenter_UP" ));
	m_Background_UP.SetSurface(2, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundRight_UP" ));	
	m_Background_UP.SetSize(222, 512);

	// 배경 하단
	m_Background_DOWN.SetType(CWindowby3::WT_VERTICAL);
	m_Background_DOWN.SetSurface(0, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundLeft_Down" ));
	m_Background_DOWN.SetSurface(1, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundCenter_Down" ));
	m_Background_DOWN.SetSurface(2, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundRight_Down" ));	
	m_Background_DOWN.SetSize(222, 94);

	// Title text surface
	//m_srfTitleText.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfMakeText" ));
	rect.SetRectWH(50, 12, 100, 14);
	m_pTitle = NTL_NEW gui::CStaticBox(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_LEFT);
	m_pTitle->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	m_pTitle->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_CHARACTER_CUSTOMISATION));
	m_pTitle->Enable(false);


	// 종족 버튼
	sDISPLAY_RACE_BUTTONS* pRaceDisplay = NTL_NEW sDISPLAY_RACE_BUTTONS;
	m_aptAttributeDisplay[ATTIRIBUTE_RACE] = pRaceDisplay;

	rect.SetRectWH(64, 47, 100, 14);
	pRaceDisplay->pAttributeName = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER );
	pRaceDisplay->pAttributeName->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	pRaceDisplay->pAttributeName->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_RACE));
	pRaceDisplay->pAttributeName->Enable(false);

	pRaceDisplay->pRaceButton[RACE_HUMAN] = (gui::CButton*)GetComponent("btnHuman");
	pRaceDisplay->pRaceButton[RACE_NAMEK] = (gui::CButton*)GetComponent("btnNamek");
	pRaceDisplay->pRaceButton[RACE_MAJIN] = (gui::CButton*)GetComponent("btnMajin");

	pRaceDisplay->pRaceButton[RACE_FREEZA] = (gui::CButton*)GetComponent("btnFreeza");
	pRaceDisplay->pRaceButton[RACE_KAIOSHIN] = (gui::CButton*)GetComponent("btnKaioshin");

	pRaceDisplay->pRaceButton[RACE_FREEZA]->Enable(false);
	pRaceDisplay->pRaceButton[RACE_KAIOSHIN]->Enable(false);

	pRaceDisplay->pRaceButton[RACE_FREEZA]->SetToolTip(L"Icerian");
	pRaceDisplay->pRaceButton[RACE_KAIOSHIN]->SetToolTip(L"Kaioshin");

	for( RwUInt8 i = 0 ; i < 4 ; ++i )
	{
		pRaceDisplay->pRaceButton[i]->SetToolTip( GetDisplayStringManager()->GetString(DST_HUMAN + i) );

		pRaceDisplay->slotButton[i]
		= pRaceDisplay->pRaceButton[i]->SigClicked().Connect(this, &CCharMakePartGui::OnClick_RaceButton);
	}

	pRaceDisplay->srfSelect.SetSurface( GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfSelect" ));


	// 클래스 버튼
	sDISPLAY_CLASS_BUTTONS* pClassDisplay = NTL_NEW sDISPLAY_CLASS_BUTTONS;
	m_aptAttributeDisplay[ATTIRIBUTE_CLASS] = pClassDisplay;

	rect.SetRectWH(64, 186, 100, 14);
	pClassDisplay->pAttributeName = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER );
	pClassDisplay->pAttributeName->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	pClassDisplay->pAttributeName->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_CLASS));
	pClassDisplay->pAttributeName->Enable(false);

	pClassDisplay->pClassButton[DEFAULT_CLASS_HUMAN_FIGHTER]= (gui::CButton*)GetComponent("btnHumanFighter");
	pClassDisplay->pClassButton[DEFAULT_CLASS_HUMAN_MYSTIC]	= (gui::CButton*)GetComponent("btnHumanMystic");
	pClassDisplay->pClassButton[DEFAULT_CLASS_NAMEK_FIGHTER]= (gui::CButton*)GetComponent("btnNamekFighter");
	pClassDisplay->pClassButton[DEFAULT_CLASS_NAMEK_MYSTIC]	= (gui::CButton*)GetComponent("btnNamekMystic");
	pClassDisplay->pClassButton[DEFAULT_CLASS_MAJIN_MIGHTY]	= (gui::CButton*)GetComponent("btnMajinMighty");
	pClassDisplay->pClassButton[DEFAULT_CLASS_MAJIN_WONDER]	= (gui::CButton*)GetComponent("btnMajinWonder");

	pClassDisplay->pClassButton[DEFAULT_CLASS_HUMAN_FIGHTER]->SetToolTip( GetDisplayStringManager()->GetString(DST_HUMAN_FIGHTER) );
	pClassDisplay->pClassButton[DEFAULT_CLASS_HUMAN_MYSTIC] ->SetToolTip( GetDisplayStringManager()->GetString(DST_HUMAN_MYSTIC) );
	pClassDisplay->pClassButton[DEFAULT_CLASS_NAMEK_FIGHTER]->SetToolTip( GetDisplayStringManager()->GetString(DST_NAMEK_FIGHTER) );
	pClassDisplay->pClassButton[DEFAULT_CLASS_NAMEK_MYSTIC] ->SetToolTip( GetDisplayStringManager()->GetString(DST_NAMEK_MYSTIC) );
	pClassDisplay->pClassButton[DEFAULT_CLASS_MAJIN_MIGHTY] ->SetToolTip( GetDisplayStringManager()->GetString(DST_MIGHTY_MAJIN) );
	pClassDisplay->pClassButton[DEFAULT_CLASS_MAJIN_WONDER] ->SetToolTip( GetDisplayStringManager()->GetString(DST_WONDER_MAJIN) );

	for( RwUInt8 i = 0 ; i < NUM_DEFAULT_CLASS ; ++i )
	{
		pClassDisplay->slotButton[i]
		= pClassDisplay->pClassButton[i]->SigClicked().Connect(this, &CCharMakePartGui::OnClick_ClassButton);
	}

	pClassDisplay->srfSelect.SetSurface( GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfSelect" ));


	// 성별 버튼
	sDISPLAY_SEX_BUTTONS* pSexDisplay = NTL_NEW sDISPLAY_SEX_BUTTONS;
	m_aptAttributeDisplay[ATTIRIBUTE_SEX] = pSexDisplay;

	rect.SetRectWH(64, 272, 100, 14);
	pClassDisplay->pAttributeName = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER );
	pClassDisplay->pAttributeName->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	pClassDisplay->pAttributeName->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_GENDER));
	pClassDisplay->pAttributeName->Enable(false);

	pSexDisplay->pSexButton[GENDER_MALE]	= (gui::CButton*)GetComponent("btnMale");
	pSexDisplay->pSexButton[GENDER_FEMALE]	= (gui::CButton*)GetComponent("btnFemale");

	pSexDisplay->pSexButton[GENDER_MALE]	->SetToolTip( GetDisplayStringManager()->GetString(DST_AVATAR_SEX_MALE) );
	pSexDisplay->pSexButton[GENDER_FEMALE]	->SetToolTip( GetDisplayStringManager()->GetString(DST_AVATAR_SEX_FEMALE) );

	for( RwUInt8 i = 0 ; i < dNUM_SEX ; ++i )
	{
		pSexDisplay->slotButton[i]
		= pSexDisplay->pSexButton[i]->SigClicked().Connect(this, &CCharMakePartGui::OnClick_SexButton);
	}

	pSexDisplay->srfSelect.SetSurface( GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfSelect" ));

	//Scale text
	rect.SetRectWH(64, 372, 100, 14);
	m_pScaleText = NTL_NEW gui::CStaticBox(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER);
	m_pScaleText->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	m_pScaleText->SetText("Scale");
	m_pScaleText->Enable(false);

	// 머리, 머리색, 얼굴, 피부색
	RwInt32 iStartLeftRightY = 410;
	for( RwUInt8 i = ATTIRIBUTE_HAIR ; i < ATTIRIBUTE_NUM ; ++i )
	{
		sDISPLAY_LEFT_RIGHT_BUTTONS* pDisplayLR = NTL_NEW sDISPLAY_LEFT_RIGHT_BUTTONS;
		m_aptAttributeDisplay[i] = pDisplayLR;

		// 왼쪽 버튼
		rect.SetRectWH(35, iStartLeftRightY, 20, 20);
		pDisplayLR->pLeftButton = (gui::CButton*) NTL_NEW gui::CButton(rect, "",
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfLeftBtnUp" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfLeftBtnDown" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfLeftBtnDis" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfLeftBtnFoc" ),
									NTL_BUTTON_UP_COLOR, NTL_BUTTON_DOWN_COLOR, NTL_BUTTON_FOCUS_COLOR, NTL_BUTTON_UP_COLOR,
									GUI_BUTTON_DOWN_COORD_X, GUI_BUTTON_DOWN_COORD_Y, m_pThis, GetNtlGuiManager()->GetSurfaceManager() );
		pDisplayLR->m_slotLeftButton = pDisplayLR->pLeftButton->SigClicked().Connect(this, &CCharMakePartGui::OnClick_LeftButton);

		// 오른쪽 버튼
		rect.SetRectWH(168, iStartLeftRightY, 20, 20);
		pDisplayLR->pRightButton = (gui::CButton*) NTL_NEW gui::CButton(rect, "",
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfRightBtnUp" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfRightBtnDown" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfRightBtnDis" ),
									GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfRightBtnFoc" ),
									NTL_BUTTON_UP_COLOR, NTL_BUTTON_DOWN_COLOR, NTL_BUTTON_FOCUS_COLOR, NTL_BUTTON_UP_COLOR,
									GUI_BUTTON_DOWN_COORD_X, GUI_BUTTON_DOWN_COORD_Y, m_pThis, GetNtlGuiManager()->GetSurfaceManager() );
		pDisplayLR->m_slotRightButton = pDisplayLR->pRightButton->SigClicked().Connect(this, &CCharMakePartGui::OnClick_RightButton);

		// 속성 이름
		rect.SetRectWH(64, iStartLeftRightY + 3, 100, 14);
		pDisplayLR->pAttributeName = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER );
		pDisplayLR->pAttributeName->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
		pDisplayLR->pAttributeName->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_RACE + i));
		pDisplayLR->pAttributeName->Enable(false);

		// 배경바
		pDisplayLR->srfBackground.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfLeftRightBackground" ));
		pDisplayLR->srfBackground.SetPositionfromParent(49, iStartLeftRightY + 1);

		iStartLeftRightY	+= 32;
	}
	fScaleValue = 1.0f;
	fScaleMaxValue = 1.8f;
	fScaleMinValue = 0.2f;
	m_fScale = fScaleMinValue;
	// 자동 선택 버튼

	

	m_pScale = (gui::CScrollBar*)GetComponent("scbScale");
	m_pScale->SetRange(0, 100);
	m_pScale->SetValue(50);

	m_slotScaleSliderMoved = m_pScale->SigSliderMoved().Connect(this, &CCharMakePartGui::OnScaleChanged);
	m_slotScaleValueChanged = m_pScale->SigValueChanged().Connect(this, &CCharMakePartGui::OnScaleChanged);

	m_pRendomButton = (gui::CButton*)GetComponent("btnRendom");
	m_pRendomButton->SetText( GetDisplayStringManager()->GetString(DST_LOBBY_RANDOM_CHOICE) );
	m_slotRendomButton = m_pRendomButton->SigClicked().Connect(this, &CCharMakePartGui::OnClick_RendomButton);


	// default value
	m_atAttributeChar[ATTIRIBUTE_RACE].byMaxValue		= RACE_COUNT;
	m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue		= DBO_CHAR_HAIR_SHAPE_COUNT;
	m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byMaxValue	= DBO_CHAR_HAIR_COLOR_COUNT;
	m_atAttributeChar[ATTIRIBUTE_FACE].byMaxValue		= DBO_CHAR_FACE_SHAPE_COUNT;
	m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue		= DBO_CHAR_SKIN_COLOR_COUNT;
	
	

	// sig
	m_slotMove			= m_pThis->SigMove().Connect(this, &CCharMakePartGui::OnMove );
	m_slotPaint			= m_pThis->SigPaint().Connect( this, &CCharMakePartGui::OnPaint );
	m_slotPostPaint		= m_pRendomButton->SigPaint().Connect( this, &CCharMakePartGui::OnPostPaint );




	LinkMsg(g_EventCharStageStateEnter);
	LinkMsg(g_EventLobbyMessage);

	Show(false);

	NTL_RETURN(TRUE);
}

VOID CCharMakePartGui::Destroy()
{
	NTL_FUNCTION("CCharMakePartGui::Destroy");

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
		NTL_DELETE(m_aptAttributeDisplay[i]);


	UnLinkMsg(g_EventCharStageStateEnter);
	UnLinkMsg(g_EventLobbyMessage);

	CNtlPLGui::DestroyComponents();
	CNtlPLGui::Destroy();

	NTL_RETURNVOID();
}

VOID CCharMakePartGui::SetPosition(RwInt32 iXPos, RwInt32 iYPos)
{
	m_pThis->SetPosition(iXPos, iYPos);

	RwInt32 iStartX = iXPos;
	RwInt32 iStartY = iYPos;

	m_Background_UP.SetPosition(iStartX, iStartY);
	m_Background_DOWN.SetPosition(iStartX, iStartY + 512);
	//m_srfTitleText.SetPosition(iStartX + 47, iStartY + 9);
}

VOID CCharMakePartGui::EnableButtons(bool bEnable)
{
	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
	{
		m_aptAttributeDisplay[i]->Enable(bEnable);
	}

	m_pRendomButton->ClickEnable(bEnable);
}

VOID CCharMakePartGui::UpdateAttribute()
{
	switch( m_atAttributeChar[ATTIRIBUTE_RACE].byValue )
	{
	case RACE_HUMAN:
		{
			m_atAttributeChar[ATTIRIBUTE_SEX].byMaxValue	= dNUM_SEX;
			m_atAttributeChar[ATTIRIBUTE_CLASS].byMaxValue	= dNUM_HUMAN_DEFAULT_CLASS;
			break;
		}
	case RACE_NAMEK:
		{
			m_atAttributeChar[ATTIRIBUTE_SEX].byMaxValue	= 0;	// 성별이 없다
			m_atAttributeChar[ATTIRIBUTE_CLASS].byMaxValue	= dNUM_NAMEK_DEFAULT_CLASS;
			break;
		}
	case RACE_MAJIN:
		{
			m_atAttributeChar[ATTIRIBUTE_SEX].byMaxValue	= dNUM_SEX;
			m_atAttributeChar[ATTIRIBUTE_CLASS].byMaxValue	= dNUM_MAJIN_DEFAULT_CLASS;
			break;
		}
	default:
		{
			NTL_ASSERT(false, "CCharMakePartGui::UpdateAttribute, Invalid race type : " << m_atAttributeChar[ATTIRIBUTE_RACE].byValue);
			return;
		}
	}	

	// 기본값
	for( RwUInt8 i = ATTIRIBUTE_CLASS ; i < ATTIRIBUTE_NUM ; ++i )
	{		
		m_atAttributeChar[i].Select(0);
		m_atAttributeChar[i].byPostValue = 0;
	}

	// 종족별 기본 클래스 보여주기
	sDISPLAY_CLASS_BUTTONS* pDisplay = reinterpret_cast<sDISPLAY_CLASS_BUTTONS*>( m_aptAttributeDisplay[ATTIRIBUTE_CLASS] );
	pDisplay->ActivateClass(m_atAttributeChar[ATTIRIBUTE_RACE].byValue);

	// 리스트가 바뀌면 모델은 정지한 상태로 정면을 바라보나
	CDboEventGenerator::LobbyEvent(LMT_ROTATION_FOREWARD);
}

VOID CCharMakePartGui::SetAttribute(RwUInt8 byAttribute, RwUInt8 byIndex)
{
	m_atAttributeChar[byAttribute].Select(byIndex);


	if( byAttribute == ATTIRIBUTE_RACE )
	{
		UpdateAttribute();
		ChangeModel(true);
	}
	else
		ChangeModel(false);


	// 버튼 사용여부 판단
	CheckEnableButton();
}

VOID CCharMakePartGui::CheckEnableButton()
{
	// 종족 비활성화 여부 판단
	sDISPLAY_RACE_BUTTONS* pDisplayRace = reinterpret_cast<sDISPLAY_RACE_BUTTONS*>( m_aptAttributeDisplay[ATTIRIBUTE_RACE] );
	for( RwUInt8 i = 0 ; i < DBO_ALLOWED_FUNC_FOR_DEV_COUNT ; ++i )
	{
	//	if( GetDboGlobal()->HaveFunction((eDBO_ALLOWED_FUNC_FOR_DEV)i) )
			pDisplayRace->pRaceButton[i]->Show(true);
	//	else
	//		pDisplayRace->pRaceButton[i]->Show(false);
	}
	

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
		m_aptAttributeDisplay[i]->Enable(true);


	switch( m_atAttributeChar[ATTIRIBUTE_RACE].byValue )
	{
	case RACE_HUMAN:
		{
			if( m_atAttributeChar[ATTIRIBUTE_SEX].byValue == GENDER_MALE )
			{
				if( m_atAttributeChar[ATTIRIBUTE_HAIR].byValue == 1 )
				{
					// 휴먼 대머리는 머리색을 선택할 수 없다
					m_aptAttributeDisplay[ATTIRIBUTE_HAIR_COLOR]->Enable(false);					
				}
				else if( m_atAttributeChar[ATTIRIBUTE_HAIR].byPostValue == 1 &&
						 m_atAttributeChar[ATTIRIBUTE_HAIR].byPostValue != m_atAttributeChar[ATTIRIBUTE_HAIR].byValue )
				{
					// 대머리였다
					m_aptAttributeDisplay[ATTIRIBUTE_HAIR_COLOR]->Enable(true);
					m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].Select( m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byValue );
				}
				else
				{
					for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
					{
						m_aptAttributeDisplay[i]->Enable(true);
					}
				}
			}
			else
			{
				for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
				{
					m_aptAttributeDisplay[i]->Enable(true);
				}

				// 남자로 선택되어 있을 때 대머리였을지도 모르기에 다시 설정
				m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].Select( m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byValue );
			}
			break;
		}
	case RACE_NAMEK:
		{
			// 나메크인의 경우 성별, 머리색을 고를 수 없다
			m_aptAttributeDisplay[ATTIRIBUTE_SEX]->Enable(false);
			m_aptAttributeDisplay[ATTIRIBUTE_HAIR_COLOR]->Enable(false);
			break;
		}
	case RACE_MAJIN:
		{
			// 마인의 경우 머리색을 고를 수 없다
			m_aptAttributeDisplay[ATTIRIBUTE_HAIR_COLOR]->Enable(false);
			break;
		}
	default:
		NTL_ASSERT(false, "CCharMakePartGui::CheckEnableButton, Invalid race index : " << m_atAttributeChar[ATTIRIBUTE_RACE].byValue );
	}
}

VOID CCharMakePartGui::ChangeModel(RwBool bSetDefaultCamera)
{
	RwUInt8 byGender, byClass;

	if( m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK )
	{
		// 나메크인의 피부색과 머리색은 같다
		m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byValue = m_atAttributeChar[ATTIRIBUTE_SKIN].byValue;
	}

	byClass = ConverIndexToClass( m_atAttributeChar[ATTIRIBUTE_CLASS].byValue );
	byGender = ConverIndexToGenderbyRace( m_atAttributeChar[ATTIRIBUTE_RACE].byValue );
	

	// +1 : 모델 인덱스가 1부터 시작하는 것도 있다
	CDboEventGenerator::CharMaking(bSetDefaultCamera,
								   m_atAttributeChar[ATTIRIBUTE_RACE].byValue,
								   byClass,
								   m_atAttributeChar[ATTIRIBUTE_HAIR].byValue + 1,
								   m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byValue + 1,
								   m_atAttributeChar[ATTIRIBUTE_FACE].byValue + 1,
								   m_atAttributeChar[ATTIRIBUTE_SKIN].byValue + 1,
								   byGender, m_fScale);
}

VOID CCharMakePartGui::CreateCharacter()
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	const WCHAR* pwcNameText = m_pCreateGui->GetInputText();
	
	if( FALSE == Logic_IsUsableCharacterName_with_WarningMessage(pwcNameText) )
		return;


	RwUInt8 byGender, byClass;

	byClass = ConverIndexToClass(m_atAttributeChar[ATTIRIBUTE_CLASS].byValue);
	byGender = ConverIndexToGenderbyRace(m_atAttributeChar[ATTIRIBUTE_RACE].byValue);

	// +1 : 모델 인덱스가 1부터 시작하는 것도 있다
	GetDboGlobal()->GetLobbyPacketGenerator()->SendCharAddReq(pwcNameText,
										m_atAttributeChar[ATTIRIBUTE_RACE].byValue,
										byClass, 
										byGender,
										m_atAttributeChar[ATTIRIBUTE_FACE].byValue + 1,
										m_atAttributeChar[ATTIRIBUTE_HAIR].byValue + 1,
										m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].byValue + 1, 
										m_atAttributeChar[ATTIRIBUTE_SKIN].byValue + 1, 1, m_fScale);

	GetCharStageState()->ChangeState(CHAR_STATE_MAKE_REQUEST);
}

VOID CCharMakePartGui::OnClick_RaceButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	sDISPLAY_RACE_BUTTONS* pDisplay = reinterpret_cast<sDISPLAY_RACE_BUTTONS*>( m_aptAttributeDisplay[ATTIRIBUTE_RACE] );

	for( RwUInt8 i = 0 ; i < RACE_COUNT ; ++i )
	{
		if( pDisplay->pRaceButton[i] == pComponent )
		{
			SetAttribute(ATTIRIBUTE_RACE, i);
			break;
		}
	}	
}

VOID CCharMakePartGui::OnClick_ClassButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	sDISPLAY_CLASS_BUTTONS* pDisplay = reinterpret_cast<sDISPLAY_CLASS_BUTTONS*>( m_aptAttributeDisplay[ATTIRIBUTE_CLASS] );
	sDISPLAY_CLASS_BUTTONS::LIST_ACTIVE_CLASS::iterator it = pDisplay->listActiveClass.begin();

	for( RwUInt8 i = 0 ; i < pDisplay->listActiveClass.size() ; ++i, ++it )
	{
		if( *it == pComponent )
		{
			SetAttribute(ATTIRIBUTE_CLASS, i);
			break;
		}
	}	
}

VOID CCharMakePartGui::OnClick_SexButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	sDISPLAY_SEX_BUTTONS* pDisplay = reinterpret_cast<sDISPLAY_SEX_BUTTONS*>( m_aptAttributeDisplay[ATTIRIBUTE_SEX] );

	for( RwUInt8 i = 0 ; i < dNUM_SEX ; ++i )
	{
		if( pDisplay->pSexButton[i] == pComponent )
		{
			SetAttribute(ATTIRIBUTE_SEX, i);
			break;
		}
	}	
}

VOID CCharMakePartGui::OnClick_LeftButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
	{
		sDISPLAY_LEFT_RIGHT_BUTTONS* pDisplayLR = reinterpret_cast<sDISPLAY_LEFT_RIGHT_BUTTONS*>( m_aptAttributeDisplay[i] );
		if( pDisplayLR->pLeftButton == pComponent )
		{
			if( m_atAttributeChar[i].byValue > 0 )
			{
				if (m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK)
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = 5;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = 5;
				}
				else
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = DBO_CHAR_HAIR_SHAPE_COUNT;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = DBO_CHAR_SKIN_COLOR_COUNT;
				}
				SetAttribute(i, m_atAttributeChar[i].byValue - 1);
			}
			else
			{
				if (m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK)
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = 5;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = 5;
				}
				else
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = DBO_CHAR_HAIR_SHAPE_COUNT;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = DBO_CHAR_SKIN_COLOR_COUNT;
				}
				SetAttribute(i, m_atAttributeChar[i].GetMaxValue() - 1);
			}

			break;
		}
	}
}

VOID CCharMakePartGui::OnClick_RightButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
	{
		sDISPLAY_LEFT_RIGHT_BUTTONS* pDisplayLR = reinterpret_cast<sDISPLAY_LEFT_RIGHT_BUTTONS*>( m_aptAttributeDisplay[i] );
		if( pDisplayLR->pRightButton == pComponent )
		{
			if( m_atAttributeChar[i].byValue < m_atAttributeChar[i].GetMaxValue() - 1)
			{
				if (m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK)
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = 5;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = 5;
				}
				else
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = DBO_CHAR_HAIR_SHAPE_COUNT;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = DBO_CHAR_SKIN_COLOR_COUNT;
				}
				SetAttribute(i, m_atAttributeChar[i].byValue + 1);
			}
			else
			{
				if (m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK)
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = 5;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = 5;
				}
				else
				{
					m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = DBO_CHAR_HAIR_SHAPE_COUNT;
					m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = DBO_CHAR_SKIN_COLOR_COUNT;
				}
				SetAttribute(i, 0);
			}

			break;
		}
	}
}

VOID CCharMakePartGui::OnClick_RendomButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_MAKE_IDLE )
		return;

	if (m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK)
	{
		m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = 5;
		m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = 5;
	}
	else
	{
		m_atAttributeChar[ATTIRIBUTE_HAIR].byMaxValue = DBO_CHAR_HAIR_SHAPE_COUNT;
		m_atAttributeChar[ATTIRIBUTE_SKIN].byMaxValue = DBO_CHAR_SKIN_COLOR_COUNT;
	}

	// 랜덤값 얻어오기
	m_atAttributeChar[ATTIRIBUTE_HAIR]		.Select((RwUInt8)NtlRandomNumber(0, DBO_CHAR_HAIR_SHAPE_COUNT - 1));
	m_atAttributeChar[ATTIRIBUTE_HAIR_COLOR].Select((RwUInt8)NtlRandomNumber(0, DBO_CHAR_HAIR_COLOR_COUNT - 1));
	m_atAttributeChar[ATTIRIBUTE_FACE]		.Select((RwUInt8)NtlRandomNumber(0, DBO_CHAR_FACE_SHAPE_COUNT - 1));
	m_atAttributeChar[ATTIRIBUTE_SKIN]		.Select((RwUInt8)NtlRandomNumber(0, DBO_CHAR_SKIN_COLOR_COUNT - 1));

	CheckEnableButton();

	// 실제 모델 바꾸기
	ChangeModel(false);
}

VOID CCharMakePartGui::OnScaleChanged(int iParam)
{
	const float sliderValueLeft = 0;
	const float sliderValueRight = 100;

	const float rangeLeft = 0.5;
	const float rangeRight = 1.25;

	float sliderValue = iParam;

	float OldRange = (sliderValueRight - sliderValueLeft);
	float NewRange = (rangeRight - rangeLeft);
	float newvalue = (((sliderValue - sliderValueLeft) * NewRange) / OldRange) + rangeLeft;


	m_fScale = newvalue;
	GetNtlSLGlobal()->GetSobAvatar()->GetSobProxy()->AddVisualSystemEffectScale(m_fScale, 0.01f);
	
	//ChangeModel(false);
}

RwUInt8 CCharMakePartGui::ConverIndexToClass(RwUInt8 byIndex)
{
	switch(m_atAttributeChar[ATTIRIBUTE_RACE].byValue)
	{
	case RACE_HUMAN:
		{
			switch(byIndex)
			{
			case 0:		return PC_CLASS_HUMAN_FIGHTER;
			case 1:		return PC_CLASS_HUMAN_MYSTIC;
			case 2:		return PC_CLASS_HUMAN_ENGINEER;
			}
		}
	case RACE_NAMEK:
		{
			switch(byIndex)
			{
			case 0:		return PC_CLASS_NAMEK_FIGHTER;
			case 1:		return PC_CLASS_NAMEK_MYSTIC;
			}
		}
	case RACE_MAJIN:
		{
			switch(byIndex)
			{
			case 0:		return PC_CLASS_MIGHTY_MAJIN;
			case 1:		return PC_CLASS_WONDER_MAJIN;
			}
		}
	}

	NTL_ASSERT(false, "CCharMakePartGui::ConverIndexToClass, Invalid class " << byIndex);
	return RACE_UNKNOWN;
}

RwUInt8 CCharMakePartGui::ConverIndexToGenderbyRace(RwInt32 iRace)
{
	RwUInt8 byGender = INVALID_BYTE;

	switch( iRace )
	{
	case RACE_HUMAN:	byGender = m_atAttributeChar[ATTIRIBUTE_SEX].byValue;	break;
	case RACE_NAMEK:	byGender = GENDER_ONE_SEX;								break;
	case RACE_MAJIN:	byGender = m_atAttributeChar[ATTIRIBUTE_SEX].byValue;	break;
	default: NTL_ASSERT(false, "CCharMakePartGui::ConverIndexToGenderbyRace, Invalid race : " << iRace);
		byGender = GENDER_ONE_SEX;break;
	}
	
	return byGender;
}

VOID CCharMakePartGui::OnMove(RwInt32 iOldX, RwInt32 iOldY)
{
	CRectangle rtScreen = m_pThis->GetScreenRect();

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
		m_aptAttributeDisplay[i]->OnMove(rtScreen.left, rtScreen.top);
}

VOID CCharMakePartGui::OnPaint()
{
	m_Background_UP.Render();
	m_Background_DOWN.Render();
	//m_srfTitleText.Render();

	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
		m_aptAttributeDisplay[i]->Render();
}

VOID CCharMakePartGui::OnPostPaint()
{
	for( RwUInt8 i = 0 ; i < ATTIRIBUTE_NUM ; ++i )
	{
		// 나메크 일 때는 성별의 선택사항을 표시하지 않는다
		if( m_atAttributeChar[ATTIRIBUTE_RACE].byValue == RACE_NAMEK )
		{
			if( ATTIRIBUTE_SEX == i )
				continue;
		}		

		m_aptAttributeDisplay[i]->PostRender( m_atAttributeChar[i].byValue );
	}
}

VOID CCharMakePartGui::HandleEvents( RWS::CMsg &msg )
{
	NTL_FUNCTION("CCharMakePartGui::HandleEvents");

	if( msg.Id == g_EventCharStageStateEnter )
	{
		SDboEventCharStageStateEnter* pEvent = reinterpret_cast<SDboEventCharStageStateEnter*>( msg.pData );

		if( GENDER_UNKNOWN == m_atAttributeChar[ATTIRIBUTE_SEX].byValue )
		{
			//EnableButtons(false);
			EnableButtons(true);
		}
		else if( pEvent->byState == CHAR_STATE_MAKE_IDLE )
		{
			EnableButtons(true);
		}
		else
		{
			//EnableButtons(false);
			EnableButtons(true);
		}


		if( pEvent->byState == CHAR_STATE_MAKE_ENTER )
		{
			m_atAttributeChar[ATTIRIBUTE_RACE].byValue = 0;
			m_atAttributeChar[ATTIRIBUTE_SEX].byValue = GENDER_MALE;//Default Character when select create
			UpdateAttribute();
			CheckEnableButton();
			ChangeModel(true);
		}
	}
	else if( msg.Id == g_EventLobbyMessage )
	{
		SDboEventLobbyMessage* pEvent = reinterpret_cast<SDboEventLobbyMessage*>( msg.pData );

		if( pEvent->byMessage == LMT_CREATE_CHARACTER )
		{
			CreateCharacter();
		}
	}	

	NTL_RETURNVOID();
}