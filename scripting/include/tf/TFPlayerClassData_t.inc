//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// tf_classdata.h

// Requires "tf_shareddefs.inc" and "Vector.inc". Requires "SMTC.inc" and SMTC_Initialize() to have been called in order to access g_pTFPlayerClassDataMgr.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TFPLAYERCLASSDATA_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TFPLAYERCLASSDATA_T

#include "Pointer.inc"
#include "Vector.inc"
#include "tf_shareddefs.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// TFPLAYERCLASSDATA_T DATA                                                 //
//////////////////////////////////////////////////////////////////////////////

#define MAX_PLAYERCLASS_SOUND_LENGTH 128
#define TF_NAME_LENGTH 128

#define DEATH_SOUND_FIRST		    ( DEATH_SOUND_GENERIC )
#define DEATH_SOUND_LAST			( DEATH_SOUND_EXPLOSION )
#define DEATH_SOUND_MVM_FIRST		( DEATH_SOUND_GENERIC_MVM )
#define DEATH_SOUND_MVM_LAST		( DEATH_SOUND_EXPLOSION_MVM )
#define DEATH_SOUND_GIANT_MVM_FIRST	( DEATH_SOUND_GENERIC_GIANT_MVM )
#define DEATH_SOUND_GIANT_MVM_LAST	( DEATH_SOUND_EXPLOSION_GIANT_MVM )

#define TF_CLASS_UNDEFINED_FILE			""
#define TF_CLASS_SCOUT_FILE				"scripts/playerclasses/scout"
#define TF_CLASS_SNIPER_FILE			"scripts/playerclasses/sniper"
#define TF_CLASS_SOLDIER_FILE			"scripts/playerclasses/soldier"
#define TF_CLASS_DEMOMAN_FILE			"scripts/playerclasses/demoman"
#define TF_CLASS_MEDIC_FILE				"scripts/playerclasses/medic"
#define TF_CLASS_HEAVYWEAPONS_FILE		"scripts/playerclasses/heavyweapons"
#define TF_CLASS_PYRO_FILE				"scripts/playerclasses/pyro"
#define TF_CLASS_SPY_FILE				"scripts/playerclasses/spy"
#define TF_CLASS_ENGINEER_FILE			"scripts/playerclasses/engineer"
#define TF_CLASS_CIVILIAN_FILE			"scripts/playerclasses/civilian"

enum DeathSoundType_t
{
	DEATH_SOUND_GENERIC = 0,
	DEATH_SOUND_CRIT,
	DEATH_SOUND_MELEE,
	DEATH_SOUND_EXPLOSION,

	DEATH_SOUND_GENERIC_MVM,
	DEATH_SOUND_CRIT_MVM,
	DEATH_SOUND_MELEE_MVM,
	DEATH_SOUND_EXPLOSION_MVM,

	DEATH_SOUND_GENERIC_GIANT_MVM,
	DEATH_SOUND_CRIT_GIANT_MVM,
	DEATH_SOUND_MELEE_GIANT_MVM,
	DEATH_SOUND_EXPLOSION_GIANT_MVM,

	DEATH_SOUND_TOTAL
};

