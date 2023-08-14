// TEST ON ENGINEER BUILDINGS WITH "m_Constructorlist"!

//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// utlmap.h

// To utilise the most of this methodmap, if the templated arguments that you would wish
// do not have the correct sizes, you must change them through the macros below. If you
// require multiple instances of this, duplicate the methodmap and macros and rename them
// accordingly. This includes the macro used below for detecting whether this include has
// been previously used.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMAP
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMAP

// Requires "CUtlRBTree.inc".
#include "CUtlRBTree.inc"

//////////////////////////////////////////////////////////////////////////////
// CUTLMAP BASE TYPES                                                       //
//////////////////////////////////////////////////////////////////////////////

methodmap SMTC_Node_t < Pointer { }
methodmap SMTC_CKeyLess < Pointer { }
methodmap Base_UtlMap_t < Pointer { } // hammande . real name baseutlmap_t methodmap name error bas for helf?

//////////////////////////////////////////////////////////////////////////////
// CUTLMAP TEMPLATED ARGUMENTS                                              //
//////////////////////////////////////////////////////////////////////////////

// template <typename K, typename T, typename I = unsigned short> 
// I should be either 1, 2 or 4 bytes
#define SMTC_CUTLMAP_SIZEOF_K (SIZEOF_int)
#define SMTC_CUTLMAP_SIZEOF_T (SIZEOF_int)
#define SMTC_CUTLMAP_SIZEOF_I (SIZEOF_SHORT)

//////////////////////////////////////////////////////////////////////////////
// NODE_T DATA                                                              //
//////////////////////////////////////////////////////////////////////////////

#define NODE_T_OFFSET_KEY 0                                          // KeyType_t key;
#define NODE_T_OFFSET_ELEM NODE_T_OFFSET_KEY + SMTC_CUTLMAP_SIZEOF_K // ElemType_t elem;

#define SIZEOF_Node_t NODE_T_OFFSET_ELEM + SMTC_CUTLMAP_SIZEOF_T     // sizeof(Node_t);

//////////////////////////////////////////////////////////////////////////////
// CKEYLESS DATA                                                            //
//////////////////////////////////////////////////////////////////////////////

// typedef bool (*LessFunc_t)( const KeyType_t &, const KeyType_t & );
enum
{
    CKEYLESS_OFFSET_M_LESSFUNC = 0,                               // LessFunc_t m_LessFunc; 

    SIZEOF_CKeyLess = CKEYLESS_OFFSET_M_LESSFUNC + SIZEOF_Pointer // sizeof(CKeyLess);
};

//////////////////////////////////////////////////////////////////////////////
// CUTLMAP DATA                                                             //
//////////////////////////////////////////////////////////////////////////////

#define CUTLMAP_OFFSET_M_TREE 0                                  // CUtlRBTre<Node_t, I, CKeyLess> m_Tree;

#define SIZEOF_CUtlMap CUTLMAP_OFFSET_M_TREE + SIZEOF_CUtlRBTree // sizeof(CUtlMap);

//////////////////////////////////////////////////////////////////////////////
// CKJEYLESS METHODMAP                                                      //
//////////////////////////////////////////////////////////////////////////////

// Function used for sorting.
// Returns true if the first parameter is "less" than the second
//
// You may use either a SM function (use SMTC_GetFunctionValue) or a function pointer, however
// if you use a SM function you must not use this in native code, otherwise the server will crash.
methodmap CKeyLess < SMTC_CKeyLess
{
    // Constructor - wrap around an existing address to make way for a CKeyLess object.
    public CKeyLess(Address address)
    {
        return view_as<CKeyLess>(address);
    }

    // Used for sorting.
    property Pointer m_LessFunc
    {
        public get() { return this.Dereference(CKEYLESS_OFFSET_M_LESSFUNC); }
        public set(Pointer value) { this.Write(value, CKEYLESS_OFFSET_M_LESSFUNC); }
    }

    // operator()
    public bool Call(const Pointer left, const Pointer right)
    {
        return SMTC_CallCKeyLess(view_as<any>(this) + CKEYLESS_OFFSET_M_LESSFUNC, left, right);
    }

    // Call constructor on existing type.
    public void ctor(Pointer lessFunc)
    {
        this.m_LessFunc = lessFunc;
    }
    
    // Constructor - allocate a new CKeyLess onto the heap. Deallocation will be required when you are finished using free().
    public static CKeyLess Malloc(Pointer lessFunc)
    {
        CKeyLess pointer = malloc(SIZEOF_CKeyLess);
        pointer.ctor(lessFunc);
        return pointer;
    }

    // Constructor - allocate a new CKeyLess onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(Pointer lessFunc)
    {
        char buffer[SIZEOF_CKeyLess];
        view_as<CKeyLess>(AddressOfString(buffer)).ctor(lessFunc);
        return buffer;
    }
}

