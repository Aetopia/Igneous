#include "Igneous.h"

HRESULT Igneous_App_Launch(App *This, PDWORD pdwProcessId)
{
    return IApplicationActivationManager_ActivateApplication(Igneous_App_Manager, This->Id, NULL, AO_NOERRORUI,
                                                             pdwProcessId);
}

HRESULT Igneous_App_Terminate(App *This)
{
    WCHAR szName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(This->Name, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){}, &(UINT32){ARRAYSIZE(szName)},
                               szName);
    return IPackageDebugSettings_TerminateAllProcesses(Igneous_App_Settings, szName);
}