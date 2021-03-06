# MyVaultManager

A slightly complicated program that maintains a database, and records your balance.

This is a (final) homework from a C programming language course.

If you aren't quite familiar with `make` and wish to use [Dev-C++][devcpp], you can take `MyVault.dev` in `Dev-C++/` and put it along with other files, so you can compile the whole project easily with Dev-C++.

There's a strange bug where when writing to file, the program may crash on Linux/OS X if compiled with optimization. I'll fix it soon. See [this question][Q1] and [this question][Q2].

  [Q1]: https://stackoverflow.com/q/47220212/5958455 "Stack Overflow"
  [Q2]: https://stackoverflow.com/q/47224138/5958455 "Stack Overflow"

---

A bug that I don't want to fix:

- The database file may not be interchangeable between different (32/64-bit POSIX, LE/BE) systems.

  However, it's OK to transfer between 32-bit Windows and 64-bit Windows.

  The `vault.dat` is a sample database file from Windows. It's also usable on any 32-bit little-endian systems.

	[devcpp]: https://sourceforge.net/projects/orwelldevcpp/
