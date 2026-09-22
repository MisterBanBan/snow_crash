In this level we have several important file. `enzo.wasm`, `validate.js`, `pipeline.sh` and `enzo_runner`.

`enzo_runner` is the executable allowing us to execute the pipeline in `pipeline.sh` with flag10 permissions.

This is the pipeline : 
```
wasmtime /opt/snowcrash/level10/enzo.wasm | qjs /opt/snowcrash/level10/validate.js
```

It basically executes `enzo.wasm` and sends it's output in `validate.js`.

If we executes `enzo.wasm` alone we can see that it outputs something like this : `v1:XXXXXX:YYYYYY`.

When looking whats inside `validate.js` we can see a script that is deliberately written in an obfuscated way using confusing names and functional programming. 

When analysing the javascript file we can unserstand better it's behavior and the format of the string taken in input.

The input strign is divided in 3 parts delimited by `:`. The first part should alway be `v1`. The second can be any string and the third should a signature of the second hashed with FNV1A32 hash algorithm.

we can see the hash alogorithm more clearely like this : 
```
function hash(s) {
    let h = 0x811c9dc5;
    for (const c of s)
        h = Math.imul(h ^ c.charCodeAt(0), 0x01000193) >>> 0;
    return h;
}
```

Then the scripts computes a key to determine which command it will execute. For that it takes the first two characters of the signature. There is three commands define here :

```
const _k0 = _hash('1fc').toString(36)
const _k1 = _hash('1d0').toString(36)
const _k2 = _hash('1a4').toString(36)

const _dispatch = _F({
    [_k0]: _b(x => std.popen(x, 'r'))(_c(x => y => x + y)('logger\x20-t\x20appd\x20')),
    [_k1]: _b(x => std.popen(x, 'r'))(_c(x => y => x + y)('systemctl\x20reload\x20')),
    [_k2]: _b(x => std.popen(x, 'r'))(_c(x => y => x + y)('ping\x20-c1\x20')),
})

const key = hash("1" + signature.slice(0, 2)).toString(36);
```

fc = logger
d0 = systemctl reload
a4 = ping

Then the script executes the corresponding command with the second element of the string as argument.

```
std.popen("systemctl reload " + NAME, "r")
```

And now we can see how to get the flag. Because name is dirercly added and executed after the command we can simply put something like this as second string : `;cat flag`.

And so if the signature starts with one of the three compinations of two characters the script will executes it and read the flag.

For that we need to recompile the wasm in order to print a combination that will do it. We can start by putting this string : ';cat /home/flag10/.flag >&2;' and then add trailing dummy characters in order to have the right signature. For example : `;cat /home/flag10/.flag >&2;lllm` gives this hash : `d0d5584f` that starts with d0.

We can now write a wasm script that will write `v1:;cat /home/flag10/.flag >&2;lllm:d0d5584f`

The `Enzo.wat` in ressources do that. We needs to compile it to replace `enzo.wasm` like this `wat2wasm test.wat -o /opt/snowcrash/level10/enzo.wasm`.

And then by executing `enzo_runner` it gives us the flag :
```
./enzo_runner 
Too few arguments.
g457ws0du8osi4t4tlea00cxe
sh: 1: lllm: not found
```
