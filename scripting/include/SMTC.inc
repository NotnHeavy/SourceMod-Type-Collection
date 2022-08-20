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

stock Pointer g_pTFPlayerClassDataMgr;

stock DHookSetup SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive;

stock Handle SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff;
stock Handle SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity;

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
    

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::CalculateFalloff()");
    SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::ApplyToEntity()");
    PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity = EndPrepSDKCall();

    delete config;

    // Call forwards.
    SMTC_VectorForward();
    SMTC_CTakeDamageInfoForward();
}

// Used in Vector.inc to initialize vec3_origin.
forward void SMTC_VectorForward();

// Used in CTakeDamageInfo.inc for mini-crit management.
forward void SMTC_CTakeDamageInfoForward();