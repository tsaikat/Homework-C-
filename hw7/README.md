The task is to implement class template, the template will implement a generic index.

The index is implemented as generic class `CIndex`. The instance is initialized with a parameter - a sequence of elements. The sequence is to be indexed. The sequence may be of the form:

-   `string` - a C or C++ string consisting of characters,
-   `vector<T>` - a vector of values (elements of type T),
-   `list<T>` - a list if values (elements of type T).

Once the instance is initialized and the input sequence is indexed, the instance may be used for searching. The search is given some sequence of elements (the data type matches the stored sequence). The result is a set of indices where the searched sequence was found in the indexed sequence.

To develop a flexible class, the matching of elements may be parameterized. The class will accept two generic parameters - the type of the indexed sequence and a comparator (second optional generic parameter). The searching does not have to compare the elements for an exact match. If a custom comparator is provided, the matching shall follow that comparator. For instance, the customized comparator may be used to implement case insensitive searching. The comparator will follow the guidelines used in STL: it is of the form of a function, functor, or a lambda function. The comparator accepts two parameters of type T (elements to compare), the result is `true` if the first element is smaller than the second element in the desired ordering. If no custom comparator is provided, operator < will be used.

The elements in the sequences are virtually unlimited. Examples are characters (char), integers (int), or strings (C++ string). In general, the element type T guarantees:

-   copying (operator = and copy constructor),
-   comparison by operator < ("less than"), or by the custom comparator,
-   freeing (destructor),
-   there may be further operations available in type T, however, your implementation cannot rely on them. Caution: default constructor, operator ==, operator !=, ... are not guaranteed.
