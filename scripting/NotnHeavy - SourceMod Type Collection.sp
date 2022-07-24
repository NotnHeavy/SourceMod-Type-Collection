// For testing purposes only. Please refer to ./scripting/include/ for all of the includes available.
// This is the most hackiest shit I've done in SourceMod so far.

#pragma semicolon true 
#pragma newdecls required

#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <tf2_stocks>
#include <smmem> // https://github.com/Scags/SM-Memory
#include <dhooks> // Not actually needed, just used for tests.

#include "SMTC"
#include "Pointer.inc"
#include "include/Vector.inc"
#include "QAngle.inc"
#include "CUtlMemory.inc"
#include "CUtlVector.inc"

#include "tf/CTakeDamageInfo.inc"
#include "tf/CTFRadiusDamageInfo.inc"
#include "tf/tf_shareddefs.inc"
#include "tf/TFPlayerClassData_t.inc"

static int CTFPlayer_m_hMyWearables;
static Handle SDKCall_CTFWearable_Equip;
static Handle SDKCall_CBaseEntity_TakeDamage;
static Handle SDKCall_CTFGameRules_RadiusDamage;
static DHookSetup DHooks_CTFPlayer_OnTakeDamage;

static int explosionModelIndex;

public void OnPluginStart()
{
    PrintToServer("------------------------------------------------------------------");

    LoadTranslations("common.phrases");

    SMTC_Initialize();

    GameData config = LoadGameConfigFile("NotnHeavy - SourceMod Type Collection (tests)");
    CTFPlayer_m_hMyWearables = config.GetOffset("CTFPlayer::m_hMyWearables");

    StartPrepSDKCall(SDKCall_Entity);
    PrepSDKCall_SetFromConf(config, SDKConf_Virtual, "CTFWearable::Equip");
    PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer);
    SDKCall_CTFWearable_Equip = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Entity);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CBaseEntity::TakeDamage");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    SDKCall_CBaseEntity_TakeDamage = EndPrepSDKCall();

    DHooks_CTFPlayer_OnTakeDamage = DHookCreateFromConf(config, "CTFPlayer::OnTakeDamage");
    DHookEnableDetour(DHooks_CTFPlayer_OnTakeDamage, false, CTFPlayer_OnTakeDamage);

    StartPrepSDKCall(SDKCall_GameRules);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFGameRules::RadiusDamage");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    SDKCall_CTFGameRules_RadiusDamage = EndPrepSDKCall();

    delete config;

    pointerOperation();
    vectorOperation();
    cutlvectorOperation(); // oh god
    ctakedamageinfoOperation();
    AddCommandListener(ctfradiusdamageinfoOperation, "voicemenu"); // ctfradiusdamageinfo operation
    qangleOperation();
    tfplayerclassdata_tOperation();

    PrintToServer("\n\"%s\" has loaded.\n------------------------------------------------------------------", "NotnHeavy - SourceMod Type Collection");
    PrintToChatAll("THE TEST PLUGIN FOR NOTNHEAVY'S SOURCEMOD TYPE COLLECTION IS CURRENTLY RUNNING.");
}

public void OnMapStart()
{
    explosionModelIndex = PrecacheModel("sprites/sprite_fire01.vmt");
}

