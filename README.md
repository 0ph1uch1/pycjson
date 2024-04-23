# py-cjson
py-cjson is a python3 json library, powered by algorithms from cJSON library, providing JSON relevant functionality. It aims to have same interface as Python json library.

## Installation
To install py-cjson, follow these steps:

1. Clone the repository to your local machine.  
2. Navigate to the cloned repository.  
3. Run the command `python -m pip install .` to install the package.

## License  
MIT License  
The content of this repository is licensed under the terms of the MIT License. See the [LICENSE](LICENSE) file for more information.

## Relevant
To understand more about the cJSON library and its usage, you can refer to the following resources:

[cJSON](https://github.com/DaveGamble/cJSON): Ultralightweight JSON parser in ANSI C.  
[json.org](https://www.json.org/): The official JSON website.  
[Python json](https://docs.python.org/3/library/json.html): The official Python json library documentation.  


## Usage  
Once py-cjson is installed, you can use it in your Python 3 projects. The interface and parameters provided by py-cjson are designed to closely resemble thouse of the JSON library. You can refer to the JSON documentation for more details on how to use the library.

Example usage:
```python
import cjson
# loads, translate json string to python object
cjson.loads(json_str)

# dumps, translate python object to json string
cjson.dumps(data)

# encode same as dumps
json_str = cjson.encode(data)

# decode same as loads
decoded_data = cjson.decode(json_str)

# load from file
with open("data.json", "r") as file:
    data = cjson.load(file)

# dump to file
with open("data.json", "w") as file:
    cjson.dump(data, file)
```

Please note that the above usage example is a simplified demonstration. Refer to the cJSON documentation for a complete understanding of the library's features and usage patterns.

## Available api and parameters
### encode
```python
cjson.encode(obj, format=None, skipkeys=None, allow_nan=None, separators=None, defaule=None) #Converts arbitrary object recursively into JSON.
cjson.dumps(obj, format=None, skipkeys=None, allow_nan=None, separators=None, defaule=None) #Converts arbitrary object recursively into JSON.
cjson.dump(obj, fp, format=None, skipkeys=None, allow_nan=None, separators=None, defaule=None) #Converts arbitrary object recursively into JSON file.

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
cjson.decode(s, object_hook=None) #Converts JSON as string to dict object structure.
cjson.loads(s, object_hook=None) #Converts JSON as string to dict object structure.
cjson.dump(fp, object_hook=None) #Converts JSON as file to dict object structure.

# optional parameter: object hook, used to implement custom decoders
def object_hook(obj):
    if "key" in obj:
        return obj["key"]
    return obj

data = cjson.loads(json_str, object_hook=object_hook)
```