// As alignment, the booleans m_bDontDoAirWalk, m_bDontDoNewJump and m_bParsed are all grouped together to make up one 32-bit block.
// On a related note, is why TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND is the offset of the previous member (m_vecThirdPersonOffset) + sizeof(Vector), but also + 1, since the final byte is padded.
enum
{
    TFPLAYERCLASSDATA_T_OFFSET_M_SZCLASSNAME = 0,                                                                                           // char m_szClassName[TF_NAME_LENGTH];
    TFPLAYERCLASSDATA_T_OFFSET_M_SZMODELNAME = TFPLAYERCLASSDATA_T_OFFSET_M_SZCLASSNAME + TF_NAME_LENGTH,                                   // char m_szModelName[TF_NAME_LENGTH];
    TFPLAYERCLASSDATA_T_OFFSET_M_SZHWMMODELNAME = TFPLAYERCLASSDATA_T_OFFSET_M_SZMODELNAME + TF_NAME_LENGTH,                                // char m_szHWMModelName[TF_NAME_LENGTH];
    TFPLAYERCLASSDATA_T_OFFSET_M_SZHANDMODELNAME = TFPLAYERCLASSDATA_T_OFFSET_M_SZHWMMODELNAME + TF_NAME_LENGTH,                            // char m_szHandModelName[TF_NAME_LENGTH];
    TFPLAYERCLASSDATA_T_OFFSET_M_SZLOCALIZABLENAME = TFPLAYERCLASSDATA_T_OFFSET_M_SZHANDMODELNAME + TF_NAME_LENGTH,                         // char m_szLocalizableName[TF_NAME_LENGTH];
    TFPLAYERCLASSDATA_T_OFFSET_M_FLMAXSPEED = TFPLAYERCLASSDATA_T_OFFSET_M_SZLOCALIZABLENAME + TF_NAME_LENGTH,                              // float m_flMaxSpeed;
    TFPLAYERCLASSDATA_T_OFFSET_M_NMAXHEALTH = TFPLAYERCLASSDATA_T_OFFSET_M_FLMAXSPEED + 4,                                                  // int m_nMaxHealth;
    TFPLAYERCLASSDATA_T_OFFSET_M_NMAXARMOR = TFPLAYERCLASSDATA_T_OFFSET_M_NMAXHEALTH + 4,                                                   // int m_nMaxArmor;
    TFPLAYERCLASSDATA_T_OFFSET_M_AWEAPONS = TFPLAYERCLASSDATA_T_OFFSET_M_NMAXARMOR + 4,                                                     // int m_aWeapons[TF_PLAYER_WEAPON_COUNT];
    TFPLAYERCLASSDATA_T_OFFSET_M_AGRENADES = TFPLAYERCLASSDATA_T_OFFSET_M_AWEAPONS + TF_PLAYER_WEAPON_COUNT * 4,                            // int m_aGrenades[TF_PLAYER_GRENADE_COUNT];
    TFPLAYERCLASSDATA_T_OFFSET_M_AAMMOMAX = TFPLAYERCLASSDATA_T_OFFSET_M_AGRENADES + TF_PLAYER_GRENADE_COUNT * 4,                           // int m_aAmmoMax[TF_AMMO_COUNT];
    TFPLAYERCLASSDATA_T_OFFSET_M_ABUILDABLE = TFPLAYERCLASSDATA_T_OFFSET_M_AAMMOMAX + view_as<int>(TF_AMMO_COUNT) * 4,                      // int m_aBuildable[TF_PLAYER_BLUEPRINT_COUNT];
    
    TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDOAIRWALK = TFPLAYERCLASSDATA_T_OFFSET_M_ABUILDABLE + TF_PLAYER_BLUEPRINT_COUNT * 4,                  // bool m_bDontDoAirwalk;
    TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDONEWJUMP = TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDOAIRWALK + 1,                                          // bool m_bDontDoNewJump;
    
    TFPLAYERCLASSDATA_T_OFFSET_M_BPARSED = TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDONEWJUMP + 1,                                                 // bool m_bParsed;
    TFPLAYERCLASSDATA_T_OFFSET_M_VECTHIRDPERSONOFFSET = TFPLAYERCLASSDATA_T_OFFSET_M_BPARSED + 1,                                           // Vector m_vecThirdPersonOffset;

    TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND = TFPLAYERCLASSDATA_T_OFFSET_M_VECTHIRDPERSONOFFSET + SIZEOF_Vector + 1,                      // char m_szDeathSound[DEATH_SOUND_TOTAL][MAX_PLAYERCLASS_SOUND_LENGTH];

    SIZEOF_TFPlayerClassData_t = TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND + view_as<int>(DEATH_SOUND_TOTAL) * MAX_PLAYERCLASS_SOUND_LENGTH // sizeof(TFPlayerClassData_t);
};

//////////////////////////////////////////////////////////////////////////////
// TFPLAYERCLASSDATA_T METHODMAP                                            //
//////////////////////////////////////////////////////////////////////////////

