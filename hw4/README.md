Your task is to develop class CFile to simulate a binary file.

The interface of the class is described below. There are read/write operations, file seeking, and truncating. Moreover, we require versioning, i.e. the current contents of the file can be stored and subsequently restored. The versioning will support an unlimited number of version (there is an implicit memory limit). Any of the previous versions can by restored in an undo-like manner. The class will not manipulate actual files on the disk (the disk will not be writable). Instead, the data re to be stored in the main memory (the class just simulates a file).

Instances of CFile class can be copied, either by copy constructor, or by assignment operator. The copies shall be identical independent instances, modifications of one instance must not affect the other. On the other hand, the modifications are likely to modify only a portion of the file contents, thus it may be desirable to share some data to save memory. The copies are identical, i.e. the copying also copies all stored versions of the file.

Required class interface:

constructor
default constructor initializes an empty instance of CFile (size 0 B, file position 0),
destructor, op=, and copy constructor
must be implemented if the automatically generated variant is not usable,
write (data, len)
method writes bytes a block of bytes (data) of length len bytes into the file. The writing start at the actual file position, the position is advanced by the number of bytes written. The method overwrites/adds the bytes to the file as needed. Return value is the number of bytes written.
read (data, len)
method reads bytes from the actual file position. The bytes are read into buffer data, the method reads up to len bytes. The actual number of bytes read is returned as a return value (may be less than len, based on the file position and file size).Next, the method advances file position by the number of bytes it actually read.
seek ( pos )
the method changes file position to pos. The modified file position will be used in subsequent read/write operations. Parameter pos must fit into interval 0 to file size (both inclusive). Return value is either true to indicate success, or false to report an error (position outside the valid range),
truncate()
the method truncates the file to the actual position in the file,
fileSize()
the method returns the actual size of the file,
addVersion()
the method adds the current version of the instance (a snapshot) into the list of file versions. Both file contents and file position are archived in the file versions. The versions are archived inside the CFile instance,
undoVersion()
the method reverts the contents (and file position) of the file to the previous version (archived by a call to addVersion). Any previously saved versions may be recovered, i.e. undoVersion may be called multiple times. The method returns true to indicate success, or false to indicate a problem (no previous version available).
