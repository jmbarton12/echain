# content/

One folder per keychain, named after its key in `devices.yaml` (gitignored;
see `devices.example.yaml`):

```
content/
  keychain1/
    cat.jpg
    beach.heic
    quotes.yaml
```

Everything in here except this README is gitignored. Photos never leave your PC
except to go to the keychain. Run `echain sync <device>` to mirror a folder to a
keychain in sync mode (A+B).
