#pragma once

class io_buf
{
public:
	io_buf(int size);

	/* 清理数据 */
	void clear();

	/* 处理长度为len的数据, 移动head */
	void pop(int len);

	/* 将已经处理的数据清空(抹去内存), 将未处理的数据移动到buf的首地址 */
	void adjust();

	/* 将其他的io_buf对象拷贝到自己中 */
	void copy(const io_buf* other);

	/* 当前buf的容量 */
	int capacity;

	/* 当前buf的有效长度 */
	int length;

	/* 当前未处理有效数据的头部索引 */
	int head;

	/* 当前buf的内存地址 */
	char* data;

	/* 存在多个io_buf采用链表 */
	io_buf* next;

};
