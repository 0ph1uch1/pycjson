import os
import sys
import cjson
import json
import unittest

class TestJsonFileIO(unittest.TestCase):
    def setUp(self):
        self.test_data = {
            "name": "XXY",
            "age": 99,
            "city": "GZ"
        }
        self.filepath = "test_data.json"

    def tearDown(self):
        if os.path.exists(self.filepath):
            os.remove(self.filepath)

    def test_json_load(self):
        class A:
            def __init__(self, a):
                self.a = a

            def __eq__(self, other):
                return isinstance(other, A) and self.a == other.a

        def d4(obj):
            if "a" in obj:
                return A(obj["a"])
            return obj
        
        with open(self.filepath, "w") as f:
            json.dump(self.test_data, f)

        with open(self.filepath, "r") as f:
            loaded_data = cjson.load(f)
        
        self.assertEqual(loaded_data, self.test_data)
        
        # keywords parse
        with open(self.filepath, "w") as f:
            json.dump(self.test_data, f)

        with open(self.filepath, "r") as f:
            loaded_data = cjson.load(object_hook=d4, fp=f)
        
        self.assertEqual(loaded_data, self.test_data)

    def test_json_dump(self):
        with open(self.filepath, "w") as f:
            cjson.dump(self.test_data, f)

        with open(self.filepath, "r") as f:
            # file_contents = f.read()
            loaded_data = json.load(f)
        # loaded_data = json.load(file_contents)

        self.assertEqual(loaded_data, self.test_data)

        # keywords parse
        with open(self.filepath, "w") as f:
            cjson.dump(skipkeys=False, fp=f, obj=self.test_data)

        with open(self.filepath, "r") as f:
            # file_contents = f.read()
            loaded_data = json.load(f)
        # loaded_data = json.load(file_contents)

        self.assertEqual(loaded_data, self.test_data)

if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()