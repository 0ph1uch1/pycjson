# py-cjson
py-cjson is a python3 json library, powered by algorithms from cJSON library, providing JSON relevant functionality. It is designed to offer a similar interface to those who are accustomed to the built-in Python json module, while providing an improved speed for JSON encoding and decoding.

## Installation
To install py-cjson, follow these steps:

1. Clone the repository to your local machine.  
2. Navigate to the cloned repository.  
3. Run the command `python -m pip install .` to install the package.

## License  
MIT License  
The content of this repository is licensed under the terms of the MIT License. See the [LICENSE](LICENSE) file for more information.


## Usage  
Once py-cjson is installed, you can use it in your Python 3 projects. The interface and parameters provided by py-cjson are designed to closely resemble thouse of the JSON library. You can refer to the JSON documentation for more details on how to use the library.

Example usage:
```python
import cjson

# Decode a JSON string into a Python object with `loads`
json_str = '{"name": "John", "age": 30, "city": "New York"}'
python_obj = cjson.loads(json_str)
print(python_obj)  # Output: {'name': 'John', 'age': 30, 'city': 'New York'}

# Encode a Python object into a JSON string with `dumps`
python_dict = {'name': 'Jane', 'age': 25, 'city': 'Los Angeles'}
json_str = cjson.dumps(python_dict)
print(json_str)  # Output: '{"name": "Jane", "age": 25, "city": "Los Angeles"}'

# Read a JSON file and convert its contents to a Python object with `load`
with open('data.json', 'r') as file:
    data = cjson.load(file)
    print(data)

# Write a Python object to a file as JSON with `dump`
new_data = {'name': 'Doe', 'age': 22, 'city': 'Chicago'}
with open('data.json', 'w') as file:
    cjson.dump(new_data, file)

# encode same as dumps
json_str = cjson.encode(data)

# decode same as loads
decoded_data = cjson.decode(json_str)
```

Please note that the above usage example is a simplified demonstration. Refer to the documentation for a complete understanding of the library's features(Ongoing).

## Available api and parameters
### encode
```python
cjson.encode(obj, *, format=False, skipkeys=False, allow_nan=True, separators=(",",":"), default=None) #Converts arbitrary object recursively into JSON.
cjson.dumps(obj, *, format=False, skipkeys=False, allow_nan=True, separators=(",",":"), default=None) #Converts arbitrary object recursively into JSON.
cjson.dump(obj, fp, *, format=False, skipkeys=False, allow_nan=True, separators=(",",":"), default=None) #Converts arbitrary object recursively into JSON file.

# dump optional parameter: default, used to implement custom encoders
def default(obj):
    if isinstance(obj, str):
        return {"key": obj}
    return obj
cjson.dump(data, file, default=default)

# dump optional parameter: allow_nan
data = {"key": float("nan")}
json_str = cjson.encode(data, allow_nan=Fasle) # raises ValueError

# dump optional parameter: separators
json_str = cjson.encode(data, separators=(",", ":"))

# dump optional parameter: skipkeys
data = {"key": float("nan")}
json_str = cjson.encode(data, skipkeys=True) # skips the key

```

### decode
```python
cjson.decode(s, *, object_hook=None) #Converts JSON as string to dict object structure.
cjson.loads(s, *, object_hook=None) #Converts JSON as string to dict object structure.
cjson.load(fp, *, object_hook=None) #Converts JSON as file to dict object structure.

# optional parameter: object hook, used to implement custom decoders
def object_hook(obj):
    if "key" in obj:
        return obj["key"]
    return obj

data = cjson.loads(json_str, object_hook=object_hook)
```


## See Also
To understand more about the cJSON library and its usage, you can refer to the following resources:

[cJSON](https://github.com/DaveGamble/cJSON): Ultralightweight JSON parser in ANSI C.  
[json.org](https://www.json.org/): The official JSON website.  
[Python json](https://docs.python.org/3/library/json.html): The official Python json library documentation.  