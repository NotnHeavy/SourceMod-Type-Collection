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
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

typeset SMTC_ForwardCallback
{
    // FORWARDTYPE_ADDPOINT (TF2 only): When a new tf_point_t object is instantiated 
    // within a CTFPointManager (or derived) object // this will be invoked through 
    // CTFPointManager::AddPoint().
#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TF_POINT_T && defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CBASEENTITY
    function void (const CBaseEntity manager, const TF_Point_t point);
#endif
};

enum SMTC_ForwardType
{
    FORWARDTYPE_ADDPOINT
}

enum struct SMTC_HookData
{
    SMTC_ForwardType type;
    SMTC_ForwardCallback callback;
    int entity;
}

static ArrayList g_Hooks;

//////////////////////////////////////////////////////////////////////////////
// SMTC FORWARDS                                                            //
//////////////////////////////////////////////////////////////////////////////

static void SMTC_HookGC()
{
    for (int i = 0, size = g_Hooks.Length; i < size; ++i)
    {
        SMTC_HookData data;
        g_Hooks.GetArray(i, data);
        if (!IsValidEntity(data.entity))
        {
            g_Hooks.Erase(i--);
            --size;
        }
    }
}

// Hook onto entities and invoke forwards whenever a specific event occurs.
// Intended to work in similar fashion to SDKHooks, uses DHooks behind the scenes.
// The garbage collector is called within this function.
stock void SMTC_HookEntity(int entity, SMTC_ForwardType type, SMTC_ForwardCallback callback)
{
    SMTC_HookGC();

    switch (type)
    {
#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TF_POINT_T
    case FORWARDTYPE_ADDPOINT:
        SMTC_DHooks_CTFPointManager_AddPoint.HookEntity(Hook_Post, entity, CTFPointManager_AddPoint);
#endif
    default:
        ThrowError("Incorrect SMTC_ForwardType specified! %u", type);
    }

    SMTC_HookData data;
    data.type = type;
    data.callback = callback;
    data.entity = EntIndexToEntRef(entity);
    g_Hooks.PushArray(data);
}

// Get hook data for the specified entity.
stock void SMTC_GetHookData(int entity, SMTC_ForwardType type, SMTC_HookData data)
{
    int ref = EntIndexToEntRef(entity);
    for (int i = 0, size = g_Hooks.Length; i < size; ++i)
    {
        SMTC_HookData temp;
        g_Hooks.GetArray(i, temp);
        if (temp.entity == ref && temp.type == type)
            data = temp;
    }
}

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

    CTFPointManager_m_Points = config.GetOffset("CTFPointManager::m_Points");

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TF_POINT_T
    SMTC_tf_point_t_vtable = config.GetAddress("tf_point_t_vtable");
#endif

    CTraceFilterSimple_ShouldHitEntity = config.GetMemSig("CTraceFilterSimple::ShouldHitEntity()");

    SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive = DynamicDetour.FromConf(config, "CTFPlayer::OnTakeDamage_Alive()");
    
    SMTC_DHooks_CTFPointManager_AddPoint = DynamicHook.FromConf(config, "CTFPointManager::AddPoint()");

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

    // Instantiate g_Hooks.
    g_Hooks = new ArrayList(sizeof(SMTC_HookData));

    // Generate the string function sdkcalls beforehand.
    STACK_ALLOC(str1, Pointer, 1);
    STACK_ALLOC(str2, Pointer, 1);
    str1.strlen();
    str1.strnlen(1);
    str1.strcmp(str2);
    str1.strncmp(str2, 1);
    str1.strchr('S');
    str1.strstr(str2);

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
// SMTC                                                                     //
//////////////////////////////////////////////////////////////////////////////

methodmap SMTC
{
    public static OSType GetOperatingSystem()
    {
        return OSTypeUsed;
    }
}