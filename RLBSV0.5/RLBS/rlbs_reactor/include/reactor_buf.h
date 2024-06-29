#pragma once

#include "io_buf.h"
#include "buf_pool.h"

class reactor_buf
{
public:
	reactor_buf();
	~reactor_buf();

	// 得到当前的buf还有多少有效数据
	int length();

	// 已经处理了多少数据
	void pop(int len);

	// 将当前的_buf清空
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

	int write2fd(int fd);
};