static void tfplayerclassdata_tOperation()
{
    // offset verification
    PrintToServer("sizeof(TFPlayerClassData_T): %i", TFPLAYERCLASSDATA_T_SIZE); // should be 2288
    PrintToServer("TFPlayerClassData_t::m_flMaxSpeed: %i", TFPLAYERCLASSDATA_T_OFFSET_M_FLMAXSPEED); // should be 640
    PrintToServer("TFPlayerClassData_t::m_aWeapons: %i", TFPLAYERCLASSDATA_T_OFFSET_M_AWEAPONS); // should be 652
    PrintToServer("TFPlayerClassData_t::m_aGrenades: %i", TFPLAYERCLASSDATA_T_OFFSET_M_AGRENADES);
    PrintToServer("TFPlayerClassData_t::m_aAmmoMax: %i", TFPLAYERCLASSDATA_T_OFFSET_M_AAMMOMAX);
    PrintToServer("TFPlayerClassData_t::m_aBuildable: %i", TFPLAYERCLASSDATA_T_OFFSET_M_ABUILDABLE);
    PrintToServer("TFPlayerClassData_t::m_bDontDoAirwalk: %i", TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDOAIRWALK);
    PrintToServer("TFPlayerClassData_t::m_bDontDoNewJump: %i", TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDONEWJUMP);
    PrintToServer("TFPlayerClassData_t::m_bParsed: %i", TFPLAYERCLASSDATA_T_OFFSET_M_BPARSED);
    PrintToServer("TFPlayerClassData_t::m_vecThirdPersonoffset: %i", TFPLAYERCLASSDATA_T_OFFSET_M_VECTHIRDPERSONOFFSET);
    PrintToServer("TFPlayerClassData_t::m_szDeathSound: %i\n", TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND); // should be 752

    // get the actual class data!
    PrintToServer("g_pTFPlayerClassDataMgr: %i", g_pTFPlayerClassDataMgr);
    TFPlayerClassData_t pyroData = GetPlayerClassData(TF_CLASS_PYRO);
    pyroData.m_flMaxSpeed = 520.00; // we do a miniscule amount of trolling
    
    TFPlayerClassData_t spyData = GetPlayerClassData(TF_CLASS_SPY);
    PrintToServer("Spy's m_flMaxSpeed: %f", spyData.m_flMaxSpeed);
    spyData.m_flMaxSpeed = 300.00; // and well you know, lol. yw, sappho :^)

    TFPlayerClassData_t soldierData = GetPlayerClassData(TF_CLASS_SOLDIER);
    soldierData.m_nMaxHealth = 10000; // :skull:
    soldierData.m_flMaxSpeed = 520.00;

    TFPlayerClassData_t heavyData = GetPlayerClassData(TF_CLASS_HEAVYWEAPONS);
    heavyData.m_nMaxHealth = 1; // heavy has recently went on a diet!
    char buffer[TF_NAME_LENGTH];

    pyroData.m_szModelName_Get(buffer);
    PrintToServer("Pyro model: %s", buffer);
    pyroData.m_szHWMModelName_Get(buffer);
    PrintToServer("Pyro HWM model: %s", buffer);
    pyroData.m_szHandModelName_Get(buffer);
    PrintToServer("Pyro hands model: %s", buffer);
    pyroData.m_szLocalizableName_Get(buffer);
    PrintToServer("Pyro localizable name: %s", buffer);
    pyroData.m_szClassName_Get(buffer);
    PrintToServer("Pyro class name: %s", buffer);

    /*
    TFPlayerClassData_t demomanData = GetPlayerClassData(TF_CLASS_DEMOMAN);
    demomanData.m_szModelName_Set("models/player/pyro.mdl");
    demomanData.m_szHWMModelName_Set("models/player/pyro.mdl");
    demomanData.m_szHandModelName_Set("models/weapons/c_models/c_pyro_arms.mdl");
    */

    PrintToServer("");
}

static void qangleOperation()
{
    STACK_ALLOC(angle, QAngle, QANGLE_SIZE);
    angle.X = 1.00;
    angle.Y = 2.00;
    angle.Z = 3.00;
    PrintToServer("QAngle angle: %f: %f: %f", angle.X, angle.Y, angle.Z);

    STACK_ALLOC(direction, QAngle, QANGLE_SIZE);
    direction.X = 5.00;
    direction.Y = 5.00;
    direction.Z = 5.00;
    STACK_ALLOC(dest, QAngle, QANGLE_SIZE);
    VectorMA(angle, 3.00, direction, dest);
    PrintToServer("after VectorMA, dest: %f: %f: %f", dest.X, dest.Y, dest.Z);

    STACK_ALLOC(forwardVector, Vector, QANGLE_SIZE);
    STACK_ALLOC(right, Vector, QANGLE_SIZE);
    STACK_ALLOC(up, Vector, QANGLE_SIZE);
    AngleVectors(dest, forwardVector, right, up);
    PrintToServer("after AngleVectors, forward: %f: %f: %f, right: %f: %f: %f, up: %f: %f: %f\n", forwardVector.X, forwardVector.Y, forwardVector.Z, right.X, right.Y, right.Z, up.X, up.Y, up.Z);
}

static Action ctfradiusdamageinfoOperation(int client, const char[] argv, int argc)
{
    char args[128];
    GetCmdArgString(args, sizeof(args));
    if (strcmp(args, "0 0") == 0)
    {
        float buffer[3];
        GetEntPropVector(1, Prop_Data, "m_vecAbsOrigin", buffer);
        Vector damagePosition = AddressOfArray(buffer);

        CTakeDamageInfo damageInfo = CTakeDamageInfo.Malloc(client, client, client, damagePosition, damagePosition, 300.00, DMG_BLAST & DMG_HALF_FALLOFF, TF_CUSTOM_STICKBOMB_EXPLOSION, damagePosition);
        CTFRadiusDamageInfo radiusInfo = CTFRadiusDamageInfo.Malloc(damageInfo, damagePosition, 200.00);
        SDKCall(SDKCall_CTFGameRules_RadiusDamage, radiusInfo);
        free(damageInfo);
        free(radiusInfo);

        TE_SetupExplosion(buffer, explosionModelIndex, 10.0, 1, 0, 200, 0);
        TE_SendToAll();
    }
    return Plugin_Continue;
}

