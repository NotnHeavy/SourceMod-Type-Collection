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

#define SMTC_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT false
#define SMTC_TF_POINT_T_TOGGLE_FUNCTIONS true
#define SMTC_CBASEENTITY_CUSTOM_DATA true

#include "SMTCHeader.inc"
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
#include "VectorAligned.inc"
#include "Ray_t.inc"
#include "UTIL.inc"
#include "CBaseEntity.inc"
#include "CHandle.inc"
#include "CEngineTrace.inc"
#include "string_t.inc"
#include "CGlobalVarsBase.inc"
#include "CGlobalVars.inc"
#include "FileWeaponInfo_t.inc"

#include "tf/CTakeDamageInfo.inc"
#include "tf/CTFRadiusDamageInfo.inc"
#include "tf/tf_shareddefs.inc"
#include "tf/TFPlayerClassData_t.inc"
#include "tf/tf_point_t.inc"
#include "tf/flame_point_t.inc"
#include "tf/CTFGameRules.inc"
#include "tf/WeaponData_t.inc"
#include "tf/CTFWeaponInfo.inc"
#include "tf/tf_item_constants.inc"

#include "SMTC.inc"

static Handle SDKCall_CTFWearable_Equip;
stock static Handle SDKCall_CBaseEntity_TakeDamage;
stock static DHookSetup DHooks_CTFPlayer_OnTakeDamage;
static Handle SDKCall_CTFGameRules_RadiusDamage;
static DHookSetup DHooks_CTFPlayer_TraceAttack;
static Handle SDKCall_CBaseCombatCharacter_Weapon_ShootPosition;

static int explosionModelIndex;

public void OnPluginStart()
{
    LoadTranslations("common.phrases");

    //SMTC_Initialize();

    GameData config = LoadGameConfigFile("NotnHeavy - SourceMod Type Collection (tests)");

    StartPrepSDKCall(SDKCall_Entity);
    PrepSDKCall_SetFromConf(config, SDKConf_Virtual, "CTFWearable::Equip");
    PrepSDKCall_AddParameter(SDKType_CBasePlayer, SDKPass_Pointer);
    SDKCall_CTFWearable_Equip = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Entity);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CBaseEntity::TakeDamage");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    SDKCall_CBaseEntity_TakeDamage = EndPrepSDKCall();

    //DHooks_CTFPlayer_OnTakeDamage = DHookCreateFromConf(config, "CTFPlayer::OnTakeDamage");
    //DHookEnableDetour(DHooks_CTFPlayer_OnTakeDamage, false, CTFPlayer_OnTakeDamage);

    StartPrepSDKCall(SDKCall_GameRules);
    PrepSDKCall_SetFromConf(config, SDKConf_Signature, "CTFGameRules::RadiusDamage");
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    SDKCall_CTFGameRules_RadiusDamage = EndPrepSDKCall();

    DHooks_CTFPlayer_TraceAttack = DHookCreateFromConf(config, "CTFPlayer::TraceAttack()");
    DHookEnableDetour(DHooks_CTFPlayer_TraceAttack, false, CTFPlayer_TraceAttack);

    StartPrepSDKCall(SDKCall_Entity);
    PrepSDKCall_SetFromConf(config, SDKConf_Virtual, "CBaseCombatCharacter::Weapon_ShootPosition()");
    PrepSDKCall_SetReturnInfo(SDKType_Vector, SDKPass_ByValue); // Vector
    SDKCall_CBaseCombatCharacter_Weapon_ShootPosition = EndPrepSDKCall();

    delete config;

    //commitTests(); // TESTS/tests.inc
}

