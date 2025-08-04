#pragma once
#ifdef PIPELINE_CORE_EXPORTS
    #define PIPELINE_CORE_API __declspec(dllexport)
#else
    #define PIPELINE_CORE_API /*__declspec(dllimport)*/
#endif

