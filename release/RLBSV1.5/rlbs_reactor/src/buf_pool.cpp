#include "buf_pool.h"
#include <assert.h>


/* ======================= 创建单例模式 ============================ */
//单例对象初始化(懒汉模式)
buf_pool* buf_pool::_instance = NULL;
pthread_once_t buf_pool::_once = PTHREAD_ONCE_INIT;


/* ========================= buf_pool 属性 ================================ */
// 保护pool map增删查改的锁
pthread_mutex_t buf_pool::_mutex = PTHREAD_MUTEX_INITIALIZER;




void buf_pool::make_io_buf_list(int cap, int num)
{
	io_buf* prev;	

	_pool[cap] = new io_buf(cap);
	if (_pool[cap] == NULL)
	{
		fprintf(stderr, "new io_buf %d error\n", cap);		
		exit(1);
	}

	prev = _pool[cap];
	
	for (int i = 1; i < num; ++i)
	{
		prev->next = new io_buf(cap);		
		if (prev->next == NULL)
		{
			fprintf(stderr, "new io_buf cap error\n");		
			exit(1);
		}
		prev = prev->next;
	}

	_total_mem += cap / 1024 * num;
	
}

buf_pool::buf_pool(): _total_mem(0)
{
	make_io_buf_list(m4K, 5000);
	make_io_buf_list(m16K, 1000);
	make_io_buf_list(m64K, 500);
	make_io_buf_list(m256K, 200);
	make_io_buf_list(m1M, 50);
	make_io_buf_list(m4M, 20);
	make_io_buf_list(m8M, 10);
}

io_buf* buf_pool::alloc_buf(int N)
{
	int index;
	if (N <= m4K)
	{
		index = m4K;		
	}
	else if (N <= m16K)
	{
		index = m16K;		
	}
	else if (N <= m64K)
	{
		index = m64K;		
	}
	else if (N <= m256K)
	{
		index = m256K;		
	}
	else if (N <= m1M)
	{
		index = m1M;		
	}
	else if (N <= m4M)
	{
		index = m4M;		
	}
	else if (N <= m8M)
	{
		index = m8M;		
	}
	else
	{
		return NULL;		
	}

	// 如果index内存以及没有了, 需要额外的申请内存
	pthread_mutex_lock(&_mutex);
	if (_pool[index] == NULL)
	{
		if (_total_mem + index / 1024 >= MEM_LIMIT)		
		{
			fprintf(stderr, "already use too many memory!!\n");		
			exit(1);
		}

		io_buf* new_buf = new io_buf(index);
		if (new_buf == NULL)
		{
			fprintf(stderr, "new io_buf error\n");		
			exit(1);
		}

		_total_mem += index / 1024;
		pthread_mutex_unlock(&_mutex);
		return new_buf;
	}

	io_buf* target = _pool[index];
	_pool[index] = target->next;
	pthread_mutex_unlock(&_mutex);

	target->next = NULL;

	return target;
}

io_buf* buf_pool::alloc_buf()
{
	return alloc_buf(m4K);
}

// 重置一个io_buf放回pool中
void buf_pool::revert(io_buf* buffer)
{
	int index = buffer->capacity;

	buffer->length = 0;
	buffer->head = 0;

	pthread_mutex_lock(&_mutex);

	assert(_pool.find(index) != _pool.end());

	buffer->next = _pool[index];
	_pool[index] = buffer;

	pthread_mutex_unlock(&_mutex);
}


