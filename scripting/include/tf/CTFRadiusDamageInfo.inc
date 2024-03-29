//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTFRADIUSDAMAGEINFO
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTFRADIUSDAMAGEINFO

#include "Vector.inc"
#include "CTakeDamageInfo.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CTFRADIUSDAMAGEINFO DATA                                                 //
//////////////////////////////////////////////////////////////////////////////

// Fill these in & call RadiusDamage()
enum
{
    CTFRADIUSDAMAGEINFO_OFFSET_DMGINFO = 0,                                                             // CTakeDamageInfo* dmgInfo;
    CTFRADIUSDAMAGEINFO_OFFSET_VECSRC = CTFRADIUSDAMAGEINFO_OFFSET_DMGINFO + SIZEOF_Pointer,            // Vector vecSrc;
    CTFRADIUSDAMAGEINFO_OFFSET_FLRADIUS = CTFRADIUSDAMAGEINFO_OFFSET_VECSRC + SIZEOF_Vector,            // float flRadius;
    CTFRADIUSDAMAGEINFO_OFFSET_PENTITYIGNORE = CTFRADIUSDAMAGEINFO_OFFSET_FLRADIUS + 4,                 // CBaseEntity* pEntityIgnore;
    CTFRADIUSDAMAGEINFO_OFFSET_FLRJRADIUS = CTFRADIUSDAMAGEINFO_OFFSET_PENTITYIGNORE + SIZEOF_Pointer,  // float flRJRadius;
    CTFRADIUSDAMAGEINFO_OFFSET_M_FLFORCESCALE = CTFRADIUSDAMAGEINFO_OFFSET_FLRJRADIUS + 4,              // float m_flForceScale;
    CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET = CTFRADIUSDAMAGEINFO_OFFSET_M_FLFORCESCALE + 4,         // CBaseEntity* m_pEntityTarget;
    CTFRADIUSDAMAGEINFO_OFFSET_FLFALLOFF = CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET + SIZEOF_Pointer, // float flFalloff;

    SIZEOF_CTFRadiusDamageInfo = CTFRADIUSDAMAGEINFO_OFFSET_FLFALLOFF + 4                               // sizeof(CTFRadiusDamageInfo);
};

//////////////////////////////////////////////////////////////////////////////
// CTFRADIUSDAMAGEINFO METHODMAP                                            //
//////////////////////////////////////////////////////////////////////////////

// Details for damaging entities within a specific radius.
methodmap CTFRadiusDamageInfo < Pointer
{
    // Constructor - wrap around an existing address to make way for a CTakeDamageInfo object.
    public CTFRadiusDamageInfo(Address address)
    {
        return view_as<CTFRadiusDamageInfo>(address);
    }

    // The actual damage info used for damaging entities.
    property CTakeDamageInfo dmgInfo
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_DMGINFO); }
        public set(CTakeDamageInfo value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_DMGINFO); }
    }

    // The location of the splash radius.
    property Vector vecSrc
    {
        public get() { return view_as<Vector>(this.Address + CTFRADIUSDAMAGEINFO_OFFSET_VECSRC); }
        public set(Vector value) { memcpy(this.Address + CTFRADIUSDAMAGEINFO_OFFSET_VECSRC, value, SIZEOF_Vector); }
    }

    // The splash radius.
    property float flRadius
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_FLRADIUS); }
        public set(float value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_FLRADIUS); }
    }

    // Is there an entity that we should ignore?
    property CBaseEntity pEntityIgnore
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET); }
        public set(CBaseEntity value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET); }
    }

    // Radius to use to calculate RJ, to maintain RJs when damage/radius changes on a RL.
    property float flRJRadius
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_FLRJRADIUS); }
        public set(float value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_FLRJRADIUS); }
    }

    // The force scale.
    property float m_flForceScale
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_M_FLFORCESCALE); }
        public set(float value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_M_FLFORCESCALE); }
    }

    // The target being directly hit, if any.
    property CBaseEntity m_pEntityTarget
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET); }
        public set(CBaseEntity value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_M_PENTITYTARGET); }
    }

    // Falloff amount. These are used during the application of the RadiusDamage.
    property float flFalloff
    {
        public get() { return this.Dereference(CTFRADIUSDAMAGEINFO_OFFSET_FLFALLOFF); }
        public set(float value) { this.Write(value, CTFRADIUSDAMAGEINFO_OFFSET_FLFALLOFF); }
    }

    // Calculate the damage falloff over distance. Requires SMTC_Initialize() to have been called.
    public void CalculateFalloff()
    {
        SDKCall(SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff, this);
    }

    // Attempt to apply the radius damage to the specified entity. Requires SMTC_Initialize() to have been called.
    public void ApplyToEntity(CBaseEntity pEntity)
    {
        SDKCall(SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity, this, pEntity);
    }
    
    // Call constructor on existing type.
    public void ctor(CTakeDamageInfo info, const Vector source, float radius, CBaseEntity ignore = NULL, float rjRadius = 0.00, float forceScale = 1.00)
    {
        this.dmgInfo = info;
        this.vecSrc = source;
        this.flRadius = radius;
        this.pEntityIgnore = ignore;
        this.flRJRadius = rjRadius;
        this.flFalloff = 0.00;
        this.m_flForceScale = forceScale;
        this.m_pEntityTarget = NULL;

        this.CalculateFalloff();
    }

    // Constructor - allocate a new CTFRadiusDamageInfo onto the heap. Deallocation will be required when you are finished using free().
    public static CTFRadiusDamageInfo Malloc(CTakeDamageInfo info, const Vector source, float radius, CBaseEntity ignore = NULL, float rjRadius = 0.00, float forceScale = 1.00)
    {
        CTFRadiusDamageInfo pointer = malloc(SIZEOF_CTFRadiusDamageInfo);
        pointer.ctor(info, source, radius, ignore, rjRadius, forceScale);
        return pointer;
    }

    // Constructor - allocate a new CTFRadiusDamageInfo onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(CTakeDamageInfo info, const Vector source, float radius, CBaseEntity ignore = NULL, float rjRadius = 0.00, float forceScale = 1.00)
    {
        char buffer[SIZEOF_CTFRadiusDamageInfo];
        view_as<CTFRadiusDamageInfo>(AddressOfString(buffer)).ctor(info, source, radius, ignore, rjRadius, forceScale);
        return buffer;
    }
}