static void ctakedamageinfoOperation()
{
    // Player tests. (TF2)
    if (IsClientInGame(1))
    {
        /*
        float buffer[3];
        GetEntPropVector(1, Prop_Data, "m_vecAbsOrigin", buffer);
        Vector damagePosition = AddressOfArray(buffer);
        */
        Vector damagePosition = GetEntVector(1, Prop_Data, "m_vecAbsOrigin", .allocate = true);

        CTakeDamageInfo hell = CTakeDamageInfo.Malloc(0, 0, 0, damagePosition, damagePosition, 100.00, DMG_BLAST, TF_CUSTOM_STICKBOMB_EXPLOSION, damagePosition);
        SDKCall(SDKCall_CBaseEntity_TakeDamage, 1, hell);
        free(hell);
        free(damagePosition);
    }

    // Server tests.
    CTakeDamageInfo info = CTakeDamageInfo.Malloc(0, 0, 0, vec3_origin, vec3_origin, 0.00, 0, 0);
    PrintToServer("info.m_vecDamagePosition: %f: %f: %f", info.m_vecDamagePosition.X, info.m_vecDamagePosition.Y, info.m_vecDamagePosition.Z);
    info.m_vecDamagePosition.X = 3.00;
    PrintToServer("info.m_vecDamagePosition.X: %f, vec3_origin.X: %f\n", info.m_vecDamagePosition.X, vec3_origin.X);
    free(info);
}

// CTFPlayer::OnTakeDamage
MRESReturn CTFPlayer_OnTakeDamage(int entity, DHookReturn returnValue, DHookParam parameters)
{
    CTakeDamageInfo info = parameters.Get(1);
    info.SetCritType(CRIT_MINI, entity); // :^)
    return MRES_Ignored;
}

static void cutlvectorOperation()
{
    // Player tests. (TF2)
    if (IsClientInGame(1))
    {
        CUtlVector m_hMyWearables = CUtlVector(GetEntityAddress(1) + CTFPlayer_m_hMyWearables);
        PrintToServer("m_hMyWearables size: %i", m_hMyWearables.Count());
        for (int i = 0; i < m_hMyWearables.Count(); ++i)
            PrintToServer("m_hMyWearables[%i]: %i", i, m_hMyWearables.Get(i).DereferenceEHandle());
        PrintToServer("");

        // fuck it, gunboats
        int gunboats = CreateEntityByName("tf_wearable");
        SetEntProp(gunboats, Prop_Send, "m_bValidatedAttachedEntity", true);
        SetEntProp(gunboats, Prop_Send, "m_iItemDefinitionIndex", 133);
        SetEntProp(gunboats, Prop_Send, "m_bInitialized", 1);
        DispatchSpawn(gunboats);
        
        SetEntProp(gunboats, Prop_Send, "m_iAccountID", GetSteamAccountID(1));
        SetEntPropEnt(gunboats, Prop_Send, "m_hOwnerEntity", 1);

        // time to blow up valve headquarters
        m_hMyWearables.Get(m_hMyWearables.AddToHead()).WriteEHandle(gunboats);
        SDKCall(SDKCall_CTFWearable_Equip, gunboats, 1);
        for (int i = 0; i < m_hMyWearables.Count(); ++i)
            PrintToServer("after adding, m_hMyWearables[%i]: %i", i, m_hMyWearables.Get(i).DereferenceEHandle());
        PrintToServer("");
    }

    // Server tests.
    //int index = 0;
    CUtlVector intVector = CUtlVector.Malloc();
    //intVector.Get(intVector.InsertBefore(index++)).Write(40);
    //intVector.Get(intVector.InsertBefore(index++)).Write(100);
    //intVector.Get(intVector.InsertBefore(index++)).Write(250);
    intVector.AddToTailGetPtr().Write(40);
    intVector.AddToTailGetPtr().Write(100);
    intVector.AddToTailGetPtr().Write(250);
    for (int i = 0; i < intVector.Count(); ++i)
    {
        PrintToServer("intVector[%i]: %i", i, intVector.Get(i).Dereference());
    }

    intVector.Reverse();
    for (int i = 0; i < intVector.Count(); ++i)
    {
        PrintToServer("after reverse: intVector[%i]: %i", i, intVector.Get(i).Dereference());
    }

    // Now just fill it up. :P
    for (int i = 0; i < 10; ++i)
    {
        if (i >= intVector.Count())
        {
            //Pointer yes = intVector.Get(intVector.InsertBefore(index++));
            Pointer yes = intVector.AddToTailGetPtr();
            yes.Write(i + 100);
        }
        PrintToServer("while filling up, intVector[%i]: %i", i, intVector.Get(i).Dereference());
    }
    PrintToServer("");
    for (int i = 0; i < intVector.Count(); ++i)
    {
        PrintToServer("after filling up, intVector[%i]: %i", i, intVector.Get(i).Dereference());
    }
    int hasElement = 109;
    PrintToServer("Does intVector have 109? %s", intVector.HasElement(Pointer(AddressOf(hasElement))) ? "yes" : "no");
    hasElement = 110;
    PrintToServer("Does intVector have 110? %s\n", intVector.HasElement(Pointer(AddressOf(hasElement))) ? "yes" : "no");

    int valueToFill = 69420;
    intVector.FillWithValue(Pointer(AddressOf(valueToFill)));
    for (int i = 0; i < intVector.Count(); ++i)
    {
        PrintToServer("after filling up buffer with valueToFill, intVector[%i]: %i", i, intVector.Get(i).Dereference());
    }
    PrintToServer("");
    
    intVector.Dispose();
}

