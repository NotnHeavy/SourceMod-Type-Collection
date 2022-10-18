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
#include "ctypes.inc"
#include "Pointer.inc"
#include "include/Vector.inc"
#include "QAngle.inc"
#include "CUtlMemory.inc"
#include "CUtlVector.inc"
#include "cplane_t.inc"
#include "CBaseTrace.inc"
#include "csurface_t.inc"
#include "CGameTrace.inc"
#include "shareddefs.inc"
#include "vtable.inc"

#include "tf/CTakeDamageInfo.inc"
#include "tf/CTFRadiusDamageInfo.inc"
#include "tf/tf_shareddefs.inc"
#include "tf/TFPlayerClassData_t.inc"

static int CTFPlayer_m_hMyWearables;
static Handle SDKCall_CTFWearable_Equip;
static Handle SDKCall_CBaseEntity_TakeDamage;
static Handle SDKCall_CTFGameRules_RadiusDamage;
static DHookSetup DHooks_CTFPlayer_OnTakeDamage;
static DHookSetup DHooks_CTFPlayer_TraceAttack;

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

    DHooks_CTFPlayer_TraceAttack = DHookCreateFromConf(config, "CTFPlayer::TraceAttack()");
    DHookEnableDetour(DHooks_CTFPlayer_TraceAttack, false, CTFPlayer_TraceAttack);

    delete config;

    //commitTests(); // TESTS/tests.inc

    pointerOperation();
    vectorOperation();
    cutlvectorOperation(); // oh god
    ctakedamageinfoOperation();
    AddCommandListener(ctfradiusdamageinfoOperation, "voicemenu"); // ctfradiusdamageinfo operation
    qangleOperation();
    tfplayerclassdata_tOperation();
    cgametrace_csurface_t_cbasetrace_cplane_tOperation();
    ctypesOperation();
    vtableOperation();

    PrintToServer("\n\"%s\" has loaded.\n------------------------------------------------------------------", "NotnHeavy - SourceMod Type Collection");
    PrintToChatAll("THE TEST PLUGIN FOR NOTNHEAVY'S SOURCEMOD TYPE COLLECTION IS CURRENTLY RUNNING.");
}

public void OnMapStart()
{
    explosionModelIndex = PrecacheModel("sprites/sprite_fire01.vmt");
}

public void OnPluginEnd()
{
    VTable.ClearVTables();
}

