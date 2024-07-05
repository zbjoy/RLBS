#include "reactor_buf.h"
#include <assert.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

reactor_buf::reactor_buf()
{
	_buf = NULL;
}

reactor_buf::~reactor_buf()
{
	this->clear();
}

int reactor_buf::length()
{
	if (_buf == NULL)	
	{
		return 0;		
	}
	else
	{
		return _buf->length;		
	}
}

void reactor_buf::pop(int len)
{
	assert(_buf != NULL && len <= _buf->length);

	_buf->pop(len);

	if (_buf->length == 0)
	{
		this->clear();		
	}
}

void reactor_buf::clear()
{
	if (_buf != NULL)
	{
		buf_pool::instance()->revert(_buf);		
		_buf = NULL;
	}
}



// 从一个fd中读取数据到reactor_buf中
int input_buf::read_data(int fd)
{
	// 读出硬件中可读的数据
	int need_read;

	// 一次性将io中的缓存数据全部读出来
	// 需要给fd设置一个属性
	if (ioctl(fd, FIONREAD, &need_read) == -1)
	{
		fprintf(stderr, "ioctl FIONREAD\n");		
		return -1;
	}

	if (_buf == NULL)
	{
		_buf = buf_pool::instance()->alloc_buf(need_read);		
		if (_buf == NULL)
		{
			fprintf(stderr, "(buf_pool::alloc_buf error): no buf for alloc!!\n");
			return -1;
		}
	}
	else
	{
		assert(_buf->head == 0);		
		if (_buf->capacity - _buf->length < need_read)
		{
			// 当前buf的内存不足		
			io_buf* new_buf = buf_pool::instance()->alloc_buf(need_read + _buf->length);
			if (new_buf == NULL)
			{
				fprintf(stderr, "(buf_pool::alloc_buf error): no buf for alloc!!\n");		
				return -1;
			}

			new_buf->copy(_buf);
			buf_pool::instance()->revert(_buf);

			_buf = new_buf;
		}
	}

	int already_read = 0;

	// 当前的buf是可以容纳 读取数据的
	do
	{
		if (need_read == 0)
		{
			already_read = read(fd, _buf->data + _buf->length, m4K);		
		}
		else
		{
			already_read = read(fd, _buf->data + _buf->length, need_read);	
		}
	} while(already_read == -1 && errno == EINTR);

	if (already_read > 0)
	{
		if (need_read != 0)		
		{
			assert(already_read == need_read);
		}

		_buf->length += already_read;
	}

	return already_read;
}

// 获取当前数据的方法
const char* input_buf::data()
{
	return _buf != NULL ? _buf->data + _buf->head : NULL;
}

// 重置缓冲区
void input_buf::adjust()
{
	if (_buf != NULL)
	{
		_buf->adjust();		
	}
}




int output_buf::send_data(const char* data, int datalen)
{
	if (_buf == NULL)
	{
		_buf = buf_pool::instance()->alloc_buf(datalen);		
		if (_buf == NULL)
		{
			fprintf(stderr, "(buf_pool::alloc_buf error): no buf for alloc!!\n");		
			return -1;
		}
	}
	else
	{
		// 这句代码是因为可能正在读_buf
		assert(_buf->head == 0);
		
		if (_buf->capacity - _buf->length < datalen)
		{
			io_buf* new_buf = buf_pool::instance()->alloc_buf(datalen + _buf->length);		
			if (new_buf == NULL)
			{
				fprintf(stderr, "(buf_pool::alloc_buf error): no buf for alloc!!\n");		
				return -1;
			}

			new_buf->copy(_buf);
			buf_pool::instance()->revert(_buf);
			_buf = new_buf;
		}
	}

	memcpy(_buf->data + _buf->length, data, datalen);
	_buf->length += datalen;

	return 0;
}

// 将_buf中的数据写到一个fd中
int output_buf::write2fd(int fd) // 用来取代io层的write方法
{
	assert(_buf != NULL && _buf->head == 0);
	
	int already_write = 0;

	do
	{
		already_write = write(fd, _buf->data, _buf->length);
	} while(already_write == -1 && errno == EINTR);

	if (already_write > 0)
	{
		_buf->pop(already_write);		
		_buf->adjust();
	}

	// 如果fd是非阻塞的, 会报already_write == -1 && errno == EAGAIN
	if (already_write == -1 && errno == EAGAIN)
	{
		// 这是非阻塞导致的 -1, 不是错误
		already_write = 0;		
	}

	return already_write;
}
