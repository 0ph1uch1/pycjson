import os
import sys
import unittest


class TestDecode(unittest.TestCase):
    def _get_benchfiles_fullpath(self):
        benchmark_folder = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "bench"
        )

        return sorted([os.path.join(benchmark_folder, f) for f in os.listdir(benchmark_folder)])

    def _check(self, a, b):
        if isinstance(a, (list, tuple)):
            if not isinstance(b, (list, tuple)):
                self.fail("type mismatch")
            if len(a) != len(b):
                self.fail("list/tuple length mismatch")
            for va, vb in zip(a, b):
                self._check(va, vb)
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
                self._check(va, vb)
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
            self._num_check(a, b)
            return
        self.assertEqual(a, b, "mismatch")

    def _num_check(self, a, b):
        while a != 0 and abs(a) > 1:
            a /= 10
            b /= 10
        self.assertAlmostEqual(a, b, msg="number mismatch")

    def test_decode(self):
        import collections
        import json
        import math

        import cjson

        test_cases_origin = [
            True,
            False,
            None,
            1,
            -1,
            2.3,
            -2.3,
            321321432.231543245,
            -321321432.231543245,
            "abc",
            math.inf,
            -math.inf,
            math.nan,  # TODO
            math.pi,
            [], {}, tuple(),
            [1, 2, 3, 4],
            [155, {}, 2.3, "a", None, True, False, [], {}, 11],
            ("a", 1, 2.3, 2.3, None, True, False, [], {}),
            {"啊啊啊": "ß", "ü": ["\uff02", "\u00f8"]},
            dict({a: b for a in range(10) for b in range(10)}),
            [[[[[[[[[[[[[[]]]]]]], [[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]],
            collections.OrderedDict(x=1),
        ]

        bench_files = self._get_benchfiles_fullpath()

        test_cases = [json.dumps(case, ensure_ascii=False) for case in test_cases_origin]
        for bench_file in bench_files:
            with open(bench_file, "r", encoding='utf-8') as f:
                test_cases.append(f.read())

        for case in test_cases:
            with self.subTest(msg=f'decoding_test(case={case})'):
                re_json = json.loads(case)
                try:
                    re_cjson = cjson.loads(case)
                except Exception:
                    pass
                self._check(re_cjson, re_json)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
