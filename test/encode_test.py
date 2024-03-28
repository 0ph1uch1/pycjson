import os
import sys
import unittest


class TestEncode(unittest.TestCase):
    def test_fail(self):
        import objgraph
        import gc
        import cjson

        class A:
            pass

        test_cases = [A()]

        for case in test_cases:
            with self.subTest(msg=f'encoding_fail_test(case={case})'):
                gc.collect()
                objs = [id(a) for a in objgraph.get_leaking_objects()]
                with self.assertRaises(Exception):
                    cjson.dumps(case)
                re = [a for a in list(objgraph.get_leaking_objects()) if id(a) not in objs]
                self.assertEqual(len(re), 0, f"leak: {re}")

    def test_encode(self):
        import json
        import math
        import objgraph
        import gc

        import cjson

        test_cases = [
            [1, 2, 3, 4],  # simple list
            [1, 2.3, "a", None, True, False, [], {}],  # list
            ("a", 1, 2.3, 2.3, None, True, False, [], {}),  # tuple
            tuple(range(100)),  # large tuple
            "a",  # simple string
            1,  # simple int
            2.3,  # simple float
            math.inf, math.nan, math.pi,
            None,
            True,
            False,
            [],
            {},
            dict({a: b for a in range(10) for b in range(10)}),  # set
            {"啊啊啊": "ß"},
            321321432.231543245,  # large float # TODO assert fail in decimal part length
            -321321432.231543245,
            -1,
            -2.3,
            -math.inf,
            -math.nan,
        ]

        for case in test_cases:
            result_json = json.dumps(case, indent=None, separators=(",", ":"), ensure_ascii=False)
            result_loadback_json = json.loads(result_json)
            #

            with self.subTest(msg=f'encoding_test(case={case})'):
                gc.collect()
                objs = [id(a) for a in objgraph.get_leaking_objects()]
                result_cjson = cjson.dumps(case)
                re = [a for a in list(objgraph.get_leaking_objects()) if id(a) not in objs]
                self.assertEqual(len(re), 0, f"leak: {re}")
                result_loadback_cjson = json.loads(result_cjson)
                self._check(result_loadback_json, result_loadback_cjson)

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


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
