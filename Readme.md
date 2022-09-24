This repo describe a bug candidate of thread sanitizer.
This bug is already reported:
https://github.com/google/sanitizers/issues/814
https://github.com/google/sanitizers/issues/1419

## Environment
Ubuntu 22.04 + Gcc-11(installed by apt)
```
$ uname -a
Linux ubuntu 5.15.0-25-generic #25-Ubuntu SMP Wed Mar 30 15:54:22 UTC 2022 x86_64 x86_64 x86_64 GNU/Linux
$ g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 11.2.0-19ubuntu1' --with-bugurl=file:///usr/share/doc/gcc-11/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,m2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-11 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-bootstrap --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --enable-libphobos-checking=release --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --enable-cet --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-11-gBFGDP/gcc-11-11.2.0/debian/tmp-nvptx/usr,amdgcn-amdhsa=/build/gcc-11-gBFGDP/gcc-11-11.2.0/debian/tmp-gcn/usr --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --with-build-config=bootstrap-lto-lean --enable-link-serialization=2
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.2.0 (Ubuntu 11.2.0-19ubuntu1) 
```

## To reproduce the bug:
```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./sanitizerDemo
```
or
```shell
mkdir build
g++ -g -fsanitize=thread -o build/sanitizerDemo main.cpp
./build/sanitizerDemo
```

## Expected behavior:
No warning and error

