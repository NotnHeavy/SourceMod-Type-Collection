//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// ????????????????
// This header is separated from vtable.inc due to the complexity and how specific this
// is. I advice that you just use gamedata for vtables for Linux binaries, as you can
// just look up the object's name in IDA or something, grab the symbol for the vtable
// and done. However, for consistency, there is still Linux code included here - it will
// just mangle the name and call dlsym().
//
// As for Windows, there typically will not be any symbols. As a result, I have written
// some insanely weird code that exploits compiled RTTI data that allows you to look up
// vtable addresses at run time.
//
// Regardless of whether you are using Windows or Linux, you will require a mangled name.
//
// For more information on how RTTI is compiled on Windows, check out the following url:
// https://www.lukaszlipski.dev/post/rtti-msvc/

#include <smmem/dynlib>

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_RUNTIMEVTABLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_RUNTIMEVTABLE

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

static char g_MapsPath[PLATFORM_MAX_PATH * 2];

char g_ServerPath[PLATFORM_MAX_PATH];

//////////////////////////////////////////////////////////////////////////////
// STATICS                                                                  //
//////////////////////////////////////////////////////////////////////////////

static void BuildSignatureFromInt(int number, char buffer[4])
{
    for (int index = 0; index < sizeof(buffer); ++index)
        buffer[index] = ((number >> (index * 8)) & 0xFF);
}

//////////////////////////////////////////////////////////////////////////////
// WINDOWS                                                                  //
//////////////////////////////////////////////////////////////////////////////

#define SIZEOF_IMAGE_NT_HEADERS     0xF8
#define SIZEOF_IMAGE_SECTION_HEADER 0x28
#define IMAGE_SIZEOF_SHORT_NAME     0x08

enum struct section_t
{
    char m_szName[64];
    any m_nRVA;
    any m_nSize;
}
enum struct sections_t
{
    any address;
    section_t data;
    section_t rdata;

    bool Get(const char[] name, section_t section)
    {
        if (strcmp(name, ".data") == 0)
        {
            section = this.data;
            return true;
        }
        else if (strcmp(name, ".rdata") == 0)
        {
            section = this.rdata;
            return true;
        }
        return false;
    }
    bool Set(const char[] name, section_t section)
    {
        if (strcmp(name, ".data") == 0)
        {
            this.data = section;
            return true;
        }
        else if (strcmp(name, ".rdata") == 0)
        {
            this.rdata = section;
            return true;
        }
        return false;
    }
    void Init(any address)
    {
        this.address = address;
        this.data.m_szName = ".data";
        this.rdata.m_szName = ".rdata";
    }
}
static char g_SectionNames[][] = { ".rdata", ".data" };
static ArrayList g_SectionData;

// Locate the sections required for this image.
static bool Windows_GetSections(any imageBase, sections_t sectionData)
{
    // Initialize the sections data.
    sectionData.Init(imageBase);

    // Jump to IMAGE_NT_HEADERS.
    any address = imageBase + LoadFromAddress(imageBase + 0x3C, NumberType_Int32); // IMAGE_DOS_HEADER + IMAGE_DOS_HEADER->e_lfanew;

    // Get the number of sections.
    any sections = LoadFromAddress(address + 0x06, NumberType_Int16); // IMAGE_NT_HEADERS->FileHeader.NumberOfSections;
    
    // Iterate through each section, and load necessary data.
    for (int section = 0; section < sizeof(g_SectionNames); ++section)
    {
        any base = address + SIZEOF_IMAGE_NT_HEADERS;
        int success = false;
        for (int i = 0; i < sections; ++i)
        {
            char buffer[IMAGE_SIZEOF_SHORT_NAME];
            for (int i2 = 0; i2 < IMAGE_SIZEOF_SHORT_NAME; ++i2)
                buffer[i2] = LoadFromAddress(base + i2, NumberType_Int8);
            if (strcmp(buffer, g_SectionNames[section]) == 0)
            {
                section_t newSection;
                newSection.m_nRVA = LoadFromAddress(base + 0x14, NumberType_Int32); // IMAGE_SECTION_HEADER->PointerToRawData;
                newSection.m_nSize = LoadFromAddress(base + 0x10, NumberType_Int32); // IMAGE_SECTION_HEADER->SizeOfRawData;
                sectionData.Set(buffer, newSection);
                success = true;
            }
            base += SIZEOF_IMAGE_SECTION_HEADER;
        }

        // Didn't work?
        if (!success)
            return false;
    }

    // Add to the global data and return.
    g_SectionData.PushArray(sectionData);
    return true;
}

// Sigscan a section in order to obtain some addresses.
static int Windows_ScanSection(any imageBase, const char[] sectionName, const char[] signature, const int siglength, any[] addresses, const int maxlength)
{
    // Initialize g_SectionData if it is not already initialized.
    sections_t sectionData;
    section_t section;
    if (g_SectionData == null)
        g_SectionData = new ArrayList(sizeof(sections_t));

    // Check if there already exists section data.
    bool found = false;
    for (int i = 0, size = g_SectionData.Length; i < size; ++i)
    {
        g_SectionData.GetArray(i, sectionData);
        if (sectionData.address == imageBase)
        {
            found = true;
            break;
        }
    }
    if (!found && !Windows_GetSections(imageBase, sectionData))
        ThrowError("COULD NOT READ SECTION DATA!");

    // Scan for the signature provided.
    sectionData.Get(sectionName, section);
    return SMTC_FindSignatures(imageBase + section.m_nRVA, imageBase + section.m_nRVA + section.m_nSize, signature, siglength, addresses, maxlength);
}

