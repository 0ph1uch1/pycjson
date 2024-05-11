import os
import sys
import unittest


class TestMemory(unittest.TestCase):

    @staticmethod
    def random_object(seed=None):
        from test_utils import FuzzGenerator
        return FuzzGenerator(seed).item()

    def test_encode_leak(self):
        """
        Developed by ESN, an Electronic Arts Inc. studio.
        Copyright (c) 2014, Electronic Arts Inc.
        All rights reserved.

        Full licence text can be found in https://github.com/ultrajson/ultrajson.
        """
        if hasattr(sys, "pypy_version_info"):
            # PyPy's GC works differently (no ref counting), so this wouldn't be useful.
            # Simply returning an empty list effectively disables the refcount test.
            return []

        import gc
        import time

        from test_utils import FuzzGenerator

        import cjson

        now = time.time()
        seeds = [now * i for i in range(1, 31)]
        for seed in seeds:
            data = self.random_object(seed)

            data_objects = FuzzGenerator.collect_all_objects(data)
            # Exclude ints because they get referenced by the lists below.
            data_objects = [o for o in data_objects if not isinstance(o, int)]
            gc.collect()
            data_ref_counts_before = [sys.getrefcount(o) for o in data_objects]
            cjson.dumps(data)
            gc.collect()
            data_ref_counts_after = [sys.getrefcount(o) for o in data_objects]
            if data_ref_counts_before != data_ref_counts_after:
                for o, before, after in zip(
                    data_objects, data_ref_counts_before, data_ref_counts_after
                ):
                    if before != after:
                        print(f"Ref count of {o!r} went from {before} to {after}")
                self.assertTrue(False, "Ref count changed")

    def tracemalloc_mem_check(self, datas, warm_up_repeat, test_repeat, test_func, mem_diff_limit):
        from test_utils import tracemalloc_mem_check
        return tracemalloc_mem_check(self, datas, warm_up_repeat, test_repeat, test_func, mem_diff_limit)

    def test_decode_leak(self):
        if hasattr(sys, "pypy_version_info"):
            # skip PyPy
            return

        from test_utils import get_benchfiles_fullpath

        import cjson

        datas = []
        for file in get_benchfiles_fullpath():
            with open(file, "r", encoding='utf-8') as f:
                datas.append(f.read())

        with self.subTest("cjson.loads"):
            self.tracemalloc_mem_check(
                datas,
                10,
                100,
                cjson.loads,
                mem_diff_limit=100
            )

    def test_dump_leak_refcount(self):
        """
        Developed by ESN, an Electronic Arts Inc. studio.
        Copyright (c) 2014, Electronic Arts Inc.
        All rights reserved.

        Full licence text can be found in https://github.com/ultrajson/ultrajson.
        """
        # A brute force fuzzer for detecting memory issues in ujson.dumps(). To use, first
        # compile ujson in debug mode:

        #     CFLAGS='-DDEBUG' python setup.py -q build_ext --inplace -f

        # Then run without arguments:

        #     python tests/fuzz.py

        # If it crashes, the last line of output is the arguments to reproduce the
        # failure.

        #     python tests/fuzz.py {{ last line of output before crash }}

        # Adding --dump-python or --dump-json will print the object it intends to
        # serialise as either a Python literal or in JSON.
        if hasattr(sys, "pypy_version_info"):
            # PyPy's GC works differently (no ref counting), so this wouldn't be useful.
            # Simply returning an empty list effectively disables the refcount test.
            return []

        import gc
        import tempfile
        import time

        from test_utils import FuzzGenerator

        import cjson

        now = time.time()
        seeds = [now * i for i in range(1, 31)]
        for seed in seeds:
            with tempfile.NamedTemporaryFile("w", delete=True) as f:
                data = self.random_object(seed)
                # print(f"--seed {seed}")

                data_objects = FuzzGenerator.collect_all_objects(data)
                # Exclude ints because they get referenced by the lists below.
                data_objects = [o for o in data_objects if not isinstance(o, int)]
                gc.collect()
                data_ref_counts_before = [sys.getrefcount(o) for o in data_objects]
                cjson.dump(data, f)
                gc.collect()
                data_ref_counts_after = [sys.getrefcount(o) for o in data_objects]
                if data_ref_counts_before != data_ref_counts_after:
                    for o, before, after in zip(
                        data_objects, data_ref_counts_before, data_ref_counts_after
                    ):
                        if before != after:
                            print(f"Ref count of {o!r} went from {before} to {after}")
                    self.assertTrue(False, "Ref count changed")

    def test_dump_leak(self):
        if hasattr(sys, "pypy_version_info"):
            # skip PyPy
            return

        import json
        import tempfile

        from test_utils import get_benchfiles_fullpath

        import cjson

        datas = []
        for file in get_benchfiles_fullpath():
            with open(file, "r", encoding='utf-8') as f:
                datas.append(json.load(f))

        with self.subTest("cjson.dump"):
            def dump(data):
                with tempfile.NamedTemporaryFile("w", delete=True) as f:
                    cjson.dump(data, f)
            mem_diff = self.tracemalloc_mem_check(
                datas,
                100,
                10,
                dump,
                mem_diff_limit=10000
            )
            mem_diff2 = self.tracemalloc_mem_check(
                datas,
                100,
                100,
                dump,
                mem_diff_limit=10000
            )
            self.assertLessEqual(mem_diff2 / max(mem_diff, 1), 5)

    def test_load_leak(self):
        if hasattr(sys, "pypy_version_info"):
            # skip PyPy
            return

        from test_utils import get_benchfiles_fullpath

        import cjson

        file_paths = get_benchfiles_fullpath()
        with self.subTest("cjson.load"):
            def load(file):
                with open(file, "r", encoding="utf-8") as f:
                    cjson.load(f)
            mem_diff = self.tracemalloc_mem_check(
                file_paths,
                100,
                10,
                load,
                mem_diff_limit=30000
            )
            mem_diff2 = self.tracemalloc_mem_check(
                file_paths,
                100,
                100,
                load,
                mem_diff_limit=30000
            )
            self.assertLessEqual(mem_diff2 / max(mem_diff, 1), 5)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