public void OnMapStart()
{
    explosionModelIndex = PrecacheModel("sprites/sprite_fire01.vmt");
    PrintToServer("------------------------------------------------------------------");

    SMTC_Initialize();
    pointerOperation();
    vectorOperation();
    cutlvectorOperation(); // oh god
    ctakedamageinfoOperation();
    AddCommandListener(ctfradiusdamageinfoOperation, "voicemenu"); // ctfradiusdamageinfo operation
    qangleOperation();
    tfplayerclassdata_tOperation();
    cgametrace_csurface_t_cbasetrace_cplane_tOperation();
    vtableOperation();
    vectoralignedOperation();
    ray_tOperation();
    utilOperation();
    entityOperation(); // CBaseEntity.inc, CHandle.inc
    gamerulesOperation();
    string_tOperation();
    cglobalvarsOperation(); // CGlobalVars.inc, CGlobalVarsBase.inc
    ctfweaponinfoOperation(); // FileWeaponInfo_t.inc, WeaponData_t.inc, CTFWeaponInfo.inc

    PrintToServer("\n\"%s\" has loaded.\n------------------------------------------------------------------", "NotnHeavy - SourceMod Type Collection");
    PrintToChatAll("THE TEST PLUGIN FOR NOTNHEAVY'S SOURCEMOD TYPE COLLECTION IS CURRENTLY RUNNING.");

    for (int i = 1; i <= MaxClients; ++i)
    {
        if (IsValidEntity(i) && IsClientInGame(i))
            OnClientPutInServer(i);
    }
}

public void OnPluginEnd()
{
    VTable.ClearVTables();
}

public void OnEntityCreated(int entity, const char[] classname)
{
    if (entity <= MaxClients)
        return;
    
    // tf_point_t.inc, flame_point_t.inc
    if (StrEqual(classname, "tf_flame_manager"))
        SMTC_HookEntity(entity, FORWARDTYPE_ADDPOINT, SMTC_AddPoint);
    
    // FileWeaponInfo_t.inc, WeaponData_t.inc, CTFWeaponInfo.inc
    else if (StrEqual(classname, "tf_weapon_shotgun_primary"))
    {
        if (entity > MaxClients && entity <= 2048 && IsValidEntity(entity))
            RequestFrame(weaponData, CBaseEntity.FromIndex(entity));
    }
}

void weaponData(CBaseEntity weapon)
{
    if (weapon == NULL || weapon.entindex() == -1 || !IsValidEntity(weapon.GetEntPropEntIndex(Prop_Send, "m_hOwnerEntity")) || !IsClientInGame(weapon.GetEntPropEntIndex(Prop_Send, "m_hOwnerEntity")))
        return;

    CTFWeaponInfo m_pWeaponInfo = weapon.Dereference(SMTC_CTFWeaponBase_m_pWeaponInfo);
    if (m_pWeaponInfo == NULL)
        return;
    PrintToServer("Engineer shotgun m_pWeaponInfo object: %u", m_pWeaponInfo);

    WeaponData_t data = m_pWeaponInfo.GetWeaponData(weapon.Dereference(SMTC_CTFWeaponBase_m_iWeaponMode));
    PrintToServer("Engineer shotgun WeaponData_t object: %u", data);
    PrintToServer("Engineer shotgun m_flSpread: %f", data.m_flSpread);

    data.m_nBulletsPerShot = 1;
    data.m_flTimeFireDelay = 0.15;
    data.m_flTimeReloadStart = 0.00;
    m_pWeaponInfo.iMaxClip1 = 12;

    PrintToServer("Engineer shotgun slot: %i", m_pWeaponInfo.iSlot);
    PrintToServer("Engineer shotgun position: %i", m_pWeaponInfo.iPosition);

    m_pWeaponInfo.iPosition = 1;
}

public void OnClientPutInServer(int client)
{
    SMTC_HookEntity(client, FORWARDTYPE_ONTAKEDAMAGE, CTFPlayer_OnTakeDamage);
}

