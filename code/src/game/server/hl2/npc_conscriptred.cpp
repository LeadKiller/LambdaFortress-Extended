
//=========================================================
// monster template
//=========================================================
// UNDONE: Holster weapon?


//Stacker/Nullen - TODO: Make them go back to following the player after combat if player made him follow before initiating combat
//Stacker/Nullen - TODO #2: Give citizen medics the ability to heal them

#include	"cbase.h"

//#if 0

#include	"npc_talker.h"
#include	"ai_schedule.h"
#include	"scripted.h"
#include	"basecombatweapon.h"
#include	"soundent.h"
#include	"NPCEvent.h"
#include	"AI_Hull.h"
#include	"AI_Node.h"
#include	"AI_Network.h"
#include	"ai_hint.h"
#include	"player.h"
#include	"npc_conscriptred.h"
#include	"activitylist.h"
#include "AI_Interactions.h"
#include "IEffects.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"

ConVar	sk_conscriptred_health( "sk_conscriptred_health","100");
ConVar	sk_conscriptred_model( "sk_conscriptred_model", "models/conscript.mdl" );


#define CONSCRIPTRED_MAD 		"CONSCRIPTRED_MAD"
#define CONSCRIPTRED_SHOT 		"CONSCRIPTRED_SHOT"
#define CONSCRIPTRED_KILL 		"CONSCRIPTRED_KILL"
#define CONSCRIPTRED_OUT_AMMO 	"CONSCRIPTRED_OUT_AMMO"
#define CONSCRIPTRED_ATTACK 	"CONSCRIPTRED_ATTACK"
#define CONSCRIPTRED_LINE_FIRE "CONSCRIPTRED_LINE_FIRE"
#define CONSCRIPTRED_POK 		"CONSCRIPTRED_POK"

#define CONSCRIPTRED_PAIN1		"CONSCRIPTRED_PAIN1"
#define CONSCRIPTRED_PAIN2		"CONSCRIPTRED_PAIN2"
#define CONSCRIPTRED_PAIN3		"CONSCRIPTRED_PAIN3"

#define CONSCRIPTRED_DIE1		"CONSCRIPTRED_DIE1"
#define CONSCRIPTRED_DIE2		"CONSCRIPTRED_DIE2"
#define CONSCRIPTRED_DIE3		"CONSCRIPTRED_DIE3"

//=========================================================
// Combine activities
//=========================================================
int	ACT_CONSCRIPTRED_AIM;

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
// first flag is barney dying for scripted sequences?
#define		CONSCRIPTRED_AE_RELOAD		( 1 )
#define		CONSCRIPTRED_AE_DRAW		( 2 )
#define		CONSCRIPTRED_AE_SHOOT		( 3 )
#define		CONSCRIPTRED_AE_HOLSTER	( 4 )

#define		CONSCRIPTRED_BODY_GUNHOLSTERED	0
#define		CONSCRIPTRED_BODY_GUNDRAWN		1
#define		CONSCRIPTRED_BODY_GUNGONE		2

//-----------------------------------------------------------------------------
// Purpose: Initialize the custom schedules
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_ConscriptRed::InitCustomSchedules(void) 
{
	INIT_CUSTOM_AI(CNPC_ConscriptRed);

	ADD_CUSTOM_TASK(CNPC_ConscriptRed,	TASK_CONSCRIPTRED_CROUCH);

	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_FOLLOW);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_DRAW);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_FACE_TARGET);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_STAND);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_AIM);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_HIT);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_PULL);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_CHOMP);
	ADD_CUSTOM_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_CHEW);

	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPTRED_AIM);

	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_FOLLOW);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_DRAW);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_FACE_TARGET);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_STAND);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_AIM);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_HIT);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_PULL);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_CHOMP);
	AI_LOAD_SCHEDULE(CNPC_ConscriptRed,	SCHED_CONSCRIPTRED_BARNACLE_CHEW);
}

