// RegisterByCategory.h

#ifndef REGISTERBYCATEGORY_H
#define REGISTERBYCATEGORY_H

enum REGISTRATION_CATEGORY
	{
		MIC_ISO,
		MIC_CSO
	};

static ICatInformation*  g_pICatInformation;
static ICatRegister*     g_pICatRegister;

HRESULT RegisterThisComponentWithItsCategory(
	REGISTRATION_CATEGORY cat, 
	GUID myCLSID);

HRESULT UnRegisterThisComponentFromItsCategory(		
	REGISTRATION_CATEGORY cat, 
	GUID myCLSID);

HRESULT Initialize(void);

HRESULT TestRegistrationOfCategory(
	REGISTRATION_CATEGORY cat, 
	bool *psuccess);

HRESULT RegisterCategory(
	REGISTRATION_CATEGORY cat,
	bool doit);  //register or unregister

HRESULT RegisterWithPreExistingCategory(
	REGISTRATION_CATEGORY cat, 
	GUID myCLSID,
	bool doit);

#endif //REGISTERBYCATEGORY_H