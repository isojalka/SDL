/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#undef STUB

#if defined(GENERATE_STUBS)
#define STUB(name) extern int name##_REAL(); int __saveds LIB_##name() { return name##_REAL(); }
#elif defined(GENERATE_POINTERS)
#define STUB(name) (APTR)&LIB_##name ,
#else
#define STUB(name) extern int LIB_##name();
#endif

	STUB(SDL_GetPlatform)

	STUB(SDL_Init)
	STUB(SDL_InitSubSystem)
	STUB(SDL_QuitSubSystem)
	STUB(SDL_WasInit)
	STUB(SDL_Quit)
	STUB(SDL_SetMainReady)

	STUB(SDL_malloc)
	STUB(SDL_calloc)
	STUB(SDL_realloc)
	STUB(SDL_free)
	STUB(SDL_qsort)
	STUB(SDL_abs)

	STUB(SDL_isdigit)
	STUB(SDL_isspace)
	STUB(SDL_toupper)
	STUB(SDL_tolower)

	STUB(SDL_memset)
	STUB(SDL_memcpy)
	STUB(SDL_memmove)
	STUB(SDL_memcmp)

	STUB(SDL_wcslen)
	STUB(SDL_wcslcpy)
	STUB(SDL_wcslcat)

	STUB(SDL_strlen)
	STUB(SDL_strlcpy)
	STUB(SDL_utf8strlcpy)
	STUB(SDL_strlcat)
	STUB(SDL_strdup)
	STUB(SDL_strrev)
	STUB(SDL_strupr)
	STUB(SDL_strlwr)
	STUB(SDL_strchr)
	STUB(SDL_strrchr)
	STUB(SDL_strstr)

	STUB(SDL_itoa)
	STUB(SDL_uitoa)
	STUB(SDL_ltoa)
	STUB(SDL_ultoa)
	STUB(SDL_lltoa)
	STUB(SDL_ulltoa)

	STUB(SDL_atoi)
	STUB(SDL_atof)
	STUB(SDL_strtol)
	STUB(SDL_strtoul)
	STUB(SDL_strtoll)
	STUB(SDL_strtoull)
	STUB(SDL_strtod)

	STUB(SDL_strcmp)
	STUB(SDL_strncmp)
	STUB(SDL_strcasecmp)
	STUB(SDL_strncasecmp)