public void ctfweaponinfoOperation()
{
    PrintToServer("&FileWeaponInfo_t::bAutoSwitchFrom: %u", FILEWEAPONINFO_T_OFFSET_BAUTOSWITCHFROM);
    PrintToServer("&FileWeaponInfo_t::iFlags: %u", FILEWEAPONINFO_T_OFFSET_IFLAGS);
    PrintToServer("&FileWeaponInfo_t::iAmmoType: %u", FILEWEAPONINFO_T_OFFSET_IAMMOTYPE);
    PrintToServer("&FileWeaponInfo_t::iSpriteCount: %u", FILEWEAPONINFO_T_OFFSET_ISPRITECOUNT);
    PrintToServer("sizeof(FileWeaponInfo_t): %u\n", SIZEOF(FileWeaponInfo_t));

    // as of 21.05.2023
    // CTFWeaponBaseGun::GetWeaponSpread();
    // float fSpread = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSpread;
    // v2 = *(float *)(*((_DWORD *)this + 441) + (*((_DWORD *)this + 438) << 6) + 1796);

    // *((_DWORD *)this + 441): CTFWeaponBase->m_pWeaponInfo
    // *((_DWORD *)this + 438): CTFWeaponBase->m_iWeaponMode
    // (*((_DWORD *)this + 438) << 6): offset for CTFWeaponInfo::m_WeaponData

    // on windows:
    // v2 = this[438];
    // v3 = this[435] << 6;
    // v4 = *(float *)(v3 + v2 + 1796);

    PrintToServer("sizeof(WeaponData_t): %u\n", SIZEOF(WeaponData_t));

    PrintToServer("&CTFWeaponInfo::m_WeaponData: %u", CTFWEAPONINFO_OFFSET_M_WEAPONDATA);
    PrintToServer("&CTFWeaponInfo::m_iWeaponType: %u", CTFWEAPONINFO_OFFSET_M_IWEAPONTYPE);
    PrintToServer("&CTFWeaponInfo::m_szMuzzleFlashModel: %u", CTFWEAPONINFO_OFFSET_M_SZMUZZLEFLASHMODEL);
    PrintToServer("&CTFWeaponInfo::m_szBrassModel: %u", CTFWEAPONINFO_OFFSET_M_SZBRASSMODEL);
    PrintToServer("sizeof(CTFWeaponInfo): %u\n", SIZEOF(CTFWeaponInfo));

    PrintToServer("m_pWeaponInfo offset: %u", SMTC_CTFWeaponBase_m_pWeaponInfo); // as of 21.05.2023, should be 1752 on windows

    PrintToServer("");
}

public void cglobalvarsOperation()
{
    PrintToServer("gpGlobals: %u", gpGlobals);
    PrintToServer("gpGlobals->curtime: %f", gpGlobals.curtime);
    PrintToServer("GetGameTime(): %f", GetGameTime());
    PrintToServer("gpGlobals->maxEntities: %i", gpGlobals.maxEntities);
    
    PrintToServer("");
}

// string_t.inc
public void string_tOperation()
{
    String_t string = String_t.Malloc(AddressOfString("Hello world!"));
    char buffer[64];
    string.ToSourcePawnStr(buffer, sizeof(buffer));
    PrintToServer(buffer);

    free(string);
    PrintToServer("");
}

// tf_point_t.inc, flame_point_t.inc
public void SMTC_AddPoint(const CBaseEntity manager, const TF_Point_t point)
{
    // this is not client predicted!
    point.m_flLifeTime = 1.00; // make flames last a second :D
    point.m_vecVelocity *= 2.00; // make them twice as fast as well

    PrintToServer("point manager %u: flame index %i", manager, point.m_iIndex);
}

static void gamerulesOperation()
{
    PrintToServer("GAMERULES: %u\n", g_pGameRules);
}

