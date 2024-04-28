import os
import sys
import unittest


class TestDecode(unittest.TestCase):
    def _check_obj_same(self, a, b):
        from test_utils import check_obj_same
        return check_obj_same(self, a, b)

    def test_fail(self):
        import cjson

        test_cases = {
            ValueError: ["0xf", "{\"a\":}", "[1,2,3", "\"111\"]", "{\"a\": 1 2}"],
            # OverflowError: ["1e500"]
        }
        for err, cases in test_cases.items():
            for case in cases:
                with self.subTest(msg=f'decoding_fail_test(case={case})'):
                    with self.assertRaises(err):
                        cjson.loads(case)

    def test_object_hook(self):
        import cjson
        import json

        class A:
            def __init__(self, a):
                self.a = a

            def __eq__(self, other):
                return isinstance(other, A) and self.a == other.a

        def d4(obj):
            if "a" in obj:
                return A(obj["a"])
            return obj

        test_cases = [
            '{"a": 1}',
            '[{"a": 1}]',
            '{"a": {"a": 1}}',
            '[{"a": {"a": 1}}, {"b": 2}]',
        ]

        for case in test_cases:
            with self.subTest(msg=f'decoding_object_hook_test(case={case})'):
                result_json = json.loads(case, object_hook=d4)
                result_cjson = cjson.loads(case, object_hook=d4)
                self._check_obj_same(result_json, result_cjson)

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
            math.nan,
            math.pi,
            [], {}, tuple(),
            [1, 2, 3, 4],
            [155, {}, 2.3, "a", None, True, False, [], {}, 11],
            ("a", 1, 2.3, 2.3, None, True, False, [], {}),
            {"啊啊啊": "ß", "ü": ["\uff02", "\u00f8"]},
            {"\"\"\"\"\"": "\"\"\""},
            dict({a: b for a in range(10) for b in range(10)}),
            [[[[[[[[[[[[[[]]]]]]], [[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]],
            collections.OrderedDict(x=1),
            "\u3000\U00010000",
        ]

        from test_utils import get_benchfiles_fullpath
        bench_files = get_benchfiles_fullpath()

        test_cases = [json.dumps(case, ensure_ascii=False) for case in test_cases_origin]
        for bench_file in bench_files:
            with open(bench_file, "r", encoding='utf-8') as f:
                test_cases.append(f.read())

        for case in test_cases:
            with self.subTest(msg=f'decoding_test(case={case})'):
                re_json = json.loads(case)
                re_cjson = cjson.loads(case)
                self._check_obj_same(re_cjson, re_json)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
