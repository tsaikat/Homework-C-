task is to develop class CDate to represent a date. The dates will follow Gregorian calendar rules. It is required that at least years 2000 to 2030 are handled by the class.

The class shall have the following interface:

- constructor with parameters (y,m,d) initializes an instance of the class with the date given. The constructor shall test the date. If the date is invalid, InvalidDateException shall be thrown,
- operator + can be used to add an integer to an instance of CDate. The operation returns a date which is shifted to the future by the given number of days (if the integer is negative, it shifts to the past),
- operator - can be used to subtract an integer from CDate. The result is a date shifted by the given number of days to the past (to the future if the integer is negative),
- operator - can be used to subtract two instances of CDate, the result is an integer equal to the number of days between the two dates,
- operator ++ and -- in bot prefix and postfix notation can be used to increase/decrease the date by one day. The operators shall follow the usual behavior,
- operators ==, !=, <, <=, >, and >= can be used to compare two instances of CDate. Dates in the future are considered greater than dates in the past.
- operator << can be used to display the date in an output stream. The conversion shall use the ISO date format (%Y-%m-%d, e.g. 2000-01-31). The mandatory tests will use the ISO format only. The format may be modified by date_format manipulator, the manipulator must be implemented to pass the bonus test (it is not used in the mandatory tests),
- operator >> can be used to read date from an input stream. The mandatory tests assume default ISO date format. If the reading fails (invalid format, invalid date, ...), the operator sets fail bit in the corresponding stream and leaves the original contents of the CDate instance unmodified. Similarly to the output operator, bonus tests require an implementation of date_format manipulator to handle arbitrary input format.
