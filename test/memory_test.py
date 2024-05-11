import os
import sys
import unittest


class TestMemory(unittest.TestCase):
    @staticmethod
    def collect_all_objects(obj):
        """Given an object, return a list of all objects referenced by it."""
        def _inner(o):
            yield o
            if isinstance(o, list):
                for v in o:
                    yield from _inner(v)
            elif isinstance(o, dict):
                for k, v in o.items():
                    yield from _inner(k)
                    yield from _inner(v)

        out = []
        seen = set()
        for o in _inner(obj):
            if id(o) not in seen and o is not None:
                seen.add(id(o))
                out.append(o)
        return out

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

        import cjson

        now = time.time()
        seeds = [now * i for i in range(1, 31)]
        for seed in seeds:
            data = self.random_object(seed)

            data_objects = self.collect_all_objects(data)
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

    def gc_repeat_task(self, datas, run_times, test_func):
        for data in datas:
            for _ in range(run_times):
                test_func(data)

    def tracemalloc_mem_check(self, datas, warm_up_repeat, test_repeat, test_func, mem_diff_limit):
        import gc
        import tracemalloc

        # warm up. CPython will not release memory immediately.
        self.gc_repeat_task(datas, warm_up_repeat, test_func)
        #
        tracemalloc.start()
        #
        gc.collect()
        snapshot_1, peak_1 = tracemalloc.get_traced_memory()
        self.gc_repeat_task(datas, test_repeat, test_func)
        gc.collect()
        snapshot_2, peak_2 = tracemalloc.get_traced_memory()
        #
        tracemalloc.stop()
        #
        mem_diff = snapshot_2 - snapshot_1
        peak_diff = peak_2 - peak_1
        print(f"testing {self._subtest._message}, mem_diff: {mem_diff}, peak_diff: {peak_diff}")
        # should not increase more than 100 bytes
        self.assertGreaterEqual(mem_diff_limit, mem_diff)
        return mem_diff_limit

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

        import cjson

        now = time.time()
        seeds = [now * i for i in range(1, 31)]
        for seed in seeds:
            with tempfile.NamedTemporaryFile("w", delete=True) as f:
                data = self.random_object(seed)
                # print(f"--seed {seed}")

                data_objects = self.collect_all_objects(data)
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
            mem_diff_limit = self.tracemalloc_mem_check(
                datas,
                10,
                100,
                dump,
                mem_diff_limit=10000
            )
            mem_diff_limit2 = self.tracemalloc_mem_check(
                datas,
                10,
                300,
                dump,
                mem_diff_limit=10000
            )
            self.assertAlmostEqual(mem_diff_limit2 / mem_diff_limit, 1., delta=0.1)

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
            self.tracemalloc_mem_check(
                file_paths,
                10,
                100,
                load,
                mem_diff_limit=100
            )


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
