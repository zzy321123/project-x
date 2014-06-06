#ifndef _MEMLEAK_H_
#define _MEMLEAK_H_

//Debug模式下引入libMemLeak模块
#ifdef _DEBUG

//引用库文件
#pragma comment(lib, "libMemLeak.lib")

//强制链接符号
#pragma comment(linker, "/include:__imp_?g_MemLeakDetect@@3Vg_MemLeakDetect@g@@A")

#endif

#endif
