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
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::CalculateFalloff()");
    SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFRadiusDamageInfo::ApplyToEntity()");
    PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity = EndPrepSDKCall();

    delete config;

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