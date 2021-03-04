#pragma once
#ifndef CRAFT_ENGINE_MATH_H_
#define CRAFT_ENGINE_MATH_H_

#include "../Common.h"
#include <assert.h>
#include <cmath>


#ifdef max
#undef max
#endif 
#ifdef min
#undef min
#endif 



#define algorithm_binary_minimum_bigger_than_reference(reference_value, array_left_part, array_index_name, array_right_part, array_size, result_name)\
					{\
					int lower_bound = array_size - 1;\
					int upper_bound = 0;\
					int array_index_name = 0;\
					int cur = 0;\
					int pos = reference_value;\
					while (upper_bound < lower_bound)\
					{\
						array_index_name = (upper_bound + lower_bound) / 2;\
						cur = array_left_part##array_index_name##array_right_part;\
						if (pos < cur)\
							lower_bound = array_index_name;\
						else\
							upper_bound = array_index_name + 1;\
					}\
					max_one = lower_bound + 1;\
					}

#define algorithm_binary_maximum_smaller_than_reference(reference_value, array_left_part, array_index_name, array_right_part, array_size, result_name)\
					{\
					int lower_bound = array_size - 1;\
					int upper_bound = 0;\
					int array_index_name = 0;\
					int cur = 0;\
					int pos = reference_value;\
					while (upper_bound < lower_bound)\
					{\
						array_index_name = (upper_bound + lower_bound) / 2;\
						cur = array_left_part##array_index_name##array_right_part;\
						if (pos < cur)\
							lower_bound = array_index_name;\
						else\
							upper_bound = array_index_name + 1;\
					}\
					max_one = lower_bound;\
					}




namespace CraftEngine
{
	/*
	 math
	*/
	namespace math
	{







	}


}

#endif // !CRAFT_ENGINE_INCLUDED