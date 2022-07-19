# SourceMod Type Collection

This is basically just a dump of random methodmaps that I decided to make over time. Yes, that includes CUtlVector, all of your sins in one methodmap. Have fun!

## How to use
You can just drag over random header files from here and add them to your own project. 

At the very least, you'll need [Scags' SM-Memory Extension](https://github.com/Scags/SM-Memory). Every .inc makes use of "Pointer.inc" as well, which is pretty much the barebones methodmap for each type.

## Methodmaps present:
- Pointer.inc: This is basically a methodmap representing a pointer - it holds an address, where its value is located.
- Vector.inc: A 3D vector. No more of "float buffer[3]" all over the place.
- CUtlVector.inc: A dynamically expanding array responsible for all acts of terrorism. You're probably better off using SourceMod's ArrayList instead, but use this if you want. This will be necessary for any CUtlVectors used internally as well, such as CBasePlayer::m_hMyWearables. Note, because this repository relies on Scags' SM-Memory extension, this will collide with the methodmap found in SM-Memory's vec.inc. You'll have to modify your smmem.inc to not include its own vec.inc. Unlike Scags' implementation, this one is made to have full functionality and also has its own CUtlMemory methodmap, much like how the internal CUtlVector class has its own CUtlMemory class.
- CUtlMemory.inc: This is a dynamically expanding buffer that is pretty much the barebones of CUtlVector.
