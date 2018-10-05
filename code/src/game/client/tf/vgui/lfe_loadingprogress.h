//============== Copyright LFE-TEAM Not All rights reserved. ==================
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef TF_LOADINGPROGRESS_H
#define TF_LOADINGPROGRESS_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include <game/client/iviewport.h>
#include <vgui/IScheme.h>
#include "../game/shared/tf/tf_shareddefs.h"
#include "GameEventListener.h"

class CTFLoadingProgress : public vgui::EditablePanel, public CGameEventListener
{
private:
	DECLARE_CLASS_SIMPLE( CTFLoadingProgress, vgui::EditablePanel );

public:
	CTFLoadingProgress();	 

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnCommand( const char *command );
	virtual void PerformLayout();
	void ShowModal();

	virtual void FireGameEvent( IGameEvent *event );
private:
	MESSAGE_FUNC( OnActivate, "activate" );
	MESSAGE_FUNC( OnDeactivate, "deactivate" );

	void Reset();
	void SetDefaultSelections();
	void UpdateDialog();
	void UpdateTip();
	void UpdateControls();
	void ClearMapLabel();
	static float SafeCalcFraction( float flNumerator, float flDemoninator );

	vgui::Label		*m_pTipLabel;
	vgui::Label		*m_pTipText;

	vgui::Button *m_pNextTipButton;
	vgui::Button *m_pCloseButton;

	bool m_bInteractive;							// are we in interactive mode
	bool m_bControlsLoaded;							// have we loaded controls yet

	int m_iTotalSpawns;								// how many spawns of all classes does this player have

};


CTFLoadingProgress *GLoadingProgress();
void DestroyLoadingProgress();

#endif // TF_LOADINGPROGRESS_H