## Observed behavior:
Get warning:
```
WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock) (pid=188565)
  Cycle in lock order graph: M9 (0x5589cf5b5040) => M10 (0x5589cf5b5080) => M11 (0x5589cf5b50c0) => M9

  Mutex M10 acquired here while holding mutex M9 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x21b3)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x2344)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x252d)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x2412)
    #5 foo() /home/jcq/workspace/sanitizerDemo/main.cpp:10 (sanitizerDemo+0x13e8)
    #6 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:14 (sanitizerDemo+0x147a)
    #7 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:27 (sanitizerDemo+0x15e3)
    #8 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x20c2)
    #9 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x1fb0)
    #10 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1e72)
    #11 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1dbe)
    #12 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1d2a)
    #13 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

    Hint: use TSAN_OPTIONS=second_deadlock_stack=1 to get more informative warning message

  Mutex M11 acquired here while holding mutex M10 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x21b3)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x2344)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x252d)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x2412)
    #5 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:16 (sanitizerDemo+0x14a6)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:27 (sanitizerDemo+0x15e3)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x20c2)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x1fb0)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1e72)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1dbe)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1d2a)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Mutex M9 acquired here while holding mutex M11 in thread T2:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x21b3)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x2344)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x252d)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x2412)
    #5 yoo() /home/jcq/workspace/sanitizerDemo/main.cpp:21 (sanitizerDemo+0x155f)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:33 (sanitizerDemo+0x161b)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x2039)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x1f11)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1e18)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1d74)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1cd8)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Thread T1 (tid=188567, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:29 (sanitizerDemo+0x166f)

  Thread T2 (tid=188568, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:35 (sanitizerDemo+0x1682)

SUMMARY: ThreadSanitizer: lock-order-inversion (potential deadlock) /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 in __gthread_mutex_lock
==================
^C
jcq@ubuntu:~/workspace/sanitizerDemo/build$ ./sanitizerDemo 
==================
WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock) (pid=188760)
  Cycle in lock order graph: M9 (0x55b7aa345040) => M10 (0x55b7aa345080) => M11 (0x55b7aa3450c0) => M9

  Mutex M10 acquired here while holding mutex M9 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x26a1)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x24cc)
    #5 foo() /home/jcq/workspace/sanitizerDemo/main.cpp:13 (sanitizerDemo+0x1428)
    #6 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:17 (sanitizerDemo+0x14ba)
    #7 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:30 (sanitizerDemo+0x1623)
    #8 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x2164)
    #9 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x2052)
    #10 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1f14)
    #11 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e60)
    #12 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1dcc)
    #13 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

    Hint: use TSAN_OPTIONS=second_deadlock_stack=1 to get more informative warning message

  Mutex M11 acquired here while holding mutex M10 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x26a1)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x24cc)
    #5 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:19 (sanitizerDemo+0x14e6)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:30 (sanitizerDemo+0x1623)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x2164)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x2052)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1f14)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e60)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1dcc)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Mutex M9 acquired here while holding mutex M11 in thread T2:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x26a1)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x24cc)
    #5 yoo() /home/jcq/workspace/sanitizerDemo/main.cpp:24 (sanitizerDemo+0x159f)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:39 (sanitizerDemo+0x1681)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x20db)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x1fb3)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1eba)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e16)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1d7a)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Thread T1 (tid=188762, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:35 (sanitizerDemo+0x1710)

  Thread T2 (tid=188763, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:45 (sanitizerDemo+0x1723)

SUMMARY: ThreadSanitizer: lock-order-inversion (potential deadlock) /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 in __gthread_mutex_lock
==================
ThreadSanitizer: reported 1 warnings
jcq@ubuntu:~/workspace/sanitizerDemo/build$ ./sanitizerDemo 
==================
WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock) (pid=188939)
  Cycle in lock order graph: M9 (0x5583b13d1040) => M10 (0x5583b13d1080) => M11 (0x5583b13d10c0) => M9

  Mutex M10 acquired here while holding mutex M9 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::lock_guard<std::mutex>::lock_guard(std::mutex&) /usr/include/c++/11/bits/std_mutex.h:229 (sanitizerDemo+0x24b8)
    #4 foo() /home/jcq/workspace/sanitizerDemo/main.cpp:13 (sanitizerDemo+0x1428)
    #5 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:17 (sanitizerDemo+0x14ba)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:30 (sanitizerDemo+0x1623)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x2164)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x2052)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1f14)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e60)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1dcc)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

    Hint: use TSAN_OPTIONS=second_deadlock_stack=1 to get more informative warning message

  Mutex M11 acquired here while holding mutex M10 in thread T1:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::lock_guard<std::mutex>::lock_guard(std::mutex&) /usr/include/c++/11/bits/std_mutex.h:229 (sanitizerDemo+0x24b8)
    #4 xoo() /home/jcq/workspace/sanitizerDemo/main.cpp:19 (sanitizerDemo+0x14e6)
    #5 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:30 (sanitizerDemo+0x1623)
    #6 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x2164)
    #7 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x2052)
    #8 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1f14)
    #9 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e60)
    #10 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1dcc)
    #11 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Mutex M9 acquired here while holding mutex M11 in thread T2:
    #0 pthread_mutex_lock ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:4240 (libtsan.so.0+0x53908)
    #1 __gthread_mutex_lock /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 (sanitizerDemo+0x2255)
    #2 std::mutex::lock() /usr/include/c++/11/bits/std_mutex.h:100 (sanitizerDemo+0x23e6)
    #3 std::unique_lock<std::mutex>::lock() /usr/include/c++/11/bits/unique_lock.h:139 (sanitizerDemo+0x2759)
    #4 std::unique_lock<std::mutex>::unique_lock(std::mutex&) /usr/include/c++/11/bits/unique_lock.h:69 (sanitizerDemo+0x2584)
    #5 yoo() /home/jcq/workspace/sanitizerDemo/main.cpp:24 (sanitizerDemo+0x159f)
    #6 operator() /home/jcq/workspace/sanitizerDemo/main.cpp:39 (sanitizerDemo+0x1681)
    #7 __invoke_impl<void, main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (sanitizerDemo+0x20db)
    #8 __invoke<main()::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (sanitizerDemo+0x1fb3)
    #9 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:253 (sanitizerDemo+0x1eba)
    #10 operator() /usr/include/c++/11/bits/std_thread.h:260 (sanitizerDemo+0x1e16)
    #11 _M_run /usr/include/c++/11/bits/std_thread.h:211 (sanitizerDemo+0x1d7a)
    #12 execute_native_thread_routine ../../../../../src/libstdc++-v3/src/c++11/thread.cc:82 (libstdc++.so.6+0xdc2c2)

  Thread T1 (tid=188941, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:35 (sanitizerDemo+0x1710)

  Thread T2 (tid=188942, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 __gthread_create /build/gcc-12-OcsLtf/gcc-12-12-20220319/build/x86_64-linux-gnu/libstdc++-v3/include/x86_64-linux-gnu/bits/gthr-default.h:663 (libstdc++.so.6+0xdc398)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) ../../../../../src/libstdc++-v3/src/c++11/thread.cc:147 (libstdc++.so.6+0xdc398)
    #3 main /home/jcq/workspace/sanitizerDemo/main.cpp:45 (sanitizerDemo+0x1723)

SUMMARY: ThreadSanitizer: lock-order-inversion (potential deadlock) /usr/include/x86_64-linux-gnu/c++/11/bits/gthr-default.h:749 in __gthread_mutex_lock
==================
ThreadSanitizer: reported 1 warnings
```