#if 0
#warning Following calls are available in link lib only
#warning sscanf(...)
#warning snprintf(...)
#warning SDL_SetError(const char *fmt, ...)
#warning SDL_LogMessage(int category, SDL_LogPriority priority, const char *fmt, ...)
#warning SDL_Log(const char *fmt, ...)
#warning SDL_LogVerbose(int category, const char *fmt, ...)
#warning SDL_LogDebug(int category, const char *fmt, ...)
#warning SDL_LogInfo(int category, const char *fmt, ...)
#warning SDL_LogWarn(int category, const char *fmt, ...)
#warning SDL_LogError(int category, const char *fmt, ...)
#warning SDL_LogCritical(int category, const char *fmt, ...)
	STUB(SDL_SetError(const char *fmt, ...);
#endif

	STUB(SDL_vsscanf)
	STUB(SDL_vsnprintf)
	STUB(SDL_acos)
	STUB(SDL_asin)
	STUB(SDL_atan)
	STUB(SDL_atan2)
	STUB(SDL_ceil)
	STUB(SDL_copysign)
	STUB(SDL_cos)
	STUB(SDL_cosf)
	STUB(SDL_fabs)
	STUB(SDL_floor)
	STUB(SDL_log)
	STUB(SDL_pow)
	STUB(SDL_scalbn)
	STUB(SDL_sin)
	STUB(SDL_sinf)
	STUB(SDL_sqrt)
	STUB(SDL_iconv_open)
	STUB(SDL_iconv_close)
	STUB(SDL_iconv)
	STUB(SDL_iconv_string)
	STUB(SDL_ReportAssertion)
	STUB(SDL_SetAssertionHandler)
	STUB(SDL_GetDefaultAssertionHandler)
	STUB(SDL_GetAssertionHandler)
	STUB(SDL_GetAssertionReport)
	STUB(SDL_ResetAssertionReport)
	STUB(SDL_AtomicTryLock)
	STUB(SDL_AtomicLock)
	STUB(SDL_AtomicUnlock)
	STUB(SDL_AtomicCAS)
	STUB(SDL_AtomicSet)
	STUB(SDL_AtomicGet)
	STUB(SDL_AtomicAdd)
	STUB(SDL_AtomicCASPtr)
	STUB(SDL_AtomicSetPtr)
	STUB(SDL_AtomicGetPtr)
	STUB(SDL_GetNumAudioDrivers)
	STUB(SDL_GetAudioDriver)
	STUB(SDL_AudioInit)
	STUB(SDL_AudioQuit)
	STUB(SDL_GetCurrentAudioDriver)
	STUB(SDL_OpenAudio)
	STUB(SDL_GetNumAudioDevices)
	STUB(SDL_GetAudioDeviceName)
	STUB(SDL_OpenAudioDevice)
	STUB(SDL_GetAudioStatus)
	STUB(SDL_GetAudioDeviceStatus)
	STUB(SDL_PauseAudio)
	STUB(SDL_PauseAudioDevice)
	STUB(SDL_LoadWAV_RW)
	STUB(SDL_FreeWAV)
	STUB(SDL_BuildAudioCVT)
	STUB(SDL_ConvertAudio)
	STUB(SDL_MixAudio)
	STUB(SDL_MixAudioFormat)
	STUB(SDL_LockAudio)
	STUB(SDL_LockAudioDevice)
	STUB(SDL_UnlockAudio)
	STUB(SDL_UnlockAudioDevice)
	STUB(SDL_CloseAudio)
	STUB(SDL_CloseAudioDevice)
	STUB(SDL_SetClipboardText)
	STUB(SDL_GetClipboardText)
	STUB(SDL_HasClipboardText)
	STUB(SDL_GetCPUCount)
	STUB(SDL_GetCPUCacheLineSize)
	STUB(SDL_HasRDTSC)
	STUB(SDL_HasAltiVec)
	STUB(SDL_HasMMX)
	STUB(SDL_Has3DNow)
	STUB(SDL_HasSSE)
	STUB(SDL_HasSSE2)
	STUB(SDL_HasSSE3)
	STUB(SDL_HasSSE41)
	STUB(SDL_HasSSE42)
	STUB(SDL_HasAVX)
	STUB(SDL_GetSystemRAM)
	STUB(SDL_GetError)
	STUB(SDL_ClearError)
	STUB(SDL_Error)
	STUB(SDL_PumpEvents)
	STUB(SDL_PeepEvents)
	STUB(SDL_HasEvent)
	STUB(SDL_HasEvents)
	STUB(SDL_FlushEvent)
	STUB(SDL_FlushEvents)
	STUB(SDL_PollEvent)
	STUB(SDL_WaitEvent)
	STUB(SDL_WaitEventTimeout)
	STUB(SDL_PushEvent)
	STUB(SDL_SetEventFilter)
	STUB(SDL_GetEventFilter)
	STUB(SDL_AddEventWatch)
	STUB(SDL_DelEventWatch)
	STUB(SDL_FilterEvents)
	STUB(SDL_EventState)
	STUB(SDL_RegisterEvents)
	STUB(SDL_GetBasePath)
	STUB(SDL_GetPrefPath)
	STUB(SDL_NumJoysticks)
	STUB(SDL_JoystickNameForIndex)
	STUB(SDL_JoystickOpen)
	STUB(SDL_JoystickName)
	STUB(SDL_JoystickGetDeviceGUID)
	STUB(SDL_JoystickGetGUID)
	STUB(SDL_JoystickGetGUIDString)
	STUB(SDL_JoystickGetGUIDFromString)
	STUB(SDL_JoystickGetAttached)
	STUB(SDL_JoystickInstanceID)
	STUB(SDL_JoystickNumAxes)
	STUB(SDL_JoystickNumBalls)
	STUB(SDL_JoystickNumHats)
	STUB(SDL_JoystickNumButtons)
	STUB(SDL_JoystickUpdate)
	STUB(SDL_JoystickEventState)
	STUB(SDL_JoystickGetAxis)
	STUB(SDL_JoystickGetHat)
	STUB(SDL_JoystickGetBall)
	STUB(SDL_JoystickGetButton)
	STUB(SDL_JoystickClose)
	STUB(SDL_GameControllerAddMappingsFromRW)
	STUB(SDL_GameControllerAddMapping)
	STUB(SDL_GameControllerMappingForGUID)
	STUB(SDL_GameControllerMapping)
	STUB(SDL_IsGameController)
	STUB(SDL_GameControllerNameForIndex)
	STUB(SDL_GameControllerOpen)
	STUB(SDL_GameControllerName)
	STUB(SDL_GameControllerGetAttached)
	STUB(SDL_GameControllerGetJoystick)
	STUB(SDL_GameControllerEventState)
	STUB(SDL_GameControllerUpdate)
	STUB(SDL_GameControllerGetAxisFromString)
	STUB(SDL_GameControllerGetStringForAxis)
	STUB(SDL_GameControllerGetBindForAxis)
	STUB(SDL_GameControllerGetAxis)
	STUB(SDL_GameControllerGetButtonFromString)
	STUB(SDL_GameControllerGetStringForButton)
	STUB(SDL_GameControllerGetBindForButton)
	STUB(SDL_GameControllerGetButton)
	STUB(SDL_GameControllerClose)

	STUB(SDL_SetHintWithPriority)
	STUB(SDL_SetHint)
	STUB(SDL_GetHint)
	STUB(SDL_AddHintCallback)
	STUB(SDL_DelHintCallback)
	STUB(SDL_ClearHints)
	STUB(SDL_LoadObject)
	STUB(SDL_LoadFunction)
	STUB(SDL_UnloadObject)
	STUB(SDL_LogSetAllPriority)
	STUB(SDL_LogSetPriority)
	STUB(SDL_LogGetPriority)
	STUB(SDL_LogResetPriorities)
	STUB(SDL_LogMessageV)
	STUB(SDL_LogGetOutputFunction)
	STUB(SDL_LogSetOutputFunction)
	STUB(SDL_ShowMessageBox)
	STUB(SDL_ShowSimpleMessageBox)
	STUB(SDL_CreateMutex)
	STUB(SDL_LockMutex)
	STUB(SDL_TryLockMutex)
	STUB(SDL_UnlockMutex)
	STUB(SDL_DestroyMutex)
	STUB(SDL_CreateSemaphore)
	STUB(SDL_DestroySemaphore)
	STUB(SDL_SemWait)
	STUB(SDL_SemTryWait)
	STUB(SDL_SemWaitTimeout)
	STUB(SDL_SemPost)
	STUB(SDL_SemValue)
	STUB(SDL_CreateCond)
	STUB(SDL_DestroyCond)
	STUB(SDL_CondSignal)
	STUB(SDL_CondBroadcast)
	STUB(SDL_CondWait)
	STUB(SDL_CondWaitTimeout)
	STUB(SDL_GetPowerInfo)
	STUB(SDL_GetNumRenderDrivers)
	STUB(SDL_GetRenderDriverInfo)
	STUB(SDL_CreateWindowAndRenderer)
	STUB(SDL_CreateRenderer)
	STUB(SDL_CreateSoftwareRenderer)
	STUB(SDL_GetRenderer)
	STUB(SDL_GetRendererInfo)
	STUB(SDL_GetRendererOutputSize)
	STUB(SDL_CreateTexture)
	STUB(SDL_CreateTextureFromSurface)
	STUB(SDL_QueryTexture)
	STUB(SDL_SetTextureColorMod)
	STUB(SDL_GetTextureColorMod)
	STUB(SDL_SetTextureAlphaMod)
	STUB(SDL_GetTextureAlphaMod)
	STUB(SDL_SetTextureBlendMode)
	STUB(SDL_GetTextureBlendMode)
	STUB(SDL_UpdateTexture)
	STUB(SDL_UpdateYUVTexture)
	STUB(SDL_LockTexture)
	STUB(SDL_UnlockTexture)
	STUB(SDL_RenderTargetSupported)
	STUB(SDL_SetRenderTarget)
	STUB(SDL_GetRenderTarget)
	STUB(SDL_RenderSetLogicalSize)
	STUB(SDL_RenderGetLogicalSize)
	STUB(SDL_RenderSetViewport)
	STUB(SDL_RenderGetViewport)
	STUB(SDL_RenderSetClipRect)
	STUB(SDL_RenderGetClipRect)
	STUB(SDL_RenderSetScale)
	STUB(SDL_RenderGetScale)
	STUB(SDL_SetRenderDrawColor)
	STUB(SDL_GetRenderDrawColor)
	STUB(SDL_SetRenderDrawBlendMode)
	STUB(SDL_GetRenderDrawBlendMode)
	STUB(SDL_RenderClear)
	STUB(SDL_RenderDrawPoint)
	STUB(SDL_RenderDrawPoints)
	STUB(SDL_RenderDrawLine)
	STUB(SDL_RenderDrawLines)
	STUB(SDL_RenderDrawRect)
	STUB(SDL_RenderDrawRects)
	STUB(SDL_RenderFillRect)
	STUB(SDL_RenderFillRects)
	STUB(SDL_RenderCopy)
	STUB(SDL_RenderCopyEx)
	STUB(SDL_RenderReadPixels)
	STUB(SDL_RenderPresent)
	STUB(SDL_DestroyTexture)
	STUB(SDL_DestroyRenderer)
	STUB(SDL_GL_BindTexture)
	STUB(SDL_GL_UnbindTexture)
	STUB(SDL_RWFromFile)
	STUB(SDL_RWFromFP_clib)
	STUB(SDL_RWFromMem)
	STUB(SDL_RWFromConstMem)
	STUB(SDL_AllocRW)
	STUB(SDL_FreeRW)
	STUB(SDL_ReadU8)
	STUB(SDL_ReadLE16)
	STUB(SDL_ReadBE16)
	STUB(SDL_ReadLE32)
	STUB(SDL_ReadBE32)
	STUB(SDL_ReadLE64)
	STUB(SDL_ReadBE64)
	STUB(SDL_WriteU8)
	STUB(SDL_WriteLE16)
	STUB(SDL_WriteBE16)
	STUB(SDL_WriteLE32)
	STUB(SDL_WriteBE32)
	STUB(SDL_WriteLE64)
	STUB(SDL_WriteBE64)
	STUB(SDL_CreateThread)
	STUB(SDL_GetThreadName)
	STUB(SDL_ThreadID)
	STUB(SDL_GetThreadID)
	STUB(SDL_SetThreadPriority)
	STUB(SDL_WaitThread)
	STUB(SDL_DetachThread)
	STUB(SDL_TLSCreate)
	STUB(SDL_TLSGet)
	STUB(SDL_TLSSet)
	STUB(SDL_GetTicks)
	STUB(SDL_GetPerformanceCounter)
	STUB(SDL_GetPerformanceFrequency)
	STUB(SDL_Delay)
	STUB(SDL_AddTimer)
	STUB(SDL_RemoveTimer)
	STUB(SDL_GetVersion)
	STUB(SDL_GetRevision)
	STUB(SDL_GetRevisionNumber)
	STUB(SDL_GetNumVideoDrivers)
	STUB(SDL_GetVideoDriver)
	STUB(SDL_VideoInit)
	STUB(SDL_VideoQuit)
	STUB(SDL_GetCurrentVideoDriver)
	STUB(SDL_GetNumVideoDisplays)
	STUB(SDL_GetDisplayName)
	STUB(SDL_GetDisplayBounds)
	STUB(SDL_GetNumDisplayModes)
	STUB(SDL_GetDisplayMode)
	STUB(SDL_GetDesktopDisplayMode)
	STUB(SDL_GetCurrentDisplayMode)
	STUB(SDL_GetClosestDisplayMode)
	STUB(SDL_GetWindowDisplayIndex)
	STUB(SDL_SetWindowDisplayMode)
	STUB(SDL_GetWindowDisplayMode)
	STUB(SDL_GetWindowPixelFormat)
	STUB(SDL_CreateWindow)
	STUB(SDL_CreateWindowFrom)
	STUB(SDL_GetWindowID)
	STUB(SDL_GetWindowFromID)
	STUB(SDL_GetWindowFlags)
	STUB(SDL_SetWindowTitle)
	STUB(SDL_GetWindowTitle)
	STUB(SDL_SetWindowIcon)
	STUB(SDL_SetWindowData)
	STUB(SDL_GetWindowData)
	STUB(SDL_SetWindowPosition)
	STUB(SDL_GetWindowPosition)
	STUB(SDL_SetWindowSize)
	STUB(SDL_GetWindowSize)
	STUB(SDL_SetWindowMinimumSize)
	STUB(SDL_GetWindowMinimumSize)
	STUB(SDL_SetWindowMaximumSize)
	STUB(SDL_GetWindowMaximumSize)
	STUB(SDL_SetWindowBordered)
	STUB(SDL_ShowWindow)
	STUB(SDL_HideWindow)
	STUB(SDL_RaiseWindow)
	STUB(SDL_MaximizeWindow)
	STUB(SDL_MinimizeWindow)
	STUB(SDL_RestoreWindow)
	STUB(SDL_SetWindowFullscreen)
	STUB(SDL_GetWindowSurface)
	STUB(SDL_UpdateWindowSurface)
	STUB(SDL_UpdateWindowSurfaceRects)
	STUB(SDL_SetWindowGrab)
	STUB(SDL_GetWindowGrab)
	STUB(SDL_SetWindowBrightness)
	STUB(SDL_GetWindowBrightness)
	STUB(SDL_SetWindowGammaRamp)
	STUB(SDL_GetWindowGammaRamp)
	STUB(SDL_DestroyWindow)
	STUB(SDL_IsScreenSaverEnabled)
	STUB(SDL_EnableScreenSaver)
	STUB(SDL_DisableScreenSaver)

	STUB(SDL_GetKeyboardFocus)
	STUB(SDL_GetKeyboardState)
	STUB(SDL_GetModState)
	STUB(SDL_SetModState)
	STUB(SDL_GetKeyFromScancode)
	STUB(SDL_GetScancodeFromKey)
	STUB(SDL_GetScancodeName)
	STUB(SDL_GetScancodeFromName)
	STUB(SDL_GetKeyName)
	STUB(SDL_GetKeyFromName)
	STUB(SDL_StartTextInput)
	STUB(SDL_IsTextInputActive)
	STUB(SDL_StopTextInput)
	STUB(SDL_SetTextInputRect)
	STUB(SDL_HasScreenKeyboardSupport)
	STUB(SDL_IsScreenKeyboardShown)

	STUB(SDL_CreateRGBSurface)
#if defined(GENERATE_STUBS)
	extern void *SDL_CreateRGBSurfaceFrom_REAL(void *a, int b, int c, int d, int e, int f, int g, int h, int i);
	void * __saveds LIB_SDL_CreateRGBSurfaceFrom(void *a, int b, int c, int d, int e, int f, int g, int h, int i)
	{
		return SDL_CreateRGBSurfaceFrom_REAL(a, b, c, d, e, f, g, h, i);
	}
#else
	STUB(SDL_CreateRGBSurfaceFrom)
#endif
	STUB(SDL_FreeSurface)
	STUB(SDL_SetSurfacePalette)
	STUB(SDL_LockSurface)
	STUB(SDL_UnlockSurface)
	STUB(SDL_LoadBMP_RW)
	STUB(SDL_SaveBMP_RW)
	STUB(SDL_SetSurfaceRLE)
	STUB(SDL_SetColorKey)
	STUB(SDL_GetColorKey)
	STUB(SDL_SetSurfaceColorMod)
	STUB(SDL_GetSurfaceColorMod)
	STUB(SDL_SetSurfaceAlphaMod)
	STUB(SDL_GetSurfaceAlphaMod)
	STUB(SDL_SetSurfaceBlendMode)
	STUB(SDL_GetSurfaceBlendMode)
	STUB(SDL_SetClipRect)
	STUB(SDL_GetClipRect)
	STUB(SDL_ConvertSurface)
	STUB(SDL_ConvertSurfaceFormat)
	STUB(SDL_ConvertPixels)
	STUB(SDL_FillRect)
	STUB(SDL_FillRects)
	STUB(SDL_UpperBlit)
	STUB(SDL_LowerBlit)
	STUB(SDL_SoftStretch)
	STUB(SDL_UpperBlitScaled)
	STUB(SDL_LowerBlitScaled)

	STUB(SDL_GetMouseFocus)
	STUB(SDL_GetMouseState)
	STUB(SDL_GetRelativeMouseState)
	STUB(SDL_WarpMouseInWindow)
	STUB(SDL_SetRelativeMouseMode)
	STUB(SDL_GetRelativeMouseMode)
	STUB(SDL_CreateCursor)
	STUB(SDL_CreateColorCursor)
	STUB(SDL_CreateSystemCursor)
	STUB(SDL_SetCursor)
	STUB(SDL_GetCursor)
	STUB(SDL_GetDefaultCursor)
	STUB(SDL_FreeCursor)
	STUB(SDL_ShowCursor)

	STUB(SDL_GetPixelFormatName)
	STUB(SDL_PixelFormatEnumToMasks)
	STUB(SDL_MasksToPixelFormatEnum)
	STUB(SDL_AllocFormat)
	STUB(SDL_FreeFormat)
	STUB(SDL_AllocPalette)
	STUB(SDL_SetPixelFormatPalette)
	STUB(SDL_SetPaletteColors)
	STUB(SDL_FreePalette)
	STUB(SDL_MapRGB)
	STUB(SDL_MapRGBA)
	STUB(SDL_GetRGB)
	STUB(SDL_GetRGBA)
	STUB(SDL_CalculateGammaRamp)

	STUB(SDL_HasIntersection)
	STUB(SDL_IntersectRect)
	STUB(SDL_UnionRect)
	STUB(SDL_EnclosePoints)
	STUB(SDL_IntersectRectAndLine)

	STUB(SDL_GetWindowWMInfo)

	STUB(SDL_RecordGesture)
	STUB(SDL_SaveAllDollarTemplates)
	STUB(SDL_SaveDollarTemplate)
	STUB(SDL_LoadDollarTemplates)

	STUB(SDL_CreateShapedWindow)
	STUB(SDL_IsShapedWindow)
	STUB(SDL_SetWindowShape)
	STUB(SDL_GetShapedWindowMode)

	STUB(SDL_CaptureMouse)
	STUB(SDL_WarpMouseGlobal)
	STUB(SDL_GetGlobalMouseState)
	STUB(SDL_SetWindowHitTest)
	STUB(SDL_GetGrabbedWindow)
	STUB(SDL_GetDisplayDPI)
	STUB(SDL_RenderIsClipEnabled)
	STUB(SDL_GameControllerFromInstanceID)
	STUB(SDL_JoystickCurrentPowerLevel)
	STUB(SDL_JoystickFromInstanceID)
	STUB(SDL_HasAVX2)
	STUB(SDL_QueueAudio)
	STUB(SDL_GetQueuedAudioSize)
	STUB(SDL_ClearQueuedAudio)
	STUB(SDL_sqrtf)
	STUB(SDL_tan)
	STUB(SDL_tanf)

	STUB(SDL_MemoryBarrierReleaseFunction)
	STUB(SDL_MemoryBarrierAcquireFunction)
	STUB(SDL_DequeueAudio)
	STUB(SDL_ComposeCustomBlendMode)
	STUB(SDL_HasNEON)
	STUB(SDL_GameControllerNumMappings)
	STUB(SDL_GameControllerMappingForIndex)
	STUB(SDL_GameControllerGetVendor)
	STUB(SDL_GameControllerGetProduct)
	STUB(SDL_GameControllerGetProductVersion)
	STUB(SDL_GetHintBoolean)
	STUB(SDL_JoystickGetDeviceVendor)
	STUB(SDL_JoystickGetDeviceProduct)
	STUB(SDL_JoystickGetDeviceProductVersion)
	STUB(SDL_JoystickGetDeviceType)
	STUB(SDL_JoystickGetDeviceInstanceID)
	STUB(SDL_JoystickGetVendor)
	STUB(SDL_JoystickGetProduct)
	STUB(SDL_JoystickGetProductVersion)
	STUB(SDL_JoystickGetType)
	STUB(SDL_JoystickGetAxisInitialState)
	STUB(SDL_GetDisplayUsableBounds)
	STUB(SDL_GetWindowBordersSize)
	STUB(SDL_SetWindowResizable)
	STUB(SDL_SetWindowOpacity)
	STUB(SDL_GetWindowOpacity)
	STUB(SDL_SetWindowModalFor)
	STUB(SDL_SetWindowInputFocus)
	STUB(SDL_CreateRGBSurfaceWithFormat)
	STUB(SDL_CreateRGBSurfaceWithFormatFrom)
	STUB(SDL_DuplicateSurface)
	STUB(SDL_wcscmp)
	STUB(SDL_utf8strlen)
	STUB(SDL_LoadFile_RW)
	STUB(SDL_RenderSetIntegerScale)
	STUB(SDL_RenderGetIntegerScale)
	
	STUB(SDL_NewAudioStream)
	STUB(SDL_AudioStreamPut)
	STUB(SDL_AudioStreamGet)
	STUB(SDL_AudioStreamAvailable)
	STUB(SDL_AudioStreamFlush)
	STUB(SDL_AudioStreamClear)
	STUB(SDL_FreeAudioStream)
	STUB(SDL_LockJoysticks)
	STUB(SDL_UnlockJoysticks)
	
	STUB(SDL_GetMemoryFunctions)
	STUB(SDL_SetMemoryFunctions)
	STUB(SDL_GetNumAllocations)
	
	STUB(SDL_RenderGetMetalLayer)
	STUB(SDL_RenderGetMetalCommandEncoder)
	STUB(SDL_acosf)
	STUB(SDL_asinf)
	STUB(SDL_atanf)
	STUB(SDL_atan2f)
	STUB(SDL_ceilf)
	STUB(SDL_copysignf)
	STUB(SDL_fabsf)
	STUB(SDL_floorf)
	STUB(SDL_fmod)
	STUB(SDL_fmodf)
	STUB(SDL_logf)
	STUB(SDL_log10)
	STUB(SDL_log10f)
	STUB(SDL_powf)
	STUB(SDL_scalbnf)
	STUB(SDL_SetYUVConversionMode)
	STUB(SDL_GetYUVConversionMode)
	STUB(SDL_GetYUVConversionModeForResolution)
	
	STUB(SDL_HasAVX512F)
	STUB(SDL_GameControllerMappingForDeviceIndex)
	STUB(SDL_GameControllerGetPlayerIndex)
	STUB(SDL_GameControllerRumble)
	STUB(SDL_JoystickGetDevicePlayerIndex)
	STUB(SDL_JoystickGetPlayerIndex)
	STUB(SDL_JoystickRumble)
	STUB(SDL_wcsdup)
	STUB(SDL_exp)
	STUB(SDL_expf)
	STUB(SDL_HasColorKey)
	STUB(SDL_IsTablet)
	STUB(SDL_CreateThreadWithStackSize)
	STUB(SDL_GetDisplayOrientation)
	
	STUB(SDL_SIMDGetAlignment)
	STUB(SDL_SIMDAlloc)
	STUB(SDL_SIMDFree)
	STUB(SDL_RenderDrawPointF)
	STUB(SDL_RenderDrawPointsF)
	STUB(SDL_RenderDrawLineF)
	STUB(SDL_RenderDrawLinesF)
	STUB(SDL_RenderDrawRectF)
	STUB(SDL_RenderDrawRectsF)
	STUB(SDL_RenderFillRectF)
	STUB(SDL_RenderFillRectsF)
	STUB(SDL_RenderCopyF)
	STUB(SDL_RenderCopyExF)
	STUB(SDL_RenderFlush)
	STUB(SDL_RWsize)
	STUB(SDL_RWseek)
	STUB(SDL_RWtell)
	STUB(SDL_RWread)
	STUB(SDL_RWwrite)
	STUB(SDL_RWclose)
	STUB(SDL_LoadFile)
	STUB(SDL_GetTouchDeviceType)

	STUB(SDL_HasARMSIMD)
	STUB(SDL_GameControllerTypeForIndex)
	STUB(SDL_GameControllerFromPlayerIndex)
	STUB(SDL_GameControllerGetType)
	STUB(SDL_GameControllerSetPlayerIndex)
	STUB(SDL_JoystickFromPlayerIndex)
	STUB(SDL_JoystickSetPlayerIndex)
	STUB(SDL_SetTextureScaleMode)
	STUB(SDL_GetTextureScaleMode)
	STUB(SDL_LockTextureToSurface)
	STUB(SDL_wcsstr)
	STUB(SDL_wcsncmp)
	STUB(SDL_strtokr)

	STUB(SDL_GL_LoadLibrary)
	STUB(SDL_GL_GetProcAddress)
	STUB(SDL_GL_UnloadLibrary)
	STUB(SDL_GL_ExtensionSupported)
	STUB(SDL_GL_ResetAttributes)
	STUB(SDL_GL_SetAttribute)
	STUB(SDL_GL_GetAttribute)
	STUB(SDL_GL_CreateContext)
	STUB(SDL_GL_MakeCurrent)
	STUB(SDL_GL_GetCurrentWindow)
	STUB(SDL_GL_GetCurrentContext)
	STUB(SDL_GL_GetDrawableSize)
	STUB(SDL_GL_SetSwapInterval)
	STUB(SDL_GL_GetSwapInterval)
	STUB(SDL_GL_SwapWindow)
	STUB(SDL_GL_DeleteContext)

	/* Haptic */
	STUB(SDL_NumHaptics)
	STUB(SDL_HapticName)
	STUB(SDL_HapticOpen)
	STUB(SDL_HapticOpened)
	STUB(SDL_HapticIndex)
	STUB(SDL_MouseIsHaptic)
	STUB(SDL_HapticOpenFromMouse)
	STUB(SDL_JoystickIsHaptic)
	STUB(SDL_HapticOpenFromJoystick)
	STUB(SDL_HapticClose)
	STUB(SDL_HapticNumEffects)
	STUB(SDL_HapticNumEffectsPlaying)
	STUB(SDL_HapticQuery)
	STUB(SDL_HapticNumAxes)
	STUB(SDL_HapticEffectSupported)
	STUB(SDL_HapticNewEffect)
	STUB(SDL_HapticUpdateEffect)
	STUB(SDL_HapticRunEffect)
	STUB(SDL_HapticStopEffect)
	STUB(SDL_HapticDestroyEffect)
	STUB(SDL_HapticGetEffectStatus)
	STUB(SDL_HapticSetGain)
	STUB(SDL_HapticSetAutocenter)
	STUB(SDL_HapticPause)
	STUB(SDL_HapticUnpause)
	STUB(SDL_HapticStopAll)
	STUB(SDL_HapticRumbleSupported)
	STUB(SDL_HapticRumbleInit)
	STUB(SDL_HapticRumblePlay)
	STUB(SDL_HapticRumbleStop)
	
	STUB(SDL_isupper)
	STUB(SDL_islower)
	STUB(SDL_JoystickAttachVirtual)
	STUB(SDL_JoystickDetachVirtual)
	STUB(SDL_JoystickIsVirtual)
	STUB(SDL_JoystickSetVirtualAxis)
	STUB(SDL_JoystickSetVirtualButton)
	STUB(SDL_JoystickSetVirtualHat)
	STUB(SDL_GetErrorMsg)
	STUB(SDL_LockSensors)
	STUB(SDL_UnlockSensors)
	STUB(SDL_Metal_GetLayer)
	STUB(SDL_Metal_GetDrawableSize)
	STUB(SDL_trunc)
	STUB(SDL_truncf)
	STUB(SDL_GetPreferredLocales)
	STUB(SDL_SIMDRealloc)
	STUB(SDL_OpenURL)
	STUB(SDL_HasSurfaceRLE)
	STUB(SDL_GameControllerHasLED)
	STUB(SDL_GameControllerSetLED)
	STUB(SDL_JoystickHasLED)
	STUB(SDL_JoystickSetLED)
	STUB(SDL_GameControllerRumbleTriggers)
	STUB(SDL_JoystickRumbleTriggers)
	STUB(SDL_GameControllerHasAxis)
	STUB(SDL_GameControllerHasButton)
	STUB(SDL_GameControllerGetNumTouchpads)
	STUB(SDL_GameControllerGetNumTouchpadFingers)
	STUB(SDL_GameControllerGetTouchpadFinger)
	STUB(SDL_GetNumTouchDevices)
	STUB(SDL_GetTouchDevice)
	STUB(SDL_GetNumTouchFingers)
	STUB(SDL_GetTouchFinger)
	STUB(SDL_crc32)
	STUB(SDL_GameControllerGetSerial)
	STUB(SDL_JoystickGetSerial)
	STUB(SDL_GameControllerHasSensor)
	STUB(SDL_GameControllerSetSensorEnabled)
	STUB(SDL_GameControllerIsSensorEnabled)
	STUB(SDL_GameControllerGetSensorData)
	STUB(SDL_wcscasecmp)
	STUB(SDL_wcsncasecmp)
		
	STUB(SDL_CopyAndSwap16)
	STUB(SDL_CopyAndSwap32)
	
	STUB(SDL_UpdateNVTexture)
	STUB(SDL_round)
	STUB(SDL_roundf)
	STUB(SDL_lround)
	STUB(SDL_lroundf)
	STUB(SDL_SoftStretchLinear)
	STUB(SDL_SetWindowKeyboardGrab)
	STUB(SDL_SetWindowMouseGrab)
	STUB(SDL_GetWindowKeyboardGrab)
	STUB(SDL_GetWindowMouseGrab)
	STUB(SDL_isalpha)
	STUB(SDL_isalnum)
	STUB(SDL_isblank)
	STUB(SDL_iscntrl)
	STUB(SDL_isxdigit)
	STUB(SDL_ispunct)
	STUB(SDL_isprint)
	STUB(SDL_isgraph)
	STUB(SDL_GetAudioDeviceSpec)
	STUB(SDL_TLSCleanup)
	STUB(SDL_SetWindowAlwaysOnTop)
	STUB(SDL_FlashWindow)
	STUB(SDL_GameControllerSendEffect)
	STUB(SDL_JoystickSendEffect)
	STUB(SDL_GameControllerGetSensorDataRate)
	
	STUB(SDL_SetTextureUserData)
	STUB(SDL_GetTextureUserData)
	STUB(SDL_RenderGeometry)
#if defined(GENERATE_STUBS)
	extern void *SDL_RenderGeometryRaw_REAL(void *a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l);
	void * __saveds LIB_SDL_RenderGeometryRaw(void *a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l)
	{
		return SDL_RenderGeometryRaw_REAL(a, b, c, d, e, f, g, h, i, j, k, l);
	}
#else
	STUB(SDL_RenderGeometryRaw)
#endif
	STUB(SDL_RenderSetVSync)
	STUB(SDL_vasprintf)
	STUB(SDL_GetWindowICCProfile)
	STUB(SDL_GetTicks64)
	STUB(SDL_GameControllerGetAppleSFSymbolsNameForButton)
	STUB(SDL_GameControllerGetAppleSFSymbolsNameForAxis)
	STUB(SDL_hid_init)
	STUB(SDL_hid_exit)
	STUB(SDL_hid_device_change_count)
	STUB(SDL_hid_enumerate)
	STUB(SDL_hid_free_enumeration)
	STUB(SDL_hid_open)
	STUB(SDL_hid_open_path)
	STUB(SDL_hid_write)
	STUB(SDL_hid_read_timeout)
	STUB(SDL_hid_read)
	STUB(SDL_hid_set_nonblocking)
	STUB(SDL_hid_send_feature_report)
	STUB(SDL_hid_get_feature_report)
	STUB(SDL_hid_close)
	STUB(SDL_hid_get_manufacturer_string)
	STUB(SDL_hid_get_product_string)
	STUB(SDL_hid_get_serial_number_string)
	STUB(SDL_hid_get_indexed_string)
	STUB(SDL_SetWindowMouseRect)
	STUB(SDL_GetWindowMouseRect)
	STUB(SDL_RenderWindowToLogical)
	STUB(SDL_RenderLogicalToWindow)
	STUB(SDL_JoystickHasRumble)
	STUB(SDL_JoystickHasRumbleTriggers)
	STUB(SDL_GameControllerHasRumble)
	STUB(SDL_GameControllerHasRumbleTriggers)
	STUB(SDL_hid_ble_scan)
	STUB(SDL_PremultiplyAlpha)
	STUB(SDL_getenv)
	STUB(SDL_setenv)

	STUB(SDL_GetTouchName)
	STUB(SDL_ClearComposition)
	STUB(SDL_IsTextInputShown)
	STUB(SDL_HasIntersectionF)
	STUB(SDL_IntersectFRect)
	STUB(SDL_UnionFRect)
	STUB(SDL_EncloseFPoints)
	STUB(SDL_IntersectFRectAndLine)
	STUB(SDL_RenderGetWindow)
	// 2.24.0
	STUB(SDL_bsearch)
	STUB(SDL_GameControllerPathForIndex)
	STUB(SDL_GameControllerPath)
	STUB(SDL_JoystickPathForIndex)
	STUB(SDL_JoystickPath)
	STUB(SDL_JoystickAttachVirtualEx)
	STUB(SDL_HasLSX)
	STUB(SDL_HasLASX)
	STUB(SDL_GameControllerGetFirmwareVersion)
	STUB(SDL_GUIDToString)
	STUB(SDL_GUIDFromString)
	STUB(SDL_JoystickGetFirmwareVersion)
	STUB(SDL_GetOriginalMemoryFunctions)
	STUB(SDL_ResetKeyboard)	
	STUB(SDL_GetPointDisplayIndex)
	STUB(SDL_GetRectDisplayIndex)
	STUB(SDL_ResetHint)
	STUB(SDL_crc16)
	STUB(SDL_GetDefaultAudioInfo)
	// 2.26.0
	STUB(SDL_GetWindowSizeInPixels)
	STUB(SDL_GetJoystickGUIDInfo)
	STUB(SDL_SetPrimarySelectionText)
	STUB(SDL_GetPrimarySelectionText)
	STUB(SDL_HasPrimarySelectionText)
	STUB(SDL_ResetHints)
	STUB(SDL_strcasestr)
	// 2.28.0
	STUB(SDL_HasWindowSurface)
	STUB(SDL_DestroyWindowSurface)
	//
	STUB(SDL_GameControllerGetSteamHandle)
	