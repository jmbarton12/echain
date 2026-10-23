# content/

One folder per keychain, named after the device in `devices.yaml`:

```
content/
  alice/
    cat.jpg
    beach.heic
    quotes.yaml
```

Everything in here except this README is gitignored. Photos never leave your PC
except to go to the keychain. Run `echain sync <device>` to mirror a folder to a
keychain in sync mode (A+B).
