#ifndef _MEMLEAK_H_
#define _MEMLEAK_H_

//Debugģʽ������libMemLeakģ��
#ifdef _DEBUG

//���ÿ��ļ�
#pragma comment(lib, "libMemLeak.lib")

//ǿ�����ӷ���
#pragma comment(linker, "/include:__imp_?g_MemLeakDetect@@3Vg_MemLeakDetect@g@@A")

#endif

#endif
