//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// utlmap.h

// To utilise the most of this methodmap, if the templated arguments that you would wish
// do not have the correct sizes, you must change them through the macros below. If you
// require multiple instances of this, duplicate the methodmap and macros and rename them
// accordingly. This includes the macro used below for detecting whether this include has
// been previously used.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLRBTREE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLRBTREE

//////////////////////////////////////////////////////////////////////////////
// CUTLRBTREE BASE TYPES                                                    //
//////////////////////////////////////////////////////////////////////////////

methodmap SMTC_UtlRBTreeLinks_t < Pointer { } 
methodmap SMTC_CUtlRBTree < Pointer { }

//////////////////////////////////////////////////////////////////////////////
// CUTLRBTREE TEMPLATED ARGUMENTS                                           //
//////////////////////////////////////////////////////////////////////////////

// template < class T, class I = unsigned short, typename L = bool (*)( const T &, const T & ), class M = CUtlMemory< UtlRBTreeNode_t< T, I >, I > >
// I should be either 1, 2 or 4 bytes.
// L should be a function pointer, or SM function (use SMTC_GetFunctionValue) if this is purely within SourceMod, or a CKeyLess instance.
// M should be ideally left as is.
#define SMTC_CUTLRBTREE_SIZEOF_T (SIZEOF_int + SIZEOF_int) // representing a CUtlMap<int, int>::Node_t //(SIZEOF_int)
#define SMTC_CUTLRBTREE_SIZEOF_I (SIZEOF_SHORT)
#define SMTC_CUTLRBTREE_SIZEOF_L (SIZEOF_Pointer)
#define SMTC_CUTLRBTREE_SIZEOF_M (SIZEOF_CUtlMemory)
#define SMTC_CUTLRBTREE_ITERATOR_T Iterator16_t // Iterator8_t for I = 1, Iterator16_t for I = 2, Iterator_t for I = 4

//////////////////////////////////////////////////////////////////////////////
// CUTLRBTREE GLOBALS                                                       //
//////////////////////////////////////////////////////////////////////////////

// This is the default LessFunc function, which just compares if lhs (int) < rhs (int).
stock Pointer SMTC_pDefaultLessFunc;
static ArrayList g_Cached;

//////////////////////////////////////////////////////////////////////////////
// UTLRBTREELINKS_T DATA                                                    //
//////////////////////////////////////////////////////////////////////////////

// These are macros instead, to allow for redefining without having to create
// new methodmaps - if that's what floats your boat.
#define UTLRBTREELINKS_T_OFFSET_M_LEFT 0                                                            // I m_Left;
#define UTLRBTREELINKS_T_OFFSET_M_RIGHT UTLRBTREELINKS_T_OFFSET_M_LEFT + SMTC_CUTLRBTREE_SIZEOF_I   // I m_Right;
#define UTLRBTREELINKS_T_OFFSET_M_PARENT UTLRBTREELINKS_T_OFFSET_M_RIGHT + SMTC_CUTLRBTREE_SIZEOF_I // I m_Parent;
#define UTLRBTREELINKS_T_OFFSET_M_TAG UTLRBTREELINKS_T_OFFSET_M_PARENT + SMTC_CUTLRBTREE_SIZEOF_I   // I m_Tag;

#define SIZEOF_UtlRBTreeLinks_t UTLRBTREELINKS_T_OFFSET_M_TAG + SMTC_CUTLRBTREE_SIZEOF_I            // sizeof(UtlRBTreeLinks_t);

//////////////////////////////////////////////////////////////////////////////
// UTLRBTREENODE_T DATA                                                     //
//////////////////////////////////////////////////////////////////////////////

#define UTLRBTREENODE_T_OFFSET_M_DATA SIZEOF_UtlRBTreeLinks_t                           // T m_Data;

#define SIZEOF_UtlRBTreeNode_t UTLRBTREENODE_T_OFFSET_M_DATA + SMTC_CUTLRBTREE_SIZEOF_T // sizeof(UtlRBTreeNode_t);

//////////////////////////////////////////////////////////////////////////////
// CUTLRBTREE DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum NodeColor_t
{
    RED = 0,
    BLACK
};

#define CUTLRBTREE_OFFSET_M_LESSFUNC 0                                                           // LessFunc_t m_LessFunc;
#define CUTLRBTREE_OFFSET_M_ELEMENTS CUTLRBTREE_OFFSET_M_LESSFUNC + SMTC_CUTLRBTREE_SIZEOF_L     // M m_Elements;
#define CUTLRBTREE_OFFSET_M_ROOT CUTLRBTREE_OFFSET_M_ELEMENTS + SMTC_CUTLRBTREE_SIZEOF_M         // I m_Root;
#define CUTLRBTREE_OFFSET_M_NUMELEMENTS CUTLRBTREE_OFFSET_M_ROOT + SMTC_CUTLRBTREE_SIZEOF_I      // I m_NumElements;
#define CUTLRBTREE_OFFSET_M_FIRSTFREE CUTLRBTREE_OFFSET_M_NUMELEMENTS + SMTC_CUTLRBTREE_SIZEOF_I // I m_FirstFree;
#define CUTLRBTREE_OFFSET_M_LASTALLOC CUTLRBTREE_OFFSET_M_FIRSTFREE + SMTC_CUTLRBTREE_SIZEOF_I   // M::Iterator_t m_LastAlloc;
#define CUTLRBTREE_OFFSET_M_PELEMENTS CUTLRBTREE_OFFSET_M_LASTALLOC + SMTC_CUTLRBTREE_SIZEOF_I   // UtlRBTreeNode_t<T, I>* m_pElements

