# py-cjson
py-cjson is a Python 3 implementation of the cJSON library, providing JSON parsing and encoding functionality. It aims to have the same interface and parameters as cJSON.

## Relevant
To understand more about the cJSON library and its usage, you can refer to the following resources:

[UltraJSON](https://github.com/ultrajson/ultrajson): Ultra fast JSON decoder and encoder written in C with Python bindings.  
[cJSON](https://github.com/DaveGamble/cJSON): Ultralightweight JSON parser in ANSI C.  
## Installation
To install py-cjson, follow these steps:

1. Clone the repository to your local machine.  
2. Navigate to the cloned repository.  
3. Run the command `python -m pip install .` to install the package.

## Usage  
Once py-cjson is installed, you can use it in your Python 3 projects. The interface and parameters provided by py-cjson are designed to be identical to the cJSON library. You can refer to the cJSON documentation for more details on how to use the library.

Example usage:
```python
import cjson

# JSON encoding
data = {"key": "value"}
json_str = cjson.encode(data)
print(json_str)

# JSON decoding
decoded_data = cjson.decode(json_str)
print(decoded_data)
```
Please note that the above usage example is a simplified demonstration. Refer to the cJSON documentation for a complete understanding of the library's features and usage patterns.

## Available api and parameters
### encode
```python
cjson.encode() #Converts arbitrary object recursively into JSON.
cjson.dumps() #Converts arbitrary object recursively into JSON.
cjson.dump() #Converts arbitrary object recursively into JSON file.
```

### decode
```python
cjson.decode() #Converts JSON as string to dict object structure.
cjson.loads() #Converts JSON as string to dict object structure.
cjson.dump() #Converts JSON as file to dict object structure.
```