static void vtableOperation()
{
    // this stuff is tested for windows

    if (SMTC.GetOperatingSystem() == OSTYPE_WINDOWS)
    {
        // normal sdkcall
        /*
        ; int __cdecl test(void):
        0x55               push ebp
        0x8B 0xEC          mov ebp, esp
        0xB8 08 00 00 00   mov eax, 0x08
        0x5D               pop ebp
        0xC3               ret
        */
        char testString[] = "\x55\x8B\xEC\xB8\x08\x00\x00\x00\x5D\xC3";
        
        StartPrepSDKCall(SDKCall_Static);
        PrepSDKCall_SetAddress(AddressOfString(testString));
        PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
        Handle SDKCall_test = EndPrepSDKCall();

        int value = SDKCall(SDKCall_test);
        PrintToServer("test() return: %i\n", value);

        // sdkcall for virtual method (what the actual fuck)
        /*
        struct obj
        {
            int val;
            obj()
                : val(8)
            {
            }

            virtual int test()
            {
                return val;
            }  
        };

        obj variable;
        variable.test();
        */

        /*
        ; int __thiscall test(void): ; would be __cdecl on linux
        0x55             push ebp
        0x8B 0xEC        mov ebp, esp
        0x51             push ecx ; this (would be first param with __cdecl)
        0x89 0x4D 0xFC   mov dword ptr [ebp - 0x04], ecx
        0x8B 0x45 0xFC   mov eax, dword ptr[ebp - 0x04]
        0x8B 0x40 0x04   mov eax, dword ptr[eax + 0x04]
        0x8B 0xE5        mov esp, ebp
        0x5D             pop ebp
        0xC3             ret
        */
    
        int offset_val = 1;

        /*
        char vtable_testString[] = "\x55\x8B\xEC\x51\x89\x4D\xFC\x8B\x45\xFC\x8B\x40\x04\x8B\xE5\x5D\xC3";
        Pointer testStringPointer = Pointer(AddressOfString(vtable_testString));
        Pointer vtable[1];
        vtable[0] = testStringPointer;
        Pointer vtable_ptr = AddressOfArray(vtable);
        */
        char testMethod[] = "\x55\x8B\xEC\x51\x89\x4D\xFC\x8B\x45\xFC\x8B\x40\x04\x8B\xE5\x5D\xC3";
        PrintToServer("creating obj vtable: %i", VTable.CreateVTable("obj", 1));
        VTable.RegisterVPointer("obj", 0, Pointer(AddressOfString(testMethod)), sizeof(testMethod));

        any variable[2];
        variable[offset_val] = 8;             // int val;
        VTable.HookOntoObject("obj", Pointer(AddressOfArray(variable)));
        PrintToServer("VTable.GetObjectVTable(variable): %i", VTable.GetObjectVTable(Pointer(AddressOfArray(variable))));

        StartPrepSDKCall(SDKCall_Raw);
        PrepSDKCall_SetVirtual(0);
        PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
        Handle SDKCall_variable_test = EndPrepSDKCall();

        value = SDKCall(SDKCall_variable_test, AddressOfArray(variable));
        PrintToServer("variable.test() return: %i", value);
        variable[offset_val] = 69420;
        value = SDKCall(SDKCall_variable_test, AddressOfArray(variable));
        PrintToServer("variable.test() return: %i\n", value);

        // override variable.test()
        /*
        ; int __thiscall test(void):
        0x55                       push ebp
        0x8B 0xEC                  mov ebp, esp
        0x51                       push ecx
        0x89 0x4D 0xFC             mov dword ptr [ebp - 0x04], ecx
        0xB8 0x28 0x00 0x00 0x00   mov eax, 0x28 ; 40
        0x8B 0xE5                  mov esp, ebp
        0x5D                       pop ebp
        0xC3                       ret
        */
        char newTest[] = "\x55\x8B\xEC\x51\x89\x4D\xFC\xB8\x28\x00\x00\x00\x8B\xE5\x5D\xC3";

        // we should not be doing this!
        // Pointer vtable = VTable.GetObjectVTable(Pointer(AddressOfArray(variable)));
        // VTable.OverrideVPointer(vtable, Pointer(AddressOfString(newTest)), 0); 

        VTable.RemoveVPointer("obj", 0);
        VTable.RegisterVPointer("obj", 0, Pointer(AddressOfString(newTest)), FROM_EXISTING_SOURCE);

        value = SDKCall(SDKCall_variable_test, AddressOfArray(variable));
        PrintToServer("after overriding, variable.test() return: %i, variable.val: %i", value, variable[offset_val]);

        char buffer[32];
        VTable.IsObjectUsingRegisteredVTable(Pointer(AddressOfArray(variable)), buffer);
        PrintToServer("does variable have obj vtable: %i\n", StrEqual(buffer, "obj"));

        // derived vtable test
        VTable.CreateVTable("derived", 1, "obj");
        VTable.HookOntoObject("derived", Pointer(AddressOfArray(variable)));
        VTable.IsObjectUsingRegisteredVTable(Pointer(AddressOfArray(variable)), buffer);
        PrintToServer("does variable have derived vtable: %i", StrEqual(buffer, "derived"));

        value = SDKCall(SDKCall_variable_test, AddressOfArray(variable));
        PrintToServer("after changing vtable to derived, variable.test() return: %i, variable.val: %i", value, variable[offset_val]);
        
        // now for the real shit
        // because vtables on objects are references, this will apply to every player.
        if (IsClientInGame(1))
        {
            PrintToServer("");
            Pointer player = GetEntityAddress(1);
            Pointer playerVTable = VTable.GetObjectVTable(player);
            PrintToServer("player vtable: %i", playerVTable);

            /*
            ; bool __thiscall ShouldGib(const CTakeDamageInfo& info):
            0x55                       push ebp
            0x8B 0xEC                  mov ebp, esp
            0x32 0xC0                  xor al, al
            0x5D                       pop ebp
            0xC2 0x04 0x00             ret 0x04
            */

            char shouldGib[] = "\x55\x8B\xEC\x32\xC0\x5D\xC2\x04\x00";
            Pointer subroutine = malloc(sizeof(shouldGib));
            memcpy(subroutine, AddressOfString(shouldGib), sizeof(shouldGib));
            VTable.OverrideVPointer(playerVTable, subroutine, 293);

            VTable.IsObjectUsingRegisteredVTable(player, buffer);
            PrintToServer("does entity index 1 (%i) have obj vtable: %i", player, StrEqual(buffer, "obj"));
        }
    }
    else
        PrintToServer("Not using Windows; skipping vtableOperation()...");
    
    PrintToServer("");
}

