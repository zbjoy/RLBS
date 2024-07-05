#pragma once

#include <ext/hash_map>
#include <pthread.h>
#include <stdint.h>
#include "io_buf.h"

typedef __gnu_cxx::hash_map<int, io_buf*> pool_t;

enum MEM_CAP
{
	m4K = 4096,
	m16K = 16384,
	m64K = 65536,
	m256K = 262144,
	m1M = 1048576,
	m4M = 4194304,
	m8M = 8388608
};

#define MEM_LIMIT (5U * 1024 * 1024)

class buf_pool
{
public:	
	static void init()
	{
		_instance = new buf_pool();
	}
	
	static buf_pool* instance()
	{
		pthread_once(&_once, init);		
		return _instance;
	}

	io_buf* alloc_buf(int N);
	io_buf* alloc_buf();

	void revert(io_buf* buffer);

	void make_io_buf_list(int cap, int num);

private:

	/* ======================= 创建单例模式 ============================ */
	buf_pool();
	buf_pool(const buf_pool&);
	const buf_pool& operator=(const buf_pool&);

	static buf_pool* _instance;
	static pthread_once_t _once;


	/* ========================= buf_pool 属性 ================================ */
	
	// 存放所有io_buf的map句柄
	pool_t _pool;

	// 当前内存池总体大小, 单位为kb
	uint64_t _total_mem;

	// 保护pool map增删查改的锁
	static pthread_mutex_t _mutex;

};
