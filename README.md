# SourceMod Type Collection

This is basically just a dump of random methodmaps that I decided to make over time. Yes, that includes CUtlVector, all of your sins in one methodmap. Have fun!

## How to use & dependencies
You can just drag over your desired header files from here (located in ./scripting/include/) and add them to your own project. 

At the very least, you'll need [Scags' SM-Memory Extension](https://github.com/Scags/SM-Memory). Every .inc makes use of "Pointer.inc" as well, which is pretty much the barebones methodmap for each type. Additional required headers will be mentioned in the respective include descriptions too.

## Includes present:
- SMTC.inc: By default you will not need this, however some headers will require or take advantage of this. Whether a header uses this will be mentioned in its section. This contains a few extra things that allow for the full potential usage of certain objects. If using this header, call SMTC_Initialize() in OnPluginStart(). This must also be #include'd at the start, above all other SMTC includes.
- Pointer.inc: This is basically a methodmap representing a pointer - it holds an address, where its value is located. This methodmap provides functionality for a few other operations, such as "stack" allocation (essentially wrap derivatives of Pointer around arrays), a few Pointer globals, and a few mathematical functions that will probably be moved to a separate header eventually (mathlib.inc).
- Vector.inc: A 3D vector. No more of "float buffer[3]" all over the place. Including SMTC.inc will allow for properly setting up vec3_origin, however it is not necessary.
- QAngle.inc: A 3D vector, but used for pitch/yaw/roll.
- CUtlVector.inc: A dynamically expanding array responsible for all acts of terrorism. You're probably better off using SourceMod's ArrayList instead, but use this if you want. This will be necessary for any CUtlVectors used internally however, such as CBasePlayer::m_hMyWearables. Note, because this repository relies on Scags' SM-Memory extension, this will collide with the methodmap found in SM-Memory's vec.inc. You'll have to modify your smmem.inc to not include its own vec.inc. Unlike Scags' implementation, this one is made to have full functionality and also has its own CUtlMemory methodmap, much like how the internal CUtlVector class has its own CUtlMemory class. This requires "CUtlMemory.inc".
- CUtlMemory.inc: This is a dynamically expanding buffer that is pretty much the barebones of CUtlVector.
- cplane_t.inc: A surface plane structure, primarily found in CBaseTrace. This requires "Vector.inc" and "ctypes.inc".
- csurface_t.inc: A structure containing information of a surface, primarily found in CGameTrace. This requires "Vector.inc", "cplane_t.inc" and "ctypes.inc".
- CBaseTrace.inc: The base object used for tracing information. This requires "Vector.inc", "cplane_t.inc" and "ctypes.inc".
- CGameTrace.inc: typedef'd as "trace_t" internally, this expands upon CBaseTrace and is typically used in trace operations. This requires "Vector.inc", "csurface_t.inc" and "ctypes.inc".
- shareddefs.inc: This just contains a few pieces of information for usage with the other headers. This is not complete, however it is slowly expanding and I figured this would be an appropriate place to include additional code. This include does not require Pointer.inc.
- ctypes.inc: This contains four methodmaps that mimick C types - UInt8_t (BYTE), Int8_t (CHAR), UInt16_t (WCHAR/USHORT), Int16_t (SHORT). As methodmaps are also cells, the underlying data type is still 4 bytes, however these methodmaps behave as if they are otherwise. For char arrays, this isn't as necessary, as char arrays are actually treated as if they are singular bytes. This include does not require Pointer.inc.
- vtable.inc: Yeah this is quite ridiculous and requires knowledge of the x86 assembly language. If you are working with objects that require vtable modification, this include allows for that - you can create a record for an object's vtable, create the necessary vpointers, then hook the vtable onto an object. You can call it with SDKCall using SDKCall_SetVirtual()... if you even need to. Otherwise this can be used for objects such as traces (which I may as well work on soon). When using this, call VTable.ClearVTables() on plugin end.

## TF2 includes present:
- tf/tf_shareddefs.inc: Similar to shareddefs.inc, however this is TF2 specific. This include is not complete either, however it is slowly expanding. This include does not require Pointer.inc
- tf/CTakeDamageInfo.inc: An object that contains all information for damaging players. Note that you will have to create your own SDKCalls (in particular, CBaseEntity::TakeDamage), or create your own DHooks (in particular, the OnTakeDamage ones) in order to utilise this methodmap fully. Note that there is a bug where if you toggle mini crits manually before "CTFGameRules::ApplyOnDamageModifyRules" is called, due to mini crits and crits both toggling the DMG_CRIT damage type, the effect will default to the full crit one. This can be fixed by including SMTC.inc and calling "CTakeDamageInfo::SetCritType()" instead of modifying "CTakeDamageInfo::m_eCritType" directly. This requires "Vector.inc".
- tf/CTFRadiusDamageInfo.inc: An object that allows for damaging players within a specific radius. Note that you will have to create your own SDKCalls (in particular, CTFGameRules::RadiusDamage) in order to utilise this methodmap fully. It's recommended to include SMTC.inc for the full functionality of "CTFRadiusDamageInfo::CalculateFalloff()", however it is a necessity for "CTFRadiusDamageInfo::ApplyToEntity()". This requires "CTakeDamageInfo.inc" and "Vector.inc".
- tf/TFPlayerClassData_t.inc: An object that holds data for each class in TF2. This can be used to interact with the game's g_pTFPlayerClassDataMgr, which holds data about all of TF2's classes. This requires "tf_shareddefs.inc" and "Vector.inc", and requires "SMTC.inc" for the g_pTFPlayerClassDataMgr global, which in turn allows for "GetPlayerClassData()" to work.

## SourceMod version
This library has been tested with SourceMod 1.11.