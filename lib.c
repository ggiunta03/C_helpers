#include "lib.h"

// No this library isnt copied/pasted from chatGPT or some shit like that. These are useful things ive learned about and needed over the years. It was also for some practice in C to keep my skillz sharp.

void zero_free(void *buf, size_t size) {
	if (NULL == buf) {
		return;
	}

	memset(buf, 0, size);
	free(buf);
	buf = NULL;
}

void free_all(void **arr) {
	if (NULL == arr) {
		return;
	}

	// Loop over and free all inner pointers
	while (NULL != *arr) {
		free(*arr);
		arr = NULL;
		arr++;
	}

	free(arr);
	arr = NULL;
}

/**
 * @brief This serves as a safe wrapper to libc's realloc to handle failures
 * 
 * @param buf User passed buffer to be resized (grown)
 * @param curr_size Pointer to the current size of the buf. This will be updated on success
 * @param new_size Requested size to grow buf to
 * 
 * @return Return pointer to newly allocated buffer along with curr_size updated
 */
void *safe_realloc(void *buf, size_t *curr_size, size_t new_size) {
	if (NULL == buf || NULL == curr_size) {
		return NULL;
	}

	if (new_size <= *curr_size) {
		printf("Will not shrink buffer....returning original pointer and maintaining size\n");
		return buf;
	}

	// create a tmp pointer in case realloc fails and we lose it
	void *tmp = buf;
	if ((buf = realloc(buf, new_size)) == NULL){
		free(tmp);
		tmp = NULL;
		return NULL;
	}

	// realloc succeeded, but there is more to do since realloc sucks
	// Zero out newly initialized memory like calloc would do. 
	memset(buf + *curr_size, 0, (new_size - *curr_size));

	// We now have a newly sized buffer with zero'd out memory at the end
	*curr_size = new_size;
	return buf;
}

/**
 * @brief This serves as a safe wrapper to libc's reallocarray to handle failures
 * 
 * @param arr User passed array to be resized (grown)
 * @param curr_memb Pointer to the current number of members array holds. This will be updated on success
 * @param n_memb Requested number of members to grow to
 * @param size Size of each member
 * 
 * @return Return pointer to newly allocated barray along with curr_memb updated
 */
void **safe_reallocarray(void **arr, size_t *curr_memb, size_t n_memb, size_t size) {
	if (NULL == arr || NULL == curr_memb) {
		return NULL;
	}

	if (n_memb <= *curr_memb) {
		printf("Will not shrink array....returning original pointer and maintaining number of members\n");
		return arr;
	}

	// Just in case we lose arr
	void **tmp = arr;

	if ((arr = reallocarray(arr, n_memb, size)) == NULL) {
		// man realloc states that upon reallocarray failure the return is NULL and original block is left untouched. 
		// This is unacceptable and we must free those inner pointers as well as the array itself. Luckily we have a helper
		free_all(tmp);
		return NULL;
	}

	// If we are successful there is still work to do. Just like safe_realloc we must initialize all our newly allocated members to NULL. This will save you from an invalid memory read when trying to loop over and free all your inner pointers. 
	for (int i = *curr_memb; i < n_memb; i++) {
		arr[i] = NULL;
	}

	*curr_memb = n_memb;
	return arr;
}

/**
 * @brief Check if string starts with a given sequence of bytes
 * 
 * @param haystack String to search
 * @param needle Bytes to search for
 * 
 * @return true if found, false if not
 */
bool does_start_with(char *haystack, char *needle) {
	if (NULL == haystack || NULL == needle) {
		return false;
	}

	if (strlen(needle) > strlen(haystack)) {
		return false;
	}

	if (strncmp(haystack, needle, strlen(needle)) == 0) {
		return true;
	}
	return false;
}

/**
 * @brief Combine two strings
 * 
 * @param str1 First string
 * @param str2 Second string
 * 
 * @note Function will allocate memory for string. CALLER is responsible for freeing it
 */
char *build_str(char *str1, char *str2) {
	if (NULL == str1 || NULL == str2) {
		return NULL;
	}

	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	// The +1 is to ensure it is a NULL terminated string
	char *final_str = (char *)calloc((len1 + len2) + 1, sizeof(uint8_t));
	strncpy(final_str, str1, len1);
	strncpy(final_str + len1, str2, len2);

	return final_str;
}

/**
 * @brief Remove any number of a given sequence occuring at the end of a string
 * 
 * @param haystack Original string to search
 * @param needle Character sequence to search for and remove
 * 
 * @return A newly allocated string without ending sequence, NULL on failure
 * 
 * @note Function will allocate memory for string. CALLER is responsible for freeing it
 */
char *trim_ending_matches(char *haystack, char *needle) {
	if (NULL == haystack || NULL == needle) {
		return NULL;
	}

	if (strlen(needle) >= strlen(haystack)) {
		printf("Ending match must be smaller than length of string\n");
		return NULL;
	}

	size_t original_size = strlen(haystack);
	size_t needle_size = strlen(needle);
	// Get ending position
	char *occurrence = haystack + (original_size - needle_size);
	size_t chars_moved = 0;

	// Does it even exist in our str, and if so is there one at the end?
	while (chars_moved < original_size) {
		printf("comparing: %s with: %s\n", occurrence, needle);
		if ((strncmp(occurrence, needle, needle_size)) != 0) {
			break;
		}
		occurrence -= needle_size;
		chars_moved += needle_size;
	}

	printf("chars moved: %lu\n", chars_moved);

	// Erm now we just create new string and return up until occurrence
	size_t new_size = original_size - chars_moved;
	char *new_str = (char *)calloc(new_size, sizeof(uint8_t));
	strncpy(new_str, haystack, new_size);

	return new_str;
}