#ifndef SM_CORE_UTIL_STRING_H
#define SM_CORE_UTIL_STRING_H

#include "smpch.h"

#include "core/data/smArray.h"
#include "core/util/smFilesystemPub.h"

/* Opaque string type. */
typedef struct sm__string *sm_string;

/*
 * Creates a new string.
 *
 * @param cap The initial capacity of the string.
 * @return The new string.
 */
sm_string sm_string_ctor(size_t cap);

/*
 * Creates a new string from a C string.
 *
 * @param c_string The C string.
 * @return The new string.
 */
sm_string sm_string_from(const char *c_string);

/*
 * Destroys a string.
 *
 * @param string The string to destroy.
 * @return NULL if the string was destroyed, otherwise the string.
 */
void *sm_string_dtor(sm_string string);

/*
 * Gets the length of a string.
 *
 * @param string The string.
 * @return The length of the string.
 */
size_t sm_string_len(sm_string string);

/*
 * Gets the capacity of a string.
 *
 * @param string The string.
 * @return The capacity of the string.
 */
size_t sm_string_cap(sm_string string);

/*
 * Gets the C string of a string.
 *
 * @param string The string.
 * @return The C string.
 */
const char *sm_string_c_str(const sm_string string);

/*
 * Makes a copy of a string.
 * The new string will be a deep copy of the original.
 *
 * @param string The String to take a copy of.
 * @return The new string.
 */
sm_string sm_string_copy(sm_string string);

/* Makes a reference to a string.
 * The new string will be a shallow copy of the original.
 *
 * @param string The string to take the reference of.
 * @return The string reference.
 */
sm_string sm_string_reference(sm_string string);

/*
 * Lowercases a string.
 *
 * @param string The string to be lowercased.
 * @return A lowercase copy of the string.
 */
sm_string sm_string_to_lower(sm_string string);

/*
 * Uppercases a string.
 *
 * @param string The string to be uppercased.
 * @return An uppercase copy of the string.
 */
sm_string sm_string_to_upper(sm_string string);

/*
 * Compares two strings.
 *
 * @param string1 The first string.
 * @param string2 The second string.
 * @return 0 if the strings are equal, > 0 if string1 is greater than string2,
 *        < 0 if string1 is less than string2.
 */
int32_t sm_string_compare(sm_string str1, sm_string str2);

/*
 * Checks if two strings are equal.
 *
 * @param string1 The first string.
 * @param string2 The second string.
 * @return true if the strings are equal, false if they are not.
 */
bool sm_string_eq(sm_string str1, sm_string str2);

/*
 * Splits a string into an array of strings.
 *
 * @param string The string to split.
 * @param delim The delimiter.
 * @return The array of strings or NULL if the string could not be split.
 *        It's up to the caller to free the array and the strings in it.
 */
SM_ARRAY(sm_string ) sm_string_split(sm_string string, char delim);

/*
 * Appends a string to another string.
 * The modificaiton is done in-place.
 *
 * @param string The string to append to.
 * @param append The string to append.
 */
void sm_string_append(sm_string string, sm_string append);

/* Sets string to a new value.
 *
 * @param string The string to set.
 * @param c_string The new value.
 */
bool sm_string_set(sm_string string, const char *value);

/*
 * Reads the contents of a file into a string.
 *
 * @param file_handle The file to read.
 * @return The new string containing the file contents.
 */
sm_string sm_string_from_file_handle(sm_file_handle_s *file_handle, uint64_t size);

/*
 * Trims whitespace from the beginning and end of a string.
 * The modification is done in-place.
 *
 * @param string The string to trim.
 */
void sm_string_trim(sm_string string);

#endif /* SM_CORE_UTIL_STRING_H */