// this is kind of cursed
static void ctypesOperation()
{
    // BYTE = UInt8_t
    // CHAR = Int8_t
    // USHORT = UInt16_t
    // SHORT = Int16_t

    // normal variables
    BYTE byte = 5; // unsigned char byte = 5;
    BYTE byte2 = 240;
    PrintToServer("%i", BYTE(byte * byte2)); // byte * byte2 on its own is 1200. this should print 176.

    CHAR character = 'h';
    PrintToServer("%c%c", character, character + 1); // should be hi

    // arrays
    STACK_ALLOC(shortArray, ARRAY, SIZEOF(SHORT) * 4); // short shortArray[4];
    SHORT shortArray_1 = 15; // short shortArray_1 = 15;
    SHORT shortArray_2 = shortArray_1 * shortArray_1;
    SHORT shortArray_3 = shortArray_2 - 300;
    SHORT shortArray_4 = shortArray_1 % 4;
    shortArray.Write(shortArray_1, 0, NumberType_Int16); // shortArray[0] = 15; // should be 15
    shortArray.Write(shortArray_2, 2, NumberType_Int16); // shortArray[1] = shortArray_1 * shortArray_1; // should be 225
    shortArray.Write(shortArray_3, 4, NumberType_Int16); // shortArray[2] = shortArray_2 = 300; // should be -75
    shortArray.Write(shortArray_4, 6, NumberType_Int16); // shortArray[3] = shortArray_1 % 4; // should be 3

    for (int i = 0; i < STACK_SIZEOF(shortArray) / SIZEOF(SHORT); ++i) // for (int i = 0; i < sizeof(shortArray) / sizeof(shortArray[0]); ++i)
        PrintToServer("shortArray[%i]: %i", i, SHORT(shortArray.Dereference(i * 2, NumberType_Int16)).ToCell()); // std::cout << "shortArray[" << i << "]: " << shortArray[i] << std::endl;

    // c string
    STACK_ALLOC(str, ARRAY, SIZEOF(CHAR) * 32); // char str[32];
    memcpy(str, AddressOfString("Hello world!"), STACK_SIZEOF(str));
    
    for (int i = 0; ; ++i)
    {
        CHAR character2 = str.Dereference(i, NumberType_Int8);
        if (character2 == '\0')
            break;
        PrintToServer("%c", character2);
    }

    // last few things
    SHORT value = 0x8000;
    USHORT uvalue = 0x8000;
    int valueInt = value;
    int uvalueInt = uvalue;
    PrintToServer("valueInt: %i, uvalueInt: %i", valueInt, uvalueInt); // should be "valueInt: -32768, uvalueInt: 32768"

    PrintToServer("");
}

