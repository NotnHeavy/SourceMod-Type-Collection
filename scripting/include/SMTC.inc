//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTC
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTC

#include <smmem>
#include <dhooks>

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

Pointer g_pTFPlayerClassDataMgr;

static DHookSetup SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive;

Handle SMTC_DHooks_CTFRadiusDamageInfo_CalculateFalloff;
Handle SMTC_DHooks_CTFRadiusDamageInfo_ApplyToEntity;

int SMTC_PlayerTakingMiniCrits = 0;

//////////////////////////////////////////////////////////////////////////////
// INITIALIZER                                                              //
//////////////////////////////////////////////////////////////////////////////

// Call this in OnPluginStart() to initialize some SMTC features, such as mini-crit effect handling with CTakeDamageInfo.
stock void SMTC_Initialize()
{
    // Set up hooks from the config file.
    GameData config = LoadGameConfigFile("NotnHeavy - SourceMod Type Collection");

    g_pTFPlayerClassDataMgr = view_as<Pointer>(config.GetAddress("g_pTFPlayerClassDataMgr"));

    SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive = DHookCreateFromConf(config, "CTFPlayer::OnTakeDamage_Alive()");
    DHookEnableDetour(SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive, false, SMTC_CTFPlayer_OnTakeDamage_Alive);

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::CalculateFalloff()");
    SMTC_DHooks_CTFRadiusDamageInfo_CalculateFalloff = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::ApplyToEntity()");
    PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    SMTC_DHooks_CTFRadiusDamageInfo_ApplyToEntity = EndPrepSDKCall();

    delete config;

    // Miscallaneous hooks.
    HookEvent("player_hurt", SMTC_ClientHurt, EventHookMode_Pre);

    // Set up globals.
#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR
    vec3_origin = Vector(AddressOfArray({0.00, 0.00, 0.00}));
#endif
}

//////////////////////////////////////////////////////////////////////////////
// HOOKS                                                                    //
//////////////////////////////////////////////////////////////////////////////

// Fix the bug where mini-crits toggled manually shows the full crit effect instead. This is due to DMG_CRIT being toggled when using either mini-crits or crits, and DMG_CRIT defaulting to the full crit effect.
static Action SMTC_ClientHurt(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (SMTC_PlayerTakingMiniCrits == client) 
        event.SetInt("bonuseffect", 1); // CRIT_MINI
    SMTC_PlayerTakingMiniCrits = 0;
    return Plugin_Changed;
}

// CTFPlayer::OnTakeDamage_Alive
static MRESReturn SMTC_CTFPlayer_OnTakeDamage_Alive(int entity, DHookReturn returnValue, DHookParam parameters)
{
    CTakeDamageInfo info = parameters.Get(1);
    if (info.m_eCritType != CRIT_MINI)
        SMTC_PlayerTakingMiniCrits = 0;
    return MRES_Ignored;
}