LINK_ENTITY_TO_CLASS( npc_conscriptred, CNPC_ConscriptRed );
IMPLEMENT_CUSTOM_AI( npc_conscriptred, CNPC_ConscriptRed );

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CNPC_ConscriptRed )

	DEFINE_FIELD( m_fGunDrawn,			FIELD_BOOLEAN ),
	DEFINE_FIELD( m_painTime,			FIELD_TIME ),
	DEFINE_FIELD( m_checkAttackTime,	FIELD_TIME ),
	DEFINE_FIELD( m_nextLineFireTime,	FIELD_TIME ),
	DEFINE_FIELD( m_lastAttackCheck,	FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bInBarnacleMouth,	FIELD_BOOLEAN ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_ConscriptRed::RunTask( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_RANGE_ATTACK1:
		if (GetEnemy() != NULL && (GetEnemy()->IsPlayer()))
		{
			m_flPlaybackRate = 1.5;
		}
		BaseClass::RunTask( pTask );
		break;
	default:
		BaseClass::RunTask( pTask );
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
Activity CNPC_ConscriptRed::NPC_TranslateActivity( Activity eNewActivity )  
//To  DL'ers: Comment this block out if you don't want them to limp when critically injured
{
		if (m_iHealth <= 15 && eNewActivity == ACT_RUN)
		{
			// limp!
			return ACT_WALK_HURT;
		}


		if (m_iHealth <= 15 && eNewActivity == ACT_WALK ) 
		{
			// limp!
			return ACT_WALK_HURT;
		}
		
	
	return eNewActivity;

	return BaseClass::NPC_TranslateActivity( eNewActivity );
}


//=========================================================
// GetSoundInterests - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CNPC_ConscriptRed::GetSoundInterests ( void) 
{
	return	SOUND_WORLD		|
			SOUND_COMBAT	|
			SOUND_CARCASS	|
			SOUND_MEAT		|
			SOUND_GARBAGE	|
			SOUND_DANGER	|
			SOUND_PLAYER;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
Class_T	CNPC_ConscriptRed::Classify ( void )
{
	return	CLASS_PLAYER_ALLY;
}

//=========================================================
// ALertSound - barney says "Freeze!"
//=========================================================
void CNPC_ConscriptRed::AlertSound( void )
{
	if ( GetEnemy() != NULL )
	{
		if ( IsOkToCombatSpeak() )
		{
			Speak( CONSCRIPTRED_ATTACK );
		}
	}

}
//=========================================================
// MaxYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
float CNPC_ConscriptRed::MaxYawSpeed ( void )
{
	switch ( GetActivity() )
	{
	case ACT_IDLE:		
		return 70;
		break;
	case ACT_WALK:
		return 70;
		break;
	case ACT_RUN:
		return 90;
		break;
	default:
		return 70;
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Set proper blend for shooting
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_ConscriptRed::ConscriptRedFirePistol ( void )
{
	Vector vecShootOrigin;

	vecShootOrigin = GetLocalOrigin() + Vector( 0, 0, 55 );
	Vector vecShootDir = GetShootEnemyDir( vecShootOrigin );

	QAngle angDir;
	VectorAngles( vecShootDir, angDir );
	SetPoseParameter( 0, angDir.x );
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================
void CNPC_ConscriptRed::HandleAnimEvent( animevent_t *pEvent )
{
	switch( pEvent->event )
	{
	case CONSCRIPTRED_AE_RELOAD:
		
		// We never actually run out of ammo, just need to refill the clip
		if (GetActiveWeapon())
		{
			GetActiveWeapon()->WeaponSound( RELOAD_NPC );
			GetActiveWeapon()->m_iClip1 = GetActiveWeapon()->GetMaxClip1(); 
			GetActiveWeapon()->m_iClip2 = GetActiveWeapon()->GetMaxClip2(); 
		}
		ClearCondition(COND_NO_PRIMARY_AMMO);
		ClearCondition(COND_NO_SECONDARY_AMMO);
		break;

	case CONSCRIPTRED_AE_SHOOT:
		ConscriptRedFirePistol();
		break;

	case CONSCRIPTRED_AE_DRAW:
		// barney's bodygroup switches here so he can pull gun from holster
		m_nBody = CONSCRIPTRED_BODY_GUNDRAWN;
		m_fGunDrawn = true;
		break;

	case CONSCRIPTRED_AE_HOLSTER:
		// change bodygroup to replace gun in holster
		m_nBody = CONSCRIPTRED_BODY_GUNHOLSTERED;
		m_fGunDrawn = false;
		break;

	default:
		BaseClass::HandleAnimEvent( pEvent );
	}
}

//=========================================================
// Spawn
//=========================================================
void CNPC_ConscriptRed::Spawn()
{
	if ( sv_hl2_beta.GetFloat() == 1 )
	{
		Precache( );

		SetModel( "models/conscript_red.mdl" );
		SetHullType(HULL_HUMAN);
		SetHullSizeNormal();

		SetSolid( SOLID_BBOX );
		AddSolidFlags( FSOLID_NOT_STANDABLE );
		SetMoveType( MOVETYPE_STEP );
		SetBloodColor( BLOOD_COLOR_RED );
		m_iHealth			= sk_conscriptred_health.GetFloat();
		SetViewOffset( Vector ( 0, 0, 50 ) );// position of the eyes relative to monster's origin.
		m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
		m_NPCState			= NPC_STATE_NONE;

		m_nBody			= 0; // gun in holster
		m_fGunDrawn			= false;
		m_bInBarnacleMouth	= false;

		m_nextLineFireTime	= 0;

		m_HackedGunPos		= Vector ( 0, 0, 55 );

		CapabilitiesAdd( bits_CAP_TURN_HEAD | bits_CAP_MOVE_GROUND | bits_CAP_MOVE_JUMP | bits_CAP_MOVE_CLIMB | bits_CAP_NO_HIT_PLAYER | bits_CAP_FRIENDLY_DMG_IMMUNE | bits_CAP_ANIMATEDFACE );
		CapabilitiesAdd	( bits_CAP_USE_WEAPONS | bits_CAP_SQUAD );
		CapabilitiesAdd	( bits_CAP_DUCK );			// In reloading and cover

		NPCInit();
		SetUse( &CNPCSimpleTalker::FollowerUse );
	}
	else
	{
		Remove();
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CNPC_ConscriptRed::Precache()
{
	engine->PrecacheModel("models/conscript_red.mdl");
	engine->PrecacheModel( sk_conscriptred_model.GetString() );

	enginesound->PrecacheSound("barney/ba_pain1.wav");
	enginesound->PrecacheSound("barney/ba_pain2.wav");
	enginesound->PrecacheSound("barney/ba_pain3.wav");

	enginesound->PrecacheSound("barney/ba_die1.wav");
	enginesound->PrecacheSound("barney/ba_die2.wav");
	enginesound->PrecacheSound("barney/ba_die3.wav");
	
	enginesound->PrecacheSound("barney/ba_close.wav");

	// every new barney must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();
	BaseClass::Precache();
}	

// Init talk data
void CNPC_ConscriptRed::TalkInit()
{
	
	BaseClass::TalkInit();

		// vortigaunt will try to talk to friends in this order:
	m_szFriends[0] = "npc_conscriptred";
	m_szFriends[1] = "npc_vortigaunt";

	// get voice for head - just one barney voice for now
	GetExpresser()->SetVoicePitch( 100 );
}


static bool IsFacing( CBaseCombatCharacter *pBCC, const Vector &reference )
{
	Vector vecDir = (reference - pBCC->GetLocalOrigin());
	vecDir.z = 0;
	VectorNormalize( vecDir );
	Vector vecForward = pBCC->BodyDirection2D( );

	// He's facing me, he meant it
	if ( DotProduct( vecForward, vecDir ) > 0.96 )	// +/- 15 degrees or so
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CNPC_ConscriptRed::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	// make sure friends talk about it if player hurts talkmonsters...
	int ret = BaseClass::OnTakeDamage_Alive( info );
	if (!IsAlive())
	{
		return ret;
	}

	if ( m_NPCState != NPC_STATE_PRONE && (info.GetAttacker()->GetFlags() & FL_CLIENT) )
	{

		// This is a heurstic to determine if the player intended to harm me
		// If I have an enemy, we can't establish intent (may just be crossfire)
		if ( GetEnemy() == NULL )
		{
			// If I'm already suspicious, get mad
			if (m_afMemory & bits_MEMORY_SUSPICIOUS)
			{
				// Alright, now I'm pissed!
				Speak( CONSCRIPTRED_MAD );

				Remember( bits_MEMORY_PROVOKED );

				// Allowed to hit the player now!
				CapabilitiesRemove(bits_CAP_NO_HIT_PLAYER);

				StopFollowing();
			}
			else
			{
				// Hey, be careful with that
				Speak( CONSCRIPTRED_SHOT );
				Remember( bits_MEMORY_SUSPICIOUS );
			}
		}
		else if ( !(GetEnemy()->IsPlayer()) && (m_lifeState != LIFE_DEAD ))
		{
			Speak( CONSCRIPTRED_SHOT );
		}
	}
	return ret;
}

//------------------------------------------------------------------------------
// Purpose : Override to always shoot at eyes (for ducking behind things)
// Input   :
// Output  :
//------------------------------------------------------------------------------
Vector CNPC_ConscriptRed::BodyTarget( const Vector &posSrc, bool bNoisy ) 
{
	return EyePosition();
}

//=========================================================
// PainSound
//=========================================================
void CNPC_ConscriptRed::PainSound ( void )
{
	if (gpGlobals->curtime < m_painTime)
		return;

	AIConcept_t concepts[] =
	{
		CONSCRIPTRED_PAIN1,
		CONSCRIPTRED_PAIN2,
		CONSCRIPTRED_PAIN3,
	};
	
	m_painTime = gpGlobals->curtime + random->RandomFloat(0.5, 0.75);

	Speak( concepts[random->RandomInt(0,2)] );
}

//=========================================================
// DeathSound 
//=========================================================
void CNPC_ConscriptRed::DeathSound ( void )
{
	AIConcept_t concepts[] =
	{
		CONSCRIPTRED_DIE1,
		CONSCRIPTRED_DIE2,
		CONSCRIPTRED_DIE3,
	};
	
	Speak( concepts[random->RandomInt(0,2)] );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_ConscriptRed::TraceAttack( const CTakeDamageInfo &inputInfo, const Vector &vecDir, trace_t *ptr )
{
	CTakeDamageInfo info = inputInfo;

	switch( ptr->hitgroup)
	{
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
		if (info.GetDamageType() & (DMG_BULLET | DMG_SLASH | DMG_BLAST))
		{
			info.ScaleDamage( 0.5f );
		}
		break;
	case 10:
		if (info.GetDamageType() & (DMG_BULLET | DMG_SLASH | DMG_CLUB))
		{
			info.SetDamage( info.GetDamage() - 20 );
			if (info.GetDamage() <= 0)
			{
				g_pEffects->Ricochet( ptr->endpos, (vecDir*-1.0f) );
				info.SetDamage( 0 );
			}
		}
		// always a head shot
		ptr->hitgroup = HITGROUP_HEAD;
		break;
	}

	//BaseClass::TraceAttack( info, vecDir, ptr );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int CNPC_ConscriptRed::TranslateSchedule( int scheduleType )
{
	int baseType;

	switch( scheduleType )
	{
	case SCHED_CONSCRIPTRED_DRAW:
		if ( GetEnemy() != NULL )
		{
			// face enemy, then draw.
			return SCHED_CONSCRIPTRED_DRAW;
		}
		else
		{
			// BUGBUG: What is this code supposed to do when there isn't an enemy?
			Warning( "BUG IN CONSCRIPTRED AI!\n");
		}
		break;

	// Hook these to make a looping schedule
	case SCHED_TARGET_FACE:
		{
			// call base class default so that barney will talk
			// when 'used' 
			baseType = BaseClass::TranslateSchedule(scheduleType);

			if (baseType == SCHED_IDLE_STAND)
				return SCHED_CONSCRIPTRED_FACE_TARGET;	// override this for different target face behavior
			else
				return baseType;
		}
		break;

	case SCHED_CHASE_ENEMY:
		{
			// ---------------------------------------------
			// If I'm in ducking, cover pause for while
			// before running towards my enemy.  See if they
			// come out first as this is a good place to be!
			// ---------------------------------------------
			if (HasMemory(bits_MEMORY_INCOVER))
			{
				Forget( bits_MEMORY_INCOVER );
				return SCHED_COMBAT_SWEEP;
			}
		}
		break;
	case SCHED_TARGET_CHASE:
		return SCHED_CONSCRIPTRED_FOLLOW;
		break;

	case SCHED_IDLE_STAND:
		{
			// call base class default so that scientist will talk
			// when standing during idle
			baseType = BaseClass::TranslateSchedule(scheduleType);

			if (baseType == SCHED_IDLE_STAND)
			{
				// just look straight ahead
				return SCHED_CONSCRIPTRED_STAND;
			}
			return baseType;
			break;

		}
	case SCHED_FAIL_ESTABLISH_LINE_OF_FIRE:
		{
			return SCHED_CONSCRIPTRED_AIM;
			break;
		}
	}
	return BaseClass::TranslateSchedule( scheduleType );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int CNPC_ConscriptRed::SelectSchedule ( void )
{
	// These things are done in any state but dead and prone
	if (m_NPCState != NPC_STATE_DEAD && m_NPCState != NPC_STATE_PRONE)
	{
		if ( HasCondition( COND_HEAR_DANGER ) )
		{
			return SCHED_TAKE_COVER_FROM_BEST_SOUND;
		}
		if ( HasCondition( COND_ENEMY_DEAD ) && IsOkToCombatSpeak() )
		{
			Speak( CONSCRIPTRED_KILL );
		}
	}
	switch( m_NPCState )
	{
	case NPC_STATE_PRONE:
		{
			if (m_bInBarnacleMouth)
			{
				return SCHED_CONSCRIPTRED_BARNACLE_CHOMP;
			}
			else
			{
				return SCHED_CONSCRIPTRED_BARNACLE_HIT;
			}
		}
	case NPC_STATE_COMBAT:
		{
// dead enemy
			if ( HasCondition( COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return BaseClass::SelectSchedule();
			}

			// always act surprized with a new enemy
			if ( HasCondition( COND_NEW_ENEMY ) && HasCondition( COND_LIGHT_DAMAGE) )
				return SCHED_SMALL_FLINCH;
				
			// wait for one schedule to draw gun
			if (!m_fGunDrawn )
				return SCHED_CONSCRIPTRED_DRAW;

			if ( HasCondition( COND_HEAVY_DAMAGE ) )
				return SCHED_TAKE_COVER_FROM_ENEMY;

			// ---------------------
			// no ammo
			// ---------------------
			if ( HasCondition ( COND_NO_PRIMARY_AMMO ) )
			{
				Speak( CONSCRIPTRED_OUT_AMMO );
				return SCHED_HIDE_AND_RELOAD;
			}
			else if (!HasCondition(COND_CAN_RANGE_ATTACK1) && HasCondition( COND_NO_SECONDARY_AMMO ))
			{
				Speak( CONSCRIPTRED_OUT_AMMO );
				return SCHED_HIDE_AND_RELOAD;
			}

			/* UNDONE: check distance for genade attacks...
			// If player is next to what I'm trying to attack...
			if ( HasCondition( COND_WEAPON_PLAYER_NEAR_TARGET ))
			{
				return SCHED_CONSCRIPTRED_AIM;
			}
			*/			

			// -------------------------------------------
			// If I might hit the player shooting...
			// -------------------------------------------
			if ( HasCondition( COND_WEAPON_PLAYER_IN_SPREAD ))
			{
				if ( IsOkToCombatSpeak() && m_nextLineFireTime	< gpGlobals->curtime)
				{
					Speak( CONSCRIPTRED_LINE_FIRE );
					m_nextLineFireTime = gpGlobals->curtime + 3.0f;
				}

				// Run to a new location or stand and aim
				if (random->RandomInt(0,2) == 0)
				{
					return SCHED_ESTABLISH_LINE_OF_FIRE;
				}
				else
				{
					return SCHED_CONSCRIPTRED_AIM;
				}
			}

			// -------------------------------------------
			// If I'm in cover and I don't have a line of
			// sight to my enemy, wait randomly before attacking
			// -------------------------------------------

		}
		break;

	case NPC_STATE_ALERT:	
	case NPC_STATE_IDLE:
		if ( HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE))
		{
			// flinch if hurt
			return SCHED_SMALL_FLINCH;
		}

	
		// try to say something about smells
		TrySmellTalk();
		break;
	}
	
	return BaseClass::SelectSchedule();
}




//-----------------------------------------------------------------------------
// Purpose:  This is a generic function (to be implemented by sub-classes) to
//			 handle specific interactions between different types of characters
//			 (For example the barnacle grabbing an NPC)
// Input  :  Constant for the type of interaction
// Output :	 true  - if sub-class has a response for the interaction
//			 false - if sub-class has no response
//-----------------------------------------------------------------------------
bool CNPC_ConscriptRed::HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt)
{
	if (interactionType == g_interactionBarnacleVictimDangle)
	{
		// Force choosing of a new schedule
		ClearSchedule("Released by Barnacle");
		m_bInBarnacleMouth	= true;
		return true;
	}
	else if ( interactionType == g_interactionBarnacleVictimReleased )
	{
		m_IdealNPCState = NPC_STATE_IDLE;

		CPASAttenuationFilter filter( this );

		m_bInBarnacleMouth	= false;
		SetAbsVelocity( vec3_origin );
		SetMoveType( MOVETYPE_STEP );
		return true;
	}
	else if ( interactionType == g_interactionBarnacleVictimGrab )
	{
		if ( GetFlags() & FL_ONGROUND )
		{
			RemoveFlag( FL_ONGROUND );
		}
		m_IdealNPCState = NPC_STATE_PRONE;
		PainSound();
		return true;
	}
	return false;
}

void CNPC_ConscriptRed::DeclineFollowing( void )
{
	Speak( CONSCRIPTRED_POK );
}

WeaponProficiency_t CNPC_ConscriptRed::CalcWeaponProficiency( CBaseCombatWeapon *pWeapon )
{
	if( FClassnameIs( pWeapon, "weapon_pistol" ) )
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}

	if( FClassnameIs( pWeapon, "weapon_smg1" ) )
	{
		return WEAPON_PROFICIENCY_VERY_GOOD;
	}

	if( FClassnameIs( pWeapon, "weapon_oicw" ) )
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}


	if( FClassnameIs( pWeapon, "weapon_tommygun" ) )
	{
		return WEAPON_PROFICIENCY_VERY_GOOD;
	}

	if( FClassnameIs( pWeapon, "weapon_dbarrel" ) ) //Gotta have perfect proficiency with the double shotgun
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}

	else
	{
		return WEAPON_PROFICIENCY_VERY_GOOD;
	}

	return BaseClass::CalcWeaponProficiency( pWeapon );
}


//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

//=========================================================
// > SCHED_CONSCRIPTRED_FOLLOW
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_FOLLOW,

	"	Tasks"
	"		TASK_GET_PATH_TO_TARGET			0"
	"		TASK_MOVE_TO_TARGET_RANGE		128"	// Move within 128 of target ent (client)
	"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_TARGET_FACE "
	""
	"	Interrupts"
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_HEAR_DANGER"
	"		COND_PROVOKED"
);

//=========================================================
//  > SCHED_CONSCRIPTRED_DRAW
//		much better looking draw schedule for when
//		conscriptred knows who he's gonna attack.
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_DRAW,

	"	Tasks"
	"		 TASK_STOP_MOVING					0"
	"		 TASK_FACE_ENEMY					0"
	"		 TASK_PLAY_SEQUENCE_FACE_ENEMY		ACTIVITY:ACT_ARM "
	""
	"	Interrupts"
);

//===============================================
//	> SCHED_CONSCRIPTRED_AIM
//
//	Stand in place and aim at enemy (used when
//  line of sight blocked by player)
//===============================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_AIM,

	"	Tasks"
	"		TASK_STOP_MOVING		0"
	"		TASK_FACE_ENEMY			0"
	"		TASK_PLAY_SEQUENCE		ACTIVITY:ACT_CONSCRIPTRED_AIM"
	""
	"	Interrupts"
	"		COND_NEW_ENEMY"
	"		COND_ENEMY_DEAD"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_NO_PRIMARY_AMMO"
	"		COND_WEAPON_HAS_LOS"
	"		COND_CAN_MELEE_ATTACK1 "
	"		COND_CAN_MELEE_ATTACK2 "
	"		COND_HEAR_DANGER"
);

//=========================================================
// > SCHED_CONSCRIPTRED_FACE_TARGET
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_FACE_TARGET,

	"	Tasks"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE"
	"		TASK_FACE_TARGET			0"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE"
	"		TASK_SET_SCHEDULE			SCHEDULE:SCHED_TARGET_CHASE"
	""
	"	Interrupts"
	//"		CLIENT_PUSH			<<TODO>>
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_HEAR_DANGER"
	"		COND_PROVOKED"
);

//=========================================================
// > SCHED_CONSCRIPTRED_STAND
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_STAND,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE "
	"		TASK_WAIT					2"	// repick IDLESTAND every two seconds.
	"		TASK_TALKER_HEADRESET		0"	// reset head position
	""
	"	Interrupts	 "
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_SMELL"
	"		COND_PROVOKED"
	"		COND_HEAR_COMBAT"
	"		COND_HEAR_DANGER"
);

//=========================================================
// > SCHED_CONSCRIPTRED_BARNACLE_HIT
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_BARNACLE_HIT,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_HIT"
	"		TASK_SET_SCHEDULE			SCHEDULE:SCHED_CONSCRIPTRED_BARNACLE_PULL"
	""
	"	Interrupts"
);

//=========================================================
// > SCHED_CONSCRIPTRED_BARNACLE_PULL
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_BARNACLE_PULL,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_PULL"
	""
	"	Interrupts"
);

//=========================================================
// > SCHED_CONSCRIPTRED_BARNACLE_CHOMP
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_BARNACLE_CHOMP,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_CHOMP"
	"		 TASK_SET_SCHEDULE			SCHEDULE:SCHED_CONSCRIPTRED_BARNACLE_CHEW"
	""
	"	Interrupts"
);

//=========================================================
// > SCHED_CONSCRIPTRED_BARNACLE_CHEW
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPTRED_BARNACLE_CHEW,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_CHEW"
);


//#endif