// also for CGameTrace
MRESReturn CTFPlayer_TraceAttack(int entity, DHookParam parameters)
{
    CGameTrace ptr = parameters.Get(3);
    PrintToServer("CTFPlayer::TraceAttack() on %i, is headshot: %i", entity, ptr.hitgroup == HITGROUP_HEAD);
    return MRES_Ignored;
}

// for CGameTrace, csurface_t, CBaseTrace and cplane_t.
static void cgametrace_csurface_t_cbasetrace_cplane_tOperation()
{
    // offset verification
    PrintToServer("sizeof(CGameTrace): %i", SIZEOF(CGameTrace)); // should be 84
    PrintToServer("CGameTrace::m_pEnt: %i\n", CGAMETRACE_OFFSET_M_PENT); // should be 76
}

static void tfplayerclassdata_tOperation()
{
    // offset verification
    PrintToServer("sizeof(TFPlayerClassData_T): %i", SIZEOF(TFPlayerClassData_t)); // should be 2288
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
    PrintToServer("g_pTFPlayerClassDataMgr: %u", g_pTFPlayerClassDataMgr);
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

    TFPlayerClassData_t civilianData = GetPlayerClassData(TF_CLASS_CIVILIAN);

    civilianData.m_szModelName_Get(buffer);
    PrintToServer("Civilian model: %s", buffer);
    civilianData.m_szHWMModelName_Get(buffer);
    PrintToServer("Civilian HWM model: %s", buffer);
    civilianData.m_szHandModelName_Get(buffer);
    PrintToServer("Civilian hands model: %s", buffer);
    civilianData.m_szLocalizableName_Get(buffer);
    PrintToServer("Civilian localizable name: %s", buffer);
    civilianData.m_szClassName_Get(buffer);
    PrintToServer("Civilian class name: %s", buffer);

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
    STACK_ALLOC(angle, QAngle, SIZEOF(QAngle));
    angle.X = 1.00;
    angle.Y = 2.00;
    angle.Z = 3.00;
    PrintToServer("QAngle angle: %f: %f: %f", angle.X, angle.Y, angle.Z);

    STACK_ALLOC(direction, QAngle, SIZEOF(QAngle));
    direction.X = 5.00;
    direction.Y = 5.00;
    direction.Z = 5.00;
    STACK_ALLOC(dest, QAngle, SIZEOF(QAngle));
    VectorMA(angle, 3.00, direction, dest);
    PrintToServer("after VectorMA, dest: %f: %f: %f", dest.X, dest.Y, dest.Z);

    STACK_ALLOC(forwardVector, Vector, SIZEOF(Vector));
    STACK_ALLOC(right, Vector, SIZEOF(Vector));
    STACK_ALLOC(up, Vector, SIZEOF(Vector));
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
        Vector damagePosition = GetEntVector(1, Prop_Data, "m_vecAbsOrigin");

        CTakeDamageInfo hell = CTakeDamageInfo.Malloc(0, 0, 0, damagePosition, damagePosition, 100.00, DMG_BLAST, TF_CUSTOM_STICKBOMB_EXPLOSION, damagePosition);
        SDKCall(SDKCall_CBaseEntity_TakeDamage, 1, hell);
        free(hell);
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

    STACK_ALLOC(stackAllocatedVector, Vector, SIZEOF(Vector));
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
    PrintToServer("where a was 10 and b was 20 before V_swap, pointer a: %i, pointer b: %i", a.Dereference(), b.Dereference());  

    // WHAT THE FUCK IS THIS
    Vector test = STACK_GETRETURN(Vector, vectorReturn());
    PrintToServer("test.X, test.Y, test.Z: %f: %f: %f\n", test.X, test.Y, test.Z);

    free(a);
    free(b);
}

static STACK vectorReturn()
{
    STACK_ALLOC(Return, Vector, SIZEOF(Vector));
    Return.X = 1.00;
    Return.Y = 434.00;
    Return.Z = 21393.00;
    STACK_RETURN(Return, SIZEOF(Vector));
}