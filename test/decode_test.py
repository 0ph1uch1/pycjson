import os
import sys
import unittest


class TestDecode(unittest.TestCase):
    def test_decode(self):
        import json
        import math

        import cjson

        test_cases = [
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
            [[[[[[[[[[[[[[]]]]]]], [[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]
        ]

        test_cases = [json.dumps(case, ensure_ascii=False) for case in test_cases]

        for case in test_cases:
            with self.subTest(msg=f'decoding_test(case={case})'):
                re_json = json.loads(case)
                re_cjson = cjson.loads(case)
                if isinstance(re_json, float) and math.isnan(re_json):
                    self.assertTrue(math.isnan(re_cjson))
                else:
                    self.assertEqual(re_cjson, re_json)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
