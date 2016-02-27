#pragma once

#include <iostream>
#include <cassert>
#include <cstdlib>

const int start_stack_length = 16;
const int   max_stack_length = 65536;
const float   up_scale = 2.0;
const float down_scale = 0.75;

enum stack_error
{
	STCK_ERR_NO,
	STCK_ERR_UNDERFLOW,
	STCK_ERR_INVALID_DATA
};

static const char *stack_error_msg[] =
{
	"no errors",
	"stack underflow",
	"invalid data pointer",
};

template <class stack_t> class Stack
{
private:
	size_t cur_stack_length;
	size_t top_of_stack;
	stack_t *data;
	stack_error err;
public:
	Stack () 
		: cur_stack_length (start_stack_length)
		, top_of_stack     (0)
		, err              (STCK_ERR_NO)
	{
		data = (stack_t*)calloc (cur_stack_length, sizeof (stack_t));
		assert (data && "Allocation error in Stack constructor");
	}

	Stack (const Stack<stack_t> &that)
	{
		cur_stack_length = that.cur_stack_length;
		top_of_stack = that.top_of_stack;
		//data = (stack_t*)calloc (cur_stack_length, sizeof (stack_t));
		data = new 
		assert (data && "Allocation error in Stack copy constructor");
		memcpy (data, that.data, sizeof (stack_t) * top_of_stack);
		err = that.err;
	}

	~Stack ()
	{
		free (data);
	}

	stack_t push (stack_t value)
	{
		if (err == STCK_ERR_INVALID_DATA)
			assert (!stack_error_msg[STCK_ERR_INVALID_DATA]);

		if (top_of_stack == max_stack_length)
		{
			std::cout << "<" << typeid(stack_t).name() << "> Stack exceeded its maximum length" << std::endl;
			assert (0);
		}

		if (top_of_stack == cur_stack_length)
		{
			cur_stack_length *= up_scale;
			data = (stack_t*)realloc (data, cur_stack_length*sizeof(stack_t));
			assert (data && "Realloc error in Stack method Push");
		}

		return data[top_of_stack++] = value;
	}

	stack_t pop (void)
	{
		if (err == STCK_ERR_INVALID_DATA)
			assert (!stack_error_msg[STCK_ERR_INVALID_DATA]);

		if (!top_of_stack)
		{
			err = STCK_ERR_UNDERFLOW;
			return (stack_t)0;
		}

		if (top_of_stack <= cur_stack_length/up_scale && cur_stack_length > start_stack_length)
		{
			cur_stack_length *= down_scale;
			data = (stack_t*)realloc (data, cur_stack_length*sizeof(stack_t));
			assert (data && "Realloc error in Stack method Pop");
		}

		return data[--top_of_stack];
	}

	stack_error is_fail ()
	{
		if (!data)
			err = STCK_ERR_INVALID_DATA;
		return err;
	}

	void print_dump (std::ostream &stream)
	{
		stream << "Stack state: " << stack_error_msg[err] << std::endl;
		stream << "Stack content (current stack_length: " << cur_stack_length << ", numbers of items: " 
			   << top_of_stack << ", top of stack: " << top_of_stack << "):" << std::endl;
		for (size_t i = 0; i < top_of_stack; i++)
			stream << data[i] << " ";
		stream << "|| ";
		for (size_t i = top_of_stack; i < cur_stack_length; i++)
			stream << data[i] << " ";
		stream << std::endl;
	}
};
