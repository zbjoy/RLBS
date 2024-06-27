#include "buf_pool.h"

// 单例对象
buf_pool* buf_pool::_instance = NULL;

// 用于保证创建单例的方法全局只执行一次
pthread_once_t buf_pool::_once = PTHREAD_ONCE_INIT;

// 初始化锁
pthread_mutex_t buf_pool::_mutex = PTHREAD_MUTEX_INITIALIZER;

buf_pool::buf_pool() 
{
	_total_mem = 0;	
	
	// 链表的头指针
	io_buf* prev;

	// 开辟4k buf 内存池
	_pool[m4K] = new io_buf(m4K);

	if (_pool[m4K] == NULL)
	{
		fprintf(stderr, "new io_buf m4K error\n");
		exit(1);
	}

	prev = _pool[m4K];

	for (int i = 1; i < 5000; ++i)
	{
		prev->next = new io_buf(m4K);
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf m4K error\n");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += 4 * 5000;


	// 开辟 16kb 的 buf 内存池
	_pool[m16K] = new io_buf(m16K);
	if (_pool[m16K] == NULL)
	{
		fprintf(stderr, "new io_buf m16K error");		
		exit(1);
	}

	prev = _pool[m16K];

	for (int i = 1; i < 1000; ++i)
	{
		prev->next = new io_buf(m16K);		
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf m16K error");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += 16 * 1000;


	// 开辟 64kb 的 buf 内存池
	_pool[m64K] = new io_buf(m64K);
	if (_pool[m64K] == NULL)
	{
		fprintf(stderr, "new io_buf m16K error");		
		exit(1);
	}

	prev = _pool[m64K];

	for (int i = 1; i < 500; ++i)
	{
		prev->next = new io_buf(m16K);		
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf m64K error");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += 64 * 500;


	// 开辟 256kb 的 buf 内存池
	_pool[m256K] = new io_buf(m256K);
	if (_pool[m256K] == NULL)
	{
		fprintf(stderr, "new io_buf m256K error");		
		exit(1);
	}

	prev = _pool[m256K];

	for (int i = 1; i < 200; ++i)
	{
		prev->next = new io_buf(m256K);		
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf m256K error");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += 256 * 200;


	// 开辟 1Mb 的 buf 内存池
	_pool[m1M] = new io_buf(m1M);
	if (_pool[m1M] == NULL)
	{
		fprintf(stderr, "new io_buf m1K error");		
		exit(1);
	}

	prev = _pool[m1M];

	for (int i = 1; i < 20; ++i)
	{
		prev->next = new io_buf(m1M);		
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf m1K error");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += 1024 * 20;

}

// 从内存池申请一块内存
io_buf* buf_pool::alloc_buf(int N)
{
	return NULL;	
}

io_buf* buf_pool::alloc_buf()
{
	return NULL;	
}

// 重置一个 io_buf 放回pool中
void buf_pool::revert(io_buf* buffer)
{
		
}
