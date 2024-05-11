import os
from typing import TYPE_CHECKING


if TYPE_CHECKING:
    import unittest


class FuzzGenerator:
    """A random JSON serialisable object generator."""

    def __init__(self, seed=None):
        import random
        self._randomizer = random.Random(seed)
        self._shrink = 1

    def key(self):
        key_types = [self.int, self.float, self.string, self.null, self.bool]
        return self._randomizer.choice(key_types)()

    def item(self):
        if self._randomizer.random() > 0.8:
            return self.key()
        return self._randomizer.choice([self.list, self.dict])()

    def int(self):
        return int(self.float())

    def float(self):
        import math
        sign = self._randomizer.choice([-1, 1, 0])
        return sign * math.exp(self._randomizer.uniform(-40, 40))

    def string(self):
        characters = ["\x00", "\t", "a", "\U0001f680", "<></>", "\u1234"]
        return self._randomizer.choice(characters) * self.length()

    def bool(self):
        return self._randomizer.random() < 0.5

    def null(self):
        return None

    def list(self):
        return [self.item() for i in range(self.length())]

    def dict(self):
        return {self.key(): self.item() for i in range(self.length())}

    def length(self):
        import math
        self._shrink *= 0.99
        return int(math.exp(self._randomizer.uniform(-0.5, 5)) * self._shrink)

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


def get_benchfiles_fullpath():
    benchmark_folder = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "bench"
    )

    return sorted([os.path.join(benchmark_folder, f) for f in os.listdir(benchmark_folder)])


def _num_check(self: "unittest.TestCase", a, b):
    while a != 0 and abs(a) > 1:
        a /= 10
        b /= 10
    self.assertAlmostEqual(a, b, msg="number mismatch")


def check_obj_same(self: "unittest.TestCase", a, b):
    if isinstance(a, (list, tuple)):
        if not isinstance(b, (list, tuple)):
            self.fail("type mismatch")
        if len(a) != len(b):
            self.fail("list/tuple length mismatch")
        for va, vb in zip(a, b):
            check_obj_same(self, va, vb)
        return
    if isinstance(a, dict):
        if not isinstance(b, dict):
            self.fail("type mismatch")
        if len(a) != len(b):
            self.fail("dict length mismatch")
        for ka, va in a.items():
            vb = b.get(ka)
            if vb is None and va is not None:
                self.fail("key mismatch")
            check_obj_same(self, va, vb)
        return
    if isinstance(a, (int, float)):
        import math
        if math.isnan(a):
            self.assertTrue(math.isnan(b), "nan mismatch")
            return
        if math.isinf(a):
            self.assertTrue(math.isinf(b), "inf mismatch")
            self.assertEqual(a > 0, b > 0, "inf sign mismatch")
            return
        _num_check(self, a, b)
        return
    self.assertEqual(a, b, "mismatch")


def gc_repeat_task(datas, run_times, test_func):
    if datas is not None:
        for data in datas:
            for _ in range(run_times):
                test_func(data)
    else:
        for _ in range(run_times):
            test_func()


def tracemalloc_mem_check(self: "unittest.TestCase", datas, warm_up_repeat, test_repeat, test_func, mem_diff_limit):
    import gc
    import tracemalloc

    # warm up. CPython will not release memory immediately.
    gc_repeat_task(datas, warm_up_repeat, test_func)
    #
    tracemalloc.start()
    #
    gc.collect()
    snapshot_1, peak_1 = tracemalloc.get_traced_memory()
    gc_repeat_task(datas, test_repeat, test_func)
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
    return mem_diff
