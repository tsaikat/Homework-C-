The task is to develop classes that implement data types in a C/C++ compiler. This problem is an extension of the first variant. Thus it is recommended to start with the simpler variant and then add the extensions needed in this variant.

The extension includes new types:

-   pointers,
-   arrays,
-   the implementation must allow any valid combination of the types. For example, it must support pointers to structures, add pointer to a structure as a field into another structure, ...

The program must implement following classes:

CDataTypeInt
------------

The class implements built-in type `int`. The interface is:

default constructor

initializes the instance,

getSize()

the method returns size of the type (4 in the case of int),

operator ==

compares two types, returns true if they are identical (both are ints),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream.

CDataTypeDouble
---------------

The class implements built-in type `double`. The interface is:

default constructor

initializes the instance,

getSize()

the method returns size of the type (8 in the case of double),

operator ==

compares two types, returns true if they are identical (both are doubles),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream.

CDataTypeEnum
-------------

The class represents an enumeration. The interface is:

default constructor

initializes the instance,

getSize()

the method returns size of the type (4 in the case of enums),

add()

the method adds a new enumeration value to the list. An exception is thrown if the same value is already included in the list (see sample runs),

operator ==

compares two types, returns true if they are identical (both are enumerations, moreover, the lists of values are identical, the values are listed in the same order),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream. Caution, enum values must be printed in the order they were added.

CDataTypeStruct
---------------

The class represents a structure. The interface is:

default constructor

initializes the instance,

getSize()

the method returns size of the type (based on the size of the fields),

addField(name, type)

the method adds a new field to the end of the field list. A field is described by its name and type (int/double/enum). If the name collides with another existing name in the field list, the method shall throw an exception (see sample runs),

field(name)

the method is used to access the type of the given field. If the name does not refer to an existing field, the method throws an exception (see sample runs). The field is accessed in read-only mode,

operator ==

compares two types, returns true if they are identical (both are structures, both have the same number of fields, the datatypes of corresponding fields are the same, HOWEVER, field names do not have to match),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream. The fields are listed in the order they were added via `addField`.

CDataTypeArray
--------------

The class implements an array. The interface is:

constructor (size,type)

initializes an instance, array size is `size`, array elements are of type `type`,

getSize()

the method returns size of the type (based on size and element type),

element()

method to access the type of the elements (see sample runs),

operator ==

compares two types, returns true if the types are identical (both are arrays, same sizes, same type of the elements),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream.

CDataTypePtr
------------

The class implements a pointer. The interface is:

constructor

initializes an instance, the pointer shall point to the type in parameter,

getSize()

the method returns size of the type (always 8 for pointers),

element()

method to access the type the pointer points to (see sample runs),

operator ==

compares two types, returns true if the types are identical (both are pointers, both point to the same type),

operator !=

compares two types, returns true, if they are not identical,

operator <<

displays type declaration in the given stream.
