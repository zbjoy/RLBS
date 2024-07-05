#pragma once

#include "io_buf.h"
#include "buf_pool.h"

class reactor_buf
{
public:	
	reactor_buf();
	~reactor_buf();

	int length();

	void pop(int len);

	void clear();

protected:
	io_buf* _buf;

};

class input_buf : public reactor_buf
{
public:	
	// 从一个fd中读取数据到reactor_buf中
	int read_data(int fd);

	// 获取当前数据的方法
	const char* data();

	// 重置缓冲区
	void adjust();
	
};

class output_buf : public reactor_buf
{
public:	
	int send_data(const char* data, int datalen);

	// 将_buf中的数据写到一个fd中
	int write2fd(int fd); // 用来取代io层的write方法

};
