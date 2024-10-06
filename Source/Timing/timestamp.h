//-----------------------------------------------------------------------------
//           Name: timestamp.h
//      Developer: Wolfire Games LLC
//         Author: Micah J Best
//           Date: Thursday, April 1, 2016
//    Description: Lightweight timing tools
//        License: Read below
//-----------------------------------------------------------------------------
//
//   Copyright 2022 Wolfire Games LLC
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//-----------------------------------------------------------------------------
#pragma once

#include <Compat/platform.h>

inline uint64_t GetTimestamp() {
    // Use rdtsc instruction to get the tsc or Time Stamp Counter
#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX)) && (defined(__i386__) || defined(__x86_64__))
    uint32_t rax, rdx;
    asm volatile("lfence" ::
                     : "memory");  // Fence memory load to everything is executed up to this point.
    asm volatile("rdtsc\n"
                 : "=a"(rax), "=d"(rdx)
                 :
                 :);
    return ((uint64_t)rdx << 32) + rax;
#elif defined(PLATFORM_WINDOWS) && _MSC_VER >= 1600
    unsigned __int64 i;
    _ReadBarrier();
    i = __rdtsc();
    return (uint64_t)i;
#elif defined(__aarch64__) // not much point checking for platform for other architectures, it's almost certainly linux or bsd
    uint64_t ret;
    asm volatile("dmb ld" ::
                      : "memory");  // not sure if dmb or dsd is a better equivalent for lfence here
    asm volatile("mrs %0, cntvct_el0"
                 : "=r" (ret));
    return ret;
#elif defined (__powerpc64__)
    uint64_t ret;
    asm volatile("lwsync" ::
                      : "memory");
    ret = __builtin_ppc_mftb();
    return ret;
#elif defined(__loongarch__)
    uint64_t ret, counterid;
    asm volatile("dbar 0" ::
                      : "memory");
    asm volatile("rdtime.d %0, %1"
                 : "=r" (ret) , "=r" (counterid));
    return ret;
#elif defined(__mips__)
    uint64_t ret;
    asm volatile("sync 0" ::
                      : "memory");
    asm volatile ("rdhwr %0, $2\n"
                      : "=r" (ret));
    return ret;
#elif defined(__riscv)
    uint64_t ret;
    asm volatile("fence r,r" ::
                     : "memory");
    asm volatile("rdcycle %0"
                 : "=r" (ret));
    return ret;
#else

#error "No timer implementation for current target platform"

#endif
}
