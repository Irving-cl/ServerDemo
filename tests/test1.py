#!/usr/bin/env python

import unittest

class TestForFun(unittest.TestCase):

    def test(self):
        self.assertEqual(1 + 1, 2)

if __name__ == '__main__':
    unittest.main()
