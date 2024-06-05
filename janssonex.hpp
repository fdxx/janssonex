#pragma once

#include <cstdio>
#include <jansson.h>
#include <string.h>
#include <type_traits>
#include <typeinfo>
#include <cassert>

// https://jansson.readthedocs.io/en/latest/apiref.html#c.json_object_update
enum ObjectUpdateType
{
	OU_NONE = 0,
	OU_EXISTING,
	OU_MISSING,
	OU_NEW,
	OU_EXISTING_NEW,
	OU_MISSING_NEW,
	OU_RECURSIVE
};

namespace fdxx {

struct JSON : public json_t
{
	// Create JSON
	static JSON *Create(bool value)			{ return (JSON*)json_boolean(value); }
	static JSON *Create(float value)		{ return (JSON*)json_real(value); }
	static JSON *Create(double value)		{ return (JSON*)json_real(value); }
	static JSON *Create(int value)			{ return (JSON*)json_integer(value); }
	static JSON *Create(json_int_t value)	{ return (JSON*)json_integer(value); }
	static JSON *Create(const char *value)	{ return (JSON*)json_string(value); }

	static JSON *CreateNull()	{ return (JSON*)json_null(); }
	static JSON *CreateObject()	{ return (JSON*)json_object(); }
	static JSON *CreateArray()	{ return (JSON*)json_array(); }

	// Loads a JSON from a file.
	//
	// @param file		 File to read from.
	// @param flags      Decoding flags.
	// @return           JSON pointer, or nullptr on failure.
	static JSON *FromFile(const char *file, size_t flags = 0)
	{
		json_error_t error;
		json_t *j = json_load_file(file, flags, &error);
		if (!j)
			printf("[JSON::FromFile] Invalid JSON in line %d, column %d: %s\n", error.line, error.column, error.text);
		return (JSON*)j;
	}

	// Loads a JSON from a string.
	//
	// @param str        String to read from.
	// @param flags      Decoding flags.
	// @return           JSON pointer, or nullptr on failure.
	static JSON *FromString(const char *str, size_t flags = 0)
	{
		json_error_t error;
		json_t *j = json_loads(str, flags, &error);
		if (!j)
			printf("[JSON::FromFile] Invalid JSON in line %d, column %d: %s\n", error.line, error.column, error.text);
		return (JSON*)j;
	}

	// Retrieves a value from the JSON.
	//
	// @return           Value read.
	template<typename T>
	T GetValue()
	{
		// if (json_is_string(this))

		if constexpr (std::is_same_v<T, bool>)
      		return json_boolean_value(this);

		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
			return json_number_value(this);
		
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, json_int_t>)
			return json_integer_value(this);

		if constexpr (std::is_same_v<T, const char*>)
			return json_string_value(this);
		
		printf("[JSON::GetValue] unknown type: %s\n", typeid(T).name());
		return {};
	}

	// Writes the JSON string representation to a file.
	//
	// @param file       File to write to.
	// @param flags      Encoding flags.
	// @return           True on success, false on failure.
	bool ToFile(const char *file, size_t flags = 0)
	{
		return (json_dump_file(this, file, flags) == 0);
	}

	// Returns the JSON representation of json as a string.
	// The return value must be freed by the caller using free(). 
	// Note that if you have called json_set_alloc_funcs() to override free(), 
	// you should call your custom free function instead to free the return value.
	//
	// @param flags      Encoding flags.
	// @return           string pointer, or nullptr on failure.
	char *ToString(size_t flags = 0)
	{
		return json_dumps(this, flags);
	}

	// Writes the JSON representation of json to the buffer of size bytes.
	// buffer is not null-terminated.
	//
	// @param buffer     String buffer to write to.
	// @param size       Maximum length of the string buffer.
	// @param flags      Encoding flags.
	// @return           Returns the number of bytes that would be written or 0 on error.
	size_t ToString(char *buffer, size_t size, size_t flags = 0)
	{
		return json_dumpb(this, buffer, size, flags);
	}

	// Returns a deep copy of value, 
	// Copying objects preserves the insertion order of keys.
	//
	// @return           Returns a deep copy of value.
	JSON *DeepCopy()
	{
		return (JSON*)json_deep_copy(this);
	}

	// https://jansson.readthedocs.io/en/latest/apiref.html#reference-count
	// Decrement the reference count of json. 
	// As soon as a call to json_decref() drops the reference count to zero, 
	// the value is destroyed and it can no longer be used.
	void decref()
	{
		json_decref(this);
	}

	// Increment the reference count of json
	// 
	// @return           Returns json.
	JSON *incref()
	{
		return (JSON*)json_incref(this);
	}




	// ===========================================================================
	// JSON Object functions
	// ===========================================================================

	// Get JSON reference.
	//
	// @param key        Key string.
	// @return           JSON reference.
	JSON& operator[](const char *key)
	{
		assert(this);
		return *(JSON*)json_object_get(this, key);
	}

	// Retrieves a value from the object.
	//
	// @param key        Key string.
	// @return           Value read.
	template<typename T>
	T GetValue(const char *key)
	{
		return (*this)[key].GetValue<T>();
	}

	// Returns whether or not a value in the object is null.
	//
	// @param key        Key string.
	// @return           True if the value is null, false otherwise.
	bool IsNull(const char *key)
	{
		return json_is_null(json_object_get(this, key));
	}

	// Returns whether or not a key exists in the object.
	//
	// @param key        Key string.
	// @return           True if the key exists, false otherwise.
	bool HasKey(const char *key)
	{
		return json_object_get(this, key) != nullptr;
	}

	// Sets an array or object value in the object, 
	// either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @param incref     Whether to increase the reference count of VALUE.
	// @return           True on success, false on failure.
	bool Set(const char *key, JSON *value, bool incref = false)
	{
		if (incref)
			return (json_object_set(this, key, value) == 0);
		return (json_object_set_new(this, key, value) == 0);
	}

	// Sets a value in the object
	// either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	template<typename T>
	bool SetValue(const char *key, T value)
	{
		return (json_object_set_new(this, key, JSON::Create(value)) == 0);
	}

	// Sets a null value in the object.
	// either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @return           True on success, false on failure.
	bool SetNullValue(const char *key)
	{
		return (json_object_set_new(this, key, json_null()) == 0);
	}

	// Update object with the key-value pairs from other.
	// https://jansson.readthedocs.io/en/latest/apiref.html#c.json_object_update
	//
	// @param other      Other object.
	// @param Type       enum ObjectUpdateType.
	// @return           True on success, false on failure.
	bool Update(JSON *other, ObjectUpdateType Type = OU_NEW)
	{
		if (Type == OU_NONE)
			return (json_object_update(this, other) == 0);

		if (Type == OU_EXISTING)
			return (json_object_update_existing(this, other) == 0);
		
		if (Type == OU_MISSING)
			return (json_object_update_missing(this, other) == 0);
		
		if (Type == OU_NEW)
			return (json_object_update_new(this, other) == 0);
		
		if (Type == OU_EXISTING_NEW)
			return (json_object_update_existing_new(this, other) == 0);
		
		if (Type == OU_MISSING_NEW)
			return (json_object_update_missing_new(this, other) == 0);
		
		if (Type == OU_RECURSIVE)
			return (json_object_update_recursive(this, other) == 0);

		return false;
	}

	// Removes an entry from the object.
	//
	// @param key        Key string.
	// @return           True on success, false if the key was not found.
	bool Remove(const char *key)
	{
		return (json_object_del(this, key) == 0);
	}

	// Clears the object of all entries.
	//
	// @return           True on success, false on failure.
	bool ObjClear()
	{
		return (json_object_clear(this) == 0);
	}

	// Retrieves the size of the object.
	size_t ObjSize()
	{
		return json_object_size(this);
	}





	// ===========================================================================
	// JSON Array functions
	// ===========================================================================
	

	// Get JSON reference.
	//
	// @param index      Index in the array.
	// @return           JSON reference.
	JSON& operator[](size_t index)
	{
		assert(this);
		return *(JSON*)json_array_get(this, index);
	}

	// Retrieves a value from the array.
	//
	// @param index      Index in the array.
	// @return           Value read.
	template<typename T>
	T GetValue(size_t index)
	{
		return (*this)[index].GetValue<T>();
	}
	
	// Returns whether or not a value in the array is null.
	//
	// @param index      Index in the array.
	// @return           True if the value is null, false otherwise.
	bool IsNull(size_t index)
	{
		return json_is_null(json_array_get(this, index));
	}

	// Sets an array or object value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @param incref     Whether to increase the reference count of VALUE.
	// @return           True on success, false on failure.
	bool Set(size_t index, JSON *value, bool incref = false)
	{
		if (incref)
			return (json_array_set(this, index, value) == 0);	
		return (json_array_set_new(this, index, value) == 0);
	}

	// Sets a value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @return           True on success, false on failure.
	template<typename T>
	bool SetValue(size_t index, T value)
	{
		return (json_array_set_new(this, index, JSON::Create(value)) == 0);
	}

	// Sets a null value in the array.
	//
	// @param index      Index in the array.
	// @return           True on success, false on failure.
	bool SetNullValue(size_t index)
	{
		return (json_array_set_new(this, index, json_null()) == 0);
	}

	// Pushes an array or object value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @param incref     Whether to increase the reference count of VALUE.
	// @return           True on success, false on failure.
	bool Push(JSON *value, bool incref = false)
	{
		if (incref)
			return (json_array_append(this, value) == 0);
		return (json_array_append_new(this, value) == 0);
	}

	// Pushes a value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @return           True on success, false on failure.
	template<typename T>
	bool PushValue(T value)
	{
		return (json_array_append_new(this, JSON::Create(value)) == 0);
	}

	// Pushes a null value onto the end of the array, adding a new index.
	//
	// @return           True on success, false on failure.
	bool PushNullValue()
	{
		return (json_array_append_new(this, json_null()) == 0);
	}

	// Inserts an array or object value to array at position index, 
	// shifting the elements at index and after it one position towards the end of the array. 
	//
	// @param index      Index in the array.
	// @param value      Value to insert.
	// @param incref     Whether to increase the reference count of VALUE.
	// @return           True on success, false on failure.
	bool Insert(size_t index, JSON *value, bool incref = false)
	{
		if (incref)
			return (json_array_insert(this, index, value) == 0);
		return (json_array_insert_new(this, index, value) == 0);
	}

	// Inserts a value to array at position index, 
	// shifting the elements at index and after it one position towards the end of the array. 
	//
	// @param index      Index in the array.
	// @param value      Value to insert.
	// @return           True on success, false on failure.
	template<typename T>
	bool InsertValue(size_t index, T value)
	{
		return (json_array_insert_new(this, index, JSON::Create(value)) == 0);
	}

	// Inserts a null value to array at position index, 
	// shifting the elements at index and after it one position towards the end of the array. 
	//
	// @param index      Index in the array.
	// @return           True on success, false on failure.
	bool InsertNullValue(size_t index)
	{
		return (json_array_insert_new(this, index, json_null()) == 0);
	}

	// Appends all elements in other_array to the end of array. 
	//
	// @param other      Other array.
	// @return           True on success, false on failure.
	bool Extend(JSON *other)
	{
		return (json_array_extend(this, other) == 0);
	}

	// Removes an entry from the array.
	//
	// @param index      Index in the array to remove.
	// @return           True on success, false on invalid index.
	bool Remove(size_t index)
	{
		return (json_array_remove(this, index) == 0);
	}

	// Clears the array of all entries.
	// 
	// @return           True on success, false on failure.
	bool ArrClear()
	{
		return (json_array_clear(this) == 0);
	}

	// Retrieves the size of the array.
	size_t ArrSize()
	{
		return json_array_size(this);
	}
};


// ===========================================================================
// JSONObjectKeys
// ===========================================================================
class JSONObjectKeys
{
public:
	JSONObjectKeys(JSON *json)
	{
		m_json = json;
		m_iter = json_object_iter(json);
	}

	bool GetKeyValue(const char **key, JSON **value = nullptr)
	{
		*key = json_object_iter_key(m_iter);
		if (*key == nullptr)
			return false;
		
		if (value)
		{
			*value = (JSON*)json_object_iter_value(m_iter);
			if (*value == nullptr)
				return false;
		}

		m_iter = json_object_iter_next(m_json, m_iter);
		return true;
	}

private:
	json_t *m_json;
	void *m_iter;
};

} // namespace fdxx
