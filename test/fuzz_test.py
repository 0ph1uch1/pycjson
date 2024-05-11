import os
import sys
import unittest


class TestFuzz(unittest.TestCase):
    def setUp(self):
        import random
        import time
        random.seed(time.time())

    def _check_obj_same(self, a, b):
        from test_utils import check_obj_same
        return check_obj_same(self, a, b)

    def test_fuzz(self):
        from test_utils import tracemalloc_mem_check
        # with self.subTest("fuzz loads"):
        #     tracemalloc_mem_check(
        #         self,
        #         datas=None,
        #         warm_up_repeat=100,
        #         test_repeat=1000,
        #         test_func=self._fuzz_loads,
        #         mem_diff_limit=100
        #     )

        # with self.subTest("fuzz dumps"):
        #     tracemalloc_mem_check(
        #         self,
        #         datas=None,
        #         warm_up_repeat=100,
        #         test_repeat=1000,
        #         test_func=self._fuzz_dumps,
        #         mem_diff_limit=100
        #     )

        with self.subTest("fuzz order"):
            tracemalloc_mem_check(
                self,
                datas=None,
                warm_up_repeat=100,
                test_repeat=1000,
                test_func=self._fuzz_order,
                mem_diff_limit=1
            )

    def _gen_string(self):
        import random
        import string
        length = random.randint(0, 1000)
        return "".join(random.choices(string.printable, k=length))

    def _gen_objs(self):
        import time

        from test_utils import FuzzGenerator
        seed = time.time()
        return FuzzGenerator(seed).item()

    def _fuzz_loads(self):
        # generate random string
        import cjson

        s = self._gen_string()
        try:
            cjson.loads(s)
        except Exception:
            pass

    def _fuzz_dumps(self):
        # generate random object
        import cjson

        obj = self._gen_objs()
        cjson.dumps(obj)

    def _fuzz_order(self):
        import cjson
        import json
        import random
        obj = json.loads(json.dumps(self._gen_objs()))
        original_obj = obj

        dumps_s = [cjson.dumps, json.dumps]
        loads_s = [cjson.loads, json.loads]

        for _ in range(10):
            dumps = random.choice(dumps_s)
            loads = random.choice(loads_s)
            obj = loads(dumps(obj))

        from test_utils import check_obj_same
        check_obj_same(self, obj, original_obj)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