static void entityOperation()
{
    if (IsClientInGame(1))
    {
        CBaseEntity entity = CBaseEntity.FromIndex(1);
        CHandle handle = entity.m_RefEHandle;
        PrintToServer("entity handle of client 1: %u", handle.m_Index);
        PrintToServer("entity index from dereferencing entity handle: %i", handle.GetAsEntIndex());

        // Get m_hMyWearables
        CUtlVector vector = view_as<CUtlVector>(entity.GetEntPropPointer(Prop_Send, "m_hMyWearables"));
        PrintToServer("CBaseEntity.FromIndex(1)->m_hMyWearables: %u, m_hMyWearables - player: %u", vector, entity);
        for (int i = 0, size = vector.Count(); i < size; ++i)
        {
            EHANDLE entityFound = view_as<EHANDLE>(vector.Get(i));
            PrintToServer("entity in CBaseEntity.FromIndex(1)->m_hMyWearables[%i]: %i", i, entityFound.GetAsEntIndex());
        }

        PrintToServer("");
    }
}

// UTIL.inc
static STACK Weapon_ShootPosition(int pThis)
{
    Vector vecResult = STACK_GETRETURN(Vector.StackAlloc());
    float buffer[3];
    SDKCall(SDKCall_CBaseCombatCharacter_Weapon_ShootPosition, pThis, buffer);
    vecResult.SetFromBuffer(buffer);
    STACK_RETURNVALUE(vecResult, SIZEOF(Vector));
}
static void utilOperation()
{
    // vtable checks
    any obj;
    char buffer[4];
    VTable.HookOntoObject("CTraceFilter", AddressOf(obj));
    Pointer(VTable.GetObjectVPointer(AddressOf(obj), 1).Dereference()).ToCharBuffer(buffer, sizeof(buffer));
    for (int i = 0; i < sizeof(buffer); ++i)
    {
        char variable = buffer[i];
        PrintToServer("\\x%X \0\0", variable);
    }
    PrintToServer("");

    VTable.CreateVTable("testie", 2, "CTraceFilter");
    VTable.HookOntoObject("testie", AddressOf(obj));
    Pointer(VTable.GetObjectVPointer(AddressOf(obj), 1).Dereference()).ToCharBuffer(buffer, sizeof(buffer));
    for (int i = 0; i < sizeof(buffer); ++i)
    {
        char variable = buffer[i];
        PrintToServer("\\x%X \0\0", variable);
    }
    PrintToServer("");

    // check trace enum
    StartPrepSDKCall(SDKCall_Raw);
    PrepSDKCall_SetVirtual(1);
    PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
    Handle GetTraceType = EndPrepSDKCall();
    CTraceFilterEntitiesOnly testFilter = STACK_GETRETURN(CTraceFilterEntitiesOnly.StackAlloc());
    TraceType_t type = SDKCall(GetTraceType, testFilter);
    PrintToServer("trace type of CTraceFilterEntitiesOnly: %i", type);
    PrintToServer("");

    // check global trace
    PrintToServer("global trace: %u\n", enginetrace);

    if (IsClientInGame(1))
    {
        // same as shield bash code
        int client = 1;
        
        // eye angles
        float eyeanglesbuffer[3];
        GetClientEyeAngles(client, eyeanglesbuffer);

        // Setup the swing range.
        Vector vecForward = STACK_GETRETURN(Vector.StackAlloc());
        Vector vecStart = STACK_GETRETURN(Vector.StackAlloc());
        Vector vecEnd = STACK_GETRETURN(Vector.StackAlloc());
        AngleVectors(AddressOfArray(eyeanglesbuffer), vecForward);
        vecStart.Assign(STACK_GETRETURN(Weapon_ShootPosition(client)));
        vecEnd.Assign(vecStart + vecForward * 48.00);

        // See if we hit anything.
        CGameTrace trace = STACK_GETRETURN(CGameTrace.StackAlloc());
        CTraceFilterSimple filter = STACK_GETRETURN(CTraceFilterSimple.StackAlloc(GetEntityAddress(client), COLLISION_GROUP_NONE));
        UTIL_TraceHull(vecStart, vecEnd, -Vector.Cache(24.00, 24.00, 24.00), Vector.Cache(24.00, 24.00, 24.00), MASK_SOLID, filter, trace);
        PrintToChatAll("TRACE HULL ATTEMPT! trace->m_pEnt: %i", trace.m_pEnt);
    }
}

