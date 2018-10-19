#ifndef __ASMH_
#define __ASMH_

#include "ASMTypes.h"

#ifdef _WIN32
#ifdef BUILD_DLL
#define ASM_API __declspec(dllexport)
#endif
#endif

#ifndef ASM_API
#define ASM_API __declspec(dllimport)
#endif

#define ASM_FUNC extern "C" ASM_API
#define ASM_NULL 0


/*!
* \brief Error codes returned by ASM Plugin API.
* Authenticator specific error codes are returned in JSON form.
* See JSON schemas for more details.
*/
enum asmResult_t
{
	Success = 0,    /* Success */
	Failure         /* Generic failure */
};

/*!
* \brief Generic structure containing JSON string in UTF-8 format.
* This structure is used throughout functions to pass and receives JSON data.
*/
typedef struct _asmJSONData_t
{
	int length;        /* JSON data length */
	char* pData;    /* JSON data */
} asmJSONData_t;

/*!
* \brief Enumeration event types for authenticators.
* These events will be fired when an authenticator becomes
* available (plugged) or unavailable (unplugged).
*/
enum asmEnumerationType_t
{
	Plugged = 0,    /* Indicates that authenticator Plugged to system */
	Unplugged,      /* Indicates that authenticator Unplugged from system */
	UnknownEventType
};

namespace ASM
{
	/*! \brief Callback listener.
	FIDO UAF Client must pass an object implementating this interface to
	Authenticator::Process function. This interface is used to provide
	ASM JSON based response data.*/

	class ICallback
	{
	public:
		virtual ~ICallback() {}

		/**
		* This function is called when ASM's response is ready.
		* @param response JSON based event data
		* @param exchangeData must be provided by ASM if it needs some
		* data back right after calling the callback function.
		* The lifecycle of this parameter must be managed by ASM. ASM must
		* allocate enough memory for getting the data back.
		*/
		virtual void Callback(const asmJSONData_t &response, asmJSONData_t &exchangeData) = 0;
	};

	/*!
	* \brief Authenticator Enumerator.
	* FIDO UAF Client must provide an object implementing this interface.
	* It will be invoked when a new authenticator is plugged or
	* when an authenticator has been unplugged.
	*/
	class IEnumerator
	{
	public:
		virtual ~IEnumerator() {}

		/**DLL_THREAD_ATTACH
		* This function is called when an authenticator is plugged or unplugged.
		* @param eventType event type (plugged/unplugged)
		* @param AuthenticatorInfo JSON based GetInfoResponse object
		*/
		virtual void Notify(const asmEnumerationType_t eventType, const asmJSONData_t &AuthenticatorInfo) = 0;
	};
}

#ifdef BUILD_DLL

/**
* Initializes ASM plugin. This is the first function to be called.
* @param pEnumerationListener caller provided Enumerator
*/
ASM_FUNC asmResult_t asmInit(void);

/**
* Process given JSON request and returns JSON response.
* If the caller wants to execute a function defined in ASM JSON
* schema then this is the function that must be called.
* @param pInData input JSON data
* @param pListener event listener for receiving events from ASM
*/
ASM_FUNC asmResult_t asmProcess(const asmJSONData_t *pInData, ASM::ICallback *pListener);

/**
* Unitializes ASM plugin.
*/
ASM_FUNC asmResult_t asmUninit(void);

#else // BUILD_DLL
typedef asmResult_t(*pAsmInit)(ASM::IEnumerator*);
typedef asmResult_t(*pAsmProcess)(const asmJSONData_t*, ASM::ICallback*);
typedef asmResult_t(*pAsmUninit)(void);
pAsmInit    asmInit = ASM_NULL;
pAsmProcess asmProcess = ASM_NULL;
pAsmUninit  asmUninit = ASM_NULL;

#endif // BUILD_DLL

#endif // __ASMH_

