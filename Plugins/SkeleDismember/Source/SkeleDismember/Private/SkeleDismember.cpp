// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkeleDismember.h"

#define LOCTEXT_NAMESPACE "FSkeleDismemberModule"

void FSkeleDismemberModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSkeleDismemberModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkeleDismemberModule, SkeleDismember)