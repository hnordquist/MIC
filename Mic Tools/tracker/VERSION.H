//$History: VERSION.H $
 * 
 * *****************  Version 3  *****************
 * User: 186846       Date: 9/13/10    Time: 8:31a
 * Updated in $/TRACKER
// * 
// * *****************  Version 2  *****************
// * User: Longo        Date: 1/06/10    Time: 3:24p
// * Updated in $/TRACKER
// * 1.9.0.0 VS 2008 changes, verison block changed. 
// * Not needed to be deployed but it's ready anyway
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 3:51p
// * Created in $/TRACKER
// * At or near retirement
// 
// *****************  Version 2  *****************
// User: Pelowitz     Date: 4/26/05    Time: 1:50p
// Updated in $/Tracker
// Added version to title bar, added VERSION class to get version from the
// resources.
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 4/26/05    Time: 1:29p
// Created in $/Tracker
// 

// version.h
///////////////////////////////////////////////////////////////////////////
//
//  Class CVersionInfo
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
     
//  CVersionInfo objects have a member function for each of the above elements
//  returned as CString objects.
//
//  Values are only retrieved from the VERSIONINFO structure; they cannot be
//  written to.
//
//  If new elements are added to VERSIONINFO structure in the future, they can
//  be retieved using the GetValue() member function. 
//
 
class CVersionInfo
{
public:
    CVersionInfo(); 
    ~CVersionInfo();
     
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
     
};