// Due to how vtables are designed on Windows, there may be multiple vtable objects present
// as a consequence of multiple inheritance. As a result, a vtable address is not returned.
// Instead, you must reference an array of a specific length where all vtable addresses will
// be stored. Theoretically, the base vtable object should be the first located one.
static int Windows(any imageBase, const char[] name, any[] vtables, int maxlength)
{
    // Locate all references to the mangled name provided (see RuntimeVTable.Mangle() if you do not have the mangled name on hand).
    // There should only be one reference theoretically, but this is just for robustness.
    any addresses[64];
    int count = Windows_ScanSection(imageBase, ".data", name, strlen(name), addresses, sizeof(addresses));
    for (int i = 0; i < count; ++i)
    {
        // Retrieve the address for the TypeDescriptor object.
        any typedescriptor = addresses[i] - 0x08;
        if (LoadFromAddress(typedescriptor + 0x04, NumberType_Int32)) // TypeDescriptor->spare should be nullptr!
            continue;

        // Locate each address that references the TypeDescriptor object.
        // These addresses could either point to CompleteObjectLocator (COL) or BaseClassDescriptor (BCD) objects.
        char signature[4];
        BuildSignatureFromInt(typedescriptor, signature);
        any COLs[64];
        int COLcount = Windows_ScanSection(imageBase, ".rdata", signature, sizeof(signature), COLs, sizeof(COLs));
        if (!COLcount)
            continue;
    
        // Walk through each address found.
        int vtableindex = 0;
        for (int col = 0; col < COLcount && vtableindex < maxlength; ++col)
        {
            // Verify that this is a COL.
            any pClassDescriptor = LoadFromAddress(COLs[col] + 0x04, NumberType_Int32); // _s_RTTICompleteObjectLocator->pClassDescriptor; (suspected)
            if (pClassDescriptor < 0x10000) // in theory it should not exceed this number
                continue;
            
            // Build a signature for the beginning of the COL and locate the address that references it.
            // Theoretically there should only be one reference.
            BuildSignatureFromInt(COLs[col] - 0x0C, signature);
            any colref[1];
            int found = Windows_ScanSection(imageBase, ".rdata", signature, sizeof(signature), colref, sizeof(colref));
            if (found != 1)
                continue; // error?
            
            // Jump 4 bytes and we have located the vtable!
            vtables[vtableindex++] = colref[0] + 0x04;
        }

        if (vtableindex)
            return vtableindex;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// NAMESPACE                                                                //
//////////////////////////////////////////////////////////////////////////////

// A global namespace containing functions which allow you to obtain vtable addresses
// at runtime.
methodmap RuntimeVTable
{
    // Locate all vtables within a specific binary. See RuntimeVTable.Mangle() if you
    // do not have a mangled name available.
    // 
    // Due to how Windows vtables work,
    // there may be multiple vtables for the same object as a consequence of multiple
    // inheritance. Usually the base and complete vtable will be the first one scanned
    // however; if you do not need the rest, keep maxlength at 1.
    //
    // On Linux, maxlength is ignored as it uses symbol lookup instead.
    public static int Find(const char[] image, const char[] mangledname, any[] vtables, const int maxlength = 1)
    {
        // Solve global paths if they currently point to NULL.
        if (!g_ServerPath[0])
        {
            if (SMTC.GetOperatingSystem() == OSTYPE_WINDOWS)
                g_ServerPath = "server";
            else
                g_ServerPath = "server_srv"; // This will have to be resolved through /proc/self/maps.
        }

        // macOS is not supported.
        if (SMTC.GetOperatingSystem() == OSTYPE_MACOS)
            return -1;
        
        // Setup.
        int count = 1;
        char buffer[PLATFORM_MAX_PATH];
        strcopy(buffer, sizeof(buffer), image);

        // Attempt to initialise the DynLib object. If we cannot find it,
        // attempt to solve the path by walking through /proc/self/maps, if on Linux.
        DynLib lib = new DynLib(buffer);
        if (lib == null && SMTC.GetOperatingSystem() == OSTYPE_LINUX)
        {
            // Open /proc/self/maps/
            // Because OpenFile() is deranged and Not Recommended due to Killing More than 5.4 Billion People,
            // I will have to write terrorist code here. Sorry.
            PrintToServer("[SMTC] Walking through /proc/self/maps to find absolute path for \"%s\"", buffer);
            File file;
            if (g_MapsPath[0])
                file = OpenFile(g_MapsPath, "r");
            else
            {
                char maps[PLATFORM_MAX_PATH * 2] = "proc/self/maps";
                while (file == null)
                {
                    Format(maps, sizeof(maps), "../%s", maps);
                    file = OpenFile(maps, "r");
                }
                g_MapsPath = maps;
            }
            
            // Walk through each line and check if image can be resolved.
            char line[PLATFORM_MAX_PATH + 1024];
            while (file.ReadLine(line, sizeof(line)))
            {
                // Skip to where the absolute directory is.
                int result = StrContains(line, "/");
                int size = strlen(line);
                if (result > 0 && StrContains(line, image) >= result && StrContains(line, ".so") == size - 4)
                {
                    buffer = "";
                    for (int i = result; i < size - 4; ++i)
                        buffer[i - result] = line[i];
                    break;
                }
            }

            // Close file and now initiate a DynLib instance.
            delete file;
            lib = new DynLib(buffer);
        }
        if (lib == null)
            return 0;

        // Windows - load image address and call Windows().
        if (SMTC.GetOperatingSystem() == OSTYPE_WINDOWS)
            count = Windows(lib.BaseAddr, mangledname, vtables, maxlength);

        // Linux - resolve the vtable's symbol.
        else
        {
            vtables[0] = lib.ResolveSymbol(mangledname);
            if (!vtables[0])
                count = 0;
            else
                vtables[0] += 0x08; // + 0x08 to go to the beginning vpointer
        }

        delete lib;
        return count;
    }
}