static void ray_tOperation()
{
    Ray_t ray = Ray_t.Malloc();
    ray.InitHull(Vector.ArrayReference({1.00, 2.00, 3.00}), Vector.ArrayReference({15.00, 3.00, 4.00}), Vector.ArrayReference({-1.00, -1.00, -1.00}), Vector.ArrayReference({2.00, 2.00, 2.00}));

    PrintToServer("ray size: %i", SIZEOF(Ray_t));
    PrintToServer("ray start: %f: %f: %f", ray.m_Start.X, ray.m_Start.Y, ray.m_Start.Z);
    PrintToServer("ray delta: %f: %f: %f", ray.m_Delta.X, ray.m_Delta.Y, ray.m_Delta.Z);
    PrintToServer("ray startoffset: %f: %f: %f", ray.m_StartOffset.X, ray.m_StartOffset.Y, ray.m_StartOffset.Z);
    PrintToServer("ray extents: %f: %f: %f", ray.m_Extents.X, ray.m_Extents.Y, ray.m_Extents.Z);
    PrintToServer("is ray: %i, is swept: %i", ray.m_IsRay, ray.m_IsSwept);

    free(ray);
    PrintToServer("");
    
    Ray_t testRay = STACK_GETRETURN(Ray_t.StackAlloc());
    testRay.Init(Vector.ArrayReference({3.00, 5.00, 0.00}), Vector.ArrayReference({50.00, 30.00, 30.00}));

    PrintToServer("testRay start: %f: %f: %f", testRay.m_Start.X, testRay.m_Start.Y, testRay.m_Start.Z);
    PrintToServer("testRay delta: %f: %f: %f", testRay.m_Delta.X, testRay.m_Delta.Y, testRay.m_Delta.Z);
    PrintToServer("testRay startoffset: %f: %f: %f", testRay.m_StartOffset.X, testRay.m_StartOffset.Y, testRay.m_StartOffset.Z);
    PrintToServer("testRay extents: %f: %f: %f", testRay.m_Extents.X, testRay.m_Extents.Y, testRay.m_Extents.Z);
    PrintToServer("is ray: %i, is swept: %i", testRay.m_IsRay, testRay.m_IsSwept);
}

static void vectoralignedOperation()
{
    STACK_ALLOC(vector, VectorAligned, SIZEOF(VectorAligned));
    STACK_ALLOC(vector2, Vector, SIZEOF(Vector));
    STACK_ALLOC(result, VectorAligned, SIZEOF(VectorAligned));

    vector.X = 1.00;
    vector.Y = 2.00;
    vector.Z = 3.00;

    vector2.X = 3.00;
    vector2.Y = 4.00;
    vector2.Z = 5.00;

    result.Assign(vector2 + vector);
    result.NormalizeInPlace();

    PrintToServer("VectorAligned result: %f : %f : %f, sizeof(result): %i", result.X, result.Y, result.Z, SIZEOF(VectorAligned));

    PrintToServer("");
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
        SetMemAccess(AddressOfString(testString), sizeof(testString), SH_MEM_EXEC | SH_MEM_READ | SH_MEM_WRITE);
        
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

        // just test vtables with malloc'd asm
        char testvpointer[] = "THIS ISN'T ACTUAL CODE";
        VTable.CreateVTable("testing!", 1);
        VTable.RegisterVPointer("testing!", 0, AddressOfString(testvpointer), strlen(testvpointer));

        // from existing
        Pointer existingVTable = VTable.GetObjectVTable(AddressOfArray(variable));
        VTable.FromExisting("testing2!", existingVTable, 2, 1);
        VTable.RegisterVPointer("testing!", 1, AddressOfString(testvpointer), strlen(testvpointer));
        
        value = SDKCall(SDKCall_variable_test, AddressOfArray(variable));
        PrintToServer("on creating vtable from existing vtable, sdkcall 0 return: %i", value);
    }
    else
        PrintToServer("Not using Windows; skipping vtableOperation()...");
    
    PrintToServer("");
}

