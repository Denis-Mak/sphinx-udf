//
// $Id$
//

#include "sphinxudf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// UDF version control
///// gets called once when the library is loaded

DLLEXPORT int udf_log_ver ()
{ 
	return SPH_UDF_VERSION;
}

DLLEXPORT void udf_log_reinit ()
{
}

DLLEXPORT int udf_log_init (SPH_UDF_INIT * init, SPH_UDF_ARGS * args, char * error_message)
{
	// check that all input params are integer	
	int i;	
	for (i = 0; i < args->arg_count; i++){
		if (args->arg_types[i] != SPH_UDF_TYPE_UINT32)
		{
			snprintf ( error_message, SPH_UDF_ERROR_LEN, "Argument %d is not int", i);	
			return 1;
		}
	}
	//reserve memory for file pointer and some other data that we get on init (in this case qty of params)	
	init->func_data = (void*)malloc(sizeof(int)+sizeof(FILE*));
	int params_count = args->arg_count;
	*(int*)init->func_data = params_count;
	FILE* stream;
	stream = fopen("/var/log/sphinx/udf.log", "w");
	*(FILE**)(init->func_data + sizeof(int*)) = stream;

	return 0;
}

DLLEXPORT void udf_log_deinit( SPH_UDF_INIT * init )
{
	//dealocate memory
	if (init->func_data)
	{
		fclose(*(FILE**)(init->func_data + sizeof(int*)));
		free (init->func_data);
		init->func_data = NULL; 
	}
}

DLLEXPORT int udf_log(SPH_UDF_INIT * init, SPH_UDF_ARGS * args, char * error_message)
{
	SPH_UDF_FACTORS factors;
	sphinx_factors_init(&factors);
	sphinx_factors_unpack((unsigned int *)args->arg_values[0], &factors);
	// Print param values
	int i;
	for (i = 0; i < *((int *)init->func_data); i++){
		fprintf(*(FILE**)(init->func_data + sizeof(int*)), "Param nm = %d, Param val = %d \n", i, *(int*)args->arg_values[i]);
	}
	sphinx_factors_deinit(&factors);

	return 0;
}

//
// $Id$
//
