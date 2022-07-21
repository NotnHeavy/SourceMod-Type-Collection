# SourceMod Type Collection

This is basically just a dump of random methodmaps that I decided to make over time. Yes, that includes CUtlVector, all of your sins in one methodmap. Have fun!

## How to use
You can just drag over your desired header files from here (located in ./scripting/include/) and add them to your own project. 

At the very least, you'll need [Scags' SM-Memory Extension](https://github.com/Scags/SM-Memory). Every .inc makes use of "Pointer.inc" as well, which is pretty much the barebones methodmap for each type.

## Includes present:
- SMTC.inc: By default you will not need this, however some headers will require or take advantage of this. Whether a header uses this will be mentioned in their section. This contains a few extra things that allow for the full potential usage of certain objects. If using this header, call SMTC_Initialize() in OnPluginStart().
- Pointer.inc: This is basically a methodmap representing a pointer - it holds an address, where its value is located.
- Vector.inc: A 3D vector. No more of "float buffer[3]" all over the place. Including SMTC.inc will allow for properly setting up vec3_origin, however it is not necessary.
- CUtlVector.inc: A dynamically expanding array responsible for all acts of terrorism. You're probably better off using SourceMod's ArrayList instead, but use this if you want. This will be necessary for any CUtlVectors used internally as well, such as CBasePlayer::m_hMyWearables. Note, because this repository relies on Scags' SM-Memory extension, this will collide with the methodmap found in SM-Memory's vec.inc. You'll have to modify your smmem.inc to not include its own vec.inc. Unlike Scags' implementation, this one is made to have full functionality and also has its own CUtlMemory methodmap, much like how the internal CUtlVector class has its own CUtlMemory class.
- CUtlMemory.inc: This is a dynamically expanding buffer that is pretty much the barebones of CUtlVector.
- CTakeDamageInfo.inc: An object that contains all information for damaging players. Note that you will have to create your own SDKCalls (in particular, CBaseEntity::TakeDamage), or create your own DHooks (in particular, the OnTakeDamage ones) in order to utilise this methodmap fully. Note that there is a bug where if you toggle mini crits manually before "CTFGameRules::ApplyOnDamageModifyRules" is called, due to mini crits and crits both toggling the DMG_CRIT damage type, the effect will default to the full crit one. This can be fixed by including SMTC.inc and calling "CTakeDamageInfo::SetCritType()" instead of modifying "CTakeDamageInfo::m_eCritType" directly.

## SourceMod version
This library has been tested with SourceMod 1.11.