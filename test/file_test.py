import os
import sys
import unittest


class TestJsonFileIO(unittest.TestCase):
    def _check_obj_same(self, a, b):
        from test_utils import check_obj_same
        return check_obj_same(self, a, b)

    def setUp(self):
        import json

        from test_utils import get_benchfiles_fullpath
        self.file_path = get_benchfiles_fullpath()
        self.datas = []
        for file in get_benchfiles_fullpath():
            with open(file, "r", encoding='utf-8') as f:
                self.datas.append(json.load(f))

    def tearDown(self):
        pass

    def test_json_load(self):
        import json

        import cjson

        class A:
            def __init__(self, a):
                self.a = a

            def __eq__(self, other):
                return isinstance(other, A) and self.a == other.a

        def d4(obj):
            if "a" in obj:
                return A(obj["a"])
            return obj

        for file in self.file_path:
            # keywords parse
            with open(file, "r+") as f:
                cjson_data = cjson.load(object_hook=d4, fp=f)
                f.seek(0)
                json_data = json.load(f)
            self._check_obj_same(cjson_data, json_data)

            with open(file, "r+") as f:
                cjson_data = cjson.load(f)
                f.seek(0)
                json_data = json.load(f)
            self._check_obj_same(cjson_data, json_data)

    def test_json_dump(self):
        import json
        import tempfile

        import cjson
        for data in self.datas:
            with tempfile.NamedTemporaryFile(mode="w+") as f:
                cjson.dump(data, f)
                f.seek(0)
                cjson_data = json.load(f)
            with tempfile.NamedTemporaryFile(mode="w+") as f:
                json.dump(data, f)
                f.seek(0)
                json_data = json.load(f)
            self._check_obj_same(cjson_data, json_data)

        # keywords parse
        for data in self.datas:
            with tempfile.NamedTemporaryFile(mode="w+", encoding='utf-8') as f:
                cjson.dump(skipkeys=False, fp=f, obj=data)
                f.seek(0)
                cjson_data = json.load(f)
            with tempfile.NamedTemporaryFile(mode="w+", encoding='utf-8') as f:
                json.dump(data, f)
                f.seek(0)
                json_data = json.load(f)
            self._check_obj_same(cjson_data, json_data)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
