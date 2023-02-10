//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTC
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTC

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// INITIALIZER                                                              //
//////////////////////////////////////////////////////////////////////////////

// Call this in OnPluginStart() to initialize some SMTC features, such as mini-crit effect handling with CTakeDamageInfo.
stock void SMTC_Initialize()
{
    // Set up hooks from the config file.
    GameData config = LoadGameConfigFile("NotnHeavy - SourceMod Type Collection");
    
    OSTypeUsed = view_as<OSType>(config.GetOffset("OSTypeUsed"));

    g_pTFPlayerClassDataMgr = config.GetAddress("g_pTFPlayerClassDataMgr");
    enginetrace = CEngineTrace(config.GetAddress("enginetrace"));
    g_pGameRules = CTFGameRules(config.GetAddress("g_pGameRules"));

    CTraceFilterSimple_ShouldHitEntity = config.GetMemSig("CTraceFilterSimple::ShouldHitEntity()");

    SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive = DHookCreateFromConf(config, "CTFPlayer::OnTakeDamage_Alive()");

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Virtual, "IEngineTrace::ClipRayToEntity()");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const Ray_t&
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // unsigned int
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // IHandleEntity*
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // trace_t*
    SMTC_SDKCall_IEngineTrace_ClipRayToEntity = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Virtual, "IEngineTrace::TraceRay()");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const Ray_t&
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // unsigned int
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // ITraceFilter*
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // trace_t*
    SMTC_SDKCall_IEngineTrace_TraceRay = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CBaseEntity::TakeDamage()");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const CTakeDamageInfo&
    PrepSDKCall_SetReturnInfo(SDKType_Float, SDKPass_Plain); // float
    SMTC_SDKCall_CBaseEntity_TakeDamage = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::CalculateFalloff()");
    SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::ApplyToEntity()");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // CBaseEntity* pEntity
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain); // int
    SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFGameRules::RadiusDamage()");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // CTFRadiusDamageInfo& info
    SMTC_SDKCall_CTFGameRules_RadiusDamage = EndPrepSDKCall();

    delete config;

    // For generating an offset on plugin start, getting datamap props from the world entity will not work 
    // because the world entity has not yet been initialised. Instead, I decided to go with getting the props
    // from CBasePlayer, which is part of the Source SDK.
    //int m_angRotation = FindDataMapInfo(0, "m_angRotation"), m_vecViewOffset = FindDataMapInfo(0, "m_vecViewOffset");
    int m_angRotation = FindSendPropInfo("CBasePlayer", "m_angRotation"), m_vecViewOffset = FindSendPropInfo("CBasePlayer", "m_vecViewOffset[0]");
    if (m_angRotation + 0x0C != m_vecViewOffset - 0x04)
        ThrowError("Could not calculate offset for CBaseEntity::m_RefEHandle.");
    m_RefEHandle_Offset = m_angRotation + 0x0C;

    // Call forwards.
    #if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR
    SMTC_VectorForward();
    #endif
    #if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTAKEDAMAGEINFO
    SMTC_CTakeDamageInfoForward();
    #endif
    #if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_UTIL
    SMTC_UTILForward();
    #endif
}

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

methodmap SMTC
{
    public static OSType GetOperatingSystem()
    {
        return OSTypeUsed;
    }
}