// also for CGameTrace
MRESReturn CTFPlayer_TraceAttack(int entity, DHookParam parameters)
{
    CGameTrace ptr = parameters.Get(3);
    PrintToServer("CTFPlayer::TraceAttack() on %i, is headshot: %i", ptr.m_pEnt, ptr.hitgroup == HITGROUP_HEAD);
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

    civilianData.m_szModelName.ToCharBuffer(buffer, sizeof(buffer));
    PrintToServer("Civilian model: %s", buffer);
    civilianData.m_szHWMModelName.ToCharBuffer(buffer, sizeof(buffer));
    PrintToServer("Civilian HWM model: %s", buffer);
    civilianData.m_szHandModelName.ToCharBuffer(buffer, sizeof(buffer));
    PrintToServer("Civilian hands model: %s", buffer);
    civilianData.m_szLocalizableName.ToCharBuffer(buffer, sizeof(buffer));
    PrintToServer("Civilian localizable name: %s", buffer);
    civilianData.m_szClassName.ToCharBuffer(buffer, sizeof(buffer));
    PrintToServer("Civilian class name: %s", buffer);

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

        CTakeDamageInfo damageInfo = CTakeDamageInfo.Malloc(CBaseEntity.FromIndex(client), CBaseEntity.FromIndex(client), CBaseEntity.FromIndex(client), damagePosition, damagePosition, 300.00, DMG_BLAST & DMG_HALF_FALLOFF, TF_CUSTOM_STICKBOMB_EXPLOSION, damagePosition);
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
    /*
    if (IsClientInGame(1))
    {
        Vector damagePosition = GetEntVector(1, Prop_Data, "m_vecAbsOrigin");

        CTakeDamageInfo hell = CTakeDamageInfo.Malloc(0, 0, 0, damagePosition, damagePosition, 100.00, DMG_BLAST, TF_CUSTOM_STICKBOMB_EXPLOSION, damagePosition);
        SDKCall(SDKCall_CBaseEntity_TakeDamage, 1, hell);
        free(hell);
    }
    */

    // Server tests.
    CTakeDamageInfo info = CTakeDamageInfo.Malloc(CBaseEntity.FromIndex(0), CBaseEntity.FromIndex(0), CBaseEntity.FromIndex(0), vec3_origin, vec3_origin, 0.00, 0, 0);
    PrintToServer("info.m_vecDamagePosition: %f: %f: %f", info.m_vecDamagePosition.X, info.m_vecDamagePosition.Y, info.m_vecDamagePosition.Z);
    info.m_vecDamagePosition.X = 3.00;
    PrintToServer("info.m_vecDamagePosition.X: %f, vec3_origin.X: %f\n", info.m_vecDamagePosition.X, vec3_origin.X);
    free(info);
}

// CTFPlayer::OnTakeDamage()
/*
MRESReturn image.png(int entity, DHookReturn returnValue, DHookParam parameters)
{
    CTakeDamageInfo info = parameters.Get(1);
    info.SetCritType(CRIT_MINI, entity); // :^)
    return MRES_Ignored;
}
*/
public MRESReturn CTFPlayer_OnTakeDamage(const CBaseEntity entity, const CTakeDamageInfo info, bool& returnValue)
{
    info.SetCritType(CRIT_MINI, entity); // :^)
    return MRES_Ignored;
}

