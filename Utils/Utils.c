#include "Utils.h"

bool
is_in_array (int value, int *array, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (value == array[i])
			return true;
	}

	return false;
}

int
str_is_in_array (char *value, char **array, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (strcmp(value, array[i]) == 0)
			return true;
	}

	return false;
}