// Data about a specific TF class.
methodmap TFPlayerClassData_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a TFPlayerClassData_t object.
    public TFPlayerClassData_t(Address address)
    {
        return view_as<TFPlayerClassData_t>(address);
    }

    // Get the class name. This buffer size is 128 (TF_NAME_LENGTH).
    property Pointer m_szClassName
    {
        public get() { return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZCLASSNAME); }
    }

    // Get the model name. This buffer size is 128 (TF_NAME_LENGTH).
    property Pointer m_szModelName
    {
        public get() { return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZMODELNAME); }
    }

    // Get the HWM model name. This buffer size is 128 (TF_NAME_LENGTH).
    property Pointer m_szHWMModelName
    {
        public get() { return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZHWMMODELNAME); }
    }

    // Get the hand model name. This buffer size is 128 (TF_NAME_LENGTH).
    property Pointer m_szHandModelName
    {
        public get() { return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZHANDMODELNAME); }
    }

    // Get the localizable name. This buffer size is 128 (TF_NAME_LENGTH).
    property Pointer m_szLocalizableName
    {
        public get() { return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZLOCALIZABLENAME); }
    }

    // Get the max speed for this class.
    property float m_flMaxSpeed
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_FLMAXSPEED); }
        public set(float value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_FLMAXSPEED); }
    }

    // Get the max health for this class.
    property int m_nMaxHealth
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_NMAXHEALTH); }
        public set(int value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_NMAXHEALTH); }
    }

    // Get the max armour for this class.
    property int m_nMaxArmor
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_NMAXARMOR); }
        public set(int value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_NMAXARMOR); }
    }

    // Get a value from the m_aWeapons buffer. Index must be 0 <= index < TF_PLAYER_WEAPON_COUNT. Returns -1 on failure, returns value from buffer on success.
    public int m_aWeapons_Get(int index)
    {
        if (index < 0 || index >= TF_PLAYER_WEAPON_COUNT)
            return -1;
        return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_AWEAPONS + index / 4);
    }

    // Set a value in the m_aWeapons buffer. Index must be 0 <= index < TF_PLAYER_WEAPON_COUNT.
    public void m_aWeapons_Set(int index, int value)
    {
        if (index < 0 || index >= TF_PLAYER_WEAPON_COUNT)
            return;
        this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_AWEAPONS + index / 4);
    }

    // Get a value from the m_aGrenades buffer. Index must be 0 <= index < TF_PLAYER_GRENADE_COUNT. Returns -1 on failure, returns value from buffer on success.
    public int m_aGrenades_Get(int index)
    {
        if (index < 0 || index >= TF_PLAYER_GRENADE_COUNT)
            return -1;
        return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_AGRENADES + index / 4);
    }

    // Set a value in the m_aGrenades buffer. Index must be 0 <= index < TF_PLAYER_WEAPON_COUNT.
    public void m_aGrenades_Set(int index, int value)
    {
        if (index < 0 || index >= TF_PLAYER_GRENADE_COUNT)
            return;
        this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_AGRENADES + index / 4);
    }

    // Get a value from the m_aAmmoMax buffer. Index must be 0 <= index < TF_AMMO_COUNT. Returns -1 on failure, returns value from buffer on success.
    public int m_aAmmoMax_Get(int index)
    {
        if (index < 0 || index >= view_as<int>(TF_AMMO_COUNT))
            return -1;
        return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_AAMMOMAX + index / 4);
    }

    // Set a value in the m_aAmmoMax buffer. Index must be 0 <= index < TF_AMMO_COUNT.
    public void m_aAmmoMax_Set(int index, int value)
    {
        if (index < 0 || index >= view_as<int>(TF_AMMO_COUNT))
            return;
        this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_AAMMOMAX + index / 4);
    }

    // Get a value from the m_aBuildable buffer. Index must be 0 <= index < TF_PLAYER_BUILDABLE_COUNT. Returns -1 on failure, returns value from buffer on success.
    public int m_aBuildable_Get(int index)
    {
        if (index < 0 || index >= TF_PLAYER_BUILDABLE_COUNT)
            return -1;
        return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_ABUILDABLE + index / 4);
    }

    // Set a value in the m_aBuildable buffer. Index must be 0 <= index < TF_PLAYER_BUILDABLE_COUNT.
    public void m_aBuildable_Set(int index, int value)
    {
        if (index < 0 || index >= TF_PLAYER_BUILDABLE_COUNT)
            return;
        this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_ABUILDABLE + index / 4);
    }

    // Can the player airwalk?
    property bool m_bDontDoAirwalk
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDOAIRWALK, NumberType_Int8); }
        public set(bool value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDOAIRWALK, NumberType_Int8); }
    }

    // Can the player do new jumps?
    property bool m_bDontDoNewJump
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDONEWJUMP, NumberType_Int8); }
        public set(bool value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_BDONTDONEWJUMP, NumberType_Int8); }
    }

    // Has this class data object been parsed already?
    property bool m_bParsed
    {
        public get() { return this.Dereference(TFPLAYERCLASSDATA_T_OFFSET_M_BPARSED, NumberType_Int8); }
        public set(bool value) { this.Write(value, TFPLAYERCLASSDATA_T_OFFSET_M_BPARSED, NumberType_Int8); }
    }

    // The third person vector offset.
    property Vector m_vecThirdPersonOffset
    {
        public get() { return view_as<Vector>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_VECTHIRDPERSONOFFSET); }
        public set(Vector vector) { memcpy(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_VECTHIRDPERSONOFFSET, vector, SIZEOF_Vector); }
    }

    // Get a string pointer from the m_szDeathSound two-dimensional array. Index must be 0 <= index < DEATH_SOUND_TOTAL. Returns NULL on failure.
    // Each buffer size in the m_szDeathSound two-dimensional array is 128 (MAX_PLAYERCLASS_SOUND_LENGTH).
    public Pointer m_szDeathSound_Get(int index)
    {
        if (index < 0 || index >= view_as<int>(DEATH_SOUND_TOTAL))
            return NULL;
        return view_as<Pointer>(this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND + index * MAX_PLAYERCLASS_SOUND_LENGTH);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZCLASSNAME, NumberType_Int8); // m_szClassName[0] = '\0';
        this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZMODELNAME, NumberType_Int8); // m_szModelName[0] = '\0';
        this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZHWMMODELNAME, NumberType_Int8); // m_szHWMModelName[0] = '\0';
        this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZHANDMODELNAME, NumberType_Int8); // m_szHandModelName[0] = '\0';
        this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZLOCALIZABLENAME, NumberType_Int8); // m_szLocalizableName[0] = '\0';
        this.m_flMaxSpeed = 0.00;
        this.m_nMaxHealth = 0;
        this.m_nMaxArmor = 0;

        for (int i = 0; i < view_as<int>(DEATH_SOUND_TOTAL); ++i) // for (int i = 0; i < ARRAYSIZE(m_szDeathSound); ++i)
            this.Write(0, this.Address + TFPLAYERCLASSDATA_T_OFFSET_M_SZDEATHSOUND + i * MAX_PLAYERCLASS_SOUND_LENGTH); // m_szDeathSound[i][0] = '\0';
        
        for (int weapon = 0; weapon < TF_PLAYER_WEAPON_COUNT; ++weapon)
            this.m_aWeapons_Set(weapon, TF_WEAPON_NONE);

        for (int grenade = 0; grenade < TF_PLAYER_GRENADE_COUNT; ++grenade)
            this.m_aGrenades_Set(grenade, TF_WEAPON_NONE);

        for (int ammo = 0; ammo < view_as<int>(TF_AMMO_COUNT); ++ammo)
            this.m_aAmmoMax_Set(ammo, view_as<int>(TF_AMMO_DUMMY));

        for (int buildable = 0; buildable < TF_PLAYER_BLUEPRINT_COUNT; ++buildable)
            this.m_aBuildable_Set(buildable, view_as<int>(OBJ_LAST));

        this.m_bParsed = false;  
    }

    // Constructor - allocate a new CTFRadiusDamageInfo onto the heap. Deallocation will be required when you are finished using free().
    public static TFPlayerClassData_t Malloc()
    {
        TFPlayerClassData_t pointer = malloc(SIZEOF_TFPlayerClassData_t);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new TFPlayerClassData_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_TFPlayerClassData_t];
        view_as<TFPlayerClassData_t>(AddressOfString(buffer)).ctor();
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// TFPLAYERCLASSDATA_T GLOBALS                                              //
//////////////////////////////////////////////////////////////////////////////

stock TFPlayerClassData_t GetPlayerClassData(int class)
{
#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTC
    return view_as<TFPlayerClassData_t>(g_pTFPlayerClassDataMgr.Get(class, SIZEOF_TFPlayerClassData_t) + 12);
#else
    ThrowError("GetPlayerClassData() requires SMTC.inc!");
    return view_as<TFPlayerClassData_t>(NULL);
#endif
}