stock bool operator!(CKeyLess self)
{
    return !self.m_LessFunc;
}

//////////////////////////////////////////////////////////////////////////////
// NODE_T METHODMAP                                                         //
//////////////////////////////////////////////////////////////////////////////

// An individual node within the CUtlMap instance.
methodmap Node_t < SMTC_Node_t
{
    // Constructor - wrap around an existing address to make way for a Node_t object.
    public Node_t(Address address)
    {
        return view_as<Node_t>(address);
    }

    // Key object
    property Pointer key
    {
        public get() { return view_as<any>(this) + NODE_T_OFFSET_KEY; }
        public set(Pointer value) { memcpy(view_as<any>(this) + NODE_T_OFFSET_KEY, value, SMTC_CUTLMAP_SIZEOF_K); }
    }

    // Value object
    property Pointer elem
    {
        public get() { return view_as<any>(this) + NODE_T_OFFSET_ELEM; }
        public set(Pointer value) { memcpy(view_as<any>(this) + NODE_T_OFFSET_ELEM, value, SMTC_CUTLMAP_SIZEOF_T); }
    }

    // Call constructor on existing type.
    public void ctor(const Node_t from = NULL)
    {
        if (from == NULL)
            return;
        this.key = from.key;
        this.elem = from.elem;
    }
    
    // Constructor - allocate a new Node_t onto the heap. Deallocation will be required when you are finished using free().
    public static Node_t Malloc(const Node_t from = NULL)
    {
        Node_t pointer = malloc(SIZEOF_Node_t);
        pointer.ctor(from);
        return pointer;
    }

    // Constructor - allocate a new Node_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(const Node_t from = NULL)
    {
        char buffer[SIZEOF_Node_t];
        view_as<Node_t>(AddressOfString(buffer)).ctor(from);
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// CUTLMAP METHODMAP                                                        //
//////////////////////////////////////////////////////////////////////////////

// Purpose:	An associative container. Pretty much identical to std::map.
methodmap CUtlMap < Base_UtlMap_t
{
    // Constructor - wrap around an existing address to make way for a CUtlMap object.
    public CUtlMap(Address address)
    {
        return view_as<CUtlMap>(address);
    }

    // Red-black binary tree.
    property CUtlRBTree m_Tree
    {
        public get() { return view_as<CUtlRBTree>(this.Address + CUTLMAP_OFFSET_M_TREE); }
        public set(CUtlRBTree value) { memcpy(this.Address + CUTLMAP_OFFSET_M_TREE, value, SIZEOF_CUtlRBTree); }
    }

    // Access the binary tree.
    public CUtlRBTree AccessTree()
    {
        return this.m_Tree;
    }

    // Ensure that the capacity is set.
    public void EnsureCapacity(int num)
    {
        this.m_Tree.EnsureCapacity(num);
    }

    // Gets particular elements.
    public Pointer Element(any i)
    {
        return view_as<Node_t>(this.m_Tree.Element(i)).elem;
    }
    
    // Gets particular elements.
    public Pointer Get(any i)
    {
        return view_as<Node_t>(this.m_Tree.Element(i)).elem;
    }

    // Gets the key of an element.
    public Pointer Key(any i)
    {
        return view_as<Node_t>(this.m_Tree.Element(i)).key;
    }

    // Num elements
    public int Count()
    {
        return this.m_Tree.Count();
    }

    // Max "size" of the vector
    public any MaxElement()
    {
        return this.MaxElement();
    }

    // Checks if a node is valid and in the map
    public bool IsValidIndex(any i)
    {
        return this.m_Tree.IsValidIndex(i);
    }

    // Checks if the map as a whole is valid
    public bool IsValid()
    {
        return this.m_Tree.IsValid();
    }

    // Invalid index
    public static any InvalidIndex()
    {
        return CUtlRBTree.InvalidIndex();
    }

    // Sets the less func
    public void SetLessFunc(any func)
    {
        // This can be either a CKeyLess or a function pointer/SM function index, 
        // either works.
        this.m_Tree.SetLessFunc(func);
    }

    // Insert method (inserts in order)
    public any Insert(const Pointer key, const Pointer insert = NULL)
    {
        Node_t node = STACK_GETRETURN(Node_t.StackAlloc());
        node.key = key;
        if (insert != NULL)
            node.elem = insert;
        return this.m_Tree.Insert(node);
    }

    // Find method
    public any Find(const Pointer key)
    {
        Node_t dummyNode = STACK_GETRETURN(Node_t.StackAlloc());
        dummyNode.key = key;
        return this.m_Tree.Find(dummyNode);
    }

    // Remove at
    public void RemoveAt(any i)
    {
        this.m_Tree.RemoveAt(i);
    }

    // Remove
    public bool Remove(const Pointer key)
    {
        Node_t dummyNode = STACK_GETRETURN(Node_t.StackAlloc());
        dummyNode.key = key;
        return this.m_Tree.Remove(dummyNode);
    }

    // Remove all
    public void RemoveAll()
    {
        this.m_Tree.RemoveAll();
    }

    // Purge all elements.
    public void Purge()
    {
        this.m_Tree.Purge();
    }

    // While this function is intended to delete all elements, we cannot
    // do this here in SourceMod. Ideally, iterate from 0 MaxElement()
    // and call your own destructor. This function as a reasult is 
    // equally the same as CUtlMap.Purge().
    public void PurgeAndDeleteElements()
    {
        this.Purge();
    }

    // Iteration.
    public any FirstInorder()
    {
        return this.m_Tree.FirstInorder();
    }

    // Iteration.
    public any NextInorder(any i)
    {
        return this.m_Tree.NextInorder(i);
    }

    // Iteration.
    public any PrevInorder(any i)
    {
        return this.m_Tree.PrevInorder(i);
    }

    // Iteration.
    public any LastInorder()
    {
        return this.m_Tree.LastInorder();
    }

    // If you change the search key, this can be used to reinsert the 
	// element into the map.
    public void Reinsert(const Pointer key, any i)
    {
        view_as<Node_t>(this.m_Tree.Get(i)).key = key;
        this.m_Tree.Reinsert(i);
    }

    // Insert or replace.
    public any InsertOrReplace(const Pointer key, const Pointer insert)
    {
        any i = this.Find(key);
        if (i != CUtlMap.InvalidIndex())
        {
            memcpy(this.Element(i), insert, SMTC_CUTLMAP_SIZEOF_T);
            return i;
        }
        return this.Insert(key, insert);
    }

    // Swap with another CUtlMap.
    public void Swap(CUtlMap that)
    {
        this.m_Tree.Swap(that.m_Tree);
    }

    // Call constructor on existing type.
    public void ctor(const any lessFunc = NULL, int growSize = 0, int initSize = 0)
    {
        this.m_Tree.ctor(lessFunc, growSize, initSize);
    }
    
    // Constructor - allocate a new CUtlMap onto the heap. Deallocation will be required when you are finished using CUtlMap.Dispose().
    public static CUtlMap Malloc(const any lessFunc = NULL, int growSize = 0, int initSize = 0)
    {
        CUtlMap pointer = malloc(SIZEOF_CUtlMap);
        pointer.ctor(lessFunc, growSize, initSize);
        return pointer;
    }

    // Constructor - allocate a new CUtlMap onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(const any lessFunc = NULL, int growSize = 0, int initSize = 0)
    {
        char buffer[SIZEOF_CUtlMap];
        view_as<CUtlMap>(AddressOfString(buffer)).ctor(lessFunc, growSize, initSize);
        return buffer;
    }

    // Dispose this boject.
    public void Dispose()
    {
        this.Purge();
        free(this);
    }
}