static void vectorOperation()
{
    Vector vector = Vector(AddressOfArray({3.00, 2.00, 1.00}));
    PrintToServer("Vector test: %f : %f : %f", vector.X, vector.Y, vector.Z);

    Vector vectorMalloc = Vector.Malloc(6.00, 5.00, 4.00);
    PrintToServer("Vector malloc test: %f : %f : %f", vectorMalloc.X, vectorMalloc.Y, vectorMalloc.Z);

    Vector result = Vector(AddressOfArray({0.00, 0.00, 0.00}));
    result.Assign(vector + vectorMalloc * 2.00);
    PrintToServer("Result vector: %f: %f: %f", result.X, result.Y, result.Z);

    Vector test = Vector(AddressOfArray({0.00, 0.00, 0.00}));
    test.X = 3.00;
    PrintToServer("Result X: %f\nTest X: %f", result.X, test.X);

    PrintToServer("result.Length(): %f", result.Length());
    PrintToServer("result.DistTo(test): %f", result.DistTo(test));
    PrintToServer("result.Dot(test): %f", result.Dot(test));

    Vector result2 = Vector(AddressOfArray({0.00, 0.00, 0.00}));
    result2.Assign(result.Cross(test));
    PrintToServer("result.Cross(test): %f: %f: %f", result2.X, result2.Y, result2.Z);

    Vector forwardVector = Vector(AddressOfArray({7.00, 8.00, 9.00}));
    Vector right = Vector(AddressOfArray({1.00, 2.00, 3.00}));
    Vector up = Vector(AddressOfArray({4.00, 5.00, 6.00}));
    VectorVectors(forwardVector, right, up);
    PrintToServer("forward/right/up after VectorVectors: %f: %f: %f, %f: %f: %f, %f: %f: %f", forwardVector.X, forwardVector.Y, forwardVector.Z, right.X, right.Y, right.Z, up.X, up.Y, up.Z);

    PrintToServer("result.NormalizeInPlace(): %f", result.NormalizeInPlace());
    PrintToServer("new co-ordinates for normalized vector: %f: %f: %f", result.X, result.Y, result.Z);

    STACK_ALLOC(stackAllocatedVector, Vector, VECTOR_SIZE);
    stackAllocatedVector.X = 3144.00;
    PrintToServer("stackAllocatedVector: %f: %f: %f\n", stackAllocatedVector.X, stackAllocatedVector.Y, stackAllocatedVector.Z);

    free(vectorMalloc);
}

static void pointerOperation()
{
    Pointer a = malloc(4);
    a.Write(10);

    Pointer b = malloc(4);
    b.Write(20);

    V_swap(a, b);
    PrintToServer("where a was 10 and b was 20 before V_swap, pointer a: %i, pointer b: %i\n", a.Dereference(), b.Dereference());  

    free(a);
    free(b);
}