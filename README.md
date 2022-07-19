# SourceMod Type Collection

This is basically just a dump of random methodmaps that I decided to make over time. Yes, that includes CUtlVector, all of your sins in one methodmap. Have fun!

## How to use
You can just drag over random header files from here and add them to your own project. 

At the very least, you'll need [Scags' SM-Memory Extension](https://github.com/Scags/SM-Memory). Every .inc makes use of "Pointer.inc" as well, which is pretty much the barebones methodmap for each type.

## Methodmaps present:
- Pointer.inc: This is basically a methodmap representing a pointer - it holds an address, where its value is located.
- Vector.inc: A 3D vector. No more of "float buffer[3]" all over the place.
- CUtlVector.inc: An expanding array responsible for all acts of terrorism. You're probably better off using SourceMod's ArrayList instead, but use this if you want. This will be necessary for any CUtlVectors used internally as well, such as CTFPlayer::m_hMyWearables.
- CUtlMemory.inc: This is an expanding buffer that is pretty much the barebones of CUtlVector.