#define SIZEOF_CUtlRBTree CUTLRBTREE_OFFSET_M_PELEMENTS + SIZEOF_Pointer                         // sizeof(CUtlRBTree);

//////////////////////////////////////////////////////////////////////////////
// UTLRBTREELINKS_T METHODMAP                                               //
//////////////////////////////////////////////////////////////////////////////

// A red-black binary search tree.
methodmap UtlRBTreeLinks_t < SMTC_UtlRBTreeLinks_t
{
    // Constructor - wrap around an existing address to make way for a UtlRBTreeLinks_t object.
    public UtlRBTreeLinks_t(Address address)
    {
        return view_as<UtlRBTreeLinks_t>(address);
    }

    // Left index.
    property any m_Left
    {
        public get() { return this.Dereference(UTLRBTREELINKS_T_OFFSET_M_LEFT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, UTLRBTREELINKS_T_OFFSET_M_LEFT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // Right index.
    property any m_Right
    {
        public get() { return this.Dereference(UTLRBTREELINKS_T_OFFSET_M_RIGHT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, UTLRBTREELINKS_T_OFFSET_M_RIGHT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // Parent index.
    property any m_Parent
    {
        public get() { return this.Dereference(UTLRBTREELINKS_T_OFFSET_M_PARENT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, UTLRBTREELINKS_T_OFFSET_M_PARENT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // Tag index.
    property any m_Tag
    {
        public get() { return this.Dereference(UTLRBTREELINKS_T_OFFSET_M_TAG, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, UTLRBTREELINKS_T_OFFSET_M_TAG, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // Constructor - allocate a new UtlRBTreeLinks_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static UtlRBTreeLinks_t Malloc()
    {
        return malloc(SIZEOF_UtlRBTreeLinks_t);
    }

    // Constructor - allocate a new UtlRBTreeLinks_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_UtlRBTreeLinks_t] = { 0 };
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// UTLRBTREENODE_T METHODMAP                                                //
//////////////////////////////////////////////////////////////////////////////

methodmap UtlRBTreeNode_t < UtlRBTreeLinks_t
{
    // Constructor - wrap around an existing address to make way for a UtlRBTreeNode_t object.
    public UtlRBTreeNode_t(Address address)
    {
        return view_as<UtlRBTreeNode_t>(address);
    }

    // Node.
    property Pointer m_Data
    {
        public get() { return view_as<any>(this) + UTLRBTREENODE_T_OFFSET_M_DATA; }
        public set(Pointer value) { memcpy(view_as<any>(this) + UTLRBTREENODE_T_OFFSET_M_DATA, value, SMTC_CUTLRBTREE_SIZEOF_T); }
    }

    // Constructor - allocate a new UtlRBTreeNode_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static UtlRBTreeNode_t Malloc()
    {
        return malloc(SIZEOF_UtlRBTreeNode_t);
    }

    // Constructor - allocate a new UtlRBTreeNode_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_UtlRBTreeNode_t] = { 0 };
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// CUTLRBTREE METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// A red-black binary search tree.
methodmap CUtlRBTree < SMTC_CUtlRBTree
{
    // Constructor - wrap around an existing address to make way for a CUtlRBTree object.
    public CUtlRBTree(Address address)
    {
        return view_as<CUtlRBTree>(address);
    }

    // Used for sorting.
    property Pointer m_LessFunc
    {
        public get() { return this.Dereference(CUTLRBTREE_OFFSET_M_LESSFUNC); }
        public set(Pointer value) { this.Write(value, CUTLRBTREE_OFFSET_M_LESSFUNC); }
    }

    // All of the elements stored in a CUtlMemory object, using 16-bit indexing.
    property CUtlMemory m_Elements
    {
        public get() { return view_as<any>(this) + CUTLRBTREE_OFFSET_M_ELEMENTS; }
        public set(CUtlMemory value) { memcpy(view_as<any>(this) + CUTLRBTREE_OFFSET_M_ELEMENTS, value, SIZEOF_CUtlMemory); }
    }

    // The root.
    property any m_Root
    {
        public get() { return this.Dereference(CUTLRBTREE_OFFSET_M_ROOT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, CUTLRBTREE_OFFSET_M_ROOT, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // The number of elements.
    property any m_NumElements
    {
        public get() { return this.Dereference(CUTLRBTREE_OFFSET_M_NUMELEMENTS, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, CUTLRBTREE_OFFSET_M_NUMELEMENTS, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // The first free.
    property any m_FirstFree
    {
        public get() { return this.Dereference(CUTLRBTREE_OFFSET_M_FIRSTFREE, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
        public set(any value) { this.Write(value, CUTLRBTREE_OFFSET_M_FIRSTFREE, BitToNumberType(SMTC_CUTLRBTREE_SIZEOF_I)); }
    }

    // The last index allocated.
    property SMTC_CUTLRBTREE_ITERATOR_T m_LastAlloc
    {
        public get() { return view_as<any>(this) + CUTLRBTREE_OFFSET_M_LASTALLOC; }
        public set(SMTC_CUTLRBTREE_ITERATOR_T value) { memcpy(view_as<any>(this) + CUTLRBTREE_OFFSET_M_LASTALLOC, value, SMTC_CUTLRBTREE_SIZEOF_I); }
    }

    // A pointer to each of the elements.
    property Pointer m_pElements
    {
        public get() { return this.Dereference(CUTLRBTREE_OFFSET_M_PELEMENTS); }
        public set(Pointer value) { this.Write(value, CUTLRBTREE_OFFSET_M_PELEMENTS); }
    }

    // Invalid index.
    public static any InvalidIndex()
    {
        return CUtlMemory.InvalidIndex(SMTC_CUTLRBTREE_SIZEOF_I);
    }
    
    // Resert the elements pointer back to the start.
    public void ResetDbgInfo()
    {
        this.m_pElements = this.m_Elements.Base();
    }

    // Remove all nodes from the tree and purges memory
    public void Purge()
    {
        this.RemoveAll();
        this.m_FirstFree = CUtlRBTree.InvalidIndex();
        this.m_Elements.Purge();
        memcpy(this.m_LastAlloc, STACK_GETRETURN(this.m_Elements.InvalidIterator(SMTC_CUTLRBTREE_SIZEOF_I)), SMTC_CUTLRBTREE_SIZEOF_I);
    }

    // Ensure capacity.
    public void EnsureCapacity(int num)
    {
        this.m_Elements.EnsureCapacity(num, SIZEOF_UtlRBTreeNode_t);
    }

    // NOTE: CopyFrom is fast but dangerous! It just memcpy's all nodes - it does NOT run copy constructors, so
	//       it is not a true deep copy (i.e 'T' must be POD for this to work - e.g CUtlString will not work).
    public void CopyFrom(const CUtlRBTree& other)
    {
        this.Purge();
        this.m_Elements.EnsureCapacity(other.m_Elements.Count(), SIZEOF_UtlRBTreeNode_t);
        memcpy(this.m_Elements.Base(), other.m_Elements.Base(), other.m_Elements.Count() * SIZEOF_UtlRBTreeNode_t);
        this.m_LessFunc = other.m_LessFunc;
        this.m_Root = other.m_Root;
        this.m_NumElements = other.m_NumElements;
        this.m_FirstFree = other.m_FirstFree;
        this.m_LastAlloc = other.m_LastAlloc;
        this.ResetDbgInfo();
    }

    // Element access - get from specified index.
    public any Get(any index)
    {
        return view_as<UtlRBTreeNode_t>(this.m_Elements.Get(index, SIZEOF_UtlRBTreeNode_t)).m_Data;
    }

    // Element access - get from specified index.
    public any Element(any index)
    {
        return view_as<UtlRBTreeNode_t>(this.m_Elements.Get(index, SIZEOF_UtlRBTreeNode_t)).m_Data;
    }

    // Gets the root
    public any Root()
    {
        return this.m_Root;
    }

    // Num elements
    public int Count()
    {
        return this.m_NumElements;
    }

    // Max "size" of the vector
    public any MaxElement()
    {
        return this.m_Elements.NumAllocated() & CUtlRBTree.InvalidIndex();
    }
    
    // Gets the parent child.
    public any Parent(any i)
    {
        return this.Links(i).m_Parent;
    }

    // Gets the left child.
    public any LeftChild(any i)
    {
        return this.Links(i).m_Left;
    }

    // Gets the right child.
    public any RightChild(any i)
    {
        return this.Links(i).m_Right;
    }

    // Tests if a node is a left child
    public bool IsLeftChild(any i)
    {
        return this.LeftChild(this.Parent(i)) == i;
    }

    // Tests if a node is a right child
    public bool IsRightChild(any i)
    {
        return this.RightChild(this.Parent(i)) == i;
    }

    // Sets the parent child.
    public void SetParent(any i, any parent)
    {
        this.Links(i).m_Parent = parent;
    }

    // Sets the left child.
    public void SetLeftChild(any i, any child)
    {
        this.Links(i).m_Left = child;
    }

    // Sets the right child.
    public void SetRightChild(any i, any child)
    {
        this.Links(i).m_Right = child;
    }
    
    // Tests if root
    public bool IsRoot(any i)
    {
        return (i & CUtlRBTree.InvalidIndex()) == this.m_Root;
    }

    // Tests if leaf
    public bool IsLeaf(any i)
    {
        return (this.LeftChild(i) == CUtlRBTree.InvalidIndex()) && (this.RightChild(i) == CUtlRBTree.InvalidIndex());
    }

    // Returns the tree depth (not a very fast operation) - default parameter is this.m_Root.
    public int Depth(any node)
    {
        if (node == CUtlRBTree.InvalidIndex())
            return 0;
        
        int depthright = this.Depth(this.RightChild(node));
        int depthleft = this.Depth(this.LeftChild(node));
        return max(depthright, depthleft) + 1;
    }

    // Checks if a link is red.
    public bool IsRed(any i)
    {
        return this.Links(i).m_Tag == RED;
    }

    // Checks if a link is black.
    public bool IsBlack(any i)
    {
        return this.Links(i).m_Tag == BLACK;
    }

    // Gets node color.
    public NodeColor_t Color(any i)
    {
        return view_as<NodeColor_t>(this.Links(i).m_Tag);
    }

    // Sets node color
    public void SetColor(any i, NodeColor_t c)
    {
        this.Links(i).m_Tag = view_as<any>(c) & CUtlRBTree.InvalidIndex(); // casted for some reason?
    }

    // Gets at the links
    public UtlRBTreeLinks_t Links(any i)
    {
        i &= CUtlRBTree.InvalidIndex();

        // Sentinel node, makes life easier
        static char s_SentinelStack[SIZEOF_UtlRBTreeLinks_t];
        static bool initialized = false;
        UtlRBTreeLinks_t s_Sentinel = AddressOfString(s_SentinelStack);
        if (initialized == false)
        {
            initialized = true;
            s_Sentinel.m_Left = CUtlRBTree.InvalidIndex();
            s_Sentinel.m_Right = CUtlRBTree.InvalidIndex();
            s_Sentinel.m_Parent = CUtlRBTree.InvalidIndex();
            s_Sentinel.m_Tag = BLACK;
        }
        
        return (i != CUtlRBTree.InvalidIndex()) ? view_as<any>(this.m_Elements.Get(i, SIZEOF_UtlRBTreeNode_t)) : view_as<any>(s_Sentinel);
    }

    // Rotates node i to the left
    public void RotateLeft(any elem)
    {
        any rightchild = this.RightChild(elem);
        this.SetRightChild(elem, this.LeftChild(rightchild));
        if (this.LeftChild(rightchild) != CUtlRBTree.InvalidIndex())
            this.SetParent(this.LeftChild(rightchild), elem);

        if (rightchild != CUtlRBTree.InvalidIndex())
            this.SetParent(rightchild, this.Parent(elem));
        if (!this.IsRoot(elem))
        {
            if (this.IsLeftChild(elem))
                this.SetLeftChild(this.Parent(elem), rightchild);
            else
                this.SetRightChild(this.Parent(elem), rightchild);
        }
        else
            this.m_Root = rightchild;
        
        this.SetLeftChild(rightchild, elem);
        if (elem != CUtlRBTree.InvalidIndex())
            this.SetParent(elem, rightchild);
    }

    // Rotates node i to the right
    public void RotateRight(any elem)
    {
        any leftchild = this.LeftChild(elem);
        this.SetLeftChild(elem, this.RightChild(leftchild));
        if (this.RightChild(leftchild) != CUtlRBTree.InvalidIndex())
            this.SetParent(this.RightChild(leftchild), elem);

        if (leftchild != CUtlRBTree.InvalidIndex())
            this.SetParent(leftchild, this.Parent(elem));
        if (!this.IsRoot(elem))
        {
            if (this.IsRightChild(elem))
                this.SetRightChild(this.Parent(elem), leftchild);
            else
                this.SetLeftChild(this.Parent(elem), leftchild);
        }
        else
            this.m_Root = leftchild;

        this.SetRightChild(leftchild, elem);
        if (elem != CUtlRBTree.InvalidIndex())
            this.SetParent(elem, leftchild);
    }

    // Rebalances the tree after an insertion
    public void InsertRebalance(any elem)
    {
        elem &= CUtlRBTree.InvalidIndex();
        while (!this.IsRoot(elem) && (this.Color(this.Parent(elem)) == RED))
        {
            any parent = this.Parent(elem);
            any grandparent = this.Parent(parent);

            // we have a violation
            if (this.IsLeftChild(parent))
            {
                any uncle = this.RightChild(grandparent);
                if (this.IsRed(uncle))
                {
                    // uncle is RED
                    // (i just wanted to say that i've been laughing at this for like 3 minutes)

                    /*
                    red skinned — Today at 20:46
                    red skinned
                    oh RB trees
                    loo
                    */

                    /*
                    @NotnHeavy  me when my uncle is now BLACK
                    jackpot jackpot ring — Today at 20:48
                    a form of theatrical makeup used predominantly by non-black people to portray a caricature of a black person.
                    */

                    this.SetColor(parent, BLACK);
                    this.SetColor(uncle, BLACK);
                    this.SetColor(grandparent, RED);
                    elem = grandparent;
                }
                else
                {
                    // uncle is BLACK
                    if (this.IsRightChild(elem))
                    {
                        // make x a left child, will change parent and grandparent
                        elem = parent;
                        this.RotateLeft(elem);
                        parent = this.Parent(elem);
                        grandparent = this.Parent(parent);
                    }
                    // recolor and rotate
                    this.SetColor(parent, BLACK);
                    this.SetColor(grandparent, RED);
                    this.RotateRight(grandparent);
                }
            }
            else
            {
                // mirror image of above code
                any uncle = this.LeftChild(grandparent);
                if (this.IsRed(uncle))
                {
                    // uncle is RED
                    this.SetColor(parent, BLACK);
                    this.SetColor(uncle, BLACK);
                    this.SetColor(grandparent, RED);
                    elem = grandparent;
                }
                else
                {
                    // uncle is BLACK
                    if (this.IsLeftChild(elem))
                    {
                        // make x a right child, will change parent and grandparent
                        elem = parent;
                        this.RotateRight(parent);
                        parent = this.Parent(elem);
                        grandparent = this.Parent(parent);
                    }
                    // recolor and rotate
                    this.SetColor(parent, BLACK);
                    this.SetColor(grandparent, RED);
                    this.RotateLeft(grandparent);
                }
            }
        }
        this.SetColor(this.m_Root, BLACK);
    }
    
    // Rebalance the tree after a deletion
    public void RemoveRebalance(any elem)
    {
        elem &= CUtlRBTree.InvalidIndex();
        while (elem != this.m_Root && this.IsBlack(elem))
        {
            any parent = this.Parent(elem);
            
            // If elem is the left child of the parent
            if (elem == this.LeftChild(parent))
            {
                // Get our sibling
                any sibling = this.RightChild(parent);
                if (this.IsRed(sibling))
                {
                    this.SetColor(sibling, BLACK);
                    this.SetColor(parent, RED);
                    this.RotateLeft(parent);

                    // We may have a new parent now
                    parent = this.Parent(elem);
                    sibling = this.RightChild(parent);
                }
                if ((this.IsBlack(this.LeftChild(sibling))) && (this.IsBlack(this.RightChild(sibling))))
                {
                    if (sibling != CUtlRBTree.InvalidIndex())
                        this.SetColor(sibling, RED);
                    elem = parent;
                }
                else
                {
                    if (this.IsBlack(this.RightChild(sibling)))
                    {
                        this.SetColor(this.LeftChild(sibling), BLACK);
                        this.SetColor(sibling, RED);
                        this.RotateRight(sibling);

                        // rotation may have changed this
                        parent = this.Parent(elem);
                        sibling = this.RightChild(parent);
                    }
                    this.SetColor(sibling, this.Color(parent));
                    this.SetColor(parent, BLACK);
                    this.SetColor(this.RightChild(sibling), BLACK);
                    this.RotateLeft(parent);
                    elem = this.m_Root;
                }
            }
            else
            {
                // Elem is the right child of the parent
                any sibling = this.LeftChild(parent);
                if (this.IsRed(sibling))
                {
                    this.SetColor(sibling, BLACK);
                    this.SetColor(parent, RED);
                    this.RotateRight(parent);

                    // We may have a new parent now
                    parent = this.Parent(elem);
                    sibling = this.LeftChild(parent);
                }
                if ((this.IsBlack(this.RightChild(sibling))) && (this.IsBlack(this.LeftChild(sibling))))
                {
                    if (sibling != CUtlRBTree.InvalidIndex())
                        this.SetColor(sibling, RED);
                    elem = parent;
                }
                else
                {
                    if (this.IsBlack(this.LeftChild(sibling)))
                    {
                        this.SetColor(this.RightChild(sibling), BLACK);
                        this.SetColor(sibling, RED);
                        this.RotateLeft(sibling);

                        // rotation may have changed this
                        parent = this.Parent(elem);
                        sibling = this.LeftChild(parent);
                    }
                    this.SetColor(sibling, this.Color(parent));
                    this.SetColor(parent, BLACK);
                    this.SetColor(this.LeftChild(sibling), BLACK);
                    this.RotateRight(parent);
                    elem = this.m_Root;
                }
            }
        }
        this.SetColor(elem, BLACK);
    }

    // Links the children.
    public void LinkToParent(any i, any parent, bool isLeft)
    {
        i &= CUtlRBTree.InvalidIndex();

        UtlRBTreeLinks_t elem = this.Links(i);
        elem.m_Parent = parent;
        elem.m_Left = elem.m_Right = CUtlRBTree.InvalidIndex();
        elem.m_Tag = RED;

        // insert node in tree
        if (parent != CUtlRBTree.InvalidIndex())
        {
            if (isLeft)
                this.Links(parent).m_Left = i;
            else
                this.Links(parent).m_Right = i;
        }
        else
            this.m_Root = i;

        this.InsertRebalance(i);
    }

    // Checks if a node is valid and in the tree
    public bool IsValidIndex(any i)
    {
        i &= CUtlRBTree.InvalidIndex();
        if (!this.m_Elements.IsIdxValid(i))
            return false;

        if (this.m_Elements.IsIdxAfter(i, this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I))
            return false; // don't read values that have been allocated, but not constructed
        
        return this.LeftChild(i) != i;
    }

    // Makes sure the tree is valid after every operation
    public bool IsValid()
    {
        if (!this.Count())
            return true;

        if (this.m_LastAlloc == view_as<SMTC_CUTLRBTREE_ITERATOR_T>(STACK_GETRETURN(this.m_Elements.InvalidIterator(SMTC_CUTLRBTREE_SIZEOF_I))))
            return false;
        
        if (!this.m_Elements.IsIdxValid(this.Root()))
            return false;
        
        if (this.Parent(this.Root()) != CUtlRBTree.InvalidIndex())
            return false;

        // UTLTREE_PARANOID is left undefined
    }

    // Sets the less func.
    // The less func may be a SM function or raw assembly code, however if
    // choosing the former, this must be only used within SourceMod.
    public void SetLessFunc(const any func)
    {
        if (!this.m_LessFunc)
            this.m_LessFunc = func;
        /*
        else if ( Count() > 0 )
        {
            // need to re-sort the tree here....
            Assert(0);
        }
        */
    }

    // Allocates a node.
    public any NewNode()
    {
        any elem;

        // Nothing in the free list; add.
        if (this.m_FirstFree == CUtlRBTree.InvalidIndex())
        {
            SMTC_CUTLRBTREE_ITERATOR_T it = this.m_Elements.IsValidIterator(this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I) ? STACK_GETRETURN(this.m_Elements.Next(this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I)) : STACK_GETRETURN(this.m_Elements.First(SMTC_CUTLRBTREE_SIZEOF_I));
            if (!this.m_Elements.IsValidIterator(it, SMTC_CUTLRBTREE_SIZEOF_I))
            {
                this.m_Elements.Grow(.typeSize = SIZEOF_UtlRBTreeNode_t, .indexSize = SMTC_CUTLRBTREE_SIZEOF_I);

                it = this.m_Elements.IsValidIterator(this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I) ? STACK_GETRETURN(this.m_Elements.Next(this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I)) : STACK_GETRETURN(this.m_Elements.First(SMTC_CUTLRBTREE_SIZEOF_I));

                if (!this.m_Elements.IsValidIterator(it, SMTC_CUTLRBTREE_SIZEOF_I))
                    ThrowError("CUtlRBTree overflow!");
            }
            this.m_LastAlloc = it;
            elem = this.m_Elements.GetIndex(this.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I);
        }
        else
        {
            elem = this.m_FirstFree;
            this.m_FirstFree = this.Links(this.m_FirstFree).m_Right;
        }

        Construct(this.Element(elem), SMTC_CUTLRBTREE_SIZEOF_T);
        this.ResetDbgInfo();

        return elem;
    }

    // Inserts a node into the tree, doesn't copy the data in.
    public void FindInsertionPosition(const Pointer insert, any& parent, bool& leftchild)
    {
        // find where node belongs
        any current = this.m_Root;
        parent = CUtlRBTree.InvalidIndex();
        leftchild = false;
        while (current != CUtlRBTree.InvalidIndex())
        {
            parent = current;

            // we're not getting the address of Element(current) in the C++ code;
            // however this is fine because the lessfunc function pointer works
            // by taking in parameters by reference
            if (SMTC_CallCKeyLess(view_as<any>(this) + CUTLRBTREE_OFFSET_M_LESSFUNC, insert, this.Element(current)))
            {
                leftchild = true;
                current = this.LeftChild(current);
            }
            else
            {
                leftchild = false;
                current = this.RightChild(current);
            }
        }
    }
    
    // Inserts a node into the tree.
    public any InsertAt(any parent, bool leftchild)
    {
        any i = this.NewNode();
        this.LinkToParent(i, parent, leftchild);
        ++this.m_NumElements;
        return i;
    }

    // Inserts a node.
    public any Insert(const Pointer insert)
    {
        // use copy constructor to copy it in
        any parent = CUtlRBTree.InvalidIndex();
        bool leftchild = false;
        this.FindInsertionPosition(insert, parent, leftchild);
        any newNode = this.InsertAt(parent, leftchild);
        CopyConstruct(this.Element(newNode), insert, SMTC_CUTLRBTREE_SIZEOF_T);
        return newNode;
    }

    // Inserts an array of nodes.
    public void InsertArray(Pointer& pArray, int nItems)
    {
        while (--nItems)
        {
            Pointer insert = pArray.Dereference();
            this.Insert(insert);
            pArray += SIZEOF_Pointer;
        }
    }

    // Inserts a node if not found.
    public any InsertIfNotFound(const Pointer insert)
    {
        // use copy constructor to copy it in
        any parent;
        bool leftchild;
        any func = view_as<any>(this) + CUTLRBTREE_OFFSET_M_LESSFUNC;

        any current = this.m_Root;
        parent = CUtlRBTree.InvalidIndex();
        leftchild = false;
        while (current != CUtlRBTree.InvalidIndex())
        {
            parent = current;
            if (SMTC_CallCKeyLess(func, insert, this.Element(current)))
            {
                leftchild = true;
                current = this.LeftChild(current);
            }
            else if (SMTC_CallCKeyLess(func, this.Element(current), insert))
            {
                leftchild = false;
                current = this.RightChild(current);
            }
            else   
                // Match found, no insertion
                return CUtlRBTree.InvalidIndex();
        }

        any newNode = this.InsertAt(parent, leftchild);
        CopyConstruct(this.Element(newNode), insert, SMTC_CUTLRBTREE_SIZEOF_T);
        return newNode;
    }

    // Finds a node in the tree
    public any Find(const Pointer search)
    {
        any current = this.m_Root;
        any func = view_as<any>(this) + CUTLRBTREE_OFFSET_M_LESSFUNC;
        while (current != CUtlRBTree.InvalidIndex())
        {
            if (SMTC_CallCKeyLess(func, search, this.Element(current)))
                current = this.LeftChild(current);
            else if (SMTC_CallCKeyLess(func, this.Element(current), search))
                current = this.RightChild(current)
            else
                break;
        }
        return current;
    }

    // Add back an element in the tree.
    public void Link(any elem)
    {
        elem &= CUtlRBTree.InvalidIndex();
        if (elem != CUtlRBTree.InvalidIndex())
        {
            any parent = CUtlRBTree.InvalidIndex();
            bool leftchild = false;

            this.FindInsertionPosition(this.Element(elem), parent, leftchild);
            this.LinkToParent(elem, parent, leftchild);
        }
    }

    // Remove an element in the tree.
    public void Unlink(any elem)
    {
        elem &= CUtlRBTree.InvalidIndex();
        if (elem != CUtlRBTree.InvalidIndex())
        {
            any x, y;

            if ((this.LeftChild(elem) == CUtlRBTree.InvalidIndex()) ||
                (this.RightChild(elem) == CUtlRBTree.InvalidIndex()))
                // y has a NIL node as a child
                y = elem;
            else
            {
                // find tree successor with a NIL node as a child
                y = this.RightChild(elem);
                while (this.LeftChild(y) != CUtlRBTree.InvalidIndex())
                    y = this.LeftChild(y);
            }

            // x is y's only child
            if (this.LeftChild(y) != CUtlRBTree.InvalidIndex())
                x = this.LeftChild(y);
            else
                x = this.RightChild(y);

            // remove y from the parent chain
            if (x != CUtlRBTree.InvalidIndex())
                this.SetParent(x, this.Parent(y))
            if (!this.IsRoot(y))
            {
                if (this.IsLeftChild(y))
                    this.SetLeftChild(this.Parent(y), x);
                else
                    this.SetRightChild(this.Parent(y), x);
            }
            else
                this.m_Root = x;

            // need to store this off, we'll be resetting y's color
            NodeColor_t ycolor = this.Color(y);
            if (y != elem)
            {
                // Standard implementations copy the data around, we cannot here.
		    	// Hook in y to link to the same stuff elem used to.
                this.SetParent(y, this.Parent(elem));
                this.SetRightChild(y, this.RightChild(elem));
                this.SetLeftChild(y, this.LeftChild(elem));

                if (!this.IsRoot(elem))
                {
                    if (this.IsLeftChild(elem))
                        this.SetLeftChild(this.Parent(elem), y);
                    else
                        this.SetRightChild(this.Parent(elem), y);
                }
                else
                    this.m_Root = y;

                if (this.LeftChild(y) != CUtlRBTree.InvalidIndex())
                    this.SetParent(this.LeftChild(y), y);
                if (this.RightChild(y) != CUtlRBTree.InvalidIndex())
                    this.SetParent(this.RightChild(y), y);

                this.SetColor(y, this.Color(elem));
            }

            if ((x != CUtlRBTree.InvalidIndex()) && (ycolor == BLACK))
                this.RemoveRebalance(x);
        }
    }

    // Frees a node from the tree.
    public void FreeNode(any i)
    {
        Destruct(this.Element(i), SMTC_CUTLRBTREE_SIZEOF_T);
        this.SetLeftChild(i, i); // indicates it's not in the tree
        this.SetRightChild(i, this.m_FirstFree);
        this.m_FirstFree = i;
    }

    // Delete a node from the tree at a specific index.
    public void RemoveAt(any elem)
    {
        elem &= CUtlRBTree.InvalidIndex();
        if (elem != CUtlRBTree.InvalidIndex())
        {
            this.Unlink(elem);
            this.FreeNode(elem);
            --this.m_NumElements;
        }
    }

    // Remove a node in the tree
    public bool Remove(const Pointer search)
    {
        any node = this.Find(search);
        if (node != CUtlRBTree.InvalidIndex())
        {
            this.RemoveAt(node);
            return true;
        }
        return false;
    }

    // Removes all nodes from the tree
    public void RemoveAll()
    {
        if (this.m_LastAlloc == view_as<SMTC_CUTLRBTREE_ITERATOR_T>(STACK_GETRETURN(this.m_Elements.InvalidIterator(SMTC_CUTLRBTREE_SIZEOF_I))))
            return;

        // for ( typename M::Iterator_t it = m_Elements.First(); it != m_Elements.InvalidIterator(); it = m_Elements.Next( it ) )
        for (int i = 0; i < this.m_Elements.m_nAllocationCount; ++i)
        {
            if (this.IsValidIndex(i)) // skip elements in the free list
            {
                Destruct(this.Element(i), SMTC_CUTLRBTREE_SIZEOF_T);
                this.SetRightChild(i, this.m_FirstFree);
                this.SetLeftChild(i, i);
                this.m_FirstFree = i;
            }
            
            if (i == this.m_LastAlloc.index)
                break; // don't destruct elements that haven't ever been constucted
        }

        this.m_Root = CUtlRBTree.InvalidIndex();
        this.m_NumElements = 0;

        if (!this.IsValid())
            ThrowError("CUtlRBTree::IsValid() call failed in CUtlRBTree::RemoveAll().");
    }

    // Iteration.
    public any FirstInorder()
    {
        any i = this.m_Root;
        while (this.LeftChild(i) != CUtlRBTree.InvalidIndex())
            i = this.LeftChild(i);
        return i;
    }

    // Iteration.
    public any NextInorder(any i)
    {
        i &= CUtlRBTree.InvalidIndex();
        if (!this.IsValidIndex(i))
            return CUtlRBTree.InvalidIndex();
        
        if (this.RightChild(i) != CUtlRBTree.InvalidIndex())
        {
            i = this.RightChild(i);
            while (this.LeftChild(i) != CUtlRBTree.InvalidIndex())
                i = this.LeftChild(i);
            return i;
        }

        any parent = this.Parent(i);
        while (this.IsRightChild(i))
        {
            i = parent;
            if (i == CUtlRBTree.InvalidIndex())
                break;
            parent = this.Parent(i);
        }
        return parent;
    }

    // Iteration.
    public any PrevInorder(any i)
    {
        i &= CUtlRBTree.InvalidIndex();
        if (!this.IsValidIndex(i))
            return CUtlRBTree.InvalidIndex();

        if (this.LeftChild(i) != CUtlRBTree.InvalidIndex())
        {
            i = this.LeftChild(i);
            while (this.RightChild(i) != CUtlRBTree.InvalidIndex())
                i = this.RightChild(i);
            return i;
        }

        any parent = this.Parent(i);
        while (this.IsLeftChild(i))
        {
            i = parent;
            if (i == CUtlRBTree.InvalidIndex())
                break;
            parent = this.Parent(i);
        }
        return parent;
    }

    // Iteration.
    public any LastInorder()
    {
        any i = this.m_Root;
        while (this.RightChild(i) != CUtlRBTree.InvalidIndex())
            i = this.RightChild(i);
        return i;
    }

    // Iteration.
    public any FirstPreorder()
    {
        return this.m_Root;
    }

    // Iteration.
    public any NextPreorder(any i)
    {
        i &= CUtlRBTree.InvalidIndex();
        if (this.LeftChild(i) != CUtlRBTree.InvalidIndex())
            return this.LeftChild(i);

        if (this.RightChild(i) != CUtlRBTree.InvalidIndex())
            return this.RightChild(i);
        
        any parent = this.Parent(i);
        while (parent != CUtlRBTree.InvalidIndex())
        {
            if (this.IsLeftChild(i) && (this.RightChild(parent) != CUtlRBTree.InvalidIndex()))
                return this.RightChild(parent);
            i = parent;
            parent = this.Parent(parent);
        }
        return CUtlRBTree.InvalidIndex();
    }

    // Iteration.
    public any PrevPreorder(any i)
    {
        // "not implemented yet"
        // THEN FUCKING MAKE IT
        return CUtlRBTree.InvalidIndex();
    }

    // Iteration.
    public any LastPreorder()
    {
        any i = this.m_Root;
        for (;;) // can't do while (1) because sourcemod does chernobyl mode
        {
            while (this.RightChild(i) != CUtlRBTree.InvalidIndex())
                i = this.RightChild(i);
            
            if (this.LeftChild(i) != CUtlRBTree.InvalidIndex())
                i = this.LeftChild(i);
            else
                break;
        }
        return i;
    }

    // Iteration.
    public any FirstPostorder()
    {
        any i = this.m_Root;
        while (!this.IsLeaf(i))
        {
            if (this.LeftChild(i))
                i = this.LeftChild(i);
            else
                i = this.RightChild(i);
        }
        return i;
    }

    // Iteration.
    public any NextPostorder(any i)
    {
        i &= CUtlRBTree.InvalidIndex();
        any parent = this.Parent(i);
        if (parent == CUtlRBTree.InvalidIndex())
            return CUtlRBTree.InvalidIndex();
        
        if (this.IsRightChild(i))
            return parent;
        
        if (this.RightChild(parent) == CUtlRBTree.InvalidIndex())
            return parent;

        i = this.RightChild(parent);
        while (!this.IsLeaf(i))
        {
            if (this.LeftChild(i))
                i = this.LeftChild(i);
            else
                i = this.RightChild(i);
        }
        return i;
    }

    // If you change the search key, this can be used to reinsert the 
	// element into the tree.
    public void Reinsert(any elem)
    {
        this.Unlink(elem);
        this.Link(elem);
    }

    // Swap in place.
    public void Swap(CUtlRBTree that)
    {
        this.m_Elements.Swap(that.m_Elements);
        V_swap(this.m_LessFunc, that.m_LessFunc, SIZEOF_Pointer);
        V_swap(this.m_Root, that.m_Root, SMTC_CUTLRBTREE_SIZEOF_I);
        V_swap(this.m_NumElements, that.m_NumElements, SMTC_CUTLRBTREE_SIZEOF_I);
        V_swap(this.m_FirstFree, that.m_FirstFree, SMTC_CUTLRBTREE_SIZEOF_I);
        V_swap(this.m_pElements, that.m_pElements, SIZEOF_Pointer);
        V_swap(this.m_LastAlloc, that.m_LastAlloc, SMTC_CUTLRBTREE_SIZEOF_I);
    }

    // Retrieve the elements pointer for debug.
    public Pointer Elements()
    {
        return this.m_Elements;
    }

    // Call constructor on existing type.
    public void ctor(const any lessFunc, int growSize = 0, int initSize = 0)
    {
        // Member initializers.
        this.m_LessFunc = lessFunc;
        this.m_Elements.ctor(growSize, initSize, SIZEOF_UtlRBTreeNode_t);
        this.m_Root = CUtlRBTree.InvalidIndex();
        this.m_NumElements = 0;
        this.m_FirstFree = CUtlRBTree.InvalidIndex();
        memcpy(this.m_LastAlloc, STACK_GETRETURN(this.m_Elements.InvalidIterator(SMTC_CUTLRBTREE_SIZEOF_I)), SMTC_CUTLRBTREE_SIZEOF_I);

        this.ResetDbgInfo();
    }

    // Call deconstructor on existing type.
    public void dtor()
    {
        this.Purge();
    }

    // Constructor - allocate a new CUtlRBTree onto the heap. Deallocation will be required when you are finished using CUtlRBTree.Dispose().
    public static CUtlRBTree Malloc(const any lessFunc, int growSize = 0, int initSize = 0)
    {
        CUtlRBTree pointer = malloc(SIZEOF_CUtlRBTree);
        pointer.ctor(lessFunc, growSize, initSize);
        return pointer;
    }

    // Constructor - allocate a new CUtlRBTree onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(const any lessFunc, int growSize = 0, int initSize = 0)
    {
        char buffer[SIZEOF_CUtlRBTree];
        view_as<CUtlRBTree>(AddressOfString(buffer)).ctor(lessFunc, growSize, initSize);
        return buffer;
    }

    // Dispose this boject.
    public void Dispose()
    {
        this.dtor();
        free(this);
    }
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

stock void SetDefLessFunc(CUtlRBTree RBTree)
{
	RBTree.SetLessFunc(SMTC_pDefaultLessFunc);
}

stock bool SMTC_CallCKeyLess(Pointer address, const Pointer left, const Pointer right)
{
    // Get the function from the address.
    any func = address.Dereference();

    // Is this potentially a SM function?
    if (SMTC_IsSMFunction(func))
    {
        Call_StartFunction(null, SMTC_GetValueAsFunction(func));
        Call_PushCell(left);
        Call_PushCell(right);

        bool result;
        Call_Finish(result);
        return result;
    }

    // Generate a SDKCall and call our function pointer.
    if (!g_Cached)
        g_Cached = new ArrayList(2);
    else
    {
        for (int i = 0, size = g_Cached.Length; i < size; ++i)
        {
            any array[2];
            g_Cached.GetArray(i, array);
            if (array[0] == view_as<int>(address))
                return SDKCall(array[1], left, right);
        }
    }
    StartPrepSDKCall(SDKCall_Static);
    PrepSDKCall_SetAddress(func);
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const KeyType_t& left
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const KeyType_t& right
    PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
    Handle handle = EndPrepSDKCall();

    any array[2];
    array[0] = address;
    array[1] = handle;
    g_Cached.PushArray(array);

    return SDKCall(handle, left, right);
}