#include "../janssonex.hpp"
#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printfn(const char *format, ...);
void PrintJson(JSON *json, size_t flags = 0);
void Test1(char **buffer);
void Test2(char **buffer);
void Test3(char **buffer);
void IterObject(JSON *root);
void IterArray(JSON *root, const char *name);


int main(int argc, char **argv)
{
	char *buffer;
	Test1(&buffer);
	Test2(&buffer);
	Test3(&buffer);
	free(buffer);
	printfn("--- json test end ---");
}

void Test1(char **buffer)
{
	printfn("--- json set test ---");
	JSON *root = JSON::CreateObject();

	root->SetValue<const char*>("strKey", "str");
	root->SetValue<int>("intKey", 1);
	root->SetValue<float>("floatKey", 1.5);
	root->SetValue<bool>("boolKey", false);
	root->SetNullValue("nullKey");

	JSON *array = JSON::CreateArray();
	root->Set("Array", array);

	JSON *object0 = JSON::CreateObject();
	JSON *object1 = JSON::CreateObject();

	array->Push(object0);
	array->Push(object1);

	JSON *strArray = JSON::CreateArray();
	strArray->PushValue<const char*>("str1");
	strArray->PushValue<const char*>("str2");

	JSON *intArray = JSON::CreateArray();
	intArray->PushValue<int>(1);
	intArray->PushValue<int>(2);
	intArray->PushValue<int>(5);

	object0->Set("strArray", strArray);
	object0->Set("intArray", intArray);

	JSON *floatArray = JSON::CreateArray();
	floatArray->PushValue<float>(1.1f);
	floatArray->PushValue<float>(2.0f);
	floatArray->PushValue<float>(5.53216111f);
	floatArray->PushValue<float>(7.7f);

	JSON *boolArray = JSON::CreateArray();
	boolArray->PushValue<bool>(true);

	object1->Set("floatArray", floatArray);
	object1->Set("boolArray", boolArray);
	
	*buffer = root->ToString(JSON_INDENT(4)|JSON_REAL_PRECISION(6));
	printfn("%s", *buffer);
	
	root->decref();
}

void Test2(char **buffer)
{
	JSON *root = JSON::FromString(*buffer, JSON_INDENT(4)|JSON_REAL_PRECISION(6));
	IterObject(root);
	root->decref();
}

void IterObject(JSON *root)
{
	printfn("------ IterObject ------");

	JSONObjectKeys iter(root);
	const char *key;
	JSON *value;

	while (iter.GetKeyValue(&key, &value))
	{
		if (json_is_object(value))
			IterObject(value);
		
		else if (json_is_array(value))
			IterArray(value, key);

		else if (json_is_string(value))
			printfn("%s = %s(%s)", key, value->GetValue<const char*>(), root->GetValue<const char*>(key));

		else if (json_is_integer(value))
			printfn("%s = %i(%i)", key, value->GetValue<int>(), root->GetValue<int>(key));

		else if (json_is_real(value))
			printfn("%s = %.3f(%.3f)", key, value->GetValue<float>(), root->GetValue<float>(key));

		else if (json_is_boolean(value))
			printfn("%s = %i(%i)", key, value->GetValue<bool>(), root->GetValue<bool>(key));
		
		else if (json_is_null(value))
			printfn("%s = null", key);
	}
}

void IterArray(JSON *root, const char *name)
{
	printfn("------ IterArray %s ------", name);

	for (size_t i = 0, len = root->ArrSize(); i < len; i++)
	{
		JSON &value = (*root)[i];

		if (json_is_object(&value))
			IterObject(&value);

		else if (json_is_string(&value))
			printfn("%s(%s)", value.GetValue<const char*>(), root->GetValue<const char*>(i));

		else if (json_is_integer(&value))
			printfn("%i(%i)", value.GetValue<int>(), root->GetValue<int>(i));

		else if (json_is_real(&value))
			printfn("%.3f(%.3f)", value.GetValue<float>(), root->GetValue<float>(i));

		else if (json_is_boolean(&value))
			printfn("%i(%i)", value.GetValue<bool>(), root->GetValue<bool>(i));
		
		else if (json_is_null(&value))
			printfn("null");
	}
}

void Test3(char **buffer)
{
	JSON *root = JSON::FromString(*buffer, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Remove boolKey Test ---");
	root->Remove("boolKey");
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Remove intArray Test ---");
	JSON &object0 = (*root)["Array"][0ul];
	object0.Remove("intArray");
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Clear object0 Test ---");
	object0.ObjClear();
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Remove floatArray[2] Test ---");
	JSON &floatArray = (*root)["Array"][1]["floatArray"];
	floatArray.Remove(2);
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Clear floatArray Test ---");
	floatArray.ArrClear();
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- InsertValue floatArray Test ---");
	floatArray.InsertValue<float>(0, 10.2f);
	floatArray.InsertValue<float>(0, 11.2f);
	floatArray.InsertValue<float>(1, 1.2f);
	PrintJson(&floatArray, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Copy Array Test ---");
	JSON *Array1 = (*root)["Array"].DeepCopy();
	PrintJson(Array1, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	printfn("--- Extend Array Test ---");
	(*root)["Array"].Extend(Array1);
	PrintJson(root, JSON_INDENT(4)|JSON_REAL_PRECISION(6));

	Array1->decref();
	root->decref();
}

void PrintJson(JSON *json, size_t flags)
{
	char *str = json->ToString(flags);
	printf("%s\n", str);
	free(str);
}

void printfn(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}

/*
--- json set test ---
{
	"strKey": "str",
	"intKey": 1,
	"floatKey": 1.5,
	"boolKey": false,
	"nullKey": null,
	"Array": [
		{
			"strArray": [
				"str1",
				"str2"
			],
			"intArray": [
				1,
				2,
				5
			]
		},
		{
			"floatArray": [
				1.1,
				2.0,
				5.53216,
				7.7
			],
			"boolArray": [
				true
			]
		}
	]
}
*/