static void cutlvectorOperation()
{
    // Player tests. (TF2)
    if (IsClientInGame(1))
    {
        CUtlVector m_hMyWearables = CUtlVector(GetEntityAddress(1) + FindSendPropInfo("CTFPlayer", "m_hMyWearables"));
        PrintToServer("m_hMyWearables size: %i", m_hMyWearables.Count());
        for (int i = 0; i < m_hMyWearables.Count(); ++i)
            PrintToServer("m_hMyWearables[%i]: %i", i, view_as<CHandle>(m_hMyWearables.Get(i)).GetAsEntIndex());
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
        view_as<CHandle>(m_hMyWearables.Get(m_hMyWearables.AddToHead())).Set(CBaseEntity.FromIndex(gunboats));
        SDKCall(SDKCall_CTFWearable_Equip, gunboats, 1);
        for (int i = 0; i < m_hMyWearables.Count(); ++i)
            PrintToServer("after adding, m_hMyWearables[%i]: %i", i, view_as<CHandle>(m_hMyWearables.Get(i)).GetAsEntIndex());
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
    PrintToServer("VECTOR CACHE SIZE: %i\n", VECTOR_CACHE_SIZE);

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
    PrintToServer("stackAllocatedVector: %f: %f: %f", stackAllocatedVector.X, stackAllocatedVector.Y, stackAllocatedVector.Z);

    Vector accum = Vector.Accumulator(1.00, 2.00, 289132931.00);
    PrintToServer("accum: %f: %f: %f", accum.X, accum.Y, accum.Z);
    
    accum.Assign(accum + Vector.Cache(1.00, 2.00, 4.00) * Vector.Cache(3.00, 3.00, 3.00) / Vector.Cache(1.00, 1.50, 2.00) - Vector.Cache(-25.00, 2.00, 1.00) * Vector.Cache(5.00, 5.00, 5.00));
    PrintToServer("accum after cache-allocated operations: %f: %f: %f", accum.X, accum.Y, accum.Z);

    free(vectorMalloc);

    // float buffer tests
    PrintToServer("float buffer equivalent tests:\n");
    float buffer[3] = {3.00, 2.00, 1.00};
    PrintToServer("Vector test: %f : %f : %f", buffer[0], buffer[1], buffer[2]);

    float buffer2[3] = {6.00, 5.00, 4.00};
    PrintToServer("Vector malloc test: %f : %f : %f", buffer2[0], buffer2[1], buffer2[2]);

    float buffer3[3];
    float tempbuffer[3];
    memcpy(AddressOfArray(tempbuffer), AddressOfArray(buffer2), SIZEOF(Vector));
    ScaleVector(tempbuffer, 2.00);
    AddVectors(buffer, tempbuffer, buffer3);
    PrintToServer("Result vector: %f: %f: %f", buffer3[0], buffer3[1], buffer3[2]);

    float buffer4[3];
    buffer4[0] = 3.00;
    PrintToServer("Result X: %f\nTest X: %f", buffer3[0], buffer4[0]);

    PrintToServer("result.Length(): %f", GetVectorLength(buffer3));
    PrintToServer("result.DistTo(test): %f", GetVectorDistance(buffer3, buffer4));
    PrintToServer("result.Dot(test): %f", GetVectorDotProduct(buffer3, buffer4));

    float buffer5[3];
    GetVectorCrossProduct(buffer3, buffer4, buffer5);
    PrintToServer("result.Cross(test): %f: %f: %f", buffer5[0], buffer5[1], buffer5[2]);

    float buffer6[3] = {7.00, 8.00, 9.00};
    float buffer7[3] = {1.00, 2.00, 3.00};
    float buffer8[3] = {4.00, 5.00, 6.00};
    GetVectorVectors(buffer6, buffer7, buffer8);
    PrintToServer("forward/right/up after VectorVectors: %f: %f: %f, %f: %f: %f, %f: %f: %f", buffer6[0], buffer6[1], buffer6[2], buffer7[0], buffer7[1], buffer7[2], buffer8[0], buffer8[1], buffer8[2]);

    PrintToServer("result.NormalizeInPlace(): %f", NormalizeVector(buffer3, buffer3));
    PrintToServer("new co-ordinates for normalized vector: %f: %f: %f", buffer3[0], buffer3[1], buffer3[2]);

    PrintToServer("");
}

bool done = false;
static void pointerOperation()
{
    if (done)
        return;
    done = true;

    strlen("test");

    Pointer a = malloc(4);
    a.Write(10);

    Pointer b = malloc(4);
    b.Write(20);

    V_swap(a, b);
    PrintToServer("where a was 10 and b was 20 before V_swap, pointer a: %i, pointer b: %i", a.Dereference(), b.Dereference());  

    // WHAT THE FUCK IS THIS
    Vector test = STACK_GETRETURN(vectorReturn());
    PrintToServer("test.X, test.Y, test.Z: %f: %f: %f", test.X, test.Y, test.Z);
    
    Pointer str1 = Pointer(AddressOfString("ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ"));
    Pointer str2 = Pointer(AddressOfString("abc"));
    Pointer str3 = Pointer(AddressOfString("abd"));
    Pointer str4 = malloc(50);

    PrintToServer("strlen(\"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\"): %u", str1.strlen());
    PrintToServer("strnlen(\"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\", 20): %u", str1.strnlen(20));
    PrintToServer("strcmp(\"abc\", \"abc\"): %i", str2.strcmp(str2));
    PrintToServer("strcmp(\"abc\", \"abd\"): %i", str2.strcmp(str3));
    PrintToServer("strncmp(\"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\", \"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\", 5): %i", str1.strncmp(str1, 5));
    
    str4.strcpy(str1);
    char buffer[50];
    str4.ToCharBuffer(buffer);
    PrintToServer("strcpy(str4, str1): %s", buffer);

    str4.strcat(str2);
    str4.ToCharBuffer(buffer);
    PrintToServer("strcat(str4, str2): %s", buffer);

    str4.strncat(AddressOfString("Hi. How to make electric guitar."), 10);
    str4.ToCharBuffer(buffer);
    PrintToServer("strncat(str4, \"Hi. How to make electric guitar.\", 10): %s", buffer);

    str4.strncpy(AddressOfString("ZYXWVU"), 3);
    str4.ToCharBuffer(buffer);
    PrintToServer("strncpy(str4, \"ZYXWVU\", 3): %s", buffer);

    str1.strchr('J').ToCharBuffer(buffer);
    PrintToServer("strchr(\"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\", 'J'): %s", buffer);

    str1.strrchr('A').ToCharBuffer(buffer);
    PrintToServer("strrchr(\"ABDJHSABDSDHAJASDHSDHJSDAHBJASDHJ\", 'A'): %s", buffer);
    
    str1.strset('B');
    str1.ToCharBuffer(buffer);
    PrintToServer("strset(str1, 'B'): %s", buffer);

    str1.strnset('C', 5);
    str1.ToCharBuffer(buffer);
    PrintToServer("strnset(str1, 'C', 5): %s", buffer);

    str1.strstr(AddressOfString("CCBB")).ToCharBuffer(buffer);
    PrintToServer("strstr(str1, \"CCBB\"): %s\n", buffer);

    free(a);
    free(b);
    free(str4);
}

static STACK vectorReturn()
{
    Vector test = STACK_GETRETURN(Vector.StackAlloc(1.00, 2.00, 3.00));
    PrintToServer("stackalloc'd test vector: %f: %f: %f", test.X, test.Y, test.Z);

    STACK_ALLOC(Return, Vector, SIZEOF(Vector));
    Return.X = 1.00;
    Return.Y = 434.00;
    Return.Z = 21393.00;

    Vector b = Return;
    STACK_RETURNVALUE(b, SIZEOF(Vector));
}