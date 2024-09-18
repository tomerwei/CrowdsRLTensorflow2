import wrap


def test_add():
    assert (wrap.add(3, 4) == 7)
    print(wrap.add(3, 4))
    print("Test Passed")


if __name__ == '__main__':
    test_add()
