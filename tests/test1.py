#!/usr/bin/env python

import unittest

class TestForFun(unittest.TestCase):

    def test(self):
        self.assertEqual(1 + 1, 2)
        self.assertEqual(1 + 2, 3)

if __name__ == '__main__':
    unittest.main()
