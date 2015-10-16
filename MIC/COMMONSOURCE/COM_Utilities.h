// COM_Utilities.h

#ifndef COM_UTILITIES_H
#define COM_UTILITIES_H

int Convert_BSTR_to_CString(BSTR& bstr_in, CString& cs_out);
int Convert_BSTR_to_Character_String(BSTR& bstr_in, char *string_out, int maxchars);
int Convert_Character_String_to_BSTR(char *string_in, BSTR& bstr_out);
int Convert_CString_to_BSTR(CString& cstring_in, BSTR& bstr_out);

#endif //COM_UTILITIES_H