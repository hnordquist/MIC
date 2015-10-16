// version.h
///////////////////////////////////////////////////////////////////////////
//
//  Class MSKVersionInfo
//
//
//  Handles accessing the VERSIONINFO resource, which is particularly poorly
//  documented by Microsoft, but does define it's items as follows:
//
/* ----- start excerpt ------------
     
====================+===============================================================
Comments            |Specifies additional information that should be displayed for
                    |diagnostic purposes.
====================+===============================================================
CompanyName         |Specifies the company that produced the file-for example,
                    |"Microsoft Corporation" or "Standard Microsystems 
                    |Corporation, Inc.". This string is required.
====================+===============================================================
FileDescription     |Specifies a file description to be presented to users. This
                    |string may be displayed in a list box when the user is 
                    |choosing files to install-for example, "Keyboard Driver for
                    |AT-Style Keyboards" or "Microsoft Word for Windows". This
                    |string is required.
====================+===============================================================
FileVersion         |Specifies the version number of the file-for example, "3.10"
                    |or "5.00.RC2". This string is required.
====================+===============================================================
InternalName        |Specifies the internal name of the file, if one exists-for
                    |example, a module name if the file is a dynamic-link library.
                    |If the file has no internal name, this string should be the
                    |original filename, without extension. This string is 
                    |required.
====================+===============================================================
LegalCopyright      |Specifies all copyright notices that apply to the file. This
                    |should include the full text of all notices, legal symbols,
                    |copyright dates, and so on-for example, "Copyright Microsoft
                    |Corp. 1990,1991". This string is optional.
====================+===============================================================
LegalTrademarks     |Specifies all trademarks and registered trademarks that apply
                    |to the file. This should include the full text of all 
                    |notices, legal symbols, trademark numbers, and so on-for
                    |example, "Windows(TM) is a trademark of Microsoft 
                    |Corporation". This string is optional.
====================+===============================================================
OriginalFilename    |Specifies the original name of the file, not including a
                    |path. This information enables an application to determine
                    |whether a file has been renamed by a user. The format of the
                    |name depends on the file system for which the file was 
                    |created. This string is required.
====================+===============================================================
PrivateBuild        |Specifies information about a private version of the file-for
                    |example, "Built by TESTER1 on \TESTBED". This string should
                    |be present only if the VS_FF_PRIVATEBUILD flag is set in the
                    |dwFileFlags member of the VS_FIXEDFILEINFO structure of the
                    |root block.
====================+===============================================================
ProductName         |Specifies the name of the product with which the file is
                    |distributed-for example, "Microsoft Windows". This string is
                    |required.
====================+===============================================================
ProductVersion      |Specifies the version of the product with which the file is
                    |distributed-for example, "3.10" or "5.00.RC2". This string is
                    |required.
====================+===============================================================
SpecialBuild        |Specifies how this version of the file differs from the
                    |standard version-for example, "Private build for TESTER1
                    |solving mouse problems on M250 and M250E computers". This
                    |string should be present only if the VS_FF_SPECIALBUILD flag
                    |is set in the dwFileFlags member of the VS_FIXEDFILEINFO
                    |structure in the root block.
====================+===============================================================
     
-------- end excerpt   --- */
     
//  MSKVersionInfo objects have a member function for each of the above elements
//  returned as CString objects.
//
//  Values are only retrieved from the VERSIONINFO structure; they cannot be
//  written to.
//
//  If new elements are added to VERSIONINFO structure in the future, they can
//  be retieved using the GetValue() member function. 
//
//
//  Example use:
//
//  MSKVersionInfo  ver;
//  // no parmeter defaults to active EXE.
     
//  MSKVersionInfo  vDoc("DOCTABLE");
//  // String parameter gives name of DLL to get info from 
//  // N.B. Win16: Any DLL in system.
//  //      Win32: Only DLLs in app's memory space. 
//
//  // Note that object creation is a "costly" operation
//  // hence you code should be structured to avoid repeatly 
//  // constructing & destroying MSKVersionInfo objects.
//
//  // Also note that an object is bound to a DLL at construction, 
//  // and there are no copy or assignment operators. Hence an
//  // object can't be "reused".
//
//  cout << ver.ProductName() << " version " << ver.ProductVersion() << endl;
//
//
     
class MSKVersionInfo
{
public:
    MSKVersionInfo(LPCTSTR pModName = NULL); 
    ~MSKVersionInfo();
     
    CString Comments()        {return GetValue("Comments");} 
    CString CompanyName()     {return GetValue("CompanyName");} 
    CString FileDescription() {return GetValue("FileDescription");} 
    CString FileVersion()     {return GetValue("FileVersion");} 
    CString InternalName()    {return GetValue("InternalName");}
    CString OriginalFilename(){return GetValue("OriginalFilename");} 
    CString ProductName()     {return GetValue("ProductName");} 
    CString ProductVersion()  {return GetValue("ProductVersion");} 
    CString SpecialBuild()    {return GetValue("SpecialBuild");} 
    CString LegalCopyright()  {return GetValue("LegalCopyright");} 
    CString LegalTrademarks() {return GetValue("LegalTrademarks");} 
    CString PrivateBuild()    {return GetValue("PrivateBuild");}
     
    CString GetValue(CString const sSubBlock);
     
private:
    CString     m_sSubBlockHead;
    void *      m_VerData;
     
    MSKVersionInfo(const MSKVersionInfo &);     // private copy ctor (can't copy)
    operator=(const MSKVersionInfo &);      // private op=  (can't assign)
};
