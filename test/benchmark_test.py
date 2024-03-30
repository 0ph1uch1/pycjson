import os
import sys
import unittest


class TestBenchmark(unittest.TestCase):
    def _get_benchfiles_fullpath(self):
        benchmark_folder = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "bench"
        )

        return sorted([os.path.join(benchmark_folder, f) for f in os.listdir(benchmark_folder)])

    def time_benchmark(self, repeat_time, func, *args, **kwargs):
        import time
        time_0 = time.time()
        for _ in range(repeat_time):
            func(*args, **kwargs)
        time_1 = time.time()
        return time_1 - time_0

    def test_benchmark_encode(self):
        import json

        import cjson

        bench_files = self._get_benchfiles_fullpath()

        for filename in bench_files:
            base_name = os.path.basename(filename)
            with open(filename, "r", encoding='utf-8') as f:
                data = json.load(f)
            time_std = self.time_benchmark(1000, json.dumps, data, indent=None, ensure_ascii=False)
            print(f"test encode, file: {base_name}, time_std: {time_std}")
            time_cjson = self.time_benchmark(1000, cjson.dumps, data)
            print(f"test encode, file: {base_name}, time_cjson: {time_cjson}")

    def test_benchmark_decode(self):
        import json

        import cjson

        bench_files = self._get_benchfiles_fullpath()

        for filename in bench_files:
            base_name = os.path.basename(filename)
            with open(filename, "r", encoding='utf-8') as f:
                data = f.read()
            time_std = self.time_benchmark(1000, json.loads, data)
            print(f"test decode, file: {base_name}, time_std: {time_std}")
            time_cjson = self.time_benchmark(1000, cjson.loads, data)
            print(f"test decode, file: {base_name}, time_cjson: {time_